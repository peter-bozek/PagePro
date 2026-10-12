//%attributes = {"invisible":true}
C_LONGINT:C283($menu; $item; $font)
C_TEXT:C284($ref; $para)

ARRAY TEXT:C222($aFonts; 0x0000)
FONT LIST:C460($aFonts)
//RW_GetFonts ($aFonts)
SORT ARRAY:C229($aFonts)
COPY ARRAY:C226($aFonts; PP_Fonts)
//find the font menu

For ($menu; 1; Count menus:C404)
	For ($item; 1; Count menu items:C405($menu))
		$para:=Get menu item parameter:C1003($menu; $item)
		If ($para="txFont")
			//found font menu
			$ref:=Create menu:C408
			
			For ($font; 1; Size of array:C274($aFonts))
				APPEND MENU ITEM:C411($ref; $aFonts{$font})
				SET MENU ITEM PARAMETER:C1004($ref; $font; "menuI="+$aFonts{$font})
				SET MENU ITEM METHOD:C982($ref; $font; "PPro_Menu")
			End for 
			
			INSERT MENU ITEM:C412($menu; $item; Get localized string:C991("PP_Fonts"); $ref)
			DELETE MENU ITEM:C413($menu; $item)
			
			$menu:=Count menus:C404
			$item:=Count menu items:C405($menu)
		End if 
	End for 
	
End for 

