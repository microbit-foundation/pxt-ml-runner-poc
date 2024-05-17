// Icon unicode characters can be found at: http://fontawesome.io/icons/
//% color=#2b64c3 weight=100 icon="\uf108" block="ML Runner" advanced=false
namespace mlrunner {
    //% blockId=mlrunner_input_length
    //% block="model input length"
    //% shim=mlrunner::ml_getInputLen
    export function inputLength(): number {
        return 0;
    }

    //% blockId=mlrunner_on_ml_event
    //% block="on ML event"
    export function onMlEvent(value: number, body: () => void): void {
        control.onEvent(71, value, body)
    }

    //% blockId=mlrunner_emit_ml_event
    //% block="emit ML event"
    //% shim=mlrunner::emit_ml_event
    export function emitMlEvent(): void {
        // Different value used in simulator vs hardware to check it works
        control.raiseEvent(71,1)
    }
}
