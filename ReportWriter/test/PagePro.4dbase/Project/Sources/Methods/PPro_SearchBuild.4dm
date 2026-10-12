//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_SearchBuild; $1)
End if 

C_TEXT:C284($value)
$value:=$1

ARRAY LONGINT:C221(PP_SearchPosition; 0)
ARRAY LONGINT:C221(PP_SearchLen; 0)

C_LONGINT:C283($pos; $foundLength)
$pos:=Position:C15($value; PP_Script; $pos; $foundLength)
While ($pos>0)
	C_LONGINT:C283($i)
	$i:=Size of array:C274(PP_SearchPosition)+1
	INSERT IN ARRAY:C227(PP_SearchPosition; $i; 1)
	INSERT IN ARRAY:C227(PP_SearchLen; $i; 1)
	PP_SearchPosition{$i}:=$pos
	PP_SearchLen{$i}:=$foundLength
	$pos:=Position:C15($value; PP_Script; $pos+$foundLength; $foundLength)
End while 
