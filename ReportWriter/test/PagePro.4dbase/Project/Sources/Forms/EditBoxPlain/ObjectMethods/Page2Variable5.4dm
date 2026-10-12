If (PP_FontSize<2)
	PP_FontSize:=11
End if 
OBJECT SET FONT SIZE:C165(PP_InlineEditPlain; PP_FontSize)

C_LONGINT:C283($err)
$err:=PPro_SetPropertyNumber(PP_ExtArea; 0; PProp_FontSize:K7906:67; PP_FontSize)