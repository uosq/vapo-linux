# Material

## Methods

### > SetColorModulation( red: **number**, green: **number**, blue: **number** )
All numbers should be in the `[0, 1]` range!

### > GetColorModulation( )
returns red: **number**, green: **number**, blue: **number**

All return values are in the `[0, 1]` range

### > SetMaterialVarFlag( flag: **int**, value: **bool** )

### > GetMaterialVarFlag( flag: **int** )
returns **bool**

### > GetTextureGroupName( )
returns **string**

### > GetName( )
returns **string**

## Examples

```lua
local vmt =
[[UnlitGeneric
{
	$basetexture "white"
}]]

local mat = materials.Create("mycoolmaterial", vmt)

local function DM(ctx: DrawModelContext)
	if ctx.entity != nil and ctx.entity:IsPlayer() then
		render.ForcedMaterialOverride(mat)
		render.SetColorModulation(1, 0, 0)
	end
end

hooks.Add("DrawModel", "myverygoodhook", DM)
```