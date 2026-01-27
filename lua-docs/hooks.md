# Hooks

You can register a callback for a hook here.

They are called when the real function is being called, so you can modify the data or do something before it runs

## CalcViewModelView ( position: **Vector3**, angle: **Vector3** )
Called before drawing the Viewmodel

## LevelInitPreEntity ( mapName: **string** )
Called before receiving the first entity packet

## LevelInitPostEntity ( )
Called after receiving the first entity packet

## LevelShutdown ( )
Called when leaving a match

## CreateMove ( cmd: **UserCmd** )
Called when the game wants to create a movement command

## DoPostScreenSpaceEffects ( )
Called when the game wants to create screen effects

You can use this to make custom bloom, glow, etc

## Draw ( )
Called before drawing stuff to the screen

## DrawModel ( ctx: **DrawModelContext** )
Called before the game draws a model

You can use this to change how a model is drawn: their color, material, transparency, etc

## ImGui ( )
Called at the ImGui draw stage so you can make custom windows using the `ui` library

## SendNetMsg ( msg: **NetMessage** )
Called before sending a `NetMessage` to the server

You can use this to change the info we are sending to the server

## FrameStageNotify ( stage: **int** )
Called at various stages of a frame

## FireGameEvent ( event: **GameEvent** )
Called when a game event is called. For example, when a player spawns

## OverrideView ( view: **ViewSetup** )
Called before drawing the localplayer's view