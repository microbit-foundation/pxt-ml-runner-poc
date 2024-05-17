/**
 * @brief Functions to interact with the ML model.
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
#include <stdlib.h>
#include "model-example/model_example.h"
#include "ml4f/ml4f.h"
#include "mlrunner.h"

using namespace mlrunner;

// Linker symbols used to find the start of the model in flash.
extern uint8_t  __etext, __data_start__, __data_end__;

// Flag to control usage of model included in model_example.h/c
static bool USE_BUILT_IN = true;

/*****************************************************************************/
/* Private API                                                               */
/*****************************************************************************/
/**
 * @brief Get the start address of the full model (header + ML4F model).
 *
 * This would also be the start address of the model header with the labels.
 * This function does not check if the data in flash is a valid model.
 *
 * @return The start address to where the full model is stored in flash.
 */
static uint32_t get_full_model_start_address() {
    if (USE_BUILT_IN) {
        return (uint32_t)model_example;
    }
    // The last section in FLASH is meant to be text, but data section contents
    // are placed immediately after it (to be copied to RAM), but there isn't
    // a symbol to indicate its end in FLASH, so we calculate how long data is
    // to figure out the last byte stored in flash.
    int end_of_text_section = (int)(&__etext);
    int data_section_size = (int)(&__data_end__ - &__data_start__);
    int end_of_flash_data = end_of_text_section + data_section_size;

    // TODO: Detect the flash page instead of hardcoding it.
    static const int flash_page_size = 4096;

    // Find the start of the next page after the end of the flash data.
    return (end_of_flash_data + flash_page_size - 1) & ~(flash_page_size - 1);
}

/**
 * @brief Get a pointer to the full model header.
 *
 * @return The model header or NULL if the model is not present or invalid.
 */
static ml_model_header_t* get_model_header() {
    ml_model_header_t *model_header = (ml_model_header_t *)get_full_model_start_address();
    if (model_header->magic0 != MODEL_HEADER_MAGIC0) {
        return NULL;
    }
    // We should have at least one label
    if (model_header->number_of_labels == 0) {
        return NULL;
    }
    // Also check the ML4F header magic values to ensure it's there too
    ml4f_header_t *ml4f_model = (ml4f_header_t *)((uint32_t)model_header + model_header->model_offset);
    if (ml4f_model->magic0 != ML4F_MAGIC0 || ml4f_model->magic1 != ML4F_MAGIC1) {
        return NULL;
    }
    return model_header;
}

/**
 * @brief Get a pointer to the ML4F model.
 *
 * @return The ML4F model or NULL if the model is not present or invalid.
 */
static ml4f_header_t* get_ml4f_model() {
    // get_model_header() already checks the ML4F header magic values
    ml_model_header_t *model_header = get_model_header();
    if (model_header == NULL) {
        return NULL;
    }
    return (ml4f_header_t *)((uint32_t)model_header + model_header->model_offset);
}

/*****************************************************************************/
/* Public API                                                                */
/*****************************************************************************/
void mlrunner::ml_useBuiltInModel(bool use) {
    USE_BUILT_IN = use;
}

bool mlrunner::ml_isModelPresent() {
    ml_model_header_t *model_header = get_model_header();
    return model_header != NULL;
}

int mlrunner::ml_getInputLen() {
    ml4f_header_t *ml4f_model = get_ml4f_model();
    if (ml4f_model == NULL) {
        return 0;
    }
    return ml4f_shape_elements(ml4f_input_shape(ml4f_model));
}

ml_labels_t* mlrunner::ml_getLabels() {
    static ml_labels_t labels = {
        .num_labels = 0,
        .labels = NULL
    };

    const ml_model_header_t* const model_header = get_model_header();
    if (model_header == NULL) {
        labels.num_labels = 0;
        if (labels.labels != NULL) {
            free(labels.labels);
            labels.labels = NULL;
        }
        return NULL;
    }

    // Workout the addresses in flash from each label, there are as many strings
    // as indicated by model_header->number_of_labels, they start from address
    // model_header->labels and are null-terminated.
    uint32_t header_end = (uint32_t)model_header + model_header->header_size;
    const char* flash_labels[model_header->number_of_labels];
    flash_labels[0] = &model_header->labels[0];
    for (int i = 1; i < model_header->number_of_labels; i++) {
        // Find the end of the previous string by looking for the null terminator
        flash_labels[i] = flash_labels[i - 1];
        while (*flash_labels[i] != '\0' && (uint32_t)flash_labels[i] < header_end) {
            flash_labels[i]++;
        }
        if ((uint32_t)flash_labels[i] >= header_end) {
            // We reached the end of the header without finding the null terminator
            free(flash_labels);
            return NULL;
        }
        // Currently pointing to the null terminator, so point to the following string
        flash_labels[i]++;
    }
    // Check the last string is null terminated at the end of header
    if (*(char *)(header_end - 1) != '\0') {
        free(flash_labels);
        return NULL;
    }

    // First check if the labels are the same, if not we need to set them again
    bool set_labels = false;
    if (labels.num_labels == 0 || labels.labels == NULL) {
        set_labels = true;
    } else if (labels.num_labels != model_header->number_of_labels) {
        set_labels = true;
    } else {
        for (size_t i = 0; i < labels.num_labels; i++) {
            if (labels.labels[i] != flash_labels[i]) {
                set_labels = true;
                break;
            }
        }
    }
    if (set_labels) {
        // First clear them out if needed
        labels.num_labels = 0;
        if (labels.labels != NULL) {
            free(labels.labels);
        }
        // Then set them to point to the strings in flash
        labels.labels = (const char **)malloc(model_header->number_of_labels * sizeof(char *));
        if (labels.labels == NULL) {
            return NULL;
        }
        labels.num_labels = model_header->number_of_labels;
        for (size_t i = 0; i < labels.num_labels; i++) {
            labels.labels[i] = flash_labels[i];
        }
    }

    return &labels;
}

ml_prediction_t* mlrunner::ml_predict(const float *input) {
    static ml_prediction_t predictions = {
        .max_index = 0,
        .num_labels = 0,
        .labels = NULL,
        .predictions = NULL,
    };

    ml_labels_t* labels = ml_getLabels();
    if (labels == NULL) {
        return NULL;
    }

    // Check if we need to resize the predictions array
    if (predictions.num_labels != labels->num_labels) {
        if (predictions.predictions != NULL) {
            free(predictions.predictions);
        }
        predictions.num_labels = labels->num_labels;
        predictions.predictions = (float *)malloc(predictions.num_labels * sizeof(float));
        if (predictions.predictions == NULL) {
            predictions.num_labels = 0;
            return NULL;
        }
    }
    // Always update the labels in case they changed
    predictions.labels = labels->labels;

    ml4f_header_t* ml4f_model = get_ml4f_model();
    int r = ml4f_full_invoke(ml4f_model, input, predictions.predictions);
    if (r != 0) {
        return NULL;
    }

    predictions.max_index = ml4f_argmax(predictions.predictions, predictions.num_labels);

    return &predictions;
}
