// modified 08.06.2011/13:59/SDS Software GmbH/Mußhoff

C_LONGINT:C283($ArraySize_L)
C_LONGINT:C283($Counter_L)
C_TEXT:C284($EditStyleText_T)
C_TEXT:C284($Ergebnis_T)
C_LONGINT:C283($Selection_L)
C_LONGINT:C283($err; $i)
C_TEXT:C284($Text_T)

$ArraySize_L:=0
$Counter_L:=0
$EditStyleText_T:=""
$Ergebnis_T:=""
$Selection_L:=0
$Text_T:=""

ARRAY TEXT:C222($ArrayText_aT; 0)
STRING LIST TO ARRAY:C511(13105; $ArrayText_aT)
$EditStyleText_T:=$ArrayText_aT{1}

$ArraySize_L:=Size of array:C274(PP_Styles)
$Text_T:=""
For ($Counter_L; 1; $ArraySize_L)
	$Text_T:=$Text_T+PP_Styles{$Counter_L}+";"
End for 
$Text_T:=$Text_T+"-;"+$EditStyleText_T
PP_Styles:=Pop up menu:C542($Text_T)

Case of 
	: (PP_Styles>$ArraySize_L)
		// Edit styles ...
		$Ergebnis_T:=PPro_EditStyles
		
	: (PP_Styles>0)
		// Set style ...
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
		
	Else 
		// sc only
End case 

PP_CurrentObject:=PP_CurrentObject  // force redraw of palette

Self:C308->:=0
