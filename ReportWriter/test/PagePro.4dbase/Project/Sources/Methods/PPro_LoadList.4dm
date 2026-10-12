//%attributes = {"invisible":true}

C_LONGINT:C283($1)
C_TEXT:C284($2)
C_LONGINT:C283($3; $0)

If (False:C215)
	C_LONGINT:C283(PPro_LoadList; $1)
	C_TEXT:C284(PPro_LoadList; $2)
	C_LONGINT:C283(PPro_LoadList; $3; $0)
End if 

C_LONGINT:C283($list; $item; $sublist; $oldItem; $j)
C_TEXT:C284($node; $attributeName; $attributeValue; $dec; $thousand)
C_BOOLEAN:C305($isNumber)

$list:=$1
$node:=DOM Get first child XML element:C723($2)

$item:=$3

If (OK=1)
	Repeat 
		$item:=$item+1
		
		C_TEXT:C284($type; $label)
		DOM GET XML ELEMENT NAME:C730($node; $type)
		DOM GET XML ATTRIBUTE BY NAME:C728($node; "text"; $label)
		
		$isNumber:=False:C215
		If ($label=":xliff:@")
			$isNumber:=Position:C15("NumberFormat"; $label)>0
			$label:=Get localized string:C991(Delete string:C232($label; 1; 7))
			If ($isNumber)
				GET SYSTEM FORMAT:C994(Decimal separator:K60:1; $dec)
				$label:=Replace string:C233($label; "."; $dec)
				GET SYSTEM FORMAT:C994(Currency symbol:K60:3; $dec)
				$label:=Replace string:C233($label; "€"; $dec)
			End if 
		End if 
		
		If ($type="menu")
			$sublist:=New list:C375
			$oldItem:=$item
			$item:=PPro_LoadList($sublist; $node; $item)
			APPEND TO LIST:C376($list; $label; $oldItem; $sublist; False:C215)
		Else 
			
			APPEND TO LIST:C376($list; $label; $item)
			If ($label#"-@")
				For ($j; 1; DOM Count XML attributes:C727($node); 1)
					
					DOM GET XML ATTRIBUTE BY INDEX:C729($node; $j; $attributeName; $attributeValue)
					If ($attributeName="reference")
						If ($isNumber)
							GET SYSTEM FORMAT:C994(Currency symbol:K60:3; $dec)
							$attributeValue:=Replace string:C233($attributeValue; "€"; $dec)
							
							GET SYSTEM FORMAT:C994(Decimal separator:K60:1; $dec)
							If ($dec=",")
								GET SYSTEM FORMAT:C994(Thousand separator:K60:2; $thousand)
								If ($thousand#".")
									$attributeValue:=Replace string:C233($attributeValue; ","; $thousand)
								End if 
							End if 
							$attributeValue:=Replace string:C233($attributeValue; "."; $dec)
						End if 
						
						SET LIST ITEM PARAMETER:C986($list; $item; "format"; $attributeValue)
					End if 
					
				End for 
				
			End if 
		End if 
		
		$node:=DOM Get next sibling XML element:C724($node)
		
	Until (OK=0)
	
Else 
	TRACE:C157
End if 

$0:=$item