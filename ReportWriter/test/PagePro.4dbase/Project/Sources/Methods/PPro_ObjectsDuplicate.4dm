//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area

If (False:C215)
	C_LONGINT:C283(PPro_ObjectsDuplicate; $1)
End if 

C_LONGINT:C283($area)

$area:=$1

PPro_XMLObjects($area; "d")