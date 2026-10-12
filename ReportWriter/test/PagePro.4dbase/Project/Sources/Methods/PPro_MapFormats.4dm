//%attributes = {"shared":true}
// PPro_MapFormats (report:T; currency:T; decimalSep:T) -> T
//
// map formats to system currency sign and decimal delimiter
// accepts report in text variable, currency symbol used by report and 
// decimal delimiter used by report
//
// returns report with mapped formats

C_TEXT:C284($0; $1; $2; $3)

If (False:C215)
	C_TEXT:C284(PPro_MapFormats; $0; $1; $2; $3)
End if 

C_TEXT:C284($currSign; $decDelim; $t)
C_LONGINT:C283($area; $err; $i; $j)

If (Count parameters:C259<2)
	$0:=$1
Else 
	If (Count parameters:C259=2)
		$currSign:=$2
		$decDelim:=""
	Else 
		$currSign:=$2
		$decDelim:=$3
	End if 
	
	ARRAY LONGINT:C221($alSections; 0)
	ARRAY LONGINT:C221($alObjects; 0)
	If (RW_NewReport($area; $1; 0)=0)  // 
		$err:=RW_GetObjects($area; $area; "body"; $alSections)
		For ($i; 0x0001; Size of array:C274($alSections))
			$err:=RW_GetObjects($area; $alSections{$i}; "objs"; $alObjects)
			For ($j; 0x0001; Size of array:C274($alObjects))
				PPro_MapFormatsObject($area; $alObjects{$j}; $currSign; $decDelim)
			End for 
		End for 
	End if 
	
	$err:=RW_SaveReport($area; $t; 0)
	$err:=RW_DeleteReport($area)
	
	$0:=$t
	
End if 