C_LONGINT:C283($err)
ARRAY LONGINT:C221($alObjects; 0)
$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "DATA"; $alObjects)
If (($err=0x0000) & (Size of array:C274($alObjects)=0x0001))
	PPro_ToolsSetButtons(99; PP_ExtArea)
	PP_CurrentObject:=$alObjects{1}
	PPro_SubformVisibility(PP_ExtArea)
End if 