Case of 
	: (Form event code:C388=On Load:K2:1)
		
		STRING LIST TO ARRAY:C511(13009; PP_RulerUnits)
		STRING LIST TO ARRAY:C511(13010; PP_UnitRounding)
		
		// pB v1.3 checking proper decimal char
		PPro_CheckDecimal(->PP_UnitRounding)
		
		C_LONGINT:C283($value; $err)
		
		$value:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_RulerUnit:K7906:115)
		If ($value=0)
			$value:=1
		End if 
		PP_RulerUnits:=$value
		
		$value:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Rounding:K7906:125)
		Case of 
			: ($value=1)
				$value:=1
			: ($value=10)
				$value:=2
			: ($value=100)
				$value:=3
				
			Else 
				$value:=1
		End case 
		PP_UnitRounding:=$value
		
		PP_ProximityLimit:=PPro_GetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_Proximity:K7906:126)
		PP_GridSize:=PPro_GetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_GridSize:K7906:117)
		
	: (Form event code:C388=On Validate:K2:3)
		
		C_LONGINT:C283($value; $err)
		If (PP_RulerUnits=0)
			PP_RulerUnits:=1
		End if 
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_RulerUnit:K7906:115; PP_RulerUnits)
		
		PP_Units:=PP_RulerUnits
		PPro_AreaFormData(PP_ExtArea)
		
		Case of 
			: (PP_UnitRounding=1)
				$value:=1
			: (PP_UnitRounding=2)
				$value:=10
			: (PP_UnitRounding=3)
				$value:=100
			Else 
				$value:=1
		End case 
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Rounding:K7906:125; $value)
		
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_Proximity:K7906:126; PP_ProximityLimit)
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_GridSize:K7906:117; PP_GridSize)
		
End case 