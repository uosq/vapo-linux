# BitBuffer

All Read & Write methods use the current bit position

## Methods

### > SetCurBitPos( pos: **int** )

### > GetCurBitPos( )
returns **int**

### > Reset( )
Resets the current bit position to 0

### > WriteBit( val: **int** )

### > ReadBit( )
returns **int**

### > WriteByte( val: **int** )

### > ReadByte( )
returns **int**

### > WriteString( text: **string** )

### > ReadString( [maxlength: **int** = 256] )

### > WriteFloat( val: **number**, [numbits: **int** = 32] )

### > ReadFloat( [numbits: **int** = 32] )

### > WriteInt( val: **int**, [numBits: **int** = 32] )

### > ReadInt( [numbits: **int** = 32] )

### > Delete( )
Call this when you want to delete the BitBuffer from memory

## Examples

```lua
local buf = BitBuffer()

buf:WriteInt(12)
buf:WriteFloat(30.5)
buf:WriteString("Hello, world!")

buf:Reset()
print(buf:ReadInt(12)) -- 12
print(buf:ReadFloat()) -- 30.5
print(buf:ReadString(256)) --- "Hello, world!

buf:Delete()
```