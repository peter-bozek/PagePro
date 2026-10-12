//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area

If (False:C215)
	C_LONGINT:C283(PPro_ScaleUp; $1)  //area
End if 

C_REAL:C285($scale)
C_LONGINT:C283($err; $l; $area)

$area:=$1

$err:=RW_GetProperty($area; $area; "scal"; ->$scale)
$l:=(4*$scale+1)
$scale:=$l/4
$err:=RW_SetProperty($area; $area; "scal"; ->$scale)
$err:=RW_GetProperty($area; $area; "scal"; ->$scale)
PP_Scaling:=Round:C94($scale*100; 0x0002)