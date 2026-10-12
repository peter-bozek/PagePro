//%attributes = {"shared":true}

C_LONGINT:C283($1)  //area

If (False:C215)
	C_LONGINT:C283(PPro_AreaFormData; $1)  //area
	
End if 

C_LONGINT:C283($area; $err; $i; $style)
C_REAL:C285($left; $top; $right; $bottom; $height; $width)
C_TEXT:C284($s; $text)

ARRAY LONGINT:C221($aSelection; 0x0000)

$area:=$1

PP_Scaling:=Round:C94(PPro_GetPropertyNumber($area; $area; PProp_Scale:K7906:123)*100; 0x0002)
PP_Units:=PPro_GetPropertyInteger($area; $area; PProp_RulerUnit:K7906:115)
PP_UnitName:=PP_Units{PP_Units}

$err:=RW_GetObjects($area; $area; "selc"; $aSelection)  // get selected items
Case of 
	: (Size of array:C274($aSelection)=0)  //this is selected obect
		//PP_CurrentObject:=0 // current object may be non-selectable
		
	: (Size of array:C274($aSelection)=1)  //this is selected obect
		$err:=RW_GetProperty(PP_ExtArea; $aSelection{1}; "kind"; ->$text)
		PP_CurrentObject:=$aSelection{1}
	: (Size of array:C274($aSelection)>1)  //this is selected obect
		If (PP_CurrentObject=0)
			$err:=RW_GetProperty($area; $area; "hito"; ->PP_CurrentObject)  // last object hit
		End if 
		If (PP_CurrentObject#0)
			If (Find in array:C230($aSelection; PP_CurrentObject)<1)  // pB v 1.3.1 this can happen when user selects several objects 
				// the hit objects is a section, but user still wants to see first or last selected object
				PP_CurrentObject:=$aSelection{1}
			End if 
		End if 
End case 



If ((PP_CurrentObject>0) & (($text#"Style") & ($text#"DataSource") & ($text#"Report")))  //
	$s:=PPro_GetPropertyString($area; PP_CurrentObject; PProp_Rect:K7906:22)
	PPro_Util_RectToCoord($s; ->$left; ->$top; ->$right; ->$bottom)
	PPro_ToolbarPositionSet(->PP_X; $left; PP_Units)
	PPro_ToolbarPositionSet(->PP_Y; $top; PP_Units)
	PPro_ToolbarPositionSet(->PP_W; $right-$left; PP_Units)
	PPro_ToolbarPositionSet(->PP_H; $bottom-$top; PP_Units)
	PPro_ToolbarPositionSet(->PP_R; $right; PP_Units)
	PPro_ToolbarPositionSet(->PP_B; $bottom; PP_Units)
	
	$err:=RW_GetProperty($area; PP_CurrentObject; "kind"; ->$text)
	Case of 
		: ($text="Guide")
			PPro_ToolbarPositionSet(->PP_W; 0)
			PPro_ToolbarPositionSet(->PP_H; 0)
			PPro_ToolbarPositionSet(->PP_R; 0)
			PPro_ToolbarPositionSet(->PP_B; 0)
			
		: ($text="Section")
			PPro_ToolbarPositionSet(->PP_W; 0)
			PPro_ToolbarPositionSet(->PP_X; 0)
			PPro_ToolbarPositionSet(->PP_Y; 0)
			PPro_ToolbarPositionSet(->PP_R; 0)
			PPro_ToolbarPositionSet(->PP_B; 0)
			
	End case 
	
	$style:=PPro_GetPropertyInteger($area; PP_CurrentObject; PProp_Styl:K7906:46)
	$i:=Find in array:C230(PP_StyleID; $style)
	If ($i>0)
		PP_Styles:=$i
	Else 
		PP_Styles:=0
	End if 
	
Else 
	PPro_ToolbarPositionSet(->PP_X; 0)
	PPro_ToolbarPositionSet(->PP_Y; 0)
	PPro_ToolbarPositionSet(->PP_W; 0)
	PPro_ToolbarPositionSet(->PP_H; 0)
	PPro_ToolbarPositionSet(->PP_R; 0)
	PPro_ToolbarPositionSet(->PP_B; 0)
	
	PP_Styles:=0
	
	
End if 

If (PP_CurrentObject#0)
	PP_ButtonLock:=0
	C_BOOLEAN:C305($bool)
	$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; PProp_Locked:K7906:19; ->$bool)
	If ($err=0)
		PP_ButtonLock:=Num:C11($bool)
	End if 
End if 

If ((Size of array:C274($aSelection)=0x0001) & ($text="group"))
	PP_ButtonGroup:=1
Else 
	PP_ButtonGroup:=0
End if 

If (PP_LockProportions=1)
	OBJECT SET VISIBLE:C603(*; "PP_SizeLinkActive"; True:C214)
	OBJECT SET VISIBLE:C603(*; "PP_SizeLinkInactive"; False:C215)
Else 
	OBJECT SET VISIBLE:C603(*; "PP_SizeLinkActive"; False:C215)
	OBJECT SET VISIBLE:C603(*; "PP_SizeLinkInactive"; True:C214)
End if 

//set up menu checkmarks etc.
PPro_MenuSetup
//set up subforms to show the object data
PPro_SubformVisibility($area)
// request redraw of popup / hList
REDRAW:C174(PP_XMLRoot)
//SET TIMER(-1)

