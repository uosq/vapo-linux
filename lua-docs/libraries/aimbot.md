# Aimbot Library

## Methods

### > GetTarget( )
returns **Entity**?

Will return nil when there is no target

### > SetTarget( target: **Entity**? )
Change the current aimbot target

If **target** is nil, current aimbot target will be none

### > GetMode( )
returns **int** (current aimbot mode)

### > SetMode( mode: **int** )
Changes the current aimbot mode

### > GetFOV( )
returns **number** (current aimbot fov)

### > SetFOV( fov: **number** )
Changes the current aimbot fov

### > IsEntityValid( ent: **Entity**? )
returns **bool**

This will return true if its a **Entity** that is valid for the aimbot (Use the aimbot's ignore filter to change what entities are valid)

### > IsRunning( )
returns **bool**

Will return true if the aimbot is running

### > GetKey( )
returns **int**?, **string**?

Does return nil if current key is invalid

### > SetKey( key: **int**|**string** )
Changes the aimbot key

Can be a integer or a string

## Examples

```lua
local function MyCreateMove(cmd: UserCmd)
	local target = aimbot.GetTarget()
	if target != nil then
		print(target:GetName())
	end
end

hooks.Add("CreateMove", "coolhook", MyCreateMove)
```