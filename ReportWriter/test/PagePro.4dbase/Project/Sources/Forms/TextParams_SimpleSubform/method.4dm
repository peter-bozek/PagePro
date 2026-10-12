// this form is unused, was used as a template 
// for properties of text / variable / field objects


C_TEXT:C284($sel)

Case of 
	: (Form event code:C388=On Bound Variable Change:K2:52)
		//variable PP_CurrentObject contains a line object
		C_TEXT:C284($s)
		C_LONGINT:C283($err)
		PPro_ObjectPropertiesSubform($sel)  //hanadling of object common properties
		
		PP_Content:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53)
		
		PP_MultiStyleBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48)
		PP_DynamicBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DynamicText:K7906:47)
		PP_KeepTogetherBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_KeepTogether:K7906:50)
		
		C_LONGINT:C283($j)
		C_TEXT:C284($draw)
		$draw:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52)
		$j:=Find in array:C230(PP_DrawPopup; $draw)
		If ($j<1)
			$j:=1
		End if 
		PP_DrawPopup:=$j
		
		C_LONGINT:C283($j)
		$draw:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_DrawIfEmpty:K7906:51)
		$j:=Find in array:C230(PP_EmptyPopup; $draw)
		If ($j<1)
			$j:=1
		End if 
		PP_EmptyPopup:=$j
		
		PP_StyleB:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FontStyleB:K7906:68)
		PP_Stylei:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FontStyleI:K7906:69)
		PP_StyleU:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FontStyleU:K7906:70)
		
		PP_WrapBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_TextWrap:K7906:75)
		
		PP_LineSpacing:=Round:C94(PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_LineSpacing:K7906:77)*100; 0)
		PP_HorScale:=Round:C94(PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_HorizontalScale:K7906:78)*100; 0)
		
		C_REAL:C285($rotation)
		PP_Rotation0:=0
		PP_Rotation90:=0
		PP_Rotation180:=0
		PP_Rotation270:=0
		$rotation:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Rotation:K7906:76)\90
		Case of 
			: ($rotation=0)
				PP_Rotation0:=1
			: ($rotation=1)
				PP_Rotation90:=1
			: ($rotation=2)
				PP_Rotation180:=1
			: ($rotation=3)
				PP_Rotation270:=1
		End case 
		
		PP_AlignLeft:=0
		PP_AlignRight:=0
		PP_AlignCenter:=0
		PP_AlignJustify:=0
		PP_AlignFullJustify:=0
		C_TEXT:C284($align)
		$align:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_TextJustification:K7906:73)
		Case of 
			: ($align="default")
				//PP_AlignLeft:=1
			: ($align="left")
				PP_AlignLeft:=1
			: ($align="right")
				PP_AlignRight:=1
			: ($align="center")
				PP_AlignCenter:=1
			: ($align="justify")
				PP_AlignJustify:=1
			: ($align="default")
				PP_AlignFullJustify:=1
		End case 
		
		PP_VAlignTop:=0
		PP_VAlignCenter:=0
		PP_VAlignBottom:=0
		$align:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_VerticalJustification:K7906:74)
		Case of 
			: ($align="top")
				PP_VAlignTop:=1
			: ($align="center")
				PP_VAlignCenter:=1
			: ($align="bottom")
				PP_VAlignBottom:=1
		End case 
		
		C_LONGINT:C283($color)
		PP_TextColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_TextColor:K7906:71))
		$color:=PP_TextColor & 0x00FFFFFF  // strip alpha
		OBJECT SET RGB COLORS:C628(*; "@TextColor"; 0x0000; $color)
		
		PP_BackColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_BackColor:K7906:72))
		PP_TransparentBackColor:=Num:C11((PP_BackColor >> 0x0018)=0)
		$color:=PP_BackColor & 0x00FFFFFF  // strip alpha
		OBJECT SET RGB COLORS:C628(*; "@BackColor"; 0x0000; $color)
		
		PP_FrameBtn:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42)
		PP_FrameOffset:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43)
		PP_LineWidth:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44)
		PP_FrameColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45))
		PP_TransparentFrameColor:=Num:C11((PP_FrameColor >> 0x0018)=0)
		$color:=PP_FrameColor & 0x00FFFFFF  // strip alpha
		OBJECT SET RGB COLORS:C628(*; "@FrameColor"; 0x0000; $color)
		
	: ((Form event code:C388=On Data Change:K2:15) | (Form event code:C388=On Clicked:K2:4))
		C_TEXT:C284($s)
		C_LONGINT:C283($err)
		
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53; PP_Content)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48; PP_MultiStyleBtn)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_DynamicText:K7906:47; PP_DynamicBtn)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_KeepTogether:K7906:50; PP_KeepTogetherBtn)
		
		C_LONGINT:C283($j)
		$j:=Find in array:C230(PP_EmptyPopup; $draw)
		If (PP_EmptyPopup<1)
			PP_EmptyPopup:=1
		End if 
		$draw:=PP_EmptyPopup{PP_EmptyPopup}
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_DrawIfEmpty:K7906:51; $draw)
		
		C_LONGINT:C283($j)
		$j:=Find in array:C230(PP_DrawPopup; $draw)
		If (PP_DrawPopup<1)
			PP_DrawPopup:=1
		End if 
		$draw:=PP_DrawPopup{PP_DrawPopup}
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Draw:K7906:52; $draw)
		
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_ID:K7906:16; PP_ObjectID)
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Name:K7906:3; PP_ObjectName)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FontStyleB:K7906:68; PP_StyleB)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FontStyleI:K7906:69; PP_StyleI)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FontStyleU:K7906:70; PP_StyleU)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_TextWrap:K7906:75; PP_WrapBtn)
		
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_LineSpacing:K7906:77; Round:C94(PP_LineSpacing/100; 2))
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_HorizontalScale:K7906:78; Round:C94(PP_HorScale/100; 2))
		
		C_TEXT:C284($align)
		Case of 
			: (PP_AlignLeft=1)
				$align:="left"
			: (PP_AlignRight=1)
				$align:="right"
			: (PP_AlignCenter=1)
				$align:="center"
			: (PP_AlignJustify=1)
				$align:="justify"
			: (PP_AlignFullJustify=1)
				$align:="default"
		End case 
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_TextJustification:K7906:73; $align)
		
		Case of 
			: (PP_VAlignTop=1)
				$align:="top"
			: (PP_VAlignCenter=1)
				$align:="center"
			: (PP_VAlignBottom=1)
				$align:="bottom"
		End case 
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_VerticalJustification:K7906:74; $align)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_TextWrap:K7906:75; PP_WrapBtn)
		
		C_REAL:C285($rotation)
		Case of 
			: (PP_Rotation0=1)
				$rotation:=0
			: (PP_Rotation90=1)
				$rotation:=90
			: (PP_Rotation180=1)
				$rotation:=180
			: (PP_Rotation270=1)
				$rotation:=270
		End case 
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Rotation:K7906:76; $rotation)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_TextColor:K7906:71; PP_TextColor)
		PP_BackColor:=(PP_BackColor & 0x00FFFFFF) | (0xFF000000*(1-PP_TransparentBackColor))
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BackColor:K7906:72; PP_BackColor)
		PP_FrameColor:=(PP_FrameColor & 0x00FFFFFF) | (0xFF000000*(1-PP_TransparentFrameColor))
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_FrameColor:K7906:45; PP_FrameColor)
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameThickness:K7906:44; PP_LineWidth)
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FrameOffset:K7906:43; PP_FrameOffset)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_Frame:K7906:42; PP_FrameBtn)
		
End case 