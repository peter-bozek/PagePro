//%attributes = {"invisible":true}
C_LONGINT:C283($eventKind; $err)
C_BOOLEAN:C305($doubleClick; $rebuildList)

$rebuildList:=False:C215

$eventKind:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; "evtT")

Case of 
	: (($eventKind=Key down event:K17:4) | ($eventKind=Auto key event:K17:6))  // keyDown, autoKey
		$rebuildList:=PPro_AreaKeyHandler(->PP_ExtArea)
		
	: (($eventKind=Mouse down event:K17:2) | ($eventKind=Mouse up event:K17:3))  // mouse events
		$rebuildList:=PPro_AreaMouseHandler(->PP_ExtArea)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Tool:K7906:124; PProTool_Select:K7902:1)
		PPro_ToolsSetButtons(On Load:K2:1; PP_ExtArea)
		
	: ($eventKind>=17)  // undo action is obsolete constant now
		$rebuildList:=PPro_EditMenuHandler($eventKind; PP_ExtArea)
		
End case 


If ($rebuildList)
	PPro_AreaReloadHList(PP_ExtArea)
End if 

PPro_AreaFormData(PP_ExtArea)
PPro_PropertiesReload(PP_ExtArea; 0)
