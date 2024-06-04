/**
 * @brief Functions to interact with the ML4F model.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 *
 * @details
 * The ML4F model has its own header, but does not include the labels.
 * So an extra header with the labels is added on top.
 * We call the "full model" the labels header + the ML4F model.
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ASCII for "MODL"
#define MODEL_HEADER_MAGIC0 0x4D4F444C

typedef struct ml_model_header_t {
    uint32_t magic0;
    uint16_t header_size;      // Size of this header + all label strings
    uint16_t model_offset;     // header_size + padding for 4-byte alignment
    uint16_t samples_period;    // Period in ms between samples
    uint16_t samples_length;   // Number of samples used per inference, not counting dimensions
    uint8_t sample_dimensions; // Number of dimensions per sample, e.g. 3 for accelerometer data
    uint8_t reserved[6];
    uint8_t number_of_labels;  // Only 255 labels supported
    char labels[];             // Mutiple null-terminated strings, as many as number_of_labels
} ml_model_header_t;

typedef struct ml_labels_s {
    size_t num_labels;
    const char **labels;
} ml_labels_t;

typedef struct ml_prediction_s {
    size_t max_index;
    size_t num_labels;
    const char **labels;
    float *predictions;
} ml_prediction_t;

/**
 * @brief Set the model to use for inference.
 *
 * @param model_address The start address of the model.
 * @return True if the model is valid and set, False otherwise.
 */
bool ml_setModel(void *model_address);

/**
 * @brief Check if a model is present.
 *
 * @return True if a model is present, False otherwise.
 */
bool ml_isModelPresent();

/**
 * @brief Get the period between samples required for the model.
 *
 * @return The period between samples required for the model.
 *         Or -1 if the model is not present.
 */
int ml_getSamplesPeriod();

/**
 * @brief Get the number of samples required for the model.
 *
 * @return The number of samples required for the model.
 *         Or -1 if the model is not present.
 */
int ml_getSamplesLength();

/**
 * @brief Get the number of dimensions per sample required for the model.
 *
 * @return The number of dimensions per sample required for the model.
 *         Or -1 if the model is not present.
 */
int ml_getSampleDimensions();

/**
 * @brief Get the input length of the model.
 *
 * @return The number of input elements required for the model.
 *         Or -1 if the model is not present.
 */
int ml_getInputLength();

/**
 * @brief Get the model labels.
 *
 * The label pointers point directly to the strings stored in flash.
 *
 * @return A pointer to a ml_labels_t object containing the labels.
 */
ml_labels_t* ml_getLabels();

/**
 * @brief Run the model inference and return the output predictions.
 *
 * @param input The input data for the model.
 * @return A pointer to a ml_prediction_t object containing the predictions.
 */
ml_prediction_t* ml_predict(const float *input);

#ifdef __cplusplus
}  // extern "C"
#endif
