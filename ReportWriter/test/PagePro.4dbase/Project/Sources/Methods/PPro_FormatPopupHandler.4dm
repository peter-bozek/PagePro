//%attributes = {"invisible":true}
C_TEXT:C284($prop)
C_LONGINT:C283($err)

GET LIST ITEM PARAMETER:C985(PP_FormatPopup; *; "format"; $prop)

If ($prop#"")
	PP_Format:=$prop
End if 

$err:=PPro_SetPropertyString(PP_ExtArea; 0; PProp_Format:K7906:41; PP_Format)