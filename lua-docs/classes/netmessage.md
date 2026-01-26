# NetMessage

## Methods

### > GetType( )
returns **int**

### > GetName( )
returns **string**

### > ToString( )
returns **string**

### > WriteToBitBuffer( data: **BitBuffer** )

### > ReadFromBitBuffer( data: **BitBuffer** )

### > IsReliable( )
returns **bool**

### > SetReliable( isReliable: **bool** )

### > GetGroup( )
returns **int**

## Examples

```lua
local function SendNetMsg(msg)
	print(msg:GetName())
	print(msg:ToString())

	local buf = BitBuffer()
	msg:WriteToBitBuffer(buf)

	--- do some stuff to it

	buf:SetCurBitPos(0) --- reset BitBuffer position
	msg:ReadFromBitBuffer(buf)
end

hooks.Add("SendNetMsg", "i love cheese", SendNetMsg)
```