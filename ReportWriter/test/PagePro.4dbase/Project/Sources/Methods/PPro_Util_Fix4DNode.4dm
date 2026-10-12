//%attributes = {"shared":true}
// PPro_Util_Fix4DNode (xmlText) -> xmlText
//
// fixes problem with 4D command DOM EXPORT TO VAR 
// that sometimes returns malformed xml text

C_TEXT:C284($1; $0)

If (False:C215)
	C_TEXT:C284(PPro_Util_Fix4DNode; $1; $0)
End if 

If (Length:C16($1)<2)
	$0:=""
Else 
	C_TEXT:C284($t)
	$t:=$1
	// fixes two 4D bugs:
	// 1. space , /l or /n as first character:
	C_LONGINT:C283($code)
	$code:=Character code:C91($t[[1]])
	If ($code<33)
		$t:=Substring:C12($t; 2)
	End if 
	// 2. node does not start with <
	If ($t[[1]]#"<")
		$t:="<"+$t
	End if 
	
	$0:=$t
End if 