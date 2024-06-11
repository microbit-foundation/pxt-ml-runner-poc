
#include <stdlib.h>
#include "ml4f.h"
#include "mlrunner.h"

// Pointer to the model in flash
static uint32_t *MODEL_ADDRESS = NULL;
static uint8_t *model_arena = NULL;
static size_t input_length = 0;
static size_t output_length = 0;

/*****************************************************************************/
/* Private API                                                               */
/*****************************************************************************/
/**
 * @return True if the model header is valid, False otherwise.
 */
static bool is_model_valid(const void *model_address) {
    ml_model_header_t *model_header = (ml_model_header_t *)model_address;
    if (model_header->magic0 != MODEL_HEADER_MAGIC0) {
        return false;
    }
    // We should have at least one label, and samples period, length and dimensions
    if (
        model_header->samples_period == 0 ||
        model_header->samples_length == 0 ||
        model_header->sample_dimensions == 0 ||
        model_header->number_of_actions == 0
    ) {
        return false;
    }
    // Also check the ML4F header magic values to ensure it's there too
    ml4f_header_t *ml4f_model = (ml4f_header_t *)((uint32_t)model_header + model_header->header_size);
    if (ml4f_model->magic0 != ML4F_MAGIC0 || ml4f_model->magic1 != ML4F_MAGIC1) {
        return false;
    }
    return true;
}

/**
 * @brief Get a pointer to the ML4F model.
 *
 * @return The ML4F model or NULL if the model is not present or invalid.
 */
static inline ml4f_header_t* get_ml4f_model() {
    if (MODEL_ADDRESS == NULL) {
        return NULL;
    }
    ml_model_header_t *model_header = (ml_model_header_t *)MODEL_ADDRESS;
    return (ml4f_header_t *)((uint32_t)model_header + model_header->header_size);
}

/*****************************************************************************/
/* Public API                                                                */
/*****************************************************************************/
bool ml_setModel(const void *model_address) {
    // Check if the model is valid
    if (!is_model_valid(model_address)) {
        return false;
    }
    MODEL_ADDRESS = (uint32_t *)model_address;

    // Allocate the model arena
    int model_arena_size = ml_getArenaSize();
    if (model_arena_size <= 0) {
        MODEL_ADDRESS = NULL;
        return false;
    }
    if (model_arena != NULL) {
        free(model_arena);
    }
    model_arena = malloc(model_arena_size);
    if (model_arena == NULL) {
        MODEL_ADDRESS = NULL;
        return false;
    }

    // Set the cached input and output lengths
    ml_getInputLength();
    ml_getOutputLength();

    return true;
}

bool ml_isModelPresent() {
    return MODEL_ADDRESS != NULL;
}

int ml_getArenaSize() {
    ml4f_header_t *ml4f_model = get_ml4f_model();
    if (ml4f_model == NULL) {
        return -1;
    }
    return ml4f_model->arena_bytes;
}

int ml_getSamplesPeriod() {
    const ml_model_header_t* const model_header = (ml_model_header_t*)MODEL_ADDRESS;
    if (model_header == NULL) {
        return -1;
    }
    return model_header->samples_period;
}

int ml_getSamplesLength() {
    const ml_model_header_t* const model_header = (ml_model_header_t*)MODEL_ADDRESS;
    if (model_header == NULL) {
        return -1;
    }
    return model_header->samples_length;
}

int ml_getSampleDimensions() {
    const ml_model_header_t* const model_header = (ml_model_header_t*)MODEL_ADDRESS;
    if (model_header == NULL) {
        return -1;
    }
    return model_header->sample_dimensions;
}

int ml_getInputLength() {
    if (input_length == 0) {
        ml4f_header_t *ml4f_model = get_ml4f_model();
        if (ml4f_model == NULL) {
            return -1;
        }
        input_length = ml4f_shape_elements(ml4f_input_shape(ml4f_model));
    }

    return input_length;
}

int ml_getOutputLength() {
    if (output_length == 0) {
        ml4f_header_t *ml4f_model = get_ml4f_model();
        if (ml4f_model == NULL) {
            return -1;
        }
        output_length = ml4f_shape_elements(ml4f_output_shape(ml4f_model));
    }

    return output_length;
}

// TODO: Remove this function and use ml_getLabels instead
ml_labels_t* ml_getLabels() {
    static ml_labels_t labels = {
        .num_labels = 0,
        .labels = NULL
    };

    const ml_model_header_t* const model_header = (ml_model_header_t*)MODEL_ADDRESS;
    if (model_header == NULL) {
        labels.num_labels = 0;
        if (labels.labels != NULL) {
            free(labels.labels);
            labels.labels = NULL;
        }
        return NULL;
    }

    // Workout the addresses in flash from each label, there are as many strings
    // as indicated by model_header->number_of_actions
    const char* flash_labels[model_header->number_of_actions];
    ml_header_action_t *action = (ml_header_action_t *)&model_header->actions[0];
    for (int i = 0; i < model_header->number_of_actions; i++) {
        flash_labels[i] = &action->label[0];
        // Check the label has a single null terminator at the end
        for (int j = 0; j < action->label_length - 1; j++) {
            if (flash_labels[i][j] == '\0') {
                return NULL;
            }
        }
        // And check the last character is a null terminator
        if (flash_labels[i][action->label_length - 1] != '\0') {
            return NULL;
        }
        action = (ml_header_action_t *)((uint32_t)action + ml_action_size_without_label + action->label_length);
        // Next action address is 4 byte aligned
        action = (ml_header_action_t *)(((uint32_t)action + 3) & ~3);
    }

    // First check if the labels are the same, if not we need to set them again
    bool set_labels = false;
    if (labels.num_labels == 0 || labels.labels == NULL) {
        set_labels = true;
    } else if (labels.num_labels != model_header->number_of_actions) {
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
        labels.labels = (const char **)malloc(model_header->number_of_actions * sizeof(char *));
        if (labels.labels == NULL) {
            return NULL;
        }
        labels.num_labels = model_header->number_of_actions;
        for (size_t i = 0; i < labels.num_labels; i++) {
            labels.labels[i] = flash_labels[i];
        }
    }

    return &labels;
}

ml_actions_t* ml_allocateActions() {
    const ml_model_header_t* const model_header = (ml_model_header_t*)MODEL_ADDRESS;
    if (model_header == NULL) {
        return NULL;
    }

    ml_actions_t *actions = (ml_actions_t *)malloc(
            sizeof(ml_actions_t) + sizeof(ml_action_t) * model_header->number_of_actions);
    if (actions == NULL) {
        return NULL;
    }
    actions->len = model_header->number_of_actions;
    return actions;
}

bool ml_getActions(ml_actions_t *actions_out) {
    const ml_model_header_t* const model_header = (ml_model_header_t*)MODEL_ADDRESS;
    if (model_header == NULL || actions_out == NULL) {
        return false;
    }
    if (actions_out->len != model_header->number_of_actions) {
        return false;
    }

    // Iterate through the actions on the header, copy the threshold and add a
    // pointer to the label stored in flash
    ml_header_action_t *action = (ml_header_action_t *)&model_header->actions[0];
    for (int i = 0; i < model_header->number_of_actions; i++) {
        // Check the label doesn't have any null terminators before the end
        for (int j = 0; j < action->label_length - 1; j++) {
            if (action->label[j] == '\0') {
                return false;
            }
        }
        // And confirm the last character is a null terminator
        if (action->label[action->label_length - 1] != '\0') {
            return false;
        }

        actions_out->action[i].label = &action->label[0];
        actions_out->action[i].threshold = action->threshold;

        // Locate the next action in flash, which is 4 byte aligned
        action = (ml_header_action_t *)((uint32_t)action + ml_action_size_without_label + action->label_length);
        action = (ml_header_action_t *)(((uint32_t)action + 3) & ~3);
    }

    return true;
}

ml_predictions_t *ml_allocatePredictions() {
    int output_size = ml_getOutputLength();
    if (output_size <= 0) {
        return NULL;
    }
    ml_predictions_t *predictions = (ml_predictions_t *)calloc(
        1, sizeof(ml_predictions_t) + sizeof(float) * output_size);
    if (predictions == NULL) {
        return NULL;
    }
    predictions->index = -1;
    predictions->len = output_size;

    return predictions;
}

bool ml_predict(const float *input, const size_t in_len, const ml_actions_t *actions, ml_predictions_t *predictions_out) {
    if (actions == NULL || actions->len != output_length ||
            predictions_out == NULL || predictions_out->len != output_length) {
        return false;
    }

    bool success = ml_runModel(input, in_len, (float *)&predictions_out->prediction, output_length);
    if (!success) {
        return false;
    }
    predictions_out->index = ml_calcPrediction(actions, (float *)&predictions_out->prediction, output_length);

    return true;
}


bool ml_runModel(const float *input, const size_t in_len, float* individual_predictions, const size_t out_len) {
    if (input == NULL || individual_predictions == NULL || input_length != in_len || output_length != out_len) {
        return false;
    }

    ml4f_header_t *ml4f_model = get_ml4f_model();
    int r = ml4f_full_invoke_arena(ml4f_model, model_arena, input, individual_predictions);
    if (r != 0) {
        return false;
    }

    return true;
}

int ml_calcPrediction(const ml_actions_t *actions, const float* predictions, const size_t len) {
    if (actions == NULL || predictions == NULL || len != actions->len) {
        return -1;
    }

    float predictions_above_threshold[len];
    for (size_t i = 0; i < len; i++) {
        if (predictions[i] >= actions->action[i].threshold) {
            predictions_above_threshold[i] = predictions[i];
        } else {
            predictions_above_threshold[i] = 0.0f;
        }
    }
    int max_index = ml4f_argmax(predictions_above_threshold, len);
    if (max_index < 0) {
        return -1;
    }
    // If the max predictionn is 0, then none were above the threshold
    if (predictions_above_threshold[max_index] == 0.0f) {
        max_index = -1;
    }

    return max_index;
}
