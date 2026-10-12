//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2)
C_BOOLEAN:C305($0)

If (False:C215)
	C_LONGINT:C283(PPro_CheckSublistState; $1; $2)
	C_BOOLEAN:C305(PPro_CheckSublistState; $0)
End if 

$0:=False:C215

If (Is a list:C621($1))
	C_LONGINT:C283($pos; $itemRef; $sublist)
	C_TEXT:C284($itemText)
	C_BOOLEAN:C305($expanded)
	$pos:=List item position:C629($1; $2)
	GET LIST ITEM:C378($1; $pos; $itemRef; $itemText; $sublist; $expanded)
	
	If ($itemRef=$2)
		$0:=$expanded
	End if 
End if 