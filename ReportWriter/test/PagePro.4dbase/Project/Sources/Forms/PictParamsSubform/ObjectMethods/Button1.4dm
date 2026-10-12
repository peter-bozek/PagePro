C_REAL:C285($width; $height)
C_LONGINT:C283($err)

$width:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Width:K7906:6)
$height:=PPro_GetPropertyNumber(PP_ExtArea; PP_CurrentObject; PProp_Height:K7906:7)

If (($width>0) & ($height>0))
	//$err:=PPro_SetPropertyNumber (PP_ExtArea;PP_CurrentObject;"posw";$width)
	//$err:=PPro_SetPropertyNumber (PP_ExtArea;PP_CurrentObject;"posh";$height)
	PP_Point2x:=$width
	PP_Point2y:=$height
End if 