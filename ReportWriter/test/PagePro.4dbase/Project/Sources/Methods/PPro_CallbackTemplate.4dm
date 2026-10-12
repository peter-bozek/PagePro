//%attributes = {}
// PPro_CallbackTemplate (
// Callback function should accept following parameters:
//
//  C_TEXT($1)    areaReference - parameter passed PPro_Editor... method
//  C_TEXT($3)    eventType groupS events into common themes
//  C_TEXT($3)    event
//  C_LONGINT($4) report
//  C_LONGINT($4) object or window, depending on event
//  C_BOOLEAN($0)  event handled - return false if event should be handled by PagePro, 
//                 true if event was handled by callback

C_BOOLEAN:C305($0)  //event handled
C_TEXT:C284($1; $2; $3)  // reference, type of event, event
C_LONGINT:C283($4; $5)  // paramters: report and object or window

If (False:C215)
	C_BOOLEAN:C305(PPro_CallbackTemplate; $0)  //event handled
	C_TEXT:C284(PPro_CallbackTemplate; $1; $2; $3)  // reference, type of event, event
	C_LONGINT:C283(PPro_CallbackTemplate; $4; $5)  // paramters: report and object or window
End if 

$0:=True:C214  //true means evet is not handled and default behavior is used

