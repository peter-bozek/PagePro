//%attributes = {"invisible":true}
C_TEXT:C284($0; $1)

If (False:C215)
	C_TEXT:C284(PPro_EditScript; $0; $1)
End if 

C_LONGINT:C283($win)
PP_Script:=$1
$win:=Open form window:C675("ScriptEdit"+PP_Environment; Sheet form window:K39:12)
DIALOG:C40("ScriptEdit"+PP_Environment)
CLOSE WINDOW:C154($win)
$0:=PP_Script
