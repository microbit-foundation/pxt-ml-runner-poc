basic.forever(function () {

})

input.onButtonPressed(Button.A, function () {
    basic.clearScreen()
})
mlrunner.onMlEvent(MlRunnerEvent.MlRunnerEventLabel1, function () {
    basic.showString("1")
})
mlrunner.onMlEvent(MlRunnerEvent.MlRunnerEventLabel2, function () {
    basic.showString("2")
})
mlrunner.onMlEvent(MlRunnerEvent.MlRunnerEventLabel3, function () {
    basic.showString("3")
})
mlrunner.onMlEvent(MlRunnerEvent.MlRunnerEventLabel4, function () {
    basic.showString("4")
})

basic.showNumber(mlrunner.inputLength())
mlrunner.runMlModelInBackground()
