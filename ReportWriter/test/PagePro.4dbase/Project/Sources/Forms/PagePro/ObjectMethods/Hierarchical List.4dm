C_LONGINT:C283($err; $i)

Case of 
	: (Form event code:C388=On Selection Change:K2:29)
		$err:=Selected list items:C379(PP_XMLRoot; PP_ItemsToSelect; *)
		
		ARRAY LONGINT:C221($alObjects; 0x0000)
		C_BOOLEAN:C305($bool)
		
		$i:=0
		$err:=PPro_SetPropertyInteger(PP_ExtArea; PP_ExtArea; "hito"; 1)
		$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "selc"; $alObjects)
		$bool:=False:C215
		For ($i; 0x0001; Size of array:C274($alObjects))
			$err:=RW_SetProperty(PP_ExtArea; $alObjects{$i}; "selc"; ->$bool)
		End for 
		$bool:=True:C214
		For ($i; 0x0001; Size of array:C274(PP_ItemsToSelect))
			$err:=RW_SetProperty(PP_ExtArea; PP_ItemsToSelect{$i}; "selc"; ->$bool)
		End for 
		
		If (Size of array:C274(PP_ItemsToSelect)=0x0001)  // this is necessary - DataSource is not selectable
			PPro_PropertiesReload(PP_ExtArea; PP_ItemsToSelect{0x0001})
			PP_CurrentObject:=PP_ItemsToSelect{0x0001}
			//$err:=RW_GetProperties (PP_ExtArea;PP_ItemsToSelect{0x0001};PP_PropID;PP_PropValue;PP_PropName)
		Else 
			PPro_PropertiesReload(PP_ExtArea; 0x0000)
			//$err:=RW_GetProperties (PP_ExtArea;0x0000;PP_PropID;PP_PropValue;PP_PropName)
		End if 
		
		PPro_AreaFormData(PP_ExtArea)
		//
	: (Form event code:C388=On Double Clicked:K2:5)
		GOTO OBJECT:C206(PP_ExtArea)
End case 

