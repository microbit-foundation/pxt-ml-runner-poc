#include <pxt.h>

// ML model is too large for V1, so avoid including it or compilation will fail
#if MICROBIT_CODAL
    #include "mlrunner.h"
#else
    // Create V1 shims to raise 927 panic code
    namespace mlrunner {
        //% blockId=mlrunner_input_length
        int ml_getInputLen() {
            return DEVICE_NOT_SUPPORTED;
        }
    }
#endif

enum MlRunnerEvent {
    //% block="label 1"
    MlRunnerEventLabel1 = 1,
    //% block="label 2"
    MlRunnerEventLabel2 = 2,
    //% block="label 3"
    MlRunnerEventLabel3 = 3,
    //% block="label 4"
    MlRunnerEventLabel4 = 4,
};

namespace mlrunner {

    //% blockId=mlrunner_emit_ml_event
    void emit_ml_event() {
        // Different value used between simulator and hardware to check it works
        MicroBitEvent evt(71, MlRunnerEvent::MlRunnerEventLabel2);
    }
}
