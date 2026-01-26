# Entity

All methods return nil if the entity is not valid!     

## Methods

### > IsValid( )
returns **bool**

### > GetName( )
returns **string**

### > GetIndex( )
returns **int**

### > GetClassName( )
returns **string**

### > GetAbsOrigin( )
returns **Vector3**

### > GetAbsAngles( )
returns **Vector3**

### > GetNetvarInt( netvar: **string** )
returns **int**

### > GetNetvarFloat( netvar: **string** )
returns **number**

### > GetNetvarVector( netvar: **string** )
returns **Vector3**

### > GetNetvarEntity( netvar: **string** )
returns **Entity**?

### > SetNetvarInt( netvar: **string**, value: **int** )

### > SetNetvarFloat( netvar: **string**, value: **numb er** )

### > SetNetvarVector( netvar: **string**, value: **Vector3** )

### > SetNetvarEntity( netvar: **string**, value: **Entity** )

### > GetHealth( )
returns **int**

### > GetMaxHealth( )
returns **int**

### > GetMins( )
returns **Vector3**

### > GetMaxs( )
returns **Vector3**

### > GetTeamNumber( )
returns **int**

### > IsWeapon( )
returns **bool**

### > IsDormant( )
returns **bool**

### > IsAlive( )
returns **bool**

### > IsPlayer( )
returns **bool**

### > GetClassID( )
returns **int**

### > GetFirstMoveChild( )
returns **Entity**?

### > GetNextMovePeer( )
returns **Entity**?

### > AttributeHookValueInt( )
returns **int**

### > AttributeHookValueFloat( )
returns **number**

## Player Methods

### > Predict( seconds: **number** )
returns **Vector3[]**

Only works with players, will return nil if Entity is not a player

Will return a table with only 1 index when player is not moving (their origin)

### > GetPlayerResources( )
returns table:

| Field | Type |
| -- | -- |
| m_iTotalScore | int |
| m_iMaxHealth | int |
| m_iMaxBuffedHealth | int |
| m_iPlayerClass | int |
| m_bArenaSpectator | bool |
| m_iActiveDominations | int |
| m_flNextRespawnTime | number |
| m_iChargeLevel | int |
| m_iDamage | int |
| m_iDamageAssist | int |
| m_iDamageBoss | int |
| m_iHealing | int |
| m_iHealingAssist | int |
| m_iDamageBlocked | int |
| m_iCurrencyCollected | int |
| m_iBonusPoints | int |
| m_iPlayerLevel | int |
| m_iStreaks | int |
| m_iUpgradeRefundCredits | int |
| m_iBuybackCredits | int |
| m_iPartyLeaderRedTeamIndex | int |
| m_iPartyLeaderBlueTeamIndex | int |
| m_iEventTeamStatus | int |
| m_iPlayerClassWhenKilled | int |
| m_iConnectionState | int |
| m_flConnectTime | number |

## Weapon Methods

### > GetWeaponID( )
returns **int**

### > GetWeaponType( )
returns **int**

### > IsInReload( )
returns **bool**

Only call this in a weapon, will return nil on anything else

### > GetSmackTime( )
returns **number**

## Examples

Getting the localplayer
```lua
local localplayer = entities.GetLocalPlayer()
if localplayer == nil then
	print("Localplayer is nil! Are you in a match?")
	return
end
```

Getting a netvar/prop
```lua
local localplayer = entities.GetLocalPlayer()
if localplayer == nil then
	print("Localplayer is nil! Are you in a match?")
	return
end

local class = localplayer:GetNetvarInt("CTFPlayer->m_iClass")

print(class)
```