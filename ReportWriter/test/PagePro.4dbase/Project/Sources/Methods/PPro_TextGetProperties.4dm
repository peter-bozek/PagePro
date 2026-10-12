//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2)
C_POINTER:C301($3; $4; $5; $6; $7)

If (False:C215)
	C_LONGINT:C283(PPro_TextGetProperties; $1; $2)
	C_POINTER:C301(PPro_TextGetProperties; $3; $4; $5; $6; $7)
End if 

C_REAL:C285($size; $scale)
C_TEXT:C284($font)
C_LONGINT:C283($style; $prop; $styleID; $color)

$styleID:=PPro_GetPropertyInteger($1; $2; "styl")

$style:=0
If (PPro_GetPropertyInteger($1; $2; "styB")=1)
	$style:=$style ?+ 0
End if 

If (PPro_GetPropertyInteger($1; $2; "styI")=1)
	$style:=$style ?+ 1
End if 

If (PPro_GetPropertyInteger($1; $2; "styU")=1)
	$style:=$style ?+ 2
End if 

$font:=PPro_GetPropertyString($1; $2; "fnam")
$size:=PPro_GetPropertyNumber($1; $2; "size")
$scale:=PPro_GetPropertyNumber($1; $1; "scal")
$color:=PPro_Util_HexToNum(PPro_GetPropertyString($1; $2; "tclr"))
$3->:=$font
$4->:=$size
$5->:=$style
$6->:=$scale
$7->:=$color