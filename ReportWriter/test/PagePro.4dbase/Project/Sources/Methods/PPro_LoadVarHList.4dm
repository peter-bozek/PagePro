//%attributes = {"shared":true}
// PPro_LoadVarHList (xmlText) -> hierarchical list reference
//
// Utility function that converts XML text to 
// 4D list with elements draggable to PagePro area

C_LONGINT:C283($0)
C_TEXT:C284($1)

If (False:C215)
	C_LONGINT:C283(PPro_LoadVarHList; $0)
	C_TEXT:C284(PPro_LoadVarHList; $1)  //text 
End if 

C_LONGINT:C283($list)
C_TEXT:C284($root; $XMLText)

$list:=New list:C375
$XMLText:=$1

If (Length:C16($XMLText)>0)
	$root:=DOM Parse XML variable:C720($XMLText)
	If (OK=1)
		C_LONGINT:C283($last)
		$last:=PPro_LoadVarList($list; $root; 0)
	End if 
End if 

$0:=$list