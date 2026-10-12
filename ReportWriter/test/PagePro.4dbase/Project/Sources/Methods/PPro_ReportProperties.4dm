//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_ReportProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		//variable PP_CurrentObject contains a correct object
		C_TEXT:C284($s; $text; $rect)
		C_LONGINT:C283($err; $i; $j; $report; $temp)
		
		$report:=OBJECT Get pointer:C1124(Object subform container:K67:4)->
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Report")  //
			//PP_Width:=PPro_GetPropertyNumber (PP_ExtArea;$report;PProp_Width)
			//PP_Height:=PPro_GetPropertyNumber (PP_ExtArea;$report;PProp_Height)
			
			PP_PaperBtn:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_PhysicalPaper:K7906:8)
			PP_StaticReport:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_StaticReport:K7906:5)
			
			//$rect:=PPro_GetPropertyString (PP_ExtArea;$report;PProp_Margins)
			//PPro_Util_RectToCoord ($rect;->PP_Left;->PP_Top;->PP_Right;->PP_Bottom)
			
			C_REAL:C285($left; $top; $right; $bottom; $height; $width)
			$width:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Width:K7906:6)
			$height:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Height:K7906:7)
			PPro_ToolbarPositionSet(->PP_Width; $width; PP_Units)
			PPro_ToolbarPositionSet(->PP_Height; $height; PP_Units)
			
			$s:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Margins:K7906:9)
			PPro_Util_RectToCoord($s; ->$left; ->$top; ->$right; ->$bottom)
			PPro_ToolbarPositionSet(->PP_Left; $left; PP_Units)
			PPro_ToolbarPositionSet(->PP_Top; $top; PP_Units)
			PPro_ToolbarPositionSet(->PP_Right; $right; PP_Units)
			PPro_ToolbarPositionSet(->PP_Bottom; $bottom; PP_Units)
			
			// modified pB 2014-6
			// added properties of v1.4
			// PP_LabelReport:=PPro_GetPropertyInteger (PP_ExtArea;$report;PProp_LabelReport)
			PP_LabelReport:=0
			OBJECT SET ENTERABLE:C238(PP_LabelReport; (PP_StaticReport=1))
			OBJECT SET ENTERABLE:C238(*; "PP_Label@"; PP_LabelReport=1)
			
			//PP_GridX:=PPro_GetPropertyInteger (PP_ExtArea;$report;PProp_GridX)
			//PP_GridY:=PPro_GetPropertyInteger (PP_ExtArea;$report;PProp_GridY)
			//
			//PP_LabelLeft:=PPro_GetPropertyNumber (PP_ExtArea;$report;PProp_LabelLeft)
			//PP_LabelTop:=PPro_GetPropertyNumber (PP_ExtArea;$report;PProp_LabelTop)
			//PP_LabelRight:=PPro_GetPropertyNumber (PP_ExtArea;$report;PProp_LabelRight)
			//PP_LabelBottom:=PPro_GetPropertyNumber (PP_ExtArea;$report;PProp_LabelBottom)
			
			// modified 02.06.2011/08:09/SDS Software GmbH/Mußhoff
			// OBJECT SET ENABLED(PP_StaticReport;False)
			PP_ReportType:=Get indexed string:C510(13103; PP_StaticReport+1)
			PP_ObjectFrame:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_ShowObjectBorders:K7906:122)
			PP_PrintableArea:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_ShowMargins:K7906:113)
			PP_ShowRuler:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_ShowRulers:K7906:114)
			PP_ShowGrid:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_ShowGrid:K7906:116)
			PP_GridSize:=PPro_GetPropertyNumber(PP_ExtArea; $report; PProp_GridSize:K7906:117)
			PP_AlignToGrid:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_SnapToGrid:K7906:118)
			PP_ShowGuides:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_ShowGuides:K7906:119)
			PP_SnapToGuides:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_SnapToGuides:K7906:120)
			PP_LockGuides:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_LockGuides:K7906:121)
			PP_ProximityLimit:=PPro_GetPropertyNumber(PP_ExtArea; $report; PProp_Proximity:K7906:126)
			PP_Units:=PPro_GetPropertyInteger(PP_ExtArea; $report; PProp_RulerUnit:K7906:115)
			PP_UnitName:=PP_Units{PP_Units}
			// end
			
		End if 
		
	: ($sel="set")
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($err; $i; $j; $report)
		
		$report:=OBJECT Get pointer:C1124(Object subform container:K67:4)->
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="Report")  //
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_PhysicalPaper:K7906:8; PP_PaperBtn)
			
			C_REAL:C285($left; $top; $right; $bottom; $height; $width)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; $report; PProp_Width:K7906:6; PPro_Util_ReturnMeasure(->PP_Width; PP_Units))
			$err:=PPro_SetPropertyNumber(PP_ExtArea; $report; PProp_Height:K7906:7; PPro_Util_ReturnMeasure(->PP_Height; PP_Units))
			// $err:=PPro_SetPropertyInteger (PP_ExtArea;$report;PProp_StaticReport;PP_StaticReport)
			
			$left:=PPro_Util_ReturnMeasure(->PP_Left; PP_Units)
			$top:=PPro_Util_ReturnMeasure(->PP_Top; PP_Units)
			$right:=PPro_Util_ReturnMeasure(->PP_Right; PP_Units)
			$bottom:=PPro_Util_ReturnMeasure(->PP_Bottom; PP_Units)
			
			$temp:=PPro_Util_CoordToRect(->$rect; $left; $top; $right; $bottom; False:C215)
			$err:=PPro_SetPropertyString(PP_ExtArea; $report; PProp_Margins:K7906:9; $rect)
			
			// modified pB 2014-6
			// added properties of v1.4
			// $err:=PPro_SetPropertyInteger (PP_ExtArea;$report;PProp_LabelReport;PP_LabelReport)
			If (PP_LabelReport=1)
				
				//$err:=PPro_SetPropertyInteger (PP_ExtArea;$report;PProp_GridX;PP_GridX)
				//$err:=PPro_SetPropertyInteger (PP_ExtArea;$report;PProp_GridY;PP_GridY)
				//
				//$err:=PPro_SetPropertyNumber (PP_ExtArea;$report;PProp_LabelLeft;PP_LabelLeft)
				//$err:=PPro_SetPropertyNumber (PP_ExtArea;$report;PProp_LabelTop;PP_LabelTop)
				//$err:=PPro_SetPropertyNumber (PP_ExtArea;$report;PProp_LabelRight;PP_LabelRight)
				//$err:=PPro_SetPropertyNumber (PP_ExtArea;$report;PProp_LabelBottom;PP_LabelBottom)
			End if 
			
			// modified 02.06.2011/08:11/SDS Software GmbH/Mußhoff
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_ShowObjectBorders:K7906:122; PP_ObjectFrame)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_ShowMargins:K7906:113; PP_PrintableArea)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_ShowRulers:K7906:114; PP_ShowRuler)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_ShowGrid:K7906:116; PP_ShowGrid)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; $report; PProp_GridSize:K7906:117; PP_GridSize)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_SnapToGrid:K7906:118; PP_AlignToGrid)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_ShowGuides:K7906:119; PP_ShowGuides)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_SnapToGuides:K7906:120; PP_SnapToGuides)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $report; PProp_LockGuides:K7906:121; PP_LockGuides)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; $report; PProp_Proximity:K7906:126; PP_ProximityLimit)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_RulerUnit:K7906:115; PP_Units)
			// end
			
		End if 
End case 