basic.forever(function () {

})

input.onButtonPressed(Button.A, function () {
    mlrunner.emitMlEvent()
})
input.onButtonPressed(Button.B, function () {
    basic.clearScreen()
})
mlrunner.onMlEvent(function () {
    basic.showIcon(IconNames.Heart)
})

basic.showNumber(mlrunner.inputLength())
basic.showIcon(IconNames.Heart)
