input.onButtonPressed(Button.A, function () {
    basic.clearScreen()
})
input.onButtonPressed(Button.B, function () {
    if (mlrunner.isRunning()) {
        mlrunner.stopRunning()
    } else {
        mlrunner.startRunning()
    }
})
mlrunner.onMlEvent(MlRunnerLabels.Jumping, function () {
    basic.showString("J")
})
mlrunner.onMlEvent(MlRunnerLabels.Running, function () {
    basic.showString("R")
})
mlrunner.onMlEvent(MlRunnerLabels.Standing, function () {
    basic.showString("S")
})
mlrunner.onMlEvent(MlRunnerLabels.Walking, function () {
    basic.showString("W")
})
basic.showNumber(mlrunner.inputLength())
basic.forever(function () {
	
})
