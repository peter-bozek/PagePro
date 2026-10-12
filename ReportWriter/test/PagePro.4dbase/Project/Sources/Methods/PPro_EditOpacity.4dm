//%attributes = {"invisible":true}
C_LONGINT:C283($2; $3)
C_REAL:C285($0; $1)

If (False:C215)
	C_LONGINT:C283(PPro_EditOpacity; $2; $3)
	C_REAL:C285(PPro_EditOpacity; $0; $1)
End if 

C_LONGINT:C283($posX; $posY)

PP_Opacity:=$1*100/255
$posX:=$2
$posY:=$3

C_LONGINT:C283($win)
$win:=Open window:C153($posX; $posY; $posX+154; $posY+28; Pop up window:K34:14)
DIALOG:C40("Opacity")
CLOSE WINDOW:C154($win)
If (OK=1)
	$0:=PP_Opacity*255/100
Else 
	$0:=$1
End if 