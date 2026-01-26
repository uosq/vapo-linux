# Texture

## Methods

### > Delete( )
Should be called when you want to delete the texture from memory

### > GetName( )
returns **string**

### > GetFlags( )
returns **int**

### > GetActualWidth( )
returns **int**

### > GetActualHeight( )
returns **int**

### > IsTranslucent( )
returns **bool**

### > IsRenderTarget( )
returns **bool**

## Examples

```lua
local fb = materials.FindTexture("_rt_FullFrameFB")
if fb == nil then
	print("Frame Buffer is nil! Are you in a match?")
	return
end

print(fb:GetActualWidth())
print(fb:GetActualHeight())
print(fb:IsTranslucent())
print(fb:IsRenderTarget())
```