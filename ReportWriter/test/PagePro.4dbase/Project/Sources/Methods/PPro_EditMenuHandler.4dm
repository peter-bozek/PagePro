//%attributes = {"shared":true}
C_LONGINT:C283($1)  //edit menu command
C_LONGINT:C283($2)  //area
C_LONGINT:C283($3)  //current object - for pasting content directly to object
C_BOOLEAN:C305($0)  //rebuild list of objects

// Edit menu is handled by external area which sets (dims)
// individual commands and does default handling of some menus
// like undo - redo
// Not handled events are passed in event record to area script
// B replaced obsolete action constants with numbers

If (False:C215)
	C_LONGINT:C283(PPro_EditMenuHandler; $1)
	C_LONGINT:C283(PPro_EditMenuHandler; $2)
	C_LONGINT:C283(PPro_EditMenuHandler; $3)
	C_BOOLEAN:C305(PPro_EditMenuHandler; $0)
End if 

C_LONGINT:C283($eventKind; $area; $err; $i; $j)
C_BOOLEAN:C305($rebuildList)

$eventKind:=$1
$area:=$2

If (PPro_EditorCallback("command"; "EditMenu:"+String:C10($eventKind); $area; PP_CurrentObject))
	
	Case of 
		: ($eventKind=17)  // "undo"
			$rebuildList:=True:C214
			
		: ($eventKind=31)  // "redo"
			$rebuildList:=True:C214
			
		: ($eventKind=18)  // "cut"
			If (PPro_GetPropertyString($area; PP_CurrentObject; "kind")="Pict")
				C_BLOB:C604($blob)
				C_PICTURE:C286($pict)
				$err:=RW_GetProperty($area; PP_CurrentObject; "data"; ->$blob)
				If ($err=0x0007)  // invalid pointer -> picture is not in BLOB format
					$err:=RW_GetProperty($area; PP_CurrentObject; "data"; ->$pict)
				Else 
					BLOB TO VARIABLE:C533($blob; $pict)
				End if 
				SET PICTURE TO PASTEBOARD:C521($pict)
				PPro_XMLObjects($area; "x")
			Else 
				PPro_XMLObjects($area; "x")
			End if 
			
			$rebuildList:=True:C214
			
		: ($eventKind=19)  // "copy"
			If (PPro_GetPropertyString($area; PP_CurrentObject; "kind")="Pict")
				C_BLOB:C604($blob)
				C_PICTURE:C286($pict)
				$err:=RW_GetProperty($area; PP_CurrentObject; "data"; ->$blob)
				If ($err=0x0007)  // invalid pointer -> picture is not in BLOB format
					$err:=RW_GetProperty($area; PP_CurrentObject; "data"; ->$pict)
				Else 
					BLOB TO VARIABLE:C533($blob; $pict)
				End if 
				SET PICTURE TO PASTEBOARD:C521($pict)
				PPro_XMLObjects($area; "c")  // will paste picture to text scrap
			Else 
				PPro_XMLObjects($area; "c")
			End if 
			$rebuildList:=True:C214
			
		: ($eventKind=20)  // "paste"
			If (PPro_GetPropertyString($area; PP_CurrentObject; "kind")="Pict")
				C_PICTURE:C286($pict)
				GET PICTURE FROM PASTEBOARD:C522($pict)
				If (Picture size:C356($pict)>0)
					C_BLOB:C604($blob)
					VARIABLE TO BLOB:C532($pict; $blob)
					$err:=RW_SetProperty($area; PP_CurrentObject; "data"; ->$blob)
				End if 
			Else 
				PPro_XMLObjects($area; "v")
			End if 
			$rebuildList:=True:C214
			
		: ($eventKind=21)  // "clear"
			ARRAY LONGINT:C221($alObjects; 0x0000)
			$err:=RW_GetObjects($area; $area; "selc"; $alObjects)
			$err:=RW_Area_SaveUndo($area; 1)
			For ($i; 0x0001; Size of array:C274($alObjects))
				$err:=RW_DeleteObject($area; $alObjects{$i})
			End for 
			$err:=RW_Area_SaveUndo($area; 0)
			$rebuildList:=True:C214
			
		: ($eventKind=22)  // "select all"
			//deselect all selected objects
			C_LONGINT:C283($lValue)
			$lValue:=0
			$err:=RW_SetProperty($area; 0; "selc"; ->$lValue)
			
			//what are all objects?
			ARRAY LONGINT:C221($alObjects; 0x0000)
			ARRAY LONGINT:C221($alLeaves; 0x0000)
			C_BOOLEAN:C305($bTrue)
			$bTrue:=True:C214
			$err:=RW_GetObjects($area; $area; "body"; $alObjects)
			For ($i; 0x0001; Size of array:C274($alObjects))
				$err:=RW_GetObjects($area; $alObjects{$i}; "objs"; $alLeaves)
				For ($j; 0x0001; Size of array:C274($alLeaves))
					$err:=RW_SetProperty($area; $alLeaves{$j}; "selc"; ->$bTrue)
				End for 
			End for 
			$err:=RW_GetObjects($area; $area; "selc"; PP_ItemsToSelect)
			$rebuildList:=True:C214
			
	End case 
End if 
$0:=$rebuildList