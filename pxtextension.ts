// Icon unicode characters can be found at: http://fontawesome.io/icons/
//% color=#2b64c3 weight=100 icon="\uf108" block="ML Runner" advanced=false
namespace mlrunner {
    //% blockId=mlrunner_input_length
    //% block="model input length"
    //% shim=mlrunner::inputLength
    export function inputLength(): number {
        return 0;
    }

    //% blockId=mlrunner_on_ml_event
    //% block="on ML event %value"
    export function onMlEvent(value: MlRunnerEvent, body: () => void): void {
        control.onEvent(MlRunnerIds.MlRunnerInference, value, body)
    }

    //% blockId=mlrunner_run_ml_model_in_background
    //% block="run ML model in background"
    //% shim=mlrunner::init
    export function runMlModelInBackground(): void {
    }

    //% blockId=mlrunner_stop_ml_model_in_background
    //% block="stop ML model in background"
    //% shim=mlrunner::deInit
    export function stopMlModelInBackground(): void {
    }
}
