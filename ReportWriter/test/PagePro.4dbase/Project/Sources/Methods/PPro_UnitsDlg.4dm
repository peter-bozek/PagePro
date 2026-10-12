//%attributes = {"invisible":true}
C_LONGINT:C283($1)
C_BOOLEAN:C305($0)

If (False:C215)
	C_LONGINT:C283(PPro_UnitsDlg; $1)
	C_BOOLEAN:C305(PPro_UnitsDlg; $0)
End if 

C_LONGINT:C283($win)

$win:=Open form window:C675("UnitsSetup"+PP_Environment; Sheet form window:K39:12)
DIALOG:C40("UnitsSetup"+PP_Environment)
CLOSE WINDOW:C154($win)

$0:=(OK=1)