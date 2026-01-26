# UI Library

Wrapper for ImGui

This library should only be used in the **ImGui** hook

## Methods

### > Begin( name: **string** )
returns **bool**

### > Button( label: **string** )
returns **bool**

The return value is true when it was clicked

### > Checkbox( label: **string**, value: **bool** )
returns changed: **bool**, new value: **bool**

### > TextUnformatted( text: **string** )

### > SliderFloat( label: **string**, value: **number**, min: **number**, max: **number** )
returns clicked: **bool**, new value: **number**

### > End( )
This should **ALWAYS** be called after **Begin**, even if it returned false!

Don't complain to me about crashes if you forget to call **End**

## Examples

```lua
local check = false

local function DrawGUI()
	ui.Begin("My Cool Lua Window")

	if ui.Button("Click me") then
		print("clicked")
	end

	--- will toggle (on & off) the check variable
	_, check = ui.Checkbox("Check me", check)

	ui.End()
end

hooks.Add("ImGui", "my cool stuff", DrawGUI)
```