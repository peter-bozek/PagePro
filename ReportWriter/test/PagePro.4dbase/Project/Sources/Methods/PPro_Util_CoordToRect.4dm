//%attributes = {"invisible":true}

C_LONGINT:C283($0)  //status
C_REAL:C285($2; $3; $4; $5)  //left, top, right, bottom
C_POINTER:C301($1)  //text with rectangle
C_BOOLEAN:C305($6)

If (False:C215)
	C_LONGINT:C283(PPro_Util_CoordToRect; $0)
	C_REAL:C285(PPro_Util_CoordToRect; $2; $3; $4; $5)  //left, top, right, bottom
	C_POINTER:C301(PPro_Util_CoordToRect; $1)  //text with rectangle
	C_BOOLEAN:C305(PPro_Util_CoordToRect; $6)  //normalize coordinates? default = true
End if 

C_TEXT:C284($s; $decimal)
C_REAL:C285($left; $top; $right; $bottom)
C_LONGINT:C283($flags; $l)
C_BOOLEAN:C305($normalize)

//defaults
$flags:=2
$left:=$2
$top:=$3
$right:=$4
$bottom:=$5
$normalize:=True:C214
If (Count parameters:C259>5)
	$normalize:=$6
End if 

Case of 
	: ($left=$right)  //vertical line
		$flags:=1
		
	: ($top=$bottom)  //horizontal line
		$flags:=0
		
	: (($top>$bottom) & ($left>$right))  // diagonal
		$flags:=2
		
	: ($left>$right)  // 
		$flags:=3
		
	: ($top>$bottom)
		$flags:=3
		
End case 

If ($normalize)
	If ($top>$bottom)
		$l:=$top
		$top:=$bottom
		$bottom:=$l
	End if 
	If ($left>$right)
		$l:=$left
		$left:=$right
		$right:=$l
	End if 
End if 

$decimal:=Substring:C12(String:C10(3/2); 2; 1)
$s:=Replace string:C233(String:C10($left); $decimal; ".")
$s:=$s+","+Replace string:C233(String:C10($top); $decimal; ".")
$s:=$s+","+Replace string:C233(String:C10($right); $decimal; ".")
$s:=$s+","+Replace string:C233(String:C10($bottom); $decimal; ".")

$1->:=$s
$0:=$flags
