//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_StylePropertiesHandler; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		
		C_LONGINT:C283($i)
		C_TEXT:C284($s)
		PP_FontSize:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FontSize:K7906:67)
		$s:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_FontName:K7906:66)
		$i:=Find in array:C230(PP_Fonts; $s)
		If ($i>0)
			PP_Fonts:=$i
			PP_Fonts{0}:=PP_Fonts{$i}
		Else 
			PP_Fonts{0}:=$s
		End if 
		
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
		$rotation:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Rotation:K7906:76)
		Case of 
			: ($rotation=0)
				PP_Rotation0:=1
			: ($rotation=90)
				PP_Rotation90:=1
			: ($rotation=180)
				PP_Rotation180:=1
			: ($rotation=270)
				PP_Rotation270:=1
		End case 
		PP_Rotation:=$rotation
		
		PP_AlignLeft:=0
		PP_AlignRight:=0
		PP_AlignCenter:=0
		PP_AlignJustify:=0
		PP_AlignFullJustify:=0
		C_TEXT:C284($align)
		$align:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_TextJustification:K7906:73)
		Case of 
			: (($align="default") | ($align=""))
				PP_AlignLeft:=1
			: ($align="left")
				PP_AlignLeft:=1
			: ($align="right")
				PP_AlignRight:=1
			: ($align="center")
				PP_AlignCenter:=1
			: ($align="justify")
				PP_AlignJustify:=1
			: ($align="fulljustify")
				PP_AlignFullJustify:=1
			Else 
				PP_AlignLeft:=1
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
			Else 
				PP_VAlignTop:=1
		End case 
		
		C_LONGINT:C283($color)
		PP_TextColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_TextColor:K7906:71))
		PP_TextOpacity:=(PP_TextColor >> 0x0018) & 0x00FF
		$color:=PP_TextColor & 0x00FFFFFF  // strip alpha
		OBJECT SET RGB COLORS:C628(*; "@TextColor"; 0x0000; $color)
		
		PP_BackColor:=PPro_Util_HexToNum(PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_BackColor:K7906:72))
		PP_FillOpacity:=(PP_BackColor >> 0x0018) & 0x00FF
		PP_TransparentBackColor:=Num:C11(PP_FillOpacity=0)
		$color:=PP_BackColor & 0x00FFFFFF  // strip alpha
		OBJECT SET RGB COLORS:C628(*; "@BackColor"; 0x0000; $color)
		
	: ($sel="set")
		
		C_LONGINT:C283($err)
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_FontSize:K7906:67; PP_FontSize)
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_FontName:K7906:66; PP_Fonts{0})
		
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
			Else 
				$align:="default"
		End case 
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_TextJustification:K7906:73; $align)
		// $err:=PPro_SetPropertyString (PP_ExtArea;PP_CurrentObject;PProp_Alignment;$align) // pB parent object can have this property
		
		$align:=""
		Case of 
			: (PP_VAlignTop=1)
				$align:="top"
			: (PP_VAlignCenter=1)
				$align:="center"
			: (PP_VAlignBottom=1)
				$align:="bottom"
			Else 
				$align:="top"
		End case 
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_VerticalJustification:K7906:74; $align)
		
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_TextWrap:K7906:75; PP_WrapBtn)
		
		C_REAL:C285($rotation)
		If (PP_Environment="Blue")
			$rotation:=PP_Rotation
		Else 
			Case of 
				: (PP_Rotation0=1)
					$rotation:=0
				: (PP_Rotation90=1)
					$rotation:=90
				: (PP_Rotation180=1)
					$rotation:=180
				: (PP_Rotation270=1)
					$rotation:=270
				Else 
					$rotation:=0
			End case 
		End if 
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Rotation:K7906:76; $rotation)
		PP_TextColor:=((PP_TextOpacity & 0x00FF) << 0x0018) | (PP_TextColor & 0x00FFFFFF)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_TextColor:K7906:71; PP_TextColor)
		PP_BackColor:=(PP_BackColor & 0x00FFFFFF) | ((PP_FillOpacity & 0x00FF) << 0x0018)
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_BackColor:K7906:72; PP_BackColor)
End case 