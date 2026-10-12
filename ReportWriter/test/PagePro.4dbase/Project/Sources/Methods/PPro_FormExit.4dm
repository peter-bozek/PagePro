//%attributes = {"invisible":true}
C_LONGINT:C283($1)

If (False:C215)
	C_LONGINT:C283(PPro_FormExit; $1)
End if 

C_LONGINT:C283($area; $err)
$area:=$1
C_TEXT:C284($text)

If (PPro_EditorCallback("form"; "close"; $area; 0))
	C_TEXT:C284($text)
	$err:=RW_SaveReport($area; $text; 0x0000)  // third arg means it is a text variable
	If (PP_Report#$text)
		C_LONGINT:C283($WinRef)
		$WinRef:=Open form window:C675("Confirm3Buttons"; Sheet form window:K39:12; Horizontally centered:K39:1; Vertically centered:K39:4)
		DIALOG:C40("Confirm3Buttons")
		CLOSE WINDOW:C154($WinRef)
		
		If (OK=1)
			If (PP_Answer="OK")
				PP_Report:=$text
				PP_ReportModified:=1
			End if 
			CANCEL:C270
		End if 
	End if 
End if 
