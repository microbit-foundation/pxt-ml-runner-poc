/**
 * @brief Data Processor for the Model Example included in mlrunner.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 *
 * TODO: Need to double buffer this so that a model can be run while the
 * next data is being collected.
 * TODO: Need to make isDataReady() more robust as right now it needs to be
 * called after each recordAccData() call.
 */
#include "mldataprocessor.h"

#if DEVICE_MLRUNNER_USE_EXAMPLE_MODEL == 2

static float *accData = NULL;
static int accDimensions = 0;
static int accDataSize = 0;
static int accDataIndex = 0;


static MldpReturn_t exampleDataProcessor_init(const MlDataProcessorConfig_t* config);
static void exampleDataProcessor_deinit();
static MldpReturn_t exampleDataProcessor_recordAccData(const float *sample, const int sample_dimensions);
static bool exampleDataProcessor_isDataReady();
static float* exampleDataProcessor_getProcessedData();


MldpReturn_t exampleDataProcessor_init(const int samples, const int dimensions, const int output_length) {
    if (samples <= 0 || dimensions <= 0 || outputLength <= 0) {
        exampleDataProcessor_deinit();
        return MLDP_ERROR_CONFIG;
    }
    if (accData != NULL) {
        exampleDataProcessor_deinit();
    }
    accDataSize = samples * dimensions;
    accData = (float*)malloc(accDataSize * sizeof(float));
    if (accData == NULL) {
        exampleDataProcessor_deinit();
        return MLDP_ERROR_ALLOC;
    }
    accDimensions = dimensions;
    accDataIndex = 0;
    return MLDP_SUCCESS;
}

void exampleDataProcessor_deinit() {
    free(accData);
    accData = NULL;
    accDimensions = 0;
    accDataSize = 0;
    accDataIndex = 0;
}

MldpReturn_t exampleDataProcessor_recordAccData(const float* sample, const int dimensions) {
    if (accData == NULL) return false;
    if (dimensions != accDimensions) return false;

    for (int i = 0; i < dimensions; i++) {
        accData[accDataIndex++] = sample[i];
    }
    if (accDataIndex >= accDataSize) {
        accDataIndex = 0;
    }
    return MLDP_SUCCESS;
}

bool exampleDataProcessor_isDataReady() {
    if (accData == NULL) return false;
    return accDataIndex == 0;
}

float* exampleDataProcessor_getProcessedData() {
    return accData;
}

MlDataProcessor_t mlDataProcessor = {
    .init = exampleDataProcessor_init,
    .deinit = exampleDataProcessor_deinit,
    .recordAccData = exampleDataProcessor_recordAccData,
    .isDataReady = exampleDataProcessor_isDataReady,
    .getProcessedData = exampleDataProcessor_getProcessedData,
};

#endif // DEVICE_MLRUNNER_USE_EXAMPLE_MODEL
