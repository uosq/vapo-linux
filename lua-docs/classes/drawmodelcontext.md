# DrawModelContext

## Fields

### entity?
Entity?

### Call
function

Calls the original DrawModelExecute function so you can stack materials or other changes

### is_chams
bool

If the current DrawModel is the cheat's chams

### is_glow
bool

If the current DrawModel is the cheat's glow

## Examples

```lua
local function DrawModel(ctx: DrawModelContext)
	if ctx.is_glow or ctx.is_chams then
		return
	end

	if ctx.entity != nil and ctx.entity:IsPlayer() then
		render.SetBlend(0.5)
		render.SetColorModulation(1, 0.5, 0.5)
	end
end

hooks.Add("DrawModel", "mycoolhook", DrawModel)
```