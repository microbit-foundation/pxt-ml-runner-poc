basic.forever(function () {

})

input.onButtonPressed(Button.A, function () {
    mlrunner.emitMlEvent()
})
input.onButtonPressed(Button.B, function () {
    basic.clearScreen()
})
mlrunner.onMlEvent(MlRunnerEvent.MlRunnerEventLabel1, function () {
    basic.showIcon(IconNames.Heart)
})
mlrunner.onMlEvent(MlRunnerEvent.MlRunnerEventLabel2, function () {
    basic.showIcon(IconNames.SmallHeart)
})

basic.showNumber(mlrunner.inputLength())
basic.showIcon(IconNames.Heart)
