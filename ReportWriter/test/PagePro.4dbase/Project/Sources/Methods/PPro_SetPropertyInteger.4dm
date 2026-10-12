//%attributes = {"shared":true}
// Property value setter for integer properties
// PPro_SetPropertyInteger (report; object; property; value) -> error


C_LONGINT:C283($1)  //report where is the object
C_LONGINT:C283($2)  //object to set property
C_TEXT:C284($3)  //property ID
C_LONGINT:C283($4)  //value
C_LONGINT:C283($0)  //error code

If (False:C215)
	C_LONGINT:C283(PPro_SetPropertyInteger; $1)
	C_LONGINT:C283(PPro_SetPropertyInteger; $2)
	C_TEXT:C284(PPro_SetPropertyInteger; $3)
	C_LONGINT:C283(PPro_SetPropertyInteger; $4)
	C_LONGINT:C283(PPro_SetPropertyInteger; $0)
End if 

C_LONGINT:C283($value)
$value:=$4
$0:=RW_SetProperty($1; $2; $3; ->$value)

