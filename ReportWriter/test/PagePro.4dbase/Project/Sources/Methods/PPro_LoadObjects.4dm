//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2; $3)

If (False:C215)
	C_LONGINT:C283(PPro_LoadObjects; $1; $2; $3)
End if 

C_LONGINT:C283($err; $i; $lGroup)
C_TEXT:C284($type; $name; $rect)

ARRAY LONGINT:C221($alObjects; 0x0000)

$err:=RW_GetObjects($1; $2; "objs"; $alObjects)
For ($i; 0x0001; Size of array:C274($alObjects))
	$err:=RW_GetProperty($1; $alObjects{$i}; "kind"; ->$type)
	$err:=RW_GetProperty($1; $alObjects{$i}; "name"; ->$name)
	$err:=RW_GetProperty($1; $alObjects{$i}; "rect"; ->$rect)
	If (($type="Group") | ($type="Table"))
		$lGroup:=New list:C375
	Else 
		$lGroup:=0x0000
	End if 
	$name:=$type+" ("+$rect+") - "+$name
	APPEND TO LIST:C376($3; $name; $alObjects{$i}; $lGroup; False:C215)
	SET LIST ITEM PROPERTIES:C386($3; 0; False:C215; 0; Use PicRef:K28:4+PPro_ObjectGetIcon($1; $alObjects{$i}))
	If ($lGroup#0x0000)
		If ($type="Group")
			PPro_LoadObjects($1; $alObjects{$i}; $lGroup)
		Else 
			PPro_LoadTable($1; $alObjects{$i}; $lGroup)
		End if 
	End if 
End for 