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

// To represent the accelerometer data x, y, z
static const int DATA_AXIS = 3;

static float *accData = NULL;
static int accDataSize = 0;
static int accDataIndex = 0;

static void mlDataProcessorExample_deinit() {
    free(accData);
    accData = NULL;
    accDataSize = 0;
    accDataIndex = 0;
}

static bool mlDataProcessorExample_init(const int samples, const int outputLength) {
    if (samples <= 0 || outputLength <= 0) {
        return false;
    }
    if (accData != NULL) {
        mlDataProcessorExample_deinit();
    }
    accDataSize = samples * DATA_AXIS;
    accData = (float*)malloc(accDataSize * sizeof(float));
    if (accData == NULL) {
        return false;
    }
    accDataIndex = 0;
    return true;
}

static void mlDataProcessorExample_recordAccData(const int x, const int y, const int z) {
    if (accData == NULL) return;

    accData[accDataIndex++] = x / 1000.0f;
    accData[accDataIndex++] = y / 1000.0f;
    accData[accDataIndex++] = z / 1000.0f;
    if (accDataIndex >= accDataSize) {
        accDataIndex = 0;
    }
}

static bool mlDataProcessorExample_isDataReady() {
    if (accData == NULL) return false;
    return accDataIndex == 0;
}

static float* mlDataProcessorExample_getModelInputData() {
    return accData;
}

MlDataProcessor_t mlDataProcessor = {
    .init = mlDataProcessorExample_init,
    .deinit = mlDataProcessorExample_deinit,
    .recordAccData = mlDataProcessorExample_recordAccData,
    .isDataReady = mlDataProcessorExample_isDataReady,
    .getModelInputData = mlDataProcessorExample_getModelInputData,
};

#endif // DEVICE_MLRUNNER_USE_EXAMPLE_MODEL
