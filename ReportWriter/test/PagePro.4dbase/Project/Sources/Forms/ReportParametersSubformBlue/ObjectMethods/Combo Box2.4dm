PP_UnitName:=PP_Units{PP_Units}

C_REAL:C285($left; $top; $right; $bottom; $height; $width)
C_TEXT:C284($s)
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

