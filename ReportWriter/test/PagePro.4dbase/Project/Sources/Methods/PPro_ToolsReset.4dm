//%attributes = {"invisible":true}
C_LONGINT:C283($tool; $err)
$tool:=0x0000
$err:=RW_SetProperty(PP_ExtArea; PP_ExtArea; "tool"; ->$tool)
PPro_ToolsSetButtons($tool; PP_ExtArea)
