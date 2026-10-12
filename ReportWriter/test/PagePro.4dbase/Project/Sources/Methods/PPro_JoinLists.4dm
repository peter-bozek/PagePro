//%attributes = {"invisible":true}
C_LONGINT:C283($1)
C_LONGINT:C283($0)

If (False:C215)
	C_LONGINT:C283(PPro_JoinLists; $1)
	C_LONGINT:C283(PPro_JoinLists; $0)
End if 

C_LONGINT:C283($sel)
$sel:=$1

C_LONGINT:C283($list)
$list:=New list:C375

Case of 
	: ($sel=1)
		//joing field and variable list
		C_LONGINT:C283($master)
		$master:=PP_TableList
		
		C_LONGINT:C283($count)
		$count:=Count list items:C380($master)
		
		If ($count>0)
			
			C_LONGINT:C283($ref; $child)
			C_BOOLEAN:C305($expanded)
			C_TEXT:C284($text; $prop)
			
			C_LONGINT:C283($i)
			For ($i; 1; $count)
				GET LIST ITEM:C378($master; $i; $ref; $text; $child; $expanded)
				If ($child#0)
					C_LONGINT:C283($subSublist)
					$subSublist:=New list:C375
					C_LONGINT:C283($j)
					For ($j; 1; Count list items:C380($child))
						GET LIST ITEM:C378($child; $j; $childRef; $ChildText)
						APPEND TO LIST:C376($subSublist; $ChildText; $childRef)
						GET LIST ITEM PARAMETER:C985($child; $childRef; "varName"; $prop)
						SET LIST ITEM PARAMETER:C986($subSublist; $childRef; "varName"; $prop)
					End for 
					$child:=$subSublist
				End if 
				APPEND TO LIST:C376($list; $text; $ref; $child; $expanded)
				GET LIST ITEM PARAMETER:C985($master; $ref; "varName"; $prop)
				SET LIST ITEM PARAMETER:C986($list; $ref; "varName"; $prop)
				
			End for 
			
		End if 
		
		$master:=PP_VariableList
		$count:=Count list items:C380($master)
		
		If ($count>0)
			C_LONGINT:C283($ref; $child)
			C_BOOLEAN:C305($expanded)
			C_TEXT:C284($text; $prop)
			
			C_LONGINT:C283($i)
			For ($i; 1; $count)
				GET LIST ITEM:C378($master; $i; $ref; $text; $child; $expanded)
				If ($child#0)
					C_LONGINT:C283($subSublist)
					$subSublist:=New list:C375
					C_LONGINT:C283($j)
					For ($j; 1; Count list items:C380($child))
						GET LIST ITEM:C378($child; $j; $childRef; $ChildText)
						APPEND TO LIST:C376($subSublist; $ChildText; $childRef)
						GET LIST ITEM PARAMETER:C985($child; $childRef; "varName"; $prop)
						SET LIST ITEM PARAMETER:C986($subSublist; $childRef; "varName"; $prop)
					End for 
					$child:=$subSublist
				End if 
				APPEND TO LIST:C376($list; $text; $ref; $child; $expanded)
				GET LIST ITEM PARAMETER:C985($master; $ref; "varName"; $prop)
				SET LIST ITEM PARAMETER:C986($list; $ref; "varName"; $prop)
				
			End for 
			
		End if 
		
	: ($sel=2)  //create table list and joinn with variable list
		
		C_LONGINT:C283($subList)
		$subList:=New list:C375
		
		ARRAY TEXT:C222($titles; 0x0000)
		ARRAY LONGINT:C221($tableNum; 0x0000)
		GET TABLE TITLES:C803($titles; $tableNum)
		
		C_LONGINT:C283($count)
		$count:=Size of array:C274($titles)
		
		If ($count>0)
			C_LONGINT:C283($ref; $child)
			C_BOOLEAN:C305($expanded)
			C_TEXT:C284($text; $prop)
			
			C_LONGINT:C283($i)
			For ($i; 1; $count)
				
				APPEND TO LIST:C376($subList; $titles{$i}; $tableNum{$i})
				SET LIST ITEM PARAMETER:C986($subList; 0; "varName"; "["+Table name:C256($tableNum{$i})+"]")
				
			End for 
			C_TEXT:C284($itemText)
			$itemText:=Get localized string:C991("PP_VSTables")
			APPEND TO LIST:C376($list; $itemText; -1; $sublist; True:C214)
		Else 
			C_LONGINT:C283($table)
			For ($table; 1; Get last table number:C254)
				If (Is table number valid:C999($table))
					APPEND TO LIST:C376($subList; Table name:C256($table); $table)
					SET LIST ITEM PARAMETER:C986($subList; 0; "varName"; "["+Table name:C256($table)+"]")
				End if 
			End for 
			C_TEXT:C284($itemText)
			$itemText:=Get localized string:C991("PP_VSTables")
			APPEND TO LIST:C376($list; $itemText; -1; $sublist; True:C214)
		End if 
		
		$master:=PP_VariableList
		$count:=Count list items:C380($master)
		
		If ($count>0)
			
			C_LONGINT:C283($ref; $child)
			C_BOOLEAN:C305($expanded)
			C_TEXT:C284($text; $prop)
			
			C_LONGINT:C283($i)
			For ($i; 1; $count)
				GET LIST ITEM:C378($master; $i; $ref; $text; $child; $expanded)
				If ($child#0)
					$child:=Copy list:C626($child)
				End if 
				APPEND TO LIST:C376($list; $text; $ref; $child; $expanded)
				GET LIST ITEM PARAMETER:C985($master; $ref; "varName"; $prop)
				SET LIST ITEM PARAMETER:C986($list; $ref; "varName"; $prop)
				
			End for 
			
		End if 
End case 

$0:=$list
