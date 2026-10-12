//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //line object to edit

If (False:C215)
	C_LONGINT:C283(PPro_EditLineDlg; $1)  //line object to edit
End if 

C_LONGINT:C283($dlg; $left; $top; $right; $bottom)

//dialog is displayed over PP_ExtArea area
OBJECT GET COORDINATES:C663(PP_ExtArea; $left; $top; $right; $bottom)
$dlg:=Open form window:C675("LineParameters"; 33; 0; 0)
PP_CurrentObject:=$1  //object to edit
DIALOG:C40("LineParameters")
CLOSE WINDOW:C154($dlg)