#include <pxt.h>
#include "mlrunner.h"

namespace mlrunner {
    //%
    void emit_ml_event() {
        MicroBitEvent evt(71, 1);
        //control::raiseEvent(71, 1);
    }
}
