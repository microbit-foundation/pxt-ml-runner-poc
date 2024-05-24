//% color=#2b64c3 weight=100 icon="\uf108" block="ML Runner" advanced=false
namespace mlrunner {
    /**
     * Run this code when the model detects the input label has been predicted.
     *
     * This automatically starts running the ML model in the background.
     * When the model predicts the indicated label, an event is raised to
     * trigger this handler.
     *
     * @param mlEvent The label event that triggers this code to run.
     * @param body The code to run when the model predicts the label.
     */
    //% blockId=mlrunner_on_ml_event
    //% block="on ML event %value"
    export function onMlEvent(mlEvent: MlRunnerLabels, body: () => void): void {
        mlrunner.startRunning();
        control.onEvent(MlRunnerIds.MlRunnerInference, mlEvent, body)
    }

    /**
     * TS shim for C++ function init(), which initialize the ML model with
     * an address to a model blob.
     *
     * @param modelBlob The model blob to initialize the ML model with.
     */
    //% blockId=mlrunner_init
    //% shim=mlrunner::init
    function initRunner(modelBlob: Buffer): void {
    }

    /**
     * Configure the ML model, start capturing accelerometer data, and run
     * the model in the background.
     */
    //% blockId=mlrunner_run_model_background
    //% block="run ML model in background"
    export function startRunning(): void {
        // modelBlob should be populated by the MakeCode extension
        initRunner(modelBlob);
    }

    /**
     * Stop running the accelerometer data capturing and ML model in the
     * background.
     */
    //% blockId=mlrunner_stop_model_running
    //% block="stop running ML"
    //% shim=mlrunner::deInit
    export function stopRunning(): void {
    }

    /**
     * Check if the ML model is currently running in the background.
     */
    //% blockId=mlrunner_is_running
    //% block="is ML running"
    //% shim=mlrunner::isModelRunning
    export function isRunning(): boolean {
        return false;
    }
}
