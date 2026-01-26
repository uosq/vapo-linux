# NetChannel

## Methods

### > SetDataRate( rate: **number** )

### > RegisterMessage( msg: **NetMessage** )
returns **bool** (if it was successful)

### > SetTimeout( seconds: **number**  )

### > SetChallengeNr( challengeNr: **integer** )

### > ProcessPlayback( )

### > SendNetMsg( msg: **NetMessage**, [isReliable: **bool** = false], [isVoice: **bool** = false] )

### > SendData( data: **BitBuffer**, isReliable: **bool** )

### > SendFile( fileName: **string**, transferID: **int** )

### > DenyFile( fileName: **string**, transferID: **int** )

### > SetChoked( )

### > SendDatagram( data: **BitBuffer** )

### > Transmit( [onlyReliable: **bool** = false] )

### > GetDropNumber( )
returns **int**

### > GetSocket( )
returns **int**

### > GetChallengeNr( )
returns **int**

### > GetSequenceData( )
returns out sequence number: **int**, in sequence number: **int**, out sequence number ack: **int**

### > SetSequenceData( outSequenceNr: **int**, inSequenceNr: **int**, outSequenceNrAck: **int** )

### > UpdateMessageStats( msgGroup: **int**, bits: **int** )

### > CanPacket( )
returns **bool**

### > IsOverflowed( )
returns **bool**

### > IsTimedOut( )
returns **bool**

### > HasPendingReliableData( )
returns **bool**

### > SetFileTransmissionMode(  isBakgroundMode: **bool** )

### > SetCompressionMode( useCompression: **bool** )

### > RequestFile( fileName: **string** )

### > GetTimeSinceLastReceived( )
returns **number**

### > SetMaxBufferSize( isReliable: **bool**, numBytes: **int**, isVoice: **bool** )

### > IsNull( )
returns **bool**

### > GetNumBitsWritten( )
returns **int**

### > SetInterpolationAmount( amount: **number** )

### > SetRemoteFramerate( frameTime: **number**, frameTimeStdDeviation: **number** )

### > SetMaxRoutablePayloadSize( splitSize: **int** )

### > GetMaxRoutablePayloadSize( )
returns **int**

### > GetProtocolVersion( )
returns **int**

## Examples

```lua
local netchannel: NetChannel = engine.GetNetChannel()
if netchannel == nil then
	print("NetChannel is nil! Are you in a match?")
	return
end

print(netchannel:IsOverflowed())
netchannel:SetChoked()
```