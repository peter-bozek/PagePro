//%attributes = {"invisible":true}
C_TEXT:C284($0; $1)
C_POINTER:C301($2)

If (False:C215)
	C_TEXT:C284(PPro_JSON_GetNextChunk; $0; $1)
	C_POINTER:C301(PPro_JSON_GetNextChunk; $2)
End if 

C_TEXT:C284($t; $result)
$t:=$1
C_LONGINT:C283($idx)
$idx:=$2->

If ($t[[$idx]]="\"")
	// escaped string
	$idx:=$idx+1
	While ($idx<=Length:C16($t))
		If ($t[[$idx]]="\\")
			If ($t[[$idx+1]]="r")
				$result:=$result+Char:C90(Carriage return:K15:38)
			Else 
				$result:=$result+$t[[$idx+1]]
			End if 
			$idx:=$idx+1
			$2->:=$idx
		Else 
			$2->:=$idx
			If ($t[[$idx]]="\"")
				$idx:=Length:C16($t)
			Else 
				$result:=$result+$t[[$idx]]
			End if 
		End if 
		$idx:=$idx+1
	End while 
	
Else 
	C_LONGINT:C283($end)
	$end:=Position:C15(","; $t; $idx)
	$result:=Substring:C12($t; $idx; $end-$idx)
	$2->:=$2->+Length:C16($result)
End if 

$2->:=$2->+1
$0:=$result