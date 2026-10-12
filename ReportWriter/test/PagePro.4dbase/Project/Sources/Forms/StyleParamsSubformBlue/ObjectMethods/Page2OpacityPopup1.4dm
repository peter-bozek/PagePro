// RectParamsSubform.PP_FrameOpacityPopup
// 

// created 30.05.2011/16:44/SDS Software GmbH/Mußhoff
// modifed

// —— Declaration ———————————————

// —— Initialisation ———————————————

// —— Main ———————————————

PP_FrameOpacity:=Round:C94(255*(Num:C11(Self:C308->{Self:C308->})/100); 0)
// modified 09.06.2011/11:33/SDS Software GmbH/Mußhoff
If (PP_FrameOpacity=0)
	PP_TransparentFrameColor:=1
Else 
	PP_TransparentFrameColor:=0
End if 
// end
Self:C308->:=0
