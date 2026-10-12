//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_RectangleProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		// variable PP_CurrentObject contains a line object
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($l)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Kind:K7906:2)
		If ($text="Rect")  // security
			PPro_ObjectPropertiesSubform($sel)  // hanadling of object common properties
			
			$s:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Rect:K7906:22)
			PPro_Util_RectToCoord($s; ->PP_Point1x; ->PP_Point1y; ->PP_Point2x; ->PP_Point2y)
			PP_Point2x:=PP_Point2x-PP_Point1x
			PP_Point2y:=PP_Point2y-PP_Point1y
			
			$l:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Flags:K7906:38)
			Case of 
				: ($l=15)
					PP_FramePopup:=1
				: ($l=0)
					PP_FramePopup:=2
				: ($l=5)
					PP_FramePopup:=3
				: ($l=10)
					PP_FramePopup:=4
				: ($l=1)
					PP_FramePopup:=5
				: ($l=2)
					PP_FramePopup:=6
				: ($l=4)
					PP_FramePopup:=7
				: ($l=8)
					PP_FramePopup:=8
				: ($l=11)
					PP_FramePopup:=9
				: ($l=7)
					PP_FramePopup:=10
				: ($l=14)
					PP_FramePopup:=11
				: ($l=13)
					PP_FramePopup:=12
					
				Else 
					PP_FramePopup:=2
			End case 
			
			PP_Columns:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_RectCols:K7906:40)
			PP_Rows:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_RectRows:K7906:39)
			
			PP_FillBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Fill:K7906:36)
			PP_LineWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Thickness:K7906:34)
			PP_LineColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_LineColor:K7906:35))
			PP_FrameOpacity:=PP_LineColor >> 24
			$color:=PP_LineColor  //& 0x00FFFFFF  // strip alpha
			OBJECT SET RGB COLORS:C628(*; "PP_LineColor"; 0x0000; $color)
			PP_FillColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_FillColor:K7906:37))
			PP_FillOpacity:=PP_FillColor >> 24
			C_LONGINT:C283($color)
			$color:=PP_FillColor  //& 0x00FFFFFF  // strip alpha
			OBJECT SET RGB COLORS:C628(*; "PP_FillColor"; 0x0000; $color)
		End if 
		
	: ($sel="set")
		C_TEXT:C284($text)
		$text:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Kind:K7906:2)
		If ($text="Rect")
			PPro_ObjectPropertiesSubform($sel)  // hanadling of object common properties
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  // start undo block
			
			C_LONGINT:C283($err; $temp; $flags)
			C_TEXT:C284($rect)
			
			Case of 
				: (PP_FramePopup=1)
					$flags:=15
				: (PP_FramePopup=2)
					$flags:=0
				: (PP_FramePopup=3)
					$flags:=5
				: (PP_FramePopup=4)
					$flags:=10
				: (PP_FramePopup=5)
					$flags:=1
				: (PP_FramePopup=6)
					$flags:=2
				: (PP_FramePopup=7)
					$flags:=4
				: (PP_FramePopup=8)
					$flags:=8
				: (PP_FramePopup=9)
					$flags:=11
				: (PP_FramePopup=10)
					$flags:=7
				: (PP_FramePopup=11)
					$flags:=14
				: (PP_FramePopup=12)
					$flags:=13
				Else 
					$flags:=0
			End case 
			
			$temp:=PPro_Util_CoordToRect(->$rect; PP_Point1x; PP_Point1y; PP_Point2x+PP_Point1x; PP_Point2y+PP_Point1y)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Rect:K7906:22; $rect)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Flags:K7906:38; $flags)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Thickness:K7906:34; PP_LineWidth)
			PP_LineColor:=((PP_FrameOpacity & 0x00FF) << 0x0018) | (PP_LineColor & 0x00FFFFFF)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_LineColor:K7906:35; PP_LineColor)
			PP_FillColor:=((PP_FillOpacity & 0x00FF) << 0x0018) | (PP_FillColor & 0x00FFFFFF)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FillColor:K7906:37; PP_FillColor)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Fill:K7906:36; PP_FillBtn)
			
			// modified 30.05.2011/15:00/SDS Software GmbH/Mußhoff
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_RectCols:K7906:40; PP_Columns)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_RectRows:K7906:39; PP_Rows)
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  // end undo block
		End if 
End case 