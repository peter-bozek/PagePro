//%attributes = {"shared":true}
// PPro_ToPt (value; unit {;area}) -> value in points
//        converts value in units to value in points
//        output is rounded to 2 decimals
//
//    unit can be one of : 'mm'' 'cm', 'inch,'"', 'pt' or 'area'
//   'area' means default unit of the area (unit of rulers)

C_REAL:C285($0; $1)
C_TEXT:C284($2)
C_LONGINT:C283($3)

If (False:C215)
	C_REAL:C285(PPro_ToPt; $0; $1)
	C_TEXT:C284(PPro_ToPt; $2)
	C_LONGINT:C283(PPro_ToPt; $3)
End if 

C_REAL:C285($value)
C_LONGINT:C283($unit)

$0:=0
If ($1#0)
	
	Case of 
		: ($2="cm")
			$value:=$1*72/2.54
		: ($2="mm")
			$value:=$1*72/25.4
		: ($2="inch")
			$value:=$1*72
		: ($2="\"")
			$value:=$1*72
		: ($2="pt")
			$value:=$1
		: ($2="area")
			C_LONGINT:C283($area)
			$area:=PP_ExtArea
			If (Count parameters:C259>2)
				If ($3#0)
					$area:=$3
				End if 
			End if 
			$unit:=PPro_GetPropertyInteger($area; $area; PProp_RulerUnit:K7906:115)
			Case of 
				: ($unit<2)
					$value:=$1
				: ($unit=2)  // mm
					$value:=$1*72/25.4
				: ($unit=3)  // inch
					$value:=$1*72
			End case 
	End case 
	$0:=Round:C94($value; 2)
End if 