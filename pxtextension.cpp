#include <pxt.h>
#include "mlrunner.h"
#if DEVICE_MLRUNNER_USE_EXAMPLE_MODEL == 1
#include "model_example.h"
#endif

enum MlRunnerIds {
    MlRunnerInference = 71,
    MlRunnerTimer = 72,
};

enum MlRunnerError {
    ErrorModelNotPresent = 800,
    ErrorInputLength = 801,
    ErrorMemAlloc = 802,
    ErrorModelInference = 803,
};

static float *accData = NULL;
static float accData2[750];
static int accDataSize = 0;
static int accDataIndex = 0;

static bool initialised = false;

static const CODAL_TIMESTAMP ML_CODAL_TIMER_PERIOD = 20;
static const uint16_t ML_CODAL_TIMER_VALUE = 1;


// Enable/disable debug print to serial
#define ENABLE_DEBUG_PRINT 1
#if ENABLE_DEBUG_PRINT
#define DEBUG_PRINT(...) uBit.serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

namespace mlrunner {

    void runModel() {
        if (!initialised) return;

        ml_prediction_t* predictions = ml_predict(accData);
        if (predictions == NULL) {
            uBit.panic(MlRunnerError::ErrorModelInference);
        }

        DEBUG_PRINT("Max prediction: %d %s\nPredictions: ",
                    predictions->max_index,
                    predictions->labels[predictions->max_index]);
        for (size_t i = 0; i < predictions->num_labels; i++) {
            DEBUG_PRINT(" %s[%d]",
                        predictions->labels[i],
                        (int)(predictions->predictions[i] * 100));
        }
        DEBUG_PRINT("\n\n");

        MicroBitEvent evt(MlRunnerIds::MlRunnerInference, predictions->max_index + 1);
    }

    void recordAccData(MicroBitEvent) {
        if (!initialised) return;

        accData[accDataIndex++] = (float)(uBit.accelerometer.getX() / 1000.0f);
        accData[accDataIndex++] = (float)(uBit.accelerometer.getY() / 1000.0f);
        accData[accDataIndex++] = (float)(uBit.accelerometer.getZ() / 1000.0f);

        if (accDataIndex >= accDataSize) {
            accDataIndex = 0;
            // Stop firing timer events while running model and resume after
            uBit.messageBus.ignore(MlRunnerIds::MlRunnerTimer, ML_CODAL_TIMER_VALUE, &recordAccData);
            runModel();
            uBit.messageBus.listen(MlRunnerIds::MlRunnerTimer, ML_CODAL_TIMER_VALUE, &recordAccData, MESSAGE_BUS_LISTENER_DROP_IF_BUSY);
        }
    }

    /*************************************************************************/
    /* Exported functions                                                    */
    /*************************************************************************/
    //% blockId=mlrunner_init
    void init(Buffer model_str) {
#if MICROBIT_CODAL != 1
        target_panic(PANIC_VARIANT_NOT_SUPPORTED);
#endif
        if (initialised) return;

#if DEVICE_MLRUNNER_USE_EXAMPLE_MODEL == 1
        DEBUG_PRINT("Using example model... ");
        void *model_address = (void *)example_model;
#else
        DEBUG_PRINT("Using embedded model... ");
        if (model_str == NULL || model_str->length <= 0 || model_str->data == NULL) {
            uBit.panic(MlRunnerError::ErrorModelNotPresent);
        }
        void *model_address = (void *)model_str->data;
#endif

        const bool setModelSuccess = ml_setModel(model_address);
        if (!setModelSuccess) {
            uBit.panic(MlRunnerError::ErrorModelNotPresent);
        }

        const int inputLen = ml_getInputLength();
        if (inputLen <= 0) {
            uBit.panic(MlRunnerError::ErrorModelNotPresent);
        }
        if (inputLen % 3 != 0) {
            uBit.panic(MlRunnerError::ErrorInputLength);
        }

        // TODO: MakeCode/CODAL fails to allocate, temp quick hack
        //accData = (float *)malloc(accDataSize * sizeof(float));
        accData = accData2;
        if (accData == NULL) {
            uBit.panic(MlRunnerError::ErrorMemAlloc);
        }
        accDataSize = inputLen;

        // Set up background timer to collect data and run model
        uBit.messageBus.listen(MlRunnerIds::MlRunnerTimer, ML_CODAL_TIMER_VALUE, &recordAccData, MESSAGE_BUS_LISTENER_DROP_IF_BUSY);
        uBit.timer.eventEvery(ML_CODAL_TIMER_PERIOD, MlRunnerIds::MlRunnerTimer, ML_CODAL_TIMER_VALUE);

        initialised = true;

        DEBUG_PRINT("Model loaded\n");
    }

    //% blockId=mlrunner_stop_model_running
    void deInit() {
#if MICROBIT_CODAL != 1
        target_panic(PANIC_VARIANT_NOT_SUPPORTED);
#endif
        if (!initialised) {
            DEBUG_PRINT("Attempting to stop running ML, but was already stopped.\n");
            return;
        }
        DEBUG_PRINT("Stop running the ML model... ");

        // Stop timer event
        uBit.messageBus.ignore(MlRunnerIds::MlRunnerTimer, ML_CODAL_TIMER_VALUE, &recordAccData);
        uBit.timer.cancel(MlRunnerIds::MlRunnerTimer, ML_CODAL_TIMER_VALUE);

        if (accData != NULL) {
            // TODO: MakeCode/CODAL fails to allocate, temp quick hack
            //free(accData);
            accData = NULL;
        }
        accDataSize = 0;
        accDataIndex = 0;
        initialised = false;
        DEBUG_PRINT("Done\n");
    }

    //% blockId=mlrunner_input_length
    int inputLength() {
#if MICROBIT_CODAL
        return ml_getInputLength();
#else
        return DEVICE_NOT_SUPPORTED;
#endif
    }

    //% blockId=mlrunner_is_running
    bool isModelRunning() {
        return initialised;
    }
}
