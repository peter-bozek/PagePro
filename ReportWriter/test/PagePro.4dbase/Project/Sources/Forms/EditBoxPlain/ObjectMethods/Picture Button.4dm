If (Self:C308->=1)
	OBJECT SET FONT STYLE:C166(PP_InlineEditPlain; Bold:K14:2)
Else 
	If (PP_StyleI=0)
		OBJECT SET FONT STYLE:C166(PP_InlineEditPlain; Plain:K14:1)
	Else 
		OBJECT SET FONT STYLE:C166(PP_InlineEditPlain; Bold:K14:2)
	End if 
End if 

C_LONGINT:C283($err)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_FontStyleB:K7906:68; PP_StyleB)
