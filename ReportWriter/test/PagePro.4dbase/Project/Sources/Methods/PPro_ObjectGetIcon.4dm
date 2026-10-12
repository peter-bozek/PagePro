//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2; $0)

If (False:C215)
	C_LONGINT:C283(PPro_ObjectGetIcon; $1; $2; $0)
End if 

C_LONGINT:C283($err; $area; $object)
C_TEXT:C284($text)

$area:=$1
$object:=$2

$err:=RW_GetProperty($area; $object; "kind"; ->$text)
Case of 
	: ($text="Text")
		$0:=12105
	: ($text="Var")
		$0:=12111
	: ($text="Field")
		$0:=12112
	: ($text="Line")
		$0:=12108
	: ($text="Rect")
		$0:=12109
	: ($text="Oval")
		$0:=12110
	: ($text="Pic")
		$0:=12107
	: ($text="Table")
		$0:=12106
End case 