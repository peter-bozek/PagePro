//%attributes = {"shared":true}
// Getter to internal component variables
// PPro_GetEditorVariable (
//      selector;          string with variable name
//      pointerToVar)      pointer to variable in that will contain returned value
//    -> error: 0 - no error
//             -1 - unknown variable


C_LONGINT:C283($0)  //error
C_TEXT:C284($1)  //selector
C_POINTER:C301($2)  //pointer to variable 

If (False:C215)
	C_LONGINT:C283(PPro_GetEditorVariable; $0)
	C_TEXT:C284(PPro_GetEditorVariable; $1)
	C_POINTER:C301(PPro_GetEditorVariable; $2)
End if 

C_TEXT:C284($sel)
$sel:=$1

C_POINTER:C301($pVar)
$pVar:=$2

C_LONGINT:C283($type)
$type:=Type:C295($pVar->)

$0:=0
Case of 
		//report related
	: ($sel="reportOld")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			$pVar->:=PP_Report
		End if 
		
	: ($sel="report")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			If (PP_ExtArea#0)
				C_TEXT:C284($report)
				C_LONGINT:C283($err)
				$err:=RW_SaveReport(PP_ExtArea; $report; 0x0000)  // third arg means it is a variable
				If ($err=0)
					$pVar->:=$report
				Else 
					$0:=$err
				End if 
			Else 
				$pVar->:=PP_Report
			End if 
		End if 
	: ($sel="callbackMethod")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			$pVar->:=PP_CallbackName
		End if 
	: ($sel="callbackRef")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			$pVar->:=PP_CallbackRef
		End if 
	: ($sel="reportModified")
		If ($type=Is longint:K8:6)
			$pVar->:=PP_ReportModified
		End if 
	: ($sel="lockedMode")
		If ($type=Is longint:K8:6)
			$pVar->:=PP_LockedMode
		End if 
	: ($sel="extArea")
		If ($type=Is longint:K8:6)
			$pVar->:=PP_ExtArea
		End if 
		
		//menu bar
	: ($sel="defaultMenuName")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			$pVar->:=PP_DefaultMenu
		End if 
	: ($sel="menuRef")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			$pVar->:=PP_MenuRef
		End if 
	: ($sel="oldMenuRef")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			$pVar->:=PP_OldmenuRef
		End if 
	: ($sel="restoreMenuID")
		If (($type=Is longint:K8:6))
			$pVar->:=PP_DefaultMenuID
		End if 
		
		//Window
	: ($sel="windowTop")
		If (($type=Is longint:K8:6) | ($type=Is real:K8:4))
			$pVar->:=PP_WindowTop
		End if 
	: ($sel="windowLeft")
		If (($type=Is longint:K8:6) | ($type=Is real:K8:4))
			$pVar->:=PP_WindowLeft
		End if 
	: ($sel="windowType")
		If (($type=Is longint:K8:6) | ($type=Is real:K8:4))
			$pVar->:=PP_WindowType
		End if 
	: ($sel="windowref")
		If (($type=Is longint:K8:6) | ($type=Is real:K8:4))
			$pVar->:=PP_WindowRef
		End if 
		
		// palette
	: ($sel="paletteList")
		If ($type=Is longint:K8:6)
			$pVar->:=PP_PaletteList
		End if 
		
		// format popup
	: ($sel="formatsList")
		If ($type=Is longint:K8:6)
			$pVar->:=PP_FormatPopup
		End if 
	Else 
		$0:=-1
End case 