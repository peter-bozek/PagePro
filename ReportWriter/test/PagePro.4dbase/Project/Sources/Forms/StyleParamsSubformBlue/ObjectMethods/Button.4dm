C_LONGINT:C283($err)
$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
If (PPro_ConfirmDelete(PP_ExtArea; PP_CurrentObject))
	$err:=RW_DeleteObject(PP_ExtArea; PP_CurrentObject)
End if 
$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
PP_CurrentObject:=0
PPro_AreaReloadHList(PP_ExtArea)
PPro_SubformVisibility(PP_ExtArea)
