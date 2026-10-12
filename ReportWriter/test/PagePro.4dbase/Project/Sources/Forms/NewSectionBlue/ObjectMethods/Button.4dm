C_LONGINT:C283($newSection; $err)

Case of 
	: (PP_ReportTypes=1)
		$newSection:=PPro_ReportAddSection("header"; PP_ExtArea; 0x000F; "Header"; 48)
		If (PP_Add2Section=1)
			$newSection:=PPro_ReportAddSection("footer"; PP_ExtArea; 0x000F; "Footer"; 48)
		End if 
		
	: (PP_ReportTypes=2)
		$newSection:=PPro_ReportAddSection("breakHeader"; PP_ExtArea; 0x000F; "Break Header"; 48)
		If ((PP_Source#"") & (PP_BreakOnPopup#0))
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $newSection; PProp_BreakType:K7906:96; PP_BreakOnPopup-1)
			$err:=PPro_SetPropertyString(PP_ExtArea; $newSection; PProp_BreakOn:K7906:95; PP_Source)
		End if 
		If (PP_Add2Section=1)
			$newSection:=PPro_ReportAddSection("breakFooter"; PP_ExtArea; 0x000F; "Break Footer"; 48)
			If ((PP_Source#"") & (PP_BreakOnPopup#0))
				$err:=PPro_SetPropertyInteger(PP_ExtArea; $newSection; PProp_BreakType:K7906:96; PP_BreakOnPopup-1)
				$err:=PPro_SetPropertyString(PP_ExtArea; $newSection; PProp_BreakOn:K7906:95; PP_Source)
			End if 
		End if 
		
	: (PP_ReportTypes=3)
		$newSection:=PPro_ReportAddSection("breakFooter"; PP_ExtArea; 0x000F; "Break Footer"; 48)
		If ((PP_Source#"") & (PP_BreakOnPopup#0))
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $newSection; PProp_BreakType:K7906:96; PP_BreakOnPopup-1)
			$err:=PPro_SetPropertyString(PP_ExtArea; $newSection; PProp_BreakOn:K7906:95; PP_Source)
		End if 
		
	: (PP_ReportTypes=4)
		$newSection:=PPro_ReportAddSection("footer"; PP_ExtArea; 0x000F; "Footer"; 48)
		
	: (PP_ReportTypes=5)
		$newSection:=PPro_ReportAddSection("watermark"; PP_ExtArea; 0x000F; "Watermark"; 0)
		
		
End case 