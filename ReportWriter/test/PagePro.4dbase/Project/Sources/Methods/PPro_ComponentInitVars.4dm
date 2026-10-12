//%attributes = {"shared":true}
// Used to initialize method variables
//      call before displaying PagePro Standard area in subform

PP_VersionString:="version 1.6.0b8"
PP_PluginVersionString:="RW plugin "+Char:C90(Carriage return:K15:38)+"version "+RW_GetVersion

PP_CallbackRef:=""
PP_CallbackName:=""

PP_WindowTop:=80
PP_WindowLeft:=40
PP_WindowType:=0

PP_WindowRef:=0

PP_DirtyFlag:=False:C215

PP_DefaultMenu:="PP_DefaultMenu"
PP_MenuRef:=""
PP_OldMenuRef:=""

PP_Report:=""

PP_DocumentPath:=""
PP_ReportModified:=0

PP_LockedMode:=2

PP_Environment:=""  // default
PP_XMLIsDisplayed:=False:C215

//Preload lists and popups
STRING LIST TO ARRAY:C511(13001; PP_AlignPopup)
STRING LIST TO ARRAY:C511(13002; PP_PrintPopup)
STRING LIST TO ARRAY:C511(13005; PP_PictFormatPopup)
STRING LIST TO ARRAY:C511(13007; PP_WidthPopup)
STRING LIST TO ARRAY:C511(13008; PP_EmptyPopup)
STRING LIST TO ARRAY:C511(13011; PP_TextTabs)
STRING LIST TO ARRAY:C511(13012; PP_ElementPopup)
STRING LIST TO ARRAY:C511(13013; PP_CalcPopup)
STRING LIST TO ARRAY:C511(13014; PP_DataSourcePopup)
STRING LIST TO ARRAY:C511(13015; PP_Relate1Popup)
STRING LIST TO ARRAY:C511(13015; PP_RelateManyPopup)
STRING LIST TO ARRAY:C511(13017; PP_PageBreakPopup)
STRING LIST TO ARRAY:C511(13018; PP_BreakOnPopup)
STRING LIST TO ARRAY:C511(13019; PP_ElementPopup)
STRING LIST TO ARRAY:C511(13020; PP_Units)
STRING LIST TO ARRAY:C511(13022; PP_DataSourcePopupMirror)
STRING LIST TO ARRAY:C511(13023; PP_RelatePopupMirror)
STRING LIST TO ARRAY:C511(13024; PP_AlignPopupMirror)
STRING LIST TO ARRAY:C511(13025; PP_PrintPopupMirror)
STRING LIST TO ARRAY:C511(13026; PP_RepeatPopup)
STRING LIST TO ARRAY:C511(13027; PP_RepeatMirror)

PP_ObjectsAlignPopup:=1

// pB v1.3 checking proper decimal char
PPro_CheckDecimal(->PP_WidthPopup)

// v1.4
STRING LIST TO ARRAY:C511(13107; PP_TabAlignPopup)
STRING LIST TO ARRAY:C511(13108; PP_TabAlignPopupMirror)
// ARRAY TO LIST(PP_TabAlignPopup;"TabAlignment")

// modified 30.05.2011/16:50/SDS Software GmbH/Mußhoff
C_LONGINT:C283($Counter)
ARRAY TEXT:C222(PP_FrameOpacityPopup; 11)
ARRAY TEXT:C222(PP_FillOpacityPopup; 0)
ARRAY TEXT:C222(PP_TextOpacityPopup; 0)
For ($Counter; 1; 11)
	PP_FrameOpacityPopup{$Counter}:=String:C10(($Counter-1)*10; "##0")+" %"
End for 
COPY ARRAY:C226(PP_FrameOpacityPopup; PP_FillOpacityPopup)
COPY ARRAY:C226(PP_FrameOpacityPopup; PP_TextOpacityPopup)
// end


// modified 19.06.2011/11:41/SDS Software GmbH/Mußhoff
PP_CurrentLanguage:=Get database localization:C1009(Current localization:K5:22)
// end

