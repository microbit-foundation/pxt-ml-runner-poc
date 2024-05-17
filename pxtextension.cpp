#include <pxt.h>
#include "mlrunner.h"

namespace mlrunner {
    //% blockId=mlrunner_emit_ml_event
    void emit_ml_event() {
        // Different value used between simulator and hardware to check it works
        MicroBitEvent evt(71, 2);
    }
}
