//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area
C_LONGINT:C283($2)  //object
C_BOOLEAN:C305($0)

If (False:C215)
	C_LONGINT:C283(PPro_HasTextAttributes; $1)  //area
	C_LONGINT:C283(PPro_HasTextAttributes; $2)  //object
	C_BOOLEAN:C305(PPro_HasTextAttributes; $0)
End if 

C_LONGINT:C283($area; $object; $err)
C_TEXT:C284($text)

$area:=$1
$object:=$2

$err:=RW_GetProperty(PP_ExtArea; $object; "kind"; ->$text)

$0:=(($text="Text") | ($text="Var") | ($text="Field") | ($text="Header") | ($text="Column"))