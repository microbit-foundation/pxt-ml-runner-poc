// Icon unicode characters can be found at: http://fontawesome.io/icons/
//% color=#9966ff weight=100 icon="\uf108" block="ML Runner" advanced=false
namespace mlrunner {
    //% blockId=mlrunner_input_length
    //% block="model input length"
    //% shim=mlrunner::ml_getInputLen
    export function inputLength(): number {
        return 0;
    }

    export function onMlEvent(body: () => void): void {
        control.onEvent(71, 1, body)
    }

    //% blockId=mlrunner_emit_ml_event
    //% block="emit ML event"
    //% shim=mlrunner::emit_ml_event
    export function emitMlEvent(): void {}
}
