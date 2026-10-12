If (Self:C308->=1)
	OBJECT SET FONT STYLE:C166(PP_InlineEdit; Italic:K14:3)
Else 
	If (PP_StyleI=0)
		OBJECT SET FONT STYLE:C166(PP_InlineEdit; Plain:K14:1)
	Else 
		OBJECT SET FONT STYLE:C166(PP_InlineEdit; Bold:K14:2)
	End if 
End if 


C_LONGINT:C283($err)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_FontStyleI:K7906:69; PP_StyleI)