//%attributes = {"shared":true}
// Property value getter for string or text properties
// PPro_GetPropertyString (report; object; property) -> value

C_LONGINT:C283($1)  //report where is the object
C_LONGINT:C283($2)  //object to set property
C_TEXT:C284($3)  //property ID
C_TEXT:C284($0)  //value

If (False:C215)
	C_LONGINT:C283(PPro_GetPropertyString; $1)
	C_LONGINT:C283(PPro_GetPropertyString; $2)
	C_TEXT:C284(PPro_GetPropertyString; $3)
	C_TEXT:C284(PPro_GetPropertyString; $0)
End if 

C_TEXT:C284($value)
C_LONGINT:C283($err)
$err:=RW_GetProperty($1; $2; $3; ->$value)
$0:=$value

