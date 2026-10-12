//%attributes = {"invisible":true}
// ----------------------------------------------------
// adjusted from Pop DB

C_TEXT:C284($1)
C_TEXT:C284($2)

If (False:C215)
	C_TEXT:C284(PPro_LoadMenu; $1)
	C_TEXT:C284(PPro_LoadMenu; $2)
End if 

C_LONGINT:C283($modifier; $j; $menuItem)
C_TEXT:C284($node; $menuReference; $menu; $attributeName; $attributeValue; $keyCode)
C_TEXT:C284($label; $type)


$menuReference:=$1
$node:=DOM Get first child XML element:C723($2)
$menuItem:=0

If (OK=1)
	Repeat 
		$menuItem:=$menuItem+1
		
		DOM GET XML ELEMENT NAME:C730($node; $type)
		DOM GET XML ATTRIBUTE BY NAME:C728($node; "text"; $label)
		
		If ($label=":xliff:@")
			$label:=Get localized string:C991(Delete string:C232($label; 1; 7))
		End if 
		
		If ($type="menu")
			$menu:=Create menu:C408
			PPro_LoadMenu($menu; $node)
			APPEND MENU ITEM:C411($menuReference; $label; $menu)
		Else 
			
			APPEND MENU ITEM:C411($menuReference; $label)
			If ($label#"-@")
				$keyCode:=""
				$modifier:=0
				For ($j; 1; DOM Count XML attributes:C727($node); 1)
					
					DOM GET XML ATTRIBUTE BY INDEX:C729($node; $j; $attributeName; $attributeValue)
					Case of 
						: ($attributeName="reference")
							SET MENU ITEM PARAMETER:C1004($menuReference; -1; $attributeValue)
							
						: ($attributeName="shortcut")
							//SET MENU ITEM SHORTCUT($menuReference;-1;$attributeValue;Get menu item modifiers($menuReference;-1))
							//SET MENU ITEM SHORTCUT($menuReference;$menuItem;"R";2048)
							$keyCode:=$attributeValue
							
						: ($attributeName="shift")
							//$modifier:=Get menu item modifiers($menuReference;-1)
							If ($attributeValue="true")
								$modifier:=$modifier ?+ Shift key bit:K16:4
							Else 
								$modifier:=$modifier ?- Shift key bit:K16:4
							End if 
							//SET MENU ITEM SHORTCUT($menuReference;-1;Get menu item key($menuReference;-1);$modifier)
							
						: ($attributeName="alt")
							//$modifier:=Get menu item modifiers($menuReference;-1)
							If ($attributeValue="true")
								$modifier:=$modifier ?+ Option key bit:K16:8
							Else 
								$modifier:=$modifier ?- Option key bit:K16:8
							End if 
							//SET MENU ITEM SHORTCUT($menuReference;-1;Get menu item key($menuReference;-1);$modifier)
							
						: ($attributeName="standard_action")
							SET MENU ITEM PROPERTY:C973($menuReference; -1; "4D_standard_action"; $attributeValue)
							
						: ($attributeName="enable")
							SET MENU ITEM PROPERTY:C973($menuReference; -1; "4D_enable"; $attributeValue)
							
						: ($attributeName="method")
							SET MENU ITEM METHOD:C982($menuReference; -1; $attributeValue)
							
						: ($attributeName="mark")
							SET MENU ITEM MARK:C208($menuReference; -1; Char:C90(18)*Num:C11($attributeValue="true"))
							
						: ($attributeName="style")
							SET MENU ITEM STYLE:C425($menuReference; -1; Num:C11($attributeValue))
					End case 
					
				End for 
				
				SET MENU ITEM SHORTCUT:C423($menuReference; -1; $keyCode; $modifier)
				
			End if 
		End if 
		
		$node:=DOM Get next sibling XML element:C724($node)
		
	Until (OK=0)
	
Else 
	TRACE:C157
End if 
