//%attributes = {"invisible":true}
C_LONGINT:C283($tool; $err)
$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "tool"; ->$tool)
PPro_ToolsSetButtons($tool; PP_ExtArea)