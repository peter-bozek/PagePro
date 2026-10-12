//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_POINTER:C301($2; $3; $4; $5)  //RGBA

If (False:C215)
	C_TEXT:C284(PPro_Util_ColorToComponents; $1)
	C_POINTER:C301(PPro_Util_ColorToComponents; $2; $3; $4; $5)
End if 

C_TEXT:C284($s; $val)
C_LONGINT:C283($i)

$s:=$1
$i:=Position:C15("#"; $s)

If ($i>0)
	$s:=Substring:C12($s; $i+1)
	
	If (Count parameters:C259>4)
		$val:=Substring:C12($s; 1; 2)
		$5->:=PPro_Util_HexToNum($val)/255
	End if 
	$val:=Substring:C12($s; 3; 2)
	$2->:=PPro_Util_HexToNum($val)
	$val:=Substring:C12($s; 5; 2)
	$3->:=PPro_Util_HexToNum($val)
	$val:=Substring:C12($s; 7; 2)
	$4->:=PPro_Util_HexToNum($val)
	
End if 
