# GameEvent

See [Valve Developer Community](https://developer.valvesoftware.com/wiki/Team_Fortress_2/Scripting/Game_Events)'s wiki for the game events

## Methods

### > GetName( )
returns **string**

### > IsReliable( )
returns **bool**

### > IsLocal( )
returns **bool**

### > IsEmpty( key: **string** )
returns **bool**

### > GetBool( key: **string** )
returns **bool**

### > GetInt( key: **string** )
returns **int**

### > GetFloat( key: **string** )
returns **number**

### > GetString( key: **string** )
returns **string**

### > SetBool( key: **string**, value: **bool** )

### > SetInt( key: **string**, value: **int** )

### > SetFloat( key: **string**, value: **number** )

### > SetString( key: **string**, value: **string** )

## Examples

```lua
local function FireGameEvent(event: GameEvent)
	local name = event:GetName()

	if name == "player_death" then
		local victim_entindex = event:GetInt("victim_entindex")
		local victim = entities.GetAtIndex(victim_entindex)

		if victim == nil then
			return
		end

		if event:GetBool("silent_kill") then
			local playerName = victim:GetName()
			print(string.format("Player %s was silent killed!", playerName))
		end
	end
end

hooks.Add("FireGameEvent", "mycoolhook", FireGameEvent)
```