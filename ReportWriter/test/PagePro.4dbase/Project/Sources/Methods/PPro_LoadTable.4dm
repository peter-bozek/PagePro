//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2; $3)
C_LONGINT:C283($err; $i; $l; $lHeaders; $lColumns; $lGroup; $count)
C_TEXT:C284($type; $name; $rect)
$err:=RW_GetProperty($1; $2; "hdrn"; ->$count)  // num header lines
ARRAY LONGINT:C221($alObjects; 0x0000)

$lHeaders:=New list:C375
For ($l; 0x0001; $count)
	$lColumns:=New list:C375
	$err:=RW_GetObjects($1; $2; "H"+String:C10($l-0x0001; "000"); $alObjects)
	For ($i; 0x0001; Size of array:C274($alObjects))
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "kind"; ->$type)
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "data"; ->$name)
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "rect"; ->$rect)
		$name:=$type+" ("+$rect+") - "+$name
		$lGroup:=0x0000
		APPEND TO LIST:C376($lColumns; $name; $alObjects{$i}; $lGroup; False:C215)
	End for 
	APPEND TO LIST:C376($lHeaders; "Line "+String:C10($l); PPro_Util_TypeToLong("heco"); $lColumns; False:C215)
End for 
APPEND TO LIST:C376($3; "Headers"; PPro_Util_TypeToLong("head"); $lHeaders; False:C215)

$lColumns:=New list:C375
$err:=RW_GetObjects($1; $2; "colu"; $alObjects)
For ($i; 0x0001; Size of array:C274($alObjects))
	$err:=RW_GetProperty($1; $alObjects{$i}; "kind"; ->$type)
	$err:=RW_GetProperty($1; $alObjects{$i}; "src"; ->$name)
	$err:=RW_GetProperty($1; $alObjects{$i}; "rect"; ->$rect)
	$name:=$type+" ("+$rect+") - "+$name
	$lGroup:=0x0000
	APPEND TO LIST:C376($lColumns; $name; $alObjects{$i}; $lGroup; False:C215)
End for 
APPEND TO LIST:C376($3; "Columns"; PPro_Util_TypeToLong("colu"); $lColumns; False:C215)
