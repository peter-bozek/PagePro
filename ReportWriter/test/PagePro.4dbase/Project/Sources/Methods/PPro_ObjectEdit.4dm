//%attributes = {"invisible":true}
C_LONGINT:C283($1)

If (False:C215)
	C_LONGINT:C283(PPro_ObjectEdit; $1)
End if 


C_LONGINT:C283($area; $err)
C_TEXT:C284($text)

$area:=$1
$err:=RW_GetProperty($area; PP_CurrentObject; "kind"; ->$text)
Case of 
	: ($text="Text")
		
	: ($text="Var")
		
	: ($text="Field")
		
	: ($text="Header")
		
	: ($text="Column")
		
		
		//properties edited in dialogs
	: ($text="Line")
		PPro_EditLineDlg(PP_CurrentObject)
	: ($text="Rect")
		PPro_EditRectDlg(PP_CurrentObject)
	: ($text="Oval")
		PPro_EditOvalDlg(PP_CurrentObject)
		
	Else 
		//??
End case 