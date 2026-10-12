//%attributes = {"invisible":true}
C_LONGINT:C283($err)

If (PP_CurrentObject>0)
	C_LONGINT:C283($attrib)
	C_TEXT:C284($type)
	$type:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Kind:K7906:2)
	$attrib:=PPro_GetPropertyInteger(PP_ExtArea; PP_CurrentObject; PProp_AttributedText:K7906:48)
	
	C_TEXT:C284($text)
	If ((Focus object:C278=(->PP_InlineEdit)) | (Focus object:C278=(->PP_InlineEditPlain)))
		If ($type="Text")
			GET HIGHLIGHT:C209(PP_InlineEdit; $start; $end)
			If (($attrib=1) & (Focus object:C278=(->PP_InlineEdit)))
				$text:=ST Get plain text:C1092(PP_InlineEdit)
				$text:=Substring:C12($text; 1; $start-1)+"<%"+Substring:C12($text; $start; $end-$start)+"%>"+Substring:C12($text; $end+1)
				ST SET PLAIN TEXT:C1136(PP_InlineEdit; $text)
				HIGHLIGHT TEXT:C210(PP_InlineEdit; $start+2; $end+2)
			Else 
				$text:=PP_InlineEditPlain
				$text:=Substring:C12($text; 1; $start-1)+"<%"+Substring:C12($text; $start; $end-$start)+"%>"+Substring:C12($text; $end+1)
				PP_InlineEditPlain:=$text
				HIGHLIGHT TEXT:C210(PP_InlineEdit; $start+2; $end+2)
			End if 
		End if 
	Else 
		$text:=PPro_GetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53)
		If ($attrib=1)
			$text:="&lt;%"+$text+"%&gt;"
		Else 
			$text:="<%"+$text+"%>"
		End if 
		$err:=PPro_SetPropertyString(PP_ExtArea; PP_CurrentObject; PProp_Data:K7906:53; $text)
	End if 
End if 
