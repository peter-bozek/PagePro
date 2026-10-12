//%attributes = {"shared":true}
// PPro_DeleteObject
// 

// created 04.12.2011/08:37/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————
C_LONGINT:C283($1; $AreaRef_L)  // RefID of the external area (0=PP_ExtArea)
C_LONGINT:C283($2; $ObjectRef_L)  // The object reference
C_LONGINT:C283($0; $Error_L)  // 0=No error otherwise #0

If (False:C215)
	C_LONGINT:C283(PPro_DeleteObject; $1)  // RefID of the external area (0=PP_ExtArea)
	C_LONGINT:C283(PPro_DeleteObject; $2)  // The object reference
	C_LONGINT:C283(PPro_DeleteObject; $0)  // 0=No error otherwise #0
End if 

// —— Initialisation ———————————————
$AreaRef_L:=$1
$ObjectRef_L:=$2
$Error_L:=0
// —— Main ———————————————

If ($AreaRef_L=0)
	$AreaRef_L:=PP_ExtArea
End if 

$Error_L:=RW_DeleteObject($AreaRef_L; $ObjectRef_L)

// —— Return ———————————————
$0:=$Error_L
