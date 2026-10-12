//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //report where is the object
C_LONGINT:C283($2)  //object to set property
C_TEXT:C284($3)  //property ID
C_LONGINT:C283($0)  //error code

If (False:C215)
	C_LONGINT:C283(PPro_ToggleProperty; $1)
	C_LONGINT:C283(PPro_ToggleProperty; $2)
	C_TEXT:C284(PPro_ToggleProperty; $3)
	C_LONGINT:C283(PPro_ToggleProperty; $0)
End if 

C_LONGINT:C283($value; $err)  //only longint properties can be toggled
$err:=RW_GetProperty($1; $2; $3; ->$value)
$value:=1-$value
$err:=RW_SetProperty($1; $2; $3; ->$value)
