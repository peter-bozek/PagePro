//%attributes = {}
// expand text into PP_TabAlign and PP_TabPosition arrays
// awaits text field with structure align:pos;

C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284($1)
End if 

C_TEXT:C284($t; $s)
$t:=$1

C_LONGINT:C283($i; $j)
While (Length:C16($t)>0)
	$i:=Position:C15(";"; $t)
	If ($i>0)
		$s:=Substring:C12($t; 1; $i-1)
		$t:=Substring:C12($t; $i+1)
	Else 
		$s:=$t
		$t:=""
	End if 
	
	$i:=Position:C15(":"; $s)
	If ($i>0)
		$j:=Find in array:C230(PP_TabAlignPopupMirror; Substring:C12($s; 1; $i-1))
		If ($j>0)
			APPEND TO ARRAY:C911(PP_TabAlign; PP_TabAlignPopup{$i})
			APPEND TO ARRAY:C911(PP_TabPosition; Num:C11(Substring:C12($s; $i+1); "."))
		End if 
	End if 
End while 