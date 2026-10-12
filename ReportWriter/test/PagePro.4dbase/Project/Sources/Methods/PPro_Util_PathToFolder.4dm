//%attributes = {"invisible":true}
C_TEXT:C284($0; $1)

If (False:C215)
	C_TEXT:C284(PPro_Util_PathToFolder$0; $1)
End if 

C_TEXT:C284($path)
$path:=$1

C_TEXT:C284($delimiter)
C_LONGINT:C283($platform)
_O_PLATFORM PROPERTIES:C365($platform)
If ($platform=Windows:K25:3)
	$delimiter:="\\"
Else 
	$delimiter:=":"
End if 

C_LONGINT:C283($charIdx)
For ($charIdx; Length:C16($path)-1; 1; -1)
	If ($path[[$charIdx]]=$delimiter)
		$path:=Substring:C12($path; 1; $charIdx)
		$charIdx:=1
	End if 
End for 

$0:=$path