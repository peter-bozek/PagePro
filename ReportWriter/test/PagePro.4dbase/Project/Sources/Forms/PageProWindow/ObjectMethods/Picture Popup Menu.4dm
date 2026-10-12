C_LONGINT:C283($sel)

$sel:=Self:C308->

Case of 
	: ($sel=0)
		
	: ($sel=1)
		PPro_ObjectsAlign(PP_ExtArea; "top")
	: ($sel=2)
		PPro_ObjectsAlign(PP_ExtArea; "left")
	: ($sel=3)
		PPro_ObjectsAlign(PP_ExtArea; "bottom")
	: ($sel=4)
		PPro_ObjectsAlign(PP_ExtArea; "right")
		
	: ($sel=5)
		PPro_ObjectsAlign(PP_ExtArea; "hCenter")
	: ($sel=6)
		PPro_ObjectsAlign(PP_ExtArea; "vCenter")
		
		
End case 

Self:C308->:=1  //default icon
