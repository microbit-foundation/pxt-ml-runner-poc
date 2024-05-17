basic.forever(function () {

})

input.onButtonPressed(Button.A, function () {
    mlrunner.emitMlEvent()
})
input.onButtonPressed(Button.B, function () {
    basic.clearScreen()
})
mlrunner.onMlEvent(1, function () {
    basic.showIcon(IconNames.Heart)
})
mlrunner.onMlEvent(2, function () {
    basic.showIcon(IconNames.SmallHeart)
})

basic.showNumber(mlrunner.inputLength())
basic.showIcon(IconNames.Heart)
