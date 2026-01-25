# Entity

All methods return nil if the entity is not valid!     

## Methods

### > IsValid( )
returns **bool**

### > GetName( )
returns **string**

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
returns **Entity?**

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

### > GetWeaponID( )
returns **int**

### > GetClassID( )
returns **int**

### > GetFirstMoveChild( )
returns **Entity?**

### > GetNextMovePeer( )
returns **Entity?**

### > AttributeHookValueInt( )
returns **int**

### > AttributeHookValueFloat( )
returns **number**

### > GetWeaponType( )
returns **int**

### > IsInReload( )
returns **bool**

### > GetSmackTime( )
returns **number**
