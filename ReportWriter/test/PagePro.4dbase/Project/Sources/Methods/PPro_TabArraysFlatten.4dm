//%attributes = {}
// flatten data in PP_TabAlign and PP_TabPosition arrays
// returns text field with structure align:pos;

C_TEXT:C284($0)

If (False:C215)
	C_TEXT:C284(PPro_TabArraysFlatten; $0)
End if 

$0:=""

C_LONGINT:C283($i; $j)
For ($i; 1; Size of array:C274(PP_TabAlign))
	$j:=Find in array:C230(PP_TabAlignPopup; PP_TabAlign{$i})
	If ($j>0)
		$0:=$0+PP_TabAlignPopupMirror{$j}+":"+Replace string:C233(String:C10(Round:C94(PP_TabPosition{$i}; 3)); ","; ".")+";"
	End if 
End for 