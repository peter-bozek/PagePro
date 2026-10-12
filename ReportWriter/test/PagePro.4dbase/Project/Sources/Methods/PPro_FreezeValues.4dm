//%attributes = {"invisible":true}
C_LONGINT:C283($1)

If (False:C215)
	C_LONGINT:C283(PPro_FreezeValues; $1)
End if 


C_LONGINT:C283($area; $i; $mode; $value; $err)
$area:=$1

$err:=RW_GetProperty($area; $area; PProp_Drawingmode:K7906:127; ->$mode)
$value:=7
$err:=RW_SetProperty($area; $area; PProp_Drawingmode:K7906:127; ->$value)  // force calculation of content

$err:=RW_Area_SaveUndo($area; 1)  //start undo block
ARRAY LONGINT:C221($alObjects; 0x0000)
$err:=RW_GetObjects($area; $area; "body"; $alObjects)
For ($i; 1; Size of array:C274($alObjects))
	PPro_FreezeSection($area; $alObjects{$i})
End for 
$err:=RW_Area_SaveUndo($area; 0)

$err:=RW_SetProperty($area; $area; PProp_Drawingmode:K7906:127; ->$mode)
