//%attributes = {"shared":true}
// Property value getter for floating number properties
// PPro_GetPropertyNumber (report; object; property) -> value

C_LONGINT:C283($1)  //report where is the object
C_LONGINT:C283($2)  //object to set property
C_TEXT:C284($3)  //property ID
C_REAL:C285($0)  //value

If (False:C215)
	C_LONGINT:C283(PPro_GetPropertyNumber; $1)
	C_LONGINT:C283(PPro_GetPropertyNumber; $2)
	C_TEXT:C284(PPro_GetPropertyNumber; $3)
	C_REAL:C285(PPro_GetPropertyNumber; $0)
End if 

C_REAL:C285($value)
C_LONGINT:C283($err)
$err:=RW_GetProperty($1; $2; $3; ->$value)
$0:=$value

