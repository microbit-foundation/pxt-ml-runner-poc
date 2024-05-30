/**
 * @brief C Object to encapsulate basic functions needed to process data for an
 * ML model to run, and functions to filter data in preparation for some models.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool (*init)(const int samples);
    void (*deinit)(void);
    void (*recordAccData)(const int x, const int y, const int z);
    bool (*isDataReady)(void);
    float* (*getModelInputData)(void);
} MlDataProcessor_t;

extern MlDataProcessor_t mlDataProcessor;

float filterMax(float *data, int size);
float filterMin(float *data, int size);
float filterMean(float *data, int size);
float filterStdDev(float *data, int size);
float filterPeaks(float *data, int size);
float filterTotalAcc(float *data, int size);
float filterZcr(float *data, int size);
float filterRms(float *data, int size);

#ifdef __cplusplus
}
#endif
