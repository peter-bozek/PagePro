//%attributes = {"invisible":true}
C_POINTER:C301($1)

If (False:C215)
	C_POINTER:C301(PPro_OpacityButtonScript; $1)
End if 

C_LONGINT:C283($left; $top; $button)
//OBJECT GET COORDINATES(*;"OpacityBtn";$left;$top;$right;$bottom) // does not work with included form
GET MOUSE:C468($left; $top; $button; *)
$1->:=PPro_EditOpacity($1->; $left; $top)