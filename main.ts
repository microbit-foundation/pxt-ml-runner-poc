input.onButtonPressed(Button.A, function () {
    basic.clearScreen()
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
mlrunner.runMlModelInBackground()
basic.showNumber(mlrunner.inputLength())
basic.forever(function () {
	
})
