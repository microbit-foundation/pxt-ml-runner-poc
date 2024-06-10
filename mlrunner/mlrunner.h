/**
 * @brief Functions to interact with the ML4F model.
 *
 * @copyright
 * Copyright 2024 Micro:bit Educational Foundation.
 * SPDX-License-Identifier: MIT
 *
 * @details
 * The ML4F model has its own header, but does not include the labels.
 * So an extra header with the labels and other information is added on top.
 * This header start and end are 4-byte aligned, with padding zeros at the
 * end if needed, so that the ML4F model is placed directly after it.
 * We call the "full model" the custom header + the ML4F model.
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

/**
 * The ML header contains a series of actions, each with a threshold and label.
 * The label is of variable length, and inside the header these instances are
 * padded to end with 4-byte alignment.
 */
typedef struct __attribute__((packed)) ml_header_action_s {
    const float threshold;              // Min threshold for this action to be active
    const uint8_t label_length;         // Length of the label string including the null terminator
    const char label[];                 // Null-terminated string for the label starts from this address
} ml_header_action_t;
const size_t ml_action_size_without_label = 5;

/**
 * The ML model header presence can be checked via the magic number.
 * The actions are padded with zeros for 4-byte alignment.
 */
typedef struct __attribute__((packed, aligned(4))) ml_model_header_s {
    const uint32_t magic0;
    const uint16_t header_size;         // Size of this header including all actions, padded to 4 bytes
    const uint16_t samples_period;      // Period in ms between samples
    const uint16_t samples_length;      // Number of samples used per inference, not counting dimensions
    const uint8_t sample_dimensions;    // Number of dimensions per sample, e.g. 3 for accelerometer data
    const uint8_t reserved[8];
    const uint8_t number_of_actions;    // Only 255 actions supported
    const ml_header_action_t actions[]; // As many actions as number_of_actions, the size of each is variable
} ml_model_header_t;

typedef struct ml_action_s {
    float threshold;
    const char *label;
} ml_action_t;

typedef struct ml_actions_s {
    size_t len;
    ml_action_t action[];
} ml_actions_t;

typedef struct ml_labels_s {
    size_t num_labels;
    const char **labels;
} ml_labels_t;

typedef struct ml_predictions_s {
    int index;
    size_t len;
    float prediction[];
} ml_predictions_t;

/**
 * @brief Set the model to use for inference.
 *
 * @param model_address The start address of the model.
 * @return True if the model is valid and set, False otherwise.
 */
bool ml_setModel(const void *model_address);

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
 * @brief Get the output length of the model.
 *
 * @return The number of output elements produced by the model.
 *         Or -1 if the model is not present.
 */
int ml_getOutputLength();

/**
 * @brief Get the model labels.
 *
 * The label pointers point directly to the strings stored in flash.
 *
 * @return A pointer to a ml_labels_t object containing the labels.
 */
ml_labels_t* ml_getLabels();

/**
 * @brief Allocate memory for the model actions.
 *
 * The caller is responsible for freeing the memory.
 *
 * @return A pointer to a ml_actions_t object to store the actions.
 */
ml_actions_t* ml_allocateActions();

/**
 * @brief Get the model actions.
 *
 * The actions_out->action[].label pointers point directly to the strings
 * stored in flash.
 *
 * @param actions_out A pointer to a ml_actions_t object to store the actions.
 * @return Success state, true if the actions were successfully retrieved,
 *         false otherwise.
 *         If false is returned, the actions_out is partially filled and it
 *         should not be used.
 */
bool ml_getActions(ml_actions_t *actions_out);

/**
 * @brief Allocate memory for the model predictions.
 *
 * The caller is responsible for freeing the memory.
 *
 * @return A pointer to a ml_predictions_t object to store the predictions.
 */
ml_predictions_t *ml_allocatePredictions();

/**
 * @brief Run the model and return the index for the predicted action.
 *
 * @param actions The actions to use for the prediction.
 * @param input The input data for the model.
 * @param in_len The length of the input data.
 * @return The index of the predicted action.
 *        Or -1 if the model is not present, the actions or input length
 *        doesn't match, or the prediction failed.
 */
bool ml_predict(const float *input, const int in_len, const ml_actions_t *actions, ml_predictions_t *predictions_out);

/**
 * @brief Run the model and return the individual predictions for each action.
 *
 * @param input The input data for the model.
 * @param in_len The length of the input data.
 * @param predictions_out An array of floats to store the results.
 * @param out_len The length of the predictions_out array.
 * @return True if the model is present and the model run was successful,
 *         False otherwise.
 */
bool ml_runModel(const float *input, const int in_len, float* predictions_out, const int out_len);

/**
 * @brief Calculate the overall prediction based on the actions thresholds.
 *
 * @param actions The actions to use for the prediction, which include
 *                their individual thresholds.
 * @param predictions The predictions for each individual action.
 * @param len The length of the predictions array.
 * @return The index of the predicted action.
 *        Or -1 if the model is not present, the actions or predictions length
 *        doesn't match, or the prediction failed.
 */
int ml_calcPrediction(const ml_actions_t *actions, const float* predictions, const int len);

#ifdef __cplusplus
}  // extern "C"
#endif
