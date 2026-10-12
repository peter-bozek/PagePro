//%attributes = {"shared":true}
// Property value getter for integer properties
// PPro_GetPropertyInteger (report; object; property) -> value


C_LONGINT:C283($1)  //report where is the object
C_LONGINT:C283($2)  //object to set property
C_TEXT:C284($3)  //property ID
C_LONGINT:C283($0)  //value

If (False:C215)
	C_LONGINT:C283(PPro_GetPropertyInteger; $1)
	C_LONGINT:C283(PPro_GetPropertyInteger; $2)
	C_TEXT:C284(PPro_GetPropertyInteger; $3)
	C_LONGINT:C283(PPro_GetPropertyInteger; $0)
End if 

C_LONGINT:C283($value; $err)
$err:=RW_GetProperty($1; $2; $3; ->$value)
$0:=$value

