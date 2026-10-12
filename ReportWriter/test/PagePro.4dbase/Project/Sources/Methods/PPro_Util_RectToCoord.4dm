//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_POINTER:C301($2; $3; $4; $5)  //left, top, right, bottom

If (False:C215)
	C_TEXT:C284(PPro_Util_RectToCoord; $1)
	C_POINTER:C301(PPro_Util_RectToCoord; $2; $3; $4; $5)
End if 


C_TEXT:C284($s; $val; $decimal; $delim)
C_LONGINT:C283($i)

$delim:=";"

$decimal:=Substring:C12(String:C10(3/2); 2; 1)
$s:=$1
$i:=Position:C15($delim; $s)
If ($i>0)
	$val:=Substring:C12($s; 1; $i-1)
	$2->:=Round:C94(Num:C11(Replace string:C233($val; "."; $decimal)); 3)
	$s:=Substring:C12($s; $i+1)
	$i:=Position:C15($delim; $s)
	If ($i>0)
		$val:=Substring:C12($s; 1; $i-1)
		$3->:=Round:C94(Num:C11(Replace string:C233($val; "."; $decimal)); 3)
		$s:=Substring:C12($s; $i+1)
		$i:=Position:C15($delim; $s)
		If ($i>0)
			$val:=Substring:C12($s; 1; $i-1)
			$4->:=Round:C94(Num:C11(Replace string:C233($val; "."; $decimal)); 3)
			$s:=Substring:C12($s; $i+1)
			$5->:=Round:C94(Num:C11(Replace string:C233($s; "."; $decimal)); 3)
		End if 
	End if 
End if 
