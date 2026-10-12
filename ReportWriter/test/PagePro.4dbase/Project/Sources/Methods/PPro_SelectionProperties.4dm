//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_LONGINT:C283($2)

If (False:C215)
	C_TEXT:C284(PPro_SelectionProperties; $1)
	C_LONGINT:C283(PPro_SelectionProperties; $2)
End if 

C_TEXT:C284($sel)
$sel:=$1

C_LONGINT:C283($object)
$object:=$2

Case of 
	: ($sel="get")
		//variable $object contains a line object
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($err; $i)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; $object; PProp_Kind:K7906:2)
		If (($text="Text") | ($text="Variable") | ($text="Field"))  //
			
			PP_ObjectID:=PPro_GetPropertyString(PP_ExtArea; $object; PProp_ID:K7906:16)
			PP_PrintOrder:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_PrintOrder:K7906:4)
			
			PP_VisibleBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_Visible:K7906:18)
			PP_LockedBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_Locked:K7906:19)
			PP_FixVBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_FixedVerticalPos:K7906:31)
			PP_BindVBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_BindVerticalPos:K7906:32)
			
			PP_PrintPopup:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52)+1
			PP_AlignPopup:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_Alignment:K7906:33)+1
			
			PP_MultiStyleBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_AttributedText:K7906:48)
			PP_DynamicBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_DynamicText:K7906:47)
			PP_KeepTogetherBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_KeepTogether:K7906:50)
			PP_ExpandableBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_FixedHeight:K7906:86)
			
			PP_EmptyPopup:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_DrawIfEmpty:K7906:51)+1
			
			PP_FrameBtn:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_Frame:K7906:42)
			PP_FrameOffset:=PPro_GetPropertyInteger(PP_ExtArea; $object; PProp_FrameOffset:K7906:43)
			PP_LineWidth:=PPro_GetPropertyNumber(PP_ExtArea; $object; PProp_FrameThickness:K7906:44)
			PP_FrameColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; $object; PProp_FrameColor:K7906:45))
			PP_TransparentFrameColor:=Num:C11((PP_FrameColor >> 0x0018)=0)
			PP_FrameOpacity:=(PP_FrameColor >> 24) & 0x00FF
			C_LONGINT:C283($color)
			$color:=PP_FrameColor & 0x00FFFFFF  // strip alpha
			OBJECT SET RGB COLORS:C628(*; "@FrameColor"; 0x0000; $color)
			
			PPro_StylePropertiesHandler("get")
		End if 
		
	: ($sel="set")
		
		C_TEXT:C284($s)
		C_LONGINT:C283($err)
		C_TEXT:C284($text)
		
		$text:=PPro_GetPropertyString(PP_ExtArea; $object; PProp_Kind:K7906:2)
		If (($text="Text") | ($text="Variable") | ($text="Field"))  //
			$err:=PPro_SetPropertyString(PP_ExtArea; $object; PProp_ID:K7906:16; PP_ObjectID)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_PrintOrder:K7906:4; PP_PrintOrder)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_Visible:K7906:18; PP_VisibleBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_Locked:K7906:19; PP_LockedBtn*2)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_FixedVerticalPos:K7906:31; PP_FixVBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_BindVerticalPos:K7906:32; PP_BindVBtn)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_Draw:K7906:52; PP_PrintPopup-1)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_Alignment:K7906:33; PP_AlignPopup-1)
			
			
			$err:=PPro_SetPropertyString(PP_ExtArea; $object; PProp_Data:K7906:53; PP_Content)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_AttributedText:K7906:48; PP_MultiStyleBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_DynamicText:K7906:47; PP_DynamicBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_KeepTogether:K7906:50; PP_KeepTogetherBtn)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_FixedHeight:K7906:86; PP_ExpandableBtn)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_DrawIfEmpty:K7906:51; PP_EmptyPopup-1)
			
			$err:=PPro_SetPropertyString(PP_ExtArea; $object; PProp_ID:K7906:16; PP_ObjectID)
			$err:=PPro_SetPropertyString(PP_ExtArea; $object; PProp_Name:K7906:3; PP_ObjectName)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_BackColor:K7906:72; PP_BackColor)
			PP_FrameColor:=(PP_FrameColor & 0x00FFFFFF) | ((PP_FrameOpacity << 0x0018) & 0x00FF)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_FrameColor:K7906:45; PP_FrameColor)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; $object; PProp_FrameThickness:K7906:44; PP_LineWidth)
			$err:=PPro_SetPropertyNumber(PP_ExtArea; $object; PProp_FrameOffset:K7906:43; PP_FrameOffset)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $object; PProp_Frame:K7906:42; PP_FrameBtn)
			
			PPro_StylePropertiesHandler("set")
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
		End if 
End case 