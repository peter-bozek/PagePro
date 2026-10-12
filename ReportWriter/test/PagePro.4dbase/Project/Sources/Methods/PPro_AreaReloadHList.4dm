//%attributes = {"shared":true}
C_LONGINT:C283($1)  //area

If (False:C215)
	C_LONGINT:C283(PPro_AreaReloadHList; $1)  //area
End if 

C_LONGINT:C283($err; $area)

$area:=$1

$err:=PPro_CreateDocumentTree($area; ->PP_XMLRoot)
