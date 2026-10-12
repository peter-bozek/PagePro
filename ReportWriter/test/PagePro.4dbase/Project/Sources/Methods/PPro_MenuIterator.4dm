//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_MenuIterator; $1)
End if 

C_LONGINT:C283($i; $menu)
C_TEXT:C284($param)

ARRAY TEXT:C222($aNames; 0x0000)
ARRAY TEXT:C222($aRefs; 0x0000)

//iteration through all menu 
GET MENU ITEMS:C977($1; $aNames; $aRefs)

For ($menu; 1; Size of array:C274($aRefs))
	If ($aRefs{$menu}#"")
		PPro_MenuIterator($aRefs{$menu})
	Else 
		$param:=Get menu item parameter:C1003($1; $menu)
		If ($param#"")
			SET MENU ITEM MARK:C208($1; $menu; "")
			Case of 
				: ($param="stBold")
					$i:=Find in array:C230(PP_PropID; PProp_FontStyleB:K7906:68)
					If ($i>0)
						If (PP_PropValue{$i}="1")
							SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
						End if 
					End if 
					
				: ($param="stItalic")
					$i:=Find in array:C230(PP_PropID; PProp_FontStyleI:K7906:69)
					If ($i>0)
						If (PP_PropValue{$i}="1")
							SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
						End if 
					End if 
					
				: ($param="stUnderline")
					$i:=Find in array:C230(PP_PropID; PProp_FontStyleU:K7906:70)
					If ($i>0)
						If (PP_PropValue{$i}="1")
							SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
						End if 
					End if 
					
				: ($param="stSize@")
					$i:=Find in array:C230(PP_PropID; PProp_FontSize:K7906:67)
					If ($i>0)
						If (PP_PropValue{$i}=Substring:C12($param; 7))
							SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
						End if 
					End if 
					
				: (($param="dpNormal") | ($param="refShowRef"))
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)
					If ($i=0)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpNames")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)
					If ($i=3)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpAlias")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpIDs")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)
					If ($i=4)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpFormats")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)
					If ($i=2)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpPrintOrder")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)
					If ($i=5)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: (($param="dpValues") | ($param="refShowVal"))
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)
					If ($i=7)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
					
				: ($param="dpRulers")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_ShowRulers:K7906:114)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpGrid")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_ShowGrid:K7906:116)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpToGrid")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_SnapToGrid:K7906:118)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpShowGuides")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_ShowGuides:K7906:119)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpSnapGuides")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_SnapToGuides:K7906:120)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpLockGuides")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_LockGuides:K7906:121)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
				: ($param="dpShowBorders")
					$i:=PPro_GetPropertyInteger(PP_ExtArea; PP_ExtArea; PProp_ShowObjectBorders:K7906:122)
					If ($i=1)
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
					
				: ($param="dpShowXMLTree")
					If (OBJECT Get visible:C1075(PP_XMLRoot))
						SET MENU ITEM MARK:C208($1; $menu; Char:C90(18))
					End if 
			End case 
		End if 
	End if 
End for 
