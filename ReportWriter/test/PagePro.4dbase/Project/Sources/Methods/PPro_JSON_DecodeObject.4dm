//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_POINTER:C301($2; $3)

If (False:C215)
	C_TEXT:C284(PPro_JSON_DecodeObject; $1)
	C_POINTER:C301(PPro_JSON_DecodeObject; $2; $3)
End if 

C_TEXT:C284($t)
$t:=$1

ARRAY TEXT:C222($aNames; 0)
ARRAY TEXT:C222($aValues; 0)

C_LONGINT:C283($offset; $len)
$offset:=1
$len:=Length:C16($t)

C_LONGINT:C283($size; $pos)
$size:=0  // next array item

While ($offset<$len)
	$pos:=PPro_JSON_GetNextChar($t; ":"; $offset)
	If (($pos>0) & ($pos<$len))  // something found
		$size:=$size+1
		INSERT IN ARRAY:C227($aNames; $size; 1)
		INSERT IN ARRAY:C227($aValues; $size; 1)
		$aNames{$size}:=Substring:C12($t; $pos)
		$offset:=$pos+1
		$aValues{$size}:=PPro_JSON_GetNextChunk($t; ->$offset)
	End if 
End while 

COPY ARRAY:C226($aNames; $2->)
COPY ARRAY:C226($aValues; $3->)
