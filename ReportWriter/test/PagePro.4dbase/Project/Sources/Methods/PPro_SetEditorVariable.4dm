//%attributes = {"shared":true}
// Setter to internal component variables
// PPro_SetEditorVariable (
//      selector;          string with variable name
//      pointerToVar)      pointer to variable in that will contain returned value
//    -> error: 0 - no error
//             -1 - unknown variable

C_LONGINT:C283($0)  //error
C_TEXT:C284($1)  //selector
C_POINTER:C301($2)  //pointer to variable 

// allow to set following internal variables:
//    - report (text)
//    - callbackMethod (text)
//    - callbackRef (text)
//    - reportModified (longint)


If (False:C215)
	C_LONGINT:C283(PPro_SetEditorVariable; $0)
	C_TEXT:C284(PPro_SetEditorVariable; $1)
	C_POINTER:C301(PPro_SetEditorVariable; $2)
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
	: ($sel="report")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			PP_Report:=$pVar->
			If (PP_ExtArea#0)
				C_LONGINT:C283($err)
				$err:=RW_ParseReport(PP_ExtArea; PP_Report; 0x0000)  // 
				PPro_InitDocument
			End if 
		End if 
	: ($sel="callbackMethod")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			PP_CallbackName:=$pVar->
		End if 
	: ($sel="callbackRef")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			PP_CallbackRef:=$pVar->
		End if 
	: ($sel="reportModified")
		If ($type=Is longint:K8:6)
			PP_ReportModified:=$pVar->
		End if 
	: ($sel="lockedMode")
		If ($type=Is longint:K8:6)
			PP_LockedMode:=$pVar->
		End if 
		
		//menu bar
	: ($sel="defaultMenuName")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			PP_DefaultMenu:=$pVar->
		End if 
		//: ($sel="menuRef") // read only
		//If (($type=Is Text) | ($type=Is String Var))
		//PP_MenuRef:=$pVar->
		//End if 
	: ($sel="oldMenuRef")
		If (($type=Is text:K8:3) | ($type=Is string var:K8:2))
			PP_OldmenuRef:=$pVar->
		End if 
	: ($sel="restoreMenuID")
		If ($type=Is longint:K8:6)
			PP_DefaultMenuID:=$pVar->
		End if 
		
		
		//Window
	: ($sel="windowTop")
		If (($type=Is longint:K8:6) | ($type=Is real:K8:4))
			PP_WindowTop:=$pVar->
		End if 
	: ($sel="windowLeft")
		If (($type=Is longint:K8:6) | ($type=Is real:K8:4))
			PP_WindowLeft:=$pVar->
		End if 
	: ($sel="windowType")
		If (($type=Is longint:K8:6) | ($type=Is real:K8:4))
			PP_WindowType:=$pVar->
		End if 
		//: ($sel="windowref") // read only
		//If (($type=Is LongInt) | ($type=Is Real))
		//PP_WindowRef:=$pVar->
		//End if 
		
		// palette
	: ($sel="paletteList")
		If ($type=Is longint:K8:6)
			PP_PaletteList:=$pVar->
		End if 
		
		// format popup
	: ($sel="formatsList")
		If ($type=Is longint:K8:6)
			PP_FormatPopup:=$pVar->
		End if 
		
	Else 
		$0:=-1
End case 