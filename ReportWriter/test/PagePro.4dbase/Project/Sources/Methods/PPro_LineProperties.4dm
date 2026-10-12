//%attributes = {"invisible":true}

C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_LineProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1


Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($s; $text)
		C_REAL:C285($r1)
		C_LONGINT:C283($l; $color)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="line")  //security
			PPro_ObjectPropertiesSubform($sel)  //hanadling of object common properties
			
			$s:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Rect:K7906:22)
			PPro_Util_RectToCoord($s; ->PP_Point1x; ->PP_Point1y; ->PP_Point2x; ->PP_Point2y)
			$l:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Flags:K7906:38)
			If ($l=3)
				$r1:=PP_Point1y
				PP_Point1y:=PP_Point2y
				PP_Point2y:=$r1
			End if 
			
			PP_LineWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Thickness:K7906:34)
			PP_LineColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_LineColor:K7906:35))
			$color:=PP_LineColor & 0x00FFFFFF  // strip alpha
			OBJECT SET RGB COLORS:C628(*; "PP_LineColor"; 0x0000; $color)
			PP_LineOpacity:=(PP_LineColor >> 0x0018) & 0x00FF
			
		End if 
		
	: ($sel="set")
		C_TEXT:C284($text)
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="line")
			PPro_ObjectPropertiesSubform($sel)  //hanadling of object common properties
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			C_LONGINT:C283($err; $flags)
			C_TEXT:C284($rect)
			
			$flags:=PPro_Util_CoordToRect(->$rect; PP_Point1x; PP_Point1y; PP_Point2x; PP_Point2y)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Rect:K7906:22; $rect)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Flags:K7906:38; String:C10($flags))
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Thickness:K7906:34; PP_LineWidth)
			
			PP_LineColor:=((PP_LineOpacity & 0x00FF) << 0x0018) | (PP_LineColor & 0x00FFFFFF)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_LineColor:K7906:35; PP_LineColor)
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
		End if 
End case 