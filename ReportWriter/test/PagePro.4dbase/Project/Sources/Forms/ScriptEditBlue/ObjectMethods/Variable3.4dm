Case of 
		
	: (Form event code:C388=On After Keystroke:K2:26)
		
		C_LONGINT:C283($start; $end)
		GET HIGHLIGHT:C209(PP_Script; $start; $end)
		
		C_TEXT:C284($text)
		$text:=Get edited text:C655
		
		PP_ScriptEditorStartSel:=-1
		PP_ScriptEditorEndSel:=-1
		
		If ($start=$end)
			C_TEXT:C284($command)
			$command:=""
			
			C_TEXT:C284($part)
			
			C_LONGINT:C283($char)
			$char:=Character code:C91(Keystroke:C390)
			If ($char=Carriage return:K15:38)
				PPro_ExecSplitToLines($text; ->$start; ->$end)
				PP_Script:=PPro_ExecBuildIndentedText(->$start; ->$end)
				HIGHLIGHT TEXT:C210(PP_Script; $start; $start)
			Else 
				If ($char>31)
					
					C_LONGINT:C283($i)
					For ($i; $start-1; 1; -1)
						C_LONGINT:C283($char)
						$char:=Character code:C91($text[[$i]])
						C_BOOLEAN:C305($b)
						$b:=($char=33) | ($char=Underline:K14:4) | (($char>=Character code:C91("a")) & ($char<=Character code:C91("z"))) | (($char>=Character code:C91("A")) & ($char<=Character code:C91("Z")))
						If (($char=Carriage return:K15:38) | ($b=False:C215))
							$part:=Substring:C12($text; $i+1; $start-$i-1)
						End if 
						If ($i=1)
							$part:=Substring:C12($text; 1; $start)
						End if 
						If ($part#"")
							$i:=1
						End if 
					End for 
					
					If ($part#"")
						C_LONGINT:C283($pos)
						$pos:=Find in list:C952(PP_CommandList; $part+"@"; 1)
						If ($pos>0)
							PP_ScriptEditorStartSel:=$start-Length:C16($part)
							PP_ScriptEditorEndSel:=$end
							
							SELECT LIST ITEMS BY POSITION:C381(PP_CommandList; $pos)
							OBJECT SET SCROLL POSITION:C906(PP_CommandList; $pos)
							
							If (PP_TypeAhead=1)
								C_LONGINT:C283($list)
								$list:=PP_CommandList
								GET LIST ITEM:C378($list; *; $ref; $command)
								PP_Script:=Substring:C12($text; 1; $start-1)+Substring:C12($command; Length:C16($part)+1)+Substring:C12($text; $end)
								HIGHLIGHT TEXT:C210(PP_Script; $end; PP_ScriptEditorStartSel+Length:C16($command))
							End if 
							
						End if 
						$i:=1
					End if 
					
				End if 
			End if 
		End if 
End case 