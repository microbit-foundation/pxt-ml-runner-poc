/**
 * @brief Data Processor to apply a collection of filters to the input data.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 *
 * TODO: Need to double buffer this so that a model can be run while the
 * next data is being collected.
 */
#include <string.h>
#include "mldataprocessor.h"


static float **input_samples = NULL;
static int sample_dimensions = 0;
static int sample_length = 0;
static int sample_index = 0;
static float *output_data = NULL;
static int output_length = 0;
static MlDataFilters_t *filters = NULL;
static int filter_size = 0;
static bool initialised = false;


static MldpReturn_t filterDataProcessor_init(const MlDataProcessorConfig_t* config);
static void filterDataProcessor_deinit();
static MldpReturn_t filterDataProcessor_recordData(const float *samples, const int elements);
static bool filterDataProcessor_isDataReady();
static float* filterDataProcessor_getProcessedData();


MldpReturn_t filterDataProcessor_init(const MlDataProcessorConfig_t* config) {
    if (config->samples <= 0 || config->dimensions <= 0 || config->output_length <= 0) {
        filterDataProcessor_deinit();
        return MLDP_ERROR_CONFIG;
    }

    // The output size will depend on output size per filter and number of dimensions
    int total_output = 0;
    for (int i = 0; i < config->filter_size; i++) {
        total_output += config->filters[i].out_size * config->dimensions;
    }
    if (config->output_length != total_output) {
        filterDataProcessor_deinit();
        return MLDP_ERROR_CONFIG;
    }

    if (initialised) {
        filterDataProcessor_deinit();
    }

    filters = (MlDataFilters_t*)malloc(config->filter_size * sizeof(MlDataFilters_t));
    output_data = (float*)malloc(config->output_length * sizeof(float));
    input_samples = (float**)calloc(config->dimensions, sizeof(float*));
    if (filters == NULL || output_data == NULL || input_samples == NULL) {
        filterDataProcessor_deinit();
        return MLDP_ERROR_ALLOC;
    }

    // Allocate for each sample dimension
    sample_dimensions = config->dimensions;
    for (int i = 0; i < sample_dimensions; i++) {
        input_samples[i] = (float*)malloc(config->samples * sizeof(float));
        if (input_samples[i] == NULL) {
            filterDataProcessor_deinit();
            return MLDP_ERROR_ALLOC;
        }
    }

    // Copy the filter pointers
    memcpy(filters, config->filters, config->filter_size * sizeof(MlDataFilters_t));

    filter_size = config->filter_size;
    output_length = config->output_length;
    sample_length = config->samples;
    sample_index = 0;

    initialised = true;
    return MLDP_SUCCESS;
}

void filterDataProcessor_deinit() {
    initialised = false;
    for (int i = 0; i < sample_dimensions; i++) {
        free(input_samples[i]);
    }
    free(input_samples);
    free(output_data);
    free(filters);
    input_samples = NULL;
    output_data = NULL;
    filters = NULL;
    filter_size = 0;
    output_length = 0;
    sample_dimensions = 0;
    sample_length = 0;
    sample_index = 0;
}

MldpReturn_t filterDataProcessor_recordData(const float* samples, const int elements) {
    if (!initialised) return MLDP_ERROR_NOINIT;
    // Only record data if the number of elements is a multiple of the sample dimensions
    if (elements % sample_dimensions != 0) return MLDP_ERROR_CONFIG;

    int number_of_samples = elements / sample_dimensions;
    for (int s_i = 0; s_i < number_of_samples; s_i++) {
        for (int d_i = 0; d_i < sample_dimensions; d_i++) {
            input_samples[d_i][sample_index + s_i] = samples[s_i * sample_dimensions + d_i];
        }
        sample_index++;
        if (sample_index >= sample_length) {
            sample_index = 0;
        }
    }

    return MLDP_SUCCESS;
}

bool filterDataProcessor_isDataReady() {
    if (!initialised) return false;

    return sample_index == 0;
}

float* filterDataProcessor_getProcessedData() {
    if (!initialised) return NULL;

    // Run all filters and save their output to output_data
    int output_i = 0;
    for (int filter_i = 0; filter_i < filter_size; filter_i++) {
        for (int dimension_i = 0; dimension_i < sample_dimensions; dimension_i++) {
            MldpReturn_t filter_result = filters[filter_i].filter(
                input_samples[dimension_i], sample_length,
                &output_data[output_i], filters[filter_i].out_size
            );
            if (filter_result != MLDP_SUCCESS) {
                return NULL;
            }
            output_i += filters[filter_i].out_size;
        }
    }
    return output_data;
}

size_t filterDataProcessor_getProcessedDataSize() {
    if (!initialised) return 0;

    return output_length;
}

MlDataProcessor_t mlDataProcessor = {
    .init = filterDataProcessor_init,
    .deinit = filterDataProcessor_deinit,
    .recordData = filterDataProcessor_recordData,
    .isDataReady = filterDataProcessor_isDataReady,
    .getProcessedData = filterDataProcessor_getProcessedData,
    .getProcessedDataSize = filterDataProcessor_getProcessedDataSize,
};
