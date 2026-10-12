// RectParamsSubform.PP_FillOpacityPopup
// 

// created 30.05.2011/16:44/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————

// —— Initialisation ———————————————

// —— Main ———————————————

PP_FillOpacity:=Round:C94(255*(Num:C11(Self:C308->{Self:C308->})/100); 0)
// modified 01.06.2011/18:17/SDS Software GmbH/Mußhoff
If (PP_FillOpacity=0)
	PP_TransparentBackColor:=1
Else 
	PP_TransparentBackColor:=0
End if 
// end
Self:C308->:=0

C_LONGINT:C283($err)
PP_BackColor:=(PP_BackColor & 0x00FFFFFF) | ((PP_FillOpacity & 0x00FF) << 0x0018)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_BackColor:K7906:72; PP_BackColor)
