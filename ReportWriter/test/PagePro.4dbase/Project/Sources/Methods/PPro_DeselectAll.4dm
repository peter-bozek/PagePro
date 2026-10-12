//%attributes = {"invisible":true,"shared":true}
C_LONGINT:C283($1)

If (False:C215)
	C_LONGINT:C283(PPro_DeselectAll; $1)
End if 

C_LONGINT:C283($area; $i; $err)
$area:=$1
C_LONGINT:C283($lFalse)
$lFalse:=0

ARRAY LONGINT:C221($alObjects; 0x0000)
$err:=RW_GetObjects($area; $area; "selc"; $alObjects)
For ($i; 1; Size of array:C274($alObjects))
	$err:=RW_SetProperty($area; $alObjects{$i}; "selc"; ->$lFalse)
End for 