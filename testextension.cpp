#include <pxt.h>
#include "mlrunner/mlrunner.h"
#include "mlrunner/mldataprocessor.h"
#include "mlrunner/example_model1.h"

// Using defines to avoid MakeCode exposing the enum to enums.d.ts
#define TEST_RUNNER_ID_INFERENCE 71
#define TEST_RUNNER_ID_TIMER 72
#define TEST_RUNNER_ERROR 800

// Enable/disable debug print to serial, can be set in pxt.json
#ifndef ML_DEBUG_PRINT
#define ML_DEBUG_PRINT 1
#endif
#if ML_DEBUG_PRINT
#define DEBUG_PRINT(...) uBit.serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif


static inline void start_ticks_cpu() {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static inline uint32_t ticks_cpu() {
    return DWT->CYCCNT;
}

static uint32_t ticks[10];
static uint32_t ticks_index = 0;
static bool ticks_start_average = false;
static inline uint32_t calcTicks(uint32_t ticks_start, uint32_t ticks_end) {
    ticks[ticks_index] = ticks_end - ticks_start;
    ticks_index++;
    if (ticks_index >= 9) {
        ticks_start_average = true;
        ticks_index = 0;
    }

    if (!ticks_start_average) {
        return 0;
    }

    uint32_t ticksAverage = 0;
    for (size_t i = 0; i < 10; i++) {
        ticksAverage += ticks[i];
    }
    return ticksAverage / 10;
}


namespace testrunner {
    static ml_actions_t *actions = NULL;
    static ml_predictions_t *predictions = NULL;
    static bool initialised = false;
    static const uint16_t ML_CODAL_TIMER_VALUE = 1;

    // Order is important for the outputData as set in:
    // https://github.com/microbit-foundation/ml-trainer/blob/v0.6.0/src/script/stores/mlStore.ts#L122-L131
    static const MlDataFilters_t mlTrainerDataFilters[] = {
        {1, filterMax},
        {1, filterMean},
        {1, filterMin},
        {1, filterStdDev},
        {1, filterPeaks},
        {1, filterTotalAcc},
        {1, filterZcr},
        {1, filterRms},
    };
    static const int mlTrainerDataFiltersLen = sizeof(mlTrainerDataFilters) / sizeof(mlTrainerDataFilters[0]);

    void runModel() {
        if (!initialised) return;

        unsigned int time_start = system_timer_current_time_us();

        int32_t ticks_start = ticks_cpu() & 0x7FFFFFFF;
        float *modelData = mlDataProcessor.getProcessedData();
        int32_t ticks_end = ticks_cpu() & 0x7FFFFFFF;
        if (modelData == NULL) {
            DEBUG_PRINT("Failed to processed data for the model\n");
            uBit.panic(TEST_RUNNER_ERROR + 21);
        }

        unsigned int time_mid = system_timer_current_time_us();

        bool success = ml_predict(
            modelData, mlDataProcessor.getProcessedDataSize(), actions, predictions);
        if (!success) {
            DEBUG_PRINT("Failed to run model\n");
            uBit.panic(TEST_RUNNER_ERROR + 22);
        }

        unsigned int time_end = system_timer_current_time_us();

        DEBUG_PRINT("Prediction (%d micros + %d micros, %d ticks): ",
                    time_mid - time_start, time_end - time_mid, calcTicks(ticks_start, ticks_end));
        if (predictions->index >= 0) {
            DEBUG_PRINT("%d %s\n",
                        predictions->index,
                        actions->action[predictions->index].label);
        } else {
            DEBUG_PRINT("None\n");
        }
        DEBUG_PRINT("\tIndividual:");
        for (size_t i = 0; i < actions->len; i++) {
            DEBUG_PRINT(" %s [%d]",
                        actions->action[i].label,
                        (int)(predictions->prediction[i] * 100));
        }
        DEBUG_PRINT("\n\n");

        MicroBitEvent evt(TEST_RUNNER_ID_INFERENCE, predictions->index + 2);
    }

    void recordAccData(MicroBitEvent) {
        if (!initialised) return;

        const float accData[3] = {
            uBit.accelerometer.getX() / 1000.0f,
            uBit.accelerometer.getY() / 1000.0f,
            uBit.accelerometer.getZ() / 1000.0f,
        };
        MldpReturn_t recordDataResult = mlDataProcessor.recordData(accData, 3);
        if (recordDataResult != MLDP_SUCCESS) {
            DEBUG_PRINT("Failed to record accelerometer data\n");
            return;
        }

        if (mlDataProcessor.isDataReady()) {
            // Stop firing timer events while running model and resume after
            uBit.messageBus.ignore(TEST_RUNNER_ID_TIMER, ML_CODAL_TIMER_VALUE, &recordAccData);
            runModel();
            uBit.messageBus.listen(TEST_RUNNER_ID_TIMER, ML_CODAL_TIMER_VALUE, &recordAccData, MESSAGE_BUS_LISTENER_DROP_IF_BUSY);
        }
    }

    /*************************************************************************/
    /* Exported functions                                                    */
    /*************************************************************************/
    //%
    void init(Buffer model_str) {
#if MICROBIT_CODAL != 1
        target_panic(PANIC_VARIANT_NOT_SUPPORTED);
#endif

        if (initialised) return;

#if DEVICE_MLRUNNER_USE_EXAMPLE_MODEL != 0
        DEBUG_PRINT("Using example model (%d)...\n", DEVICE_MLRUNNER_USE_EXAMPLE_MODEL);
        void *model_address = (void *)example_model;
        const MlDataFilters_t* mlDataFilters = example_mlDataFilters;
        const int mlDataFiltersLen = example_mlDataFiltersLen;
        const int expectedDimensions = (DEVICE_MLRUNNER_USE_EXAMPLE_MODEL == 2) ? 1 : 3;
#else
        DEBUG_PRINT("Using embedded model...\n");
        if (model_str == NULL || model_str->length <= 0 || model_str->data == NULL) {
            DEBUG_PRINT("Model string not present\n");
            uBit.panic(TEST_RUNNER_ERROR + 1);
        }
        void *model_address = (void *)model_str->data;
        const MlDataFilters_t* mlDataFilters = mlTrainerDataFilters;
        const int mlDataFiltersLen = mlTrainerDataFiltersLen;
        const int expectedDimensions = 3;
#endif

        const bool setModelSuccess = ml_setModel(model_address);
        if (!setModelSuccess) {
            DEBUG_PRINT("Model magic invalid\n");
            uBit.panic(TEST_RUNNER_ERROR + 2);
        }

        const int samplesLen = ml_getSamplesLength();
        DEBUG_PRINT("\tModel samples length: %d\n", samplesLen);
        if (samplesLen <= 0) {
            DEBUG_PRINT("Model samples length invalid\n");
            uBit.panic(TEST_RUNNER_ERROR + 3);
        }

        const int sampleDimensions = ml_getSampleDimensions();
        DEBUG_PRINT("\tModel sample dimensions: %d\n", sampleDimensions);
        if (sampleDimensions != expectedDimensions) {
            DEBUG_PRINT("Model sample dimensions invalid\n");
            uBit.panic(TEST_RUNNER_ERROR + 4);
        }

        const int samplesPeriodMillisec = ml_getSamplesPeriod();
        DEBUG_PRINT("\tModel samples period: %d ms\n", samplesPeriodMillisec);
        if (samplesPeriodMillisec <= 0) {
            DEBUG_PRINT("Model samples period invalid\n");
            uBit.panic(TEST_RUNNER_ERROR + 5);
        }

        const int modelInputLen = ml_getInputLength();
        DEBUG_PRINT("\tModel input length: %d\n", modelInputLen);
        if (modelInputLen <= 0) {
            DEBUG_PRINT("Model input length invalid\n");
            uBit.panic(TEST_RUNNER_ERROR + 6);
        }

        const int modelOutputLen = ml_getInputLength();
        DEBUG_PRINT("\tModel output length: %d\n", modelOutputLen);
        if (modelOutputLen <= 0) {
            DEBUG_PRINT("Model output length invalid\n");
            uBit.panic(TEST_RUNNER_ERROR + 7);
        }

        const int modelArenaSize = ml_getArenaSize();
        DEBUG_PRINT("\tModel arena size: %d bytes\n", modelArenaSize);
        if (modelArenaSize <= 0) {
            DEBUG_PRINT("Model arena size length invalid\n");
            uBit.panic(TEST_RUNNER_ERROR + 8);
        }

        actions = ml_allocateActions();
        if (actions == NULL) {
            DEBUG_PRINT("Failed to allocate memory for actions\n");
            uBit.panic(TEST_RUNNER_ERROR + 9);
        }
        const bool getActionsSuccess = ml_getActions(actions);
        if (!getActionsSuccess) {
            DEBUG_PRINT("Failed to retrieve actions\n");
            uBit.panic(TEST_RUNNER_ERROR + 10);
        }
        DEBUG_PRINT("\tActions (%d):\n", actions->len);
        for (size_t i = 0; i < actions->len; i++) {
            DEBUG_PRINT("\t\t'%s' threshold = %d%%\n", actions->action[i].label, (int)(actions->action[i].threshold * 100));
        }

        predictions = ml_allocatePredictions();
        if (predictions == NULL) {
            DEBUG_PRINT("Failed to allocate memory for predictions\n");
            uBit.panic(TEST_RUNNER_ERROR + 11);
        }

        const MlDataProcessorConfig_t mlDataConfig = {
            .samples = samplesLen,
            .dimensions = sampleDimensions,
            .output_length = modelInputLen,
            .filter_size = mlDataFiltersLen,
            .filters = mlDataFilters,
        };
        MldpReturn_t mlInitResult = mlDataProcessor.init(&mlDataConfig);
        if (mlInitResult != MLDP_SUCCESS) {
            DEBUG_PRINT("Failed to initialise ML data processor (%d)\n", mlInitResult);
            // TODO: Check error type and set panic value accordingly
            uBit.panic(TEST_RUNNER_ERROR + 12);
        }

        // Set up background timer to collect data and run model
        uBit.messageBus.listen(TEST_RUNNER_ID_TIMER, ML_CODAL_TIMER_VALUE, &recordAccData, MESSAGE_BUS_LISTENER_DROP_IF_BUSY);
        uBit.timer.eventEvery(samplesPeriodMillisec, TEST_RUNNER_ID_TIMER, ML_CODAL_TIMER_VALUE);

        start_ticks_cpu();

        initialised = true;

        DEBUG_PRINT("\tModel loaded\n");
    }
}
