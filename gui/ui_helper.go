package main

import (
	"fmt"
	"image/color"
	"strconv"
	"strings"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/widget"
)

/*func CreateListBoxContainer(label string, options []string, value *string) *fyne.Container {
	labelWidget := widget.NewLabel(label)
	group := widget.NewRadioGroup(options, func(s string) {
		*value = s
	})
	group.Horizontal = true
	return container.NewBorder(nil, nil, labelWidget, nil, group)
}*/

func CreateList(label string, options []string, value *string, defaultValue string) *fyne.Container {
	labelWidget := widget.NewLabel(label)
	group := widget.NewSelect(options, func(s string) {
		*value = s
	})
	*value = defaultValue
	group.SetSelected(defaultValue)
	return container.NewBorder(nil, nil, labelWidget, nil, group)
}

/*func CreateKeySelection(label string, option *string) *fyne.Container {
	labelWidget := widget.NewLabel(label)
	selection := widget.NewSelectEntry(keys)

	selection.OnChanged = func(s string) {
		*option = s
	}

	selection.Text = "KEY_NONE"
	*option = "KEY_NONE"

	return container.NewBorder(nil, nil, labelWidget, nil, selection)
}*/

func CreateSlider(label string, value *float64, min float64, max float64) *fyne.Container {
	fovLabel := widget.NewLabel(label)
	fovValueLabel := widget.NewLabel(strconv.FormatFloat(*value, 'f', 2, 64))
	fovSlider := widget.NewSlider(min, max)
	fovSlider.SetValue(*value)
	fovSlider.OnChanged = func(f float64) {
		*value = f
		fovValueLabel.SetText(strconv.FormatFloat(f, 'f', 2, 64))
	}
	sliderContainer := container.NewBorder(nil, nil, fovLabel, fovValueLabel, fovSlider)
	return sliderContainer
}

func CreateSliderStepped(label string, value *float64, min float64, max float64, step float64) *fyne.Container {
	fovLabel := widget.NewLabel(label)
	fovValueLabel := widget.NewLabel(strconv.FormatFloat(*value, 'f', 2, 64))
	fovSlider := widget.NewSlider(min, max)
	fovSlider.SetValue(*value)
	fovSlider.OnChanged = func(f float64) {
		*value = f
		fovValueLabel.SetText(strconv.FormatFloat(f, 'f', 2, 64))
	}
	fovSlider.Step = step
	sliderContainer := container.NewBorder(nil, nil, fovLabel, fovValueLabel, fovSlider)
	return sliderContainer
}

func CreateEntry(label string, value *string) *fyne.Container {
	labelWidget := widget.NewLabel(label)
	textEntry := widget.NewEntry()
	textEntry.SetText(*value)
	textEntry.OnChanged = func(s string) {
		*value = strings.ToUpper(s)
	}
	return container.NewBorder(nil, nil, labelWidget, nil, textEntry)
}

func CreateToggle(label string, value *bool) *widget.Check {
	toggle := widget.NewCheck(label, func(b bool) {
		*value = b
	})
	toggle.Checked = *value
	return toggle
}

func CreateToggleFlag16(label string, value *uint16, flagNum uint16) *widget.Check {
	toggle := widget.NewCheck(label, func(b bool) {
		ToggleFlagBit16(value, flagNum)
		//fmt.Println(ReadFlagBit(*value, flagNum))
	})

	toggle.Checked = ReadFlagBit16(*value, flagNum)
	return toggle
}

func CreateToggleFlag8(label string, value *uint8, flagNum uint8) *widget.Check {
	toggle := widget.NewCheck(label, func(b bool) {
		ToggleFlagBit8(value, flagNum)
		//fmt.Println(ReadFlagBit(*value, flagNum))
	})

	toggle.Checked = ReadFlagBit8(*value, flagNum)
	return toggle
}

func CreateColorPickerButton(label string, colorVal *color.RGBA, window fyne.Window) *widget.Button {
	button := widget.NewButton(label, func() {
		picker := dialog.NewColorPicker(label, "Choose Color", func(c color.Color) {
			r, g, b, a := c.RGBA()
			colorVal.R = uint8(r)
			colorVal.G = uint8(g)
			colorVal.B = uint8(b)
			colorVal.A = uint8(a)
		}, window)
		picker.Advanced = true
		picker.SetColor(colorVal)
		picker.Show()
	})
	return button
}

func CreateCenterLabel(label string) *fyne.Container {
	return container.NewCenter(widget.NewLabel(label))
}

func ReadFlagBit16(value uint16, flag uint16) bool {
	return (value&(1<<flag) != 0)
}

func ReadFlagBit8(value uint8, flag uint8) bool {
	return (value&(1<<flag) != 0)
}

// Pointers are so useful
// Why does Lua not have them?!?!?!
func ToggleFlagBit16(value *uint16, n uint16) {
	if n > 15 {
		fmt.Println("Trying to set bit bigger than integer can hold!")
		return
	}

	*value ^= 1 << n
}

// Pointers are so useful
// Why does Lua not have them?!?!?!
func ToggleFlagBit8(value *uint8, n uint8) {
	if n > 7 {
		fmt.Println("Trying to set bit bigger than integer can hold!")
		return
	}

	*value ^= 1 << n
}

func GroupV(title string, content ...fyne.CanvasObject) fyne.CanvasObject {
	return widget.NewCard(title, "", container.NewVBox(content...))
}

func GroupH(title string, content ...fyne.CanvasObject) fyne.CanvasObject {
	return widget.NewCard(title, "", container.NewHBox(content...))
}
