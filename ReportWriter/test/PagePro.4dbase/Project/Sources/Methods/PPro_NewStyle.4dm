//%attributes = {"invisible":true}
C_LONGINT:C283($1)

If (False:C215)
	C_LONGINT:C283(PPro_NewStyle; $1)
End if 

C_LONGINT:C283($err; $lValue; $counter; $newStyle)
C_TEXT:C284($sValue)

//create new style
$err:=RW_NewObject($1; $newStyle; PPro_Style:K7905:25; 0)
$sValue:="New Style"
$err:=RW_SetProperty($1; $newStyle; PProp_Name:K7906:3; ->$sValue)
$sValue:="Tahome"
$err:=RW_SetProperty($1; $newStyle; PProp_FontName:K7906:66; ->$sValue)
$sValue:="12"
$err:=RW_SetProperty($1; $newStyle; PProp_FontSize:K7906:67; ->$sValue)
$lValue:=PPro_Util_ComponentsToColor(0; 0; 0; 1)  //non-transparent black
$err:=RW_SetProperty($1; $newStyle; PProp_TextColor:K7906:71; ->$lValue)

//deselect all selected objects
$lValue:=0
$err:=RW_SetProperty($1; 0; "selc"; ->$lValue)
//select new style 
ARRAY LONGINT:C221(PP_ItemsToSelect; 1)
PP_ItemsToSelect{1}:=$newStyle
PP_CurrentObject:=PP_ItemsToSelect{0x0001}
// SET TIMER(1)
SELECT LIST ITEMS BY REFERENCE:C630(PP_XMLRoot; 0x0000; PP_ItemsToSelect)
REDRAW:C174(PP_XMLRoot)

