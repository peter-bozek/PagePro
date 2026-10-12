//%attributes = {"invisible":true}
C_BOOLEAN:C305($0)  //event handled
C_TEXT:C284($1; $2)  // type of event, event
C_LONGINT:C283($3; $4)  // paramters: report and object or window

If (False:C215)
	C_BOOLEAN:C305(PPro_EditorCallback; $0)  // event handled
	C_TEXT:C284(PPro_EditorCallback; $1; $2)  // type of event, event
	C_LONGINT:C283(PPro_EditorCallback; $3; $4)  // paramters: report and object or window
End if 

$0:=True:C214  //true means evet is not handled and default behavior is used

C_BOOLEAN:C305($return)

If (PP_CallbackName#"")
	EXECUTE METHOD:C1007(PP_CallbackName; $return; PP_CallbackRef; $1; $2; $3; $4)
	$0:=$return
End if 