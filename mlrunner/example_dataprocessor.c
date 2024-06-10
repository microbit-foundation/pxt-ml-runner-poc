/**
 * @brief Data Processor for the Model Example 2 included in mlrunner.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 *
 * TODO: Need to double buffer this so that a model can be run while the
 * next data is being collected.
 * TODO: Need to make isDataReady() more robust as right now it needs to be
 * called after each recordData() call.
 */
#include "mldataprocessor.h"

#if DEVICE_MLRUNNER_USE_EXAMPLE_PROCESSOR > 0

static float *accData = NULL;
static int accDimensions = 0;
static int accDataSize = 0;
static int accDataIndex = 0;


static MldpReturn_t exampleDataProcessor_init(const MlDataProcessorConfig_t* config);
static void exampleDataProcessor_deinit();
static MldpReturn_t exampleDataProcessor_recordData(const float *sample, const int sample_dimensions);
static bool exampleDataProcessor_isDataReady();
static float* exampleDataProcessor_getProcessedData();


MldpReturn_t exampleDataProcessor_init(const MlDataProcessorConfig_t* config) {
    if (config->samples <= 0 || config->dimensions <= 0 || config->output_length <= 0) {
        exampleDataProcessor_deinit();
        return MLDP_ERROR_CONFIG;
    }
    if (accData != NULL) {
        exampleDataProcessor_deinit();
    }

    accDataIndex = 0;
    accDimensions = config->dimensions;
    accDataSize = config->samples * config->dimensions;

    if (config->output_length != accDataSize) {
        exampleDataProcessor_deinit();
        return MLDP_ERROR_CONFIG;
    }

    accData = (float*)malloc(accDataSize * sizeof(float));
    if (accData == NULL) {
        exampleDataProcessor_deinit();
        return MLDP_ERROR_ALLOC;
    }

    return MLDP_SUCCESS;
}

void exampleDataProcessor_deinit() {
    free(accData);
    accData = NULL;
    accDimensions = 0;
    accDataSize = 0;
    accDataIndex = 0;
}

MldpReturn_t exampleDataProcessor_recordData(const float* samples, const int elements) {
    if (accData == NULL) return false;
    if (elements != accDimensions) return false;

    for (int i = 0; i < accDimensions; i++) {
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

size_t exampleDataProcessor_getProcessedDataSize() {
    return accDataSize;
}

MlDataProcessor_t mlDataProcessor = {
    .init = exampleDataProcessor_init,
    .deinit = exampleDataProcessor_deinit,
    .recordData = exampleDataProcessor_recordData,
    .isDataReady = exampleDataProcessor_isDataReady,
    .getProcessedData = exampleDataProcessor_getProcessedData,
    .getProcessedDataSize = exampleDataProcessor_getProcessedDataSize,
};

#endif // DEVICE_MLRUNNER_USE_EXAMPLE_PROCESSOR
