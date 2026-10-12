//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_PictureProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($l)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Pict")  //security
			PPro_ObjectPropertiesSubform($sel)  //handling of object's common properties
			
			PP_PictFormatPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41)+1
			
			$s:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Rect:K7906:22)
			PPro_Util_RectToCoord($s; ->PP_Point1x; ->PP_Point1y; ->PP_Point2x; ->PP_Point2y)
			PP_Point2x:=PP_Point2x-PP_Point1x
			PP_Point2y:=PP_Point2y-PP_Point1y
			
			PP_FrameBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42)
			PP_FrameOffset:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43)
			PP_LineWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44)
			PP_LineColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45))
			PP_FrameOpacity:=(PP_LineColor & 0xFF000000) >> 0x0018
			C_LONGINT:C283($color)
			$color:=PP_LineColor & 0x00FFFFFF  // strip alpha
			OBJECT SET RGB COLORS:C628(*; "PP_LineColor"; 0; $color)
			
			C_REAL:C285($width; $height; $size)
			$width:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Width:K7906:6)
			$height:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Height:K7906:7)
			PP_ImageSize:=String:C10($width)+" x "+String:C10($height)
			$size:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; "imgs")
			PP_PictSize:=String:C10(Round:C94($size/1024; 0))
			
			// v1.4
			PP_ObjectRotation:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_ObjectRotation:K7906:129)
			//PP_ObjectSkewing:=PPro_GetPropertyNumber (PP_ExtArea;PP_CurrentObject;PProp_ObjectSkewing)
			//PP_ObjectMirroring:=PPro_GetPropertyNumber (PP_ExtArea;PP_CurrentObject;PProp_ObjectMirroring)
			PP_Dimming:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BackColor:K7906:72) >> 24
			
		End if 
		
	: ($sel="set")
		C_TEXT:C284($text)
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Pict")
			PPro_ObjectPropertiesSubform($sel)  //handling of object's common properties
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			C_LONGINT:C283($err; $temp; $flags)
			C_TEXT:C284($rect)
			
			If (PP_PictFormatPopup=0)
				PP_PictFormatPopup:=1
			End if 
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Format:K7906:41; PP_PictFormatPopup-1)
			
			$temp:=PPro_Util_CoordToRect(->$rect; PP_Point1x; PP_Point1y; PP_Point2x+PP_Point1x; PP_Point2y+PP_Point1y)
			$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Rect:K7906:22; $rect)
			
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44; PP_LineWidth)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43; PP_FrameOffset)
			PP_LineColor:=((PP_FrameOpacity & 0x00FF) << 0x0018)+(PP_LineColor & 0x00FFFFFF)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45; PP_LineColor)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42; PP_FrameBtn)
			
			// v1.4
			$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_ObjectRotation:K7906:129; PP_ObjectRotation)
			//$err:=PPro_SetPropertyNumber (PP_ExtArea;PP_CurrentObject;PProp_ObjectSkewing;PP_ObjectSkewing)
			//$err:=PPro_SetPropertyNumber (PP_ExtArea;PP_CurrentObject;PProp_ObjectMirroring;PP_ObjectMirroring)
			// $err:=PPro_SetPropertyInteger (PP_ExtArea;PP_CurrentObject;PProp_BackColor;PP_Dimming << 24)
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
		End if 
End case 