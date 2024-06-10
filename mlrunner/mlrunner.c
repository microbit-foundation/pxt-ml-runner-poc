
#include <stdlib.h>
#include "ml4f.h"
#include "mlrunner.h"

// Pointer to the model in flash
static uint32_t* MODEL_ADDRESS = NULL;

/*****************************************************************************/
/* Private API                                                               */
/*****************************************************************************/
/**
 * @return True if the model header is valid, False otherwise.
 */
static bool is_model_valid(void* model_address) {
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
static ml4f_header_t* get_ml4f_model() {
    if (MODEL_ADDRESS == NULL || !is_model_valid(MODEL_ADDRESS)) {
        return NULL;
    }
    ml_model_header_t *model_header = (ml_model_header_t *)MODEL_ADDRESS;
    return (ml4f_header_t *)((uint32_t)model_header + model_header->header_size);
}

/*****************************************************************************/
/* Public API                                                                */
/*****************************************************************************/
bool ml_setModel(void *model_address) {
    // Check if the model is valid
    if (!is_model_valid(model_address)) {
        return false;
    }
    MODEL_ADDRESS = (uint32_t *)model_address;
    return true;
}

bool ml_isModelPresent() {
    return MODEL_ADDRESS != NULL;
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
    ml4f_header_t *ml4f_model = get_ml4f_model();
    if (ml4f_model == NULL) {
        return -1;
    }
    return ml4f_shape_elements(ml4f_input_shape(ml4f_model));
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

ml_predictions_t* ml_allocatePredictions() {
    const ml_model_header_t* const model_header = (ml_model_header_t*)MODEL_ADDRESS;
    if (model_header == NULL) {
        return NULL;
    }

    ml_predictions_t *predictions = (ml_predictions_t *)malloc(
            sizeof(ml_predictions_t) + sizeof(ml_prediction_t) * model_header->number_of_actions);
    if (predictions == NULL) {
        return NULL;
    }
    predictions->len = model_header->number_of_actions;
    return predictions;

}

bool ml_predict(const float *input, ml_predictions_t *predictions_out) {
    // Assuming the model is the same address we can cache the actions
    static ml_actions_t *actions = NULL;
    static ml4f_header_t *ml4f_model = NULL;

    if (actions == NULL) {
        free(actions);
        actions = ml_allocateActions();
        if (actions == NULL) return false;
        const bool getActionsSuccess = ml_getActions(actions);
        if (!getActionsSuccess) return false;

        ml4f_model = get_ml4f_model();
    }
    if (ml4f_model == NULL) {
        free(actions);
        actions = NULL;
        return false;
    }

    // Check the output predictions size is correct
    if (predictions_out->len != actions->len) {
        return false;
    }

    // Run the model inference and obtain the predictions
    float ml4f_output[predictions_out->len];
    int r = ml4f_full_invoke(ml4f_model, input, ml4f_output);
    if (r != 0) {
        return NULL;
    }

    // Populate output predictions
    float output_above_threshold[predictions_out->len];
    for (size_t i = 0; i < predictions_out->len; i++) {
        predictions_out->prediction[i].action.threshold = actions->action[i].threshold;
        predictions_out->prediction[i].action.label = actions->action[i].label;
        predictions_out->prediction[i].prediction = ml4f_output[i];
        if (ml4f_output[i] >= actions->action[i].threshold) {
            output_above_threshold[i] = ml4f_output[i];
        } else {
            output_above_threshold[i] = 0.0f;
        }
    }
    predictions_out->max_index = ml4f_argmax(ml4f_output, predictions_out->len);
    predictions_out->prediction_index = ml4f_argmax(output_above_threshold, predictions_out->len);

    // If the prediction_index prediction is 0.0, then none of the predictions were above the threshold
    if (output_above_threshold[predictions_out->prediction_index] == 0.0f) {
        predictions_out->prediction_index = -1;
    }

    return true;
}
