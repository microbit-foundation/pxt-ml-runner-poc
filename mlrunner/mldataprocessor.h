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

// enum for return types
typedef enum {
    MLDP_SUCCESS = 0,
    MLDP_ERROR = -1,
    MLDP_ERROR_CONFIG = -2,
    MLDP_ERROR_ALLOC = -3,
    MLDP_ERROR_NOINIT = -4,
} MldpReturn_t;

typedef struct {
    const int out_size;
    MldpReturn_t (*filter)(const float *data_in, const int in_size, float *data_out, const int out_size);
} MlDataFilters_t;

typedef struct {
    const int samples;
    const int dimensions;
    const int output_length;
    const int filter_size;
    const MlDataFilters_t *filters;
} MlDataProcessorConfig_t;

typedef struct {
    MldpReturn_t (*init)(const MlDataProcessorConfig_t *config);
    void (*deinit)(void);
    MldpReturn_t (*recordData)(const float *samples, const int elements);
    bool (*isDataReady)(void);
    float* (*getProcessedData)(void);
} MlDataProcessor_t;

extern MlDataProcessor_t mlDataProcessor;

MldpReturn_t filterMax(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterMin(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterMean(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterStdDev(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterPeaks(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterTotalAcc(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterZcr(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterRms(const float *data_in, const int in_size, float *data_out, const int out_size);
MldpReturn_t filterPassThrough(const float *data_in, const int in_size, float *data_out, const int out_size);

#ifdef __cplusplus
}
#endif
