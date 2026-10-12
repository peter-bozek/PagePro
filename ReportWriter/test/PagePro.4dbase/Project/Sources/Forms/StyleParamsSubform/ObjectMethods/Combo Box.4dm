C_TEXT:C284($s)
C_LONGINT:C283($i)

$s:=PP_Fonts{0}
$i:=Find in array:C230(PP_Fonts; $s)
If ($i>0)
	PP_Fonts:=$i
End if 
