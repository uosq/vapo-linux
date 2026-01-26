# Vector3

## Methods

### > Set( x: number, y: number, z: number )

### > Normalize( )
returns **number**

Normalizes in place and returns length

### > Length( )
returns **number**

### > Length2D( )
returns **number**

### > LengthSqr( )
returns **number**

### > Length2DSqr( )
returns **number**

### > Cross( other: **Vector3**  )
returns **Vector3**

### > Dot( other: **Vector3** )
returns **number**

### > DistTo( other: **Vector3** )
returns **number**

### > DistTo2D( other: **Vector3** )

### > ToAngle( )
returns **Vector3**

### > Angles( )
returns forward: **Vector3**, right: **Vector3**, up: **Vector3**

## Examples

```lua
local vec = Vector3()
vec += Vector3(10, 50, 20)
vec -= Vector3(123, 456, 789)
vec *= 50
print(vec.Length(), vec.Normalize())
```