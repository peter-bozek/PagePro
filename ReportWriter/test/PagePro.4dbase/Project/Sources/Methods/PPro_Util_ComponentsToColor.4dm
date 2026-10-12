//%attributes = {"invisible":true}
C_LONGINT:C283($0)  //color
C_LONGINT:C283($1; $2; $3)  //RGB components of color with 0 - 255 range
C_REAL:C285($4)  //transparency 0 - 1

If (False:C215)
	C_LONGINT:C283(PPro_Util_ComponentsToColor; $0)
	C_LONGINT:C283(PPro_Util_ComponentsToColor; $1; $2; $3)
	C_REAL:C285(PPro_Util_ComponentsToColor; $4)
End if 

C_LONGINT:C283($l)
C_REAL:C285($t)

If (Count parameters:C259<4)
	
	$t:=1
Else 
	$t:=$4
End if 

$l:=$t*255
$l:=(($l & 0x00FF) << 24)+(($1 & 0x00FF) << 0x0010)+(($2 & 0x00FF) << 0x0008)+($3 & 0x00FF)
$0:=$l