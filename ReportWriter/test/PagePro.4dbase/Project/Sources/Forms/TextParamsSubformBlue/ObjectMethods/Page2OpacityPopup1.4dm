// RectParamsSubform.PP_FrameOpacityPopup
// 

// created 30.05.2011/16:44/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————

// —— Initialisation ———————————————

// —— Main ———————————————

PP_FrameOpacity:=Round:C94(255*(Num:C11(Self:C308->{Self:C308->})/100); 0)
Self:C308->:=0

C_LONGINT:C283($err)
PP_FrameColor:=(PP_FrameColor & 0x00FFFFFF) | ((PP_FrameOpacity & 0x00FF) << 0x0018)
$err:=PPro_SetPropertyInteger(PP_ExtArea; 0; PProp_FrameColor:K7906:45; PP_FrameColor)
