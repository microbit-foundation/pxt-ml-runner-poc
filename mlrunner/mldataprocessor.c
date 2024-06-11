/**
 * Functions to filter data in preparation for some ML models.
 *
 * Adapted from:
 *   https://github.com/microbit-foundation/ml-trainer/blob/v0.6.0/src/script/datafunctions.ts
 *   (c) 2023, Center for Computational Thinking and Design at Aarhus University and contributors
 *   SPDX-License-Identifier: MIT
 */
#include <math.h>
#include <string.h>
#include "mldataprocessor.h"

MldpReturn_t filterMax(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 1 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    float max = data_in[0];
    for (int i = 1; i < in_size; i++) {
        if (data_in[i] > max) {
            max = data_in[i];
        }
    }
    *data_out = max;

    return MLDP_SUCCESS;
}

MldpReturn_t filterMin(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 1 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    float min = data_in[0];
    for (int i = 1; i < in_size; i++) {
        if (data_in[i] < min) {
            min = data_in[i];
        }
    }
    *data_out = min;

    return MLDP_SUCCESS;
}

MldpReturn_t filterMean(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 1 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    float sum = 0;
    for (int i = 0; i < in_size; i++) {
        sum += data_in[i];
    }
    *data_out = sum / in_size;

    return MLDP_SUCCESS;
}

// Standard Deviation
MldpReturn_t filterStdDev(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 1 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    float mean;
    MldpReturn_t mean_result = filterMean(data_in, in_size, &mean, 1);
    if (mean_result != MLDP_SUCCESS) {
        return mean_result;
    }

    float std = 0;
    float f = 0;
    for (int i = 0; i < in_size; i++) {
        f = data_in[i] - mean;
        std += f * f;
    }
    std /= in_size;
    *data_out = sqrtf(std);

    return MLDP_SUCCESS;
}

// Count the number of peaks
// Warning! This can allocate 5x the in_size of the data in the stack
// so ensure DEVICE_STACK_SIZE is appropriately set
// TODO: Move to the heap, pxt automatically uses its allocator
MldpReturn_t filterPeaks(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 5 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    const int lag = 5;
    const float threshold = 3.5;
    const float influence = 0.5;
    int peaksCounter = 0;

    float signals[in_size];
    float filteredY[in_size];
    float lead_in[lag];
    float avgFilter[in_size];
    float stdFilter[in_size];
    memset(signals, 0, in_size * sizeof(float));
    memcpy(filteredY, data_in, in_size * sizeof(float));
    memcpy(lead_in, data_in, lag * sizeof(float));

    float mean_lag, stdDev_lag;
    MldpReturn_t mean_result = filterMean(lead_in, lag, &mean_lag, 1);
    MldpReturn_t stdDev_result = filterStdDev(lead_in, lag, &stdDev_lag, 1);
    if (stdDev_result != MLDP_SUCCESS || mean_result != MLDP_SUCCESS) {
        return MLDP_ERROR_CONFIG;
    }

    avgFilter[lag - 1] = mean_lag;
    stdFilter[lag - 1] = stdDev_lag;

    for (int i = lag; i < in_size; i++) {
        if (fabsf(data_in[i] - avgFilter[i - 1]) > 0.1f &&
            fabsf(data_in[i] - avgFilter[i - 1]) > threshold * stdFilter[i - 1]
        ) {
            if (data_in[i] > avgFilter[i - 1]) {
                signals[i] = +1; // positive signal
                if (i - 1 > 0 && signals[i - 1] == 0) {
                    peaksCounter++;
                }
            } else {
                signals[i] = -1; // negative signal
            }
            // make influence lower
            filteredY[i] = influence * data_in[i] + (1.0f - influence) * filteredY[i - 1];
        } else {
            signals[i] = 0; // no signal
            filteredY[i] = data_in[i];
        }

        // adjust the filters
        float y_lag[lag];
        memcpy(y_lag, &filteredY[i - lag], lag * sizeof(float));
        filterMean(y_lag, lag, &mean_lag, 1);
        filterStdDev(y_lag, lag, &stdDev_lag, 1);
        avgFilter[i] = mean_lag;
        stdFilter[i] = stdDev_lag;
    }
    *data_out = peaksCounter;

    return MLDP_SUCCESS;
}

// Total Absolute Acceleration
MldpReturn_t filterTotalAcc(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 1 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    float total = 0;
    for (int i = 0; i < in_size; i++) {
        total += fabsf(data_in[i]);
    }
    *data_out = total;

    return MLDP_SUCCESS;
}

// Zero Crossing Rate
MldpReturn_t filterZcr(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 2 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    int count = 0;
    for (int i = 1; i < in_size; i++) {
        if ((data_in[i] >= 0 && data_in[i - 1] < 0) ||
            (data_in[i] < 0 && data_in[i - 1] >= 0)) {
            count++;
        }
    }
    *data_out = count / (in_size - 1);

    return MLDP_SUCCESS;
}

// Root Mean Square
MldpReturn_t filterRms(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size < 1 || out_size != 1) {
        return MLDP_ERROR_CONFIG;
    }

    float rms = 0;
    for (int i = 0; i < in_size; i++) {
        rms += data_in[i] * data_in[i];
    }
    *data_out = sqrtf(rms / in_size);

    return MLDP_SUCCESS;
}

MldpReturn_t filterPassThrough(const float *data_in, const int in_size, float *data_out, const int out_size) {
    if (in_size > out_size) {
        return MLDP_ERROR_CONFIG;
    }

    memcpy(data_out, data_in, in_size * sizeof(float));

    return MLDP_SUCCESS;
}
