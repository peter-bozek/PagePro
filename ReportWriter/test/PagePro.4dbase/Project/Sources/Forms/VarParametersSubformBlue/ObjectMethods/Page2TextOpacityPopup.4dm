// RectParamsSubform.PP_FillOpacityPopup
// 

// created 30.05.2011/16:44/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————

// —— Initialisation ———————————————

// —— Main ———————————————

PP_TextOpacity:=Round:C94(255*(Num:C11(Self:C308->{Self:C308->})/100); 0)
Self:C308->:=0

C_LONGINT:C283($err)
PP_TextColor:=((PP_TextOpacity & 0x00FF) << 0x0018) | (PP_TextColor & 0x00FFFFFF)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_TextColor:K7906:71; PP_TextColor)
