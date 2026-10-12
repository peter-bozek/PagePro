//%attributes = {"invisible":true}
C_TEXT:C284($1)

If (False:C215)
	C_TEXT:C284(PPro_DataSourceProperties; $1)
End if 

C_TEXT:C284($sel)
$sel:=$1

Case of 
	: ($sel="get")
		
		//variable PP_CurrentObject contains a data source object reference
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($err; $i; $j; $dataSource)
		
		$dataSource:=OBJECT Get pointer:C1124(Object subform container:K67:4)->
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="DataSource")  //
			
			$text:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_Source:K7906:80)
			$i:=Find in array:C230(PP_DataSourcePopupMirror; $text)
			If ($i>0)
				PP_DataSourcePopup:=$i
			Else 
				PP_DataSourcePopup:=1
			End if 
			
			If (PP_DataSourcePopup=2)  //table
				$i:=PPro_GetPropertyInteger(PP_ExtArea; $dataSource; PProp_MainTable:K7906:104)
				If (($i>0) & ($i<=Get last table number:C254) & Is table number valid:C999($i))
					PP_Name:="["+Table name:C256($i)+"]"
				Else 
					PP_Name:=""
				End if 
			Else 
				PP_Name:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_Name:K7906:3)
			End if 
			PP_Iterations:=PPro_GetPropertyInteger(PP_ExtArea; $dataSource; PPRop_FixedIterations:K7906:102)
			
			$text:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_RelateOne:K7906:106)
			$i:=Find in array:C230(PP_RelatePopupMirror; $text)
			If ($i>0)
				PP_Relate1Popup:=$i
			Else 
				PP_Relate1Popup:=1
			End if 
			$text:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_RelateMany:K7906:107)
			$i:=Find in array:C230(PP_RelatePopupMirror; $text)
			If ($i>0)
				PP_RelateManyPopup:=$i
			Else 
				PP_RelateManyPopup:=1
			End if 
			
			PP_StartScript:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_StartScript:K7906:108)
			PP_BodyScript:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_BodyScript:K7906:109)
			PP_EndScript:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_EndScript:K7906:110)
			PP_Callback:=PPro_GetPropertyString(PP_ExtArea; $dataSource; PProp_Callback:K7906:111)
			
			PP_SRPComaptibility:=PPro_GetPropertyInteger(PP_ExtArea; $dataSource; PProp_SRPCompatibility:K7906:112)
			
		End if 
		
	: ($sel="set")
		C_TEXT:C284($s; $text)
		C_LONGINT:C283($err; $i; $j; $dataSource)
		
		$dataSource:=OBJECT Get pointer:C1124(Object subform container:K67:4)->
		$text:=PPro_GetPropertyString(PP_ExtArea; OBJECT Get pointer:C1124(Object subform container:K67:4)->; PProp_Kind:K7906:2)
		If ($text="DataSource")  //
			$err:=RW_Area_SaveUndo(PP_ExtArea; 1)  //start undo block
			
			If (PP_DataSourcePopup>0)
				$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_Source:K7906:80; PP_DataSourcePopupMirror{PP_DataSourcePopup})
				If (PP_DataSourcePopup=2)  //table
					$i:=PPro_ResolveTableName(PP_Name)
					If ($i>0)
						$err:=PPro_SetPropertyInteger(PP_ExtArea; $dataSource; PProp_MainTable:K7906:104; $i)
						$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_Name:K7906:3; PP_Name)
					Else 
						BEEP:C151
						PP_Name:=""
					End if 
				Else 
					$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_Name:K7906:3; PP_Name)
				End if 
			End if 
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $dataSource; PPRop_FixedIterations:K7906:102; PP_Iterations)
			
			If (PP_Relate1Popup>0)
				$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_RelateOne:K7906:106; PP_RelatePopupMirror{PP_Relate1Popup})
			End if 
			If (PP_RelateManyPopup>0)
				$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_RelateMany:K7906:107; PP_RelatePopupMirror{PP_RelateManyPopup})
			End if 
			
			
			$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_StartScript:K7906:108; PP_StartScript)
			$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_BodyScript:K7906:109; PP_BodyScript)
			$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_EndScript:K7906:110; PP_EndScript)
			$err:=PPro_SetPropertyString(PP_ExtArea; $dataSource; PProp_Callback:K7906:111; PP_Callback)
			
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $dataSource; PProp_SRPCompatibility:K7906:112; PP_SRPComaptibility)
			
			$err:=RW_Area_SaveUndo(PP_ExtArea; 0)  //end undo block
			
		End if 
End case 