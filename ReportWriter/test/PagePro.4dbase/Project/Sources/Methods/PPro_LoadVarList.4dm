//%attributes = {"invisible":true}

C_LONGINT:C283($1)
C_TEXT:C284($2)
C_LONGINT:C283($3; $0)

If (False:C215)
	C_LONGINT:C283(PPro_LoadVarList; $1)
	C_TEXT:C284(PPro_LoadVarList; $2)
	C_LONGINT:C283(PPro_LoadVarList; $3; $0)
End if 

C_LONGINT:C283($list; $item; $sublist; $oldItem; $j)
C_TEXT:C284($node; $attributeName; $attributeValue)

$list:=$1
$node:=DOM Get first child XML element:C723($2)

$item:=$3

If (OK=1)
	Repeat 
		$item:=$item+1
		
		C_TEXT:C284($type; $label)
		DOM GET XML ELEMENT NAME:C730($node; $type)
		DOM GET XML ATTRIBUTE BY NAME:C728($node; "text"; $label)
		
		If ($label=":xliff:@")
			$label:=Get localized string:C991(Delete string:C232($label; 1; 7))
		End if 
		
		If ($type="group")
			$sublist:=New list:C375
			$oldItem:=$item
			$item:=PPro_LoadVarList($sublist; $node; $item)
			APPEND TO LIST:C376($list; $label; $oldItem; $sublist; False:C215)
		Else 
			
			APPEND TO LIST:C376($list; $label; $item)
			If ($label#"-@")
				For ($j; 1; DOM Count XML attributes:C727($node); 1)
					
					DOM GET XML ATTRIBUTE BY INDEX:C729($node; $j; $attributeName; $attributeValue)
					Case of 
						: ($attributeName="type")
							SET LIST ITEM PARAMETER:C986($list; $item; "type"; $attributeValue)
						: ($attributeName="reference")
							SET LIST ITEM PARAMETER:C986($list; $item; "ref"; $attributeValue)
						: ($attributeName="format")
							SET LIST ITEM PARAMETER:C986($list; $item; "format"; $attributeValue)
						: ($attributeName="index")
							SET LIST ITEM PARAMETER:C986($list; $item; "index"; $attributeValue)
							
					End case 
					
				End for 
				
			End if 
		End if 
		
		$node:=DOM Get next sibling XML element:C724($node)
		
	Until (OK=0)
	
Else 
	TRACE:C157
End if 

$0:=$item