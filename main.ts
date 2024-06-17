input.onButtonPressed(Button.A, function () {
    basic.clearScreen()
})
testrunner.onMlEvent(TestRunnerLabels.None, function () {
    basic.clearScreen()
})
testrunner.onMlEvent(TestRunnerLabels.Shake, function () {
    basic.showString("S")
})
testrunner.onMlEvent(TestRunnerLabels.Still, function () {
    basic.showIcon(IconNames.Asleep)
})
testrunner.onMlEvent(TestRunnerLabels.Circle, function () {
    basic.showString("C")
})
basic.forever(function () {
	
})
