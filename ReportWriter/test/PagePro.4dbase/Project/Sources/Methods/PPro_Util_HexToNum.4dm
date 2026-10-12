//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_LONGINT:C283($0)

If (False:C215)
	C_TEXT:C284(PPro_Util_HexToNum; $1)
	C_LONGINT:C283(PPro_Util_HexToNum; $0)
End if 

C_LONGINT:C283($l; $i)
$l:=0
For ($i; 1; Length:C16($1))
	$l:=$l << 0x0004
	Case of 
		: (($1[[$i]]>="0") & ($1[[$i]]<="9"))
			$l:=$l+Num:C11($1[[$i]])
		: ($1[[$i]]="a")
			$l:=$l+10
		: ($1[[$i]]="b")
			$l:=$l+11
		: ($1[[$i]]="c")
			$l:=$l+12
		: ($1[[$i]]="d")
			$l:=$l+13
		: ($1[[$i]]="e")
			$l:=$l+14
		: ($1[[$i]]="f")
			$l:=$l+15
			
	End case 
End for 
$0:=$l