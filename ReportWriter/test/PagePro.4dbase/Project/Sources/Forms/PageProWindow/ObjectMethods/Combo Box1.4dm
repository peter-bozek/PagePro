C_LONGINT:C283($err; $i)

$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "selc"; PP_ItemsToSelect)
If (Size of array:C274(PP_ItemsToSelect)>1)
	For ($i; 1; Size of array:C274(PP_ItemsToSelect))
		$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_ItemsToSelect{$i}; PProp_Styl:K7906:46; PP_StyleID{PP_Styles})
	End for 
Else 
	If (Size of array:C274(PP_ItemsToSelect)=1)
		If (PP_CurrentObject>0)
			C_TEXT:C284($type)
			$err:=RW_GetProperty(PP_ExtArea; PP_CurrentObject; "kind"; ->$type)
			If (Focus object:C278=(->PP_InlineEdit))
				//inside editing session
				//set property for character run
				If ($type="Text")
					C_LONGINT:C283($start; $end)
					GET HIGHLIGHT:C209(PP_InlineEdit; $start; $end)
					
					C_TEXT:C284($text)
					$err:=RW_ApplyStyle($text; PP_InlineEdit; $start; $end; PP_ExtArea; PP_StyleID{PP_Styles})
					PP_InlineEdit:=$text
					HIGHLIGHT TEXT:C210(PP_InlineEdit; $start; $end)
					
					
				End if 
			Else 
				$err:=PPro_SetPropertyNumber(PP_ExtArea; PP_ItemsToSelect{$i}; PProp_Styl:K7906:46; PP_StyleID{PP_Styles})
			End if 
		End if 
	End if 
End if 
PP_CurrentObject:=PP_CurrentObject  // force redraw of palette