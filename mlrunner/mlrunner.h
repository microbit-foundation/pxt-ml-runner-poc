#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
namespace mlrunner {
extern "C" {
#endif

// ASCII for "MODL"
const uint32_t MODEL_HEADER_MAGIC0 = 0x4D4F444C;

typedef struct ml_model_header_t {
    uint32_t magic0;
    uint16_t header_size;      // Size of this header + all label strings
    uint16_t model_offset;     // header_size + padding to 4 bytes
    uint8_t reserved[7];
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
 * @brief Select the Built-in model or the model added to a flash region.
 * 
 * Built-in model is compiled with MicroPython as an array of bytes.
 * The model added to a flash region is added by the coding editors.
 *
 * @param use True to use the built-in model, False to use the flash section.
 */
void ml_useBuiltInModel(bool use);

/**
 * @brief Check if a model is present.
 *
 * @return True if a model is present, False otherwise.
 */
bool ml_isModelPresent();

/**
 * @brief Get the input length of the model.
 *
 * @return The number of input elements required for the model.
 */
//% blockId=mlrunner_input_length
int ml_getInputLen();

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
}  // namespace mlrunner
#endif
