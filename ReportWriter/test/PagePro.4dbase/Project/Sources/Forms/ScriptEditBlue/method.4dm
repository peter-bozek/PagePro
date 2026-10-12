Case of 
	: (Form event code:C388=On Load:K2:1)
		PP_ScriptOld:=PP_Script
		
	: (Form event code:C388=On Data Change:K2:15)
		C_TEXT:C284($name)
		$name:=OBJECT Get name:C1087(Object with focus:K67:3)
		If ($name="SearchText@")
			
			C_TEXT:C284($value)
			$value:=OBJECT Get pointer:C1124(Object with focus:K67:3)->
			If ($value#"")
				PPro_SearchBuild($value)
				If (Size of array:C274(PP_SearchPosition)>0)
					HIGHLIGHT TEXT:C210(PP_Script; PP_SearchPosition{1}; PP_SearchPosition{1}+PP_SearchLen{1})
				End if 
				
			Else 
				ARRAY LONGINT:C221(PP_SearchPosition; 0)
				ARRAY LONGINT:C221(PP_SearchLen; 0)
			End if 
		End if 
End case 