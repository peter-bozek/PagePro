//%attributes = {"shared":true}
// Opens windows with standard PagePro editor
// PPro_EditorFull (
//      report;           text with report definition
//      callback;         name of callback method
//      reportReference;  string passed to callback method
//      menuBar)          optional - menu bar number to restore on exit
//    -> modified report

C_TEXT:C284($1)  //report to edit
C_TEXT:C284($2)  //callback to call
C_TEXT:C284($3)  //reference passed to callback
C_LONGINT:C283($4)  //menu bar
C_TEXT:C284($0)  //returned report

If (False:C215)
	C_TEXT:C284(PPro_EditorFull; $1; $2; $3)  //report to edit, callback, reference
	C_LONGINT:C283(PPro_EditorFull; $4)  //menu bar to restore
	C_TEXT:C284(PPro_EditorFull; $0)  //returned report
End if 

Compiler_PProArrays
Compiler_PProVariables

//set component to their default values
PPro_ComponentInitVars

If (Count parameters:C259>0)
	PP_Report:=$1
	If (Count parameters:C259>1)
		PP_CallbackName:=$2
		If (Count parameters:C259>2)
			PP_CallbackRef:=$3
			If (Count parameters:C259>3)
				PP_DefaultMenuID:=$4
			End if 
		End if 
	End if 
End if 

//create and install virtual structure
PPro_VirtualStructCreate

//create and install component default menu
PPro_MenuCreate

//open window
PPro_WindowOpen

//display build in component dialog
DIALOG:C40("PageProWindow")

PPro_WindowClose

PPro_MenuClose

PPro_VirtualStructClose

//following line will return original report 
//if editor did not modified the PP_ExtArea variable
//or modified report

$0:=PP_Report

