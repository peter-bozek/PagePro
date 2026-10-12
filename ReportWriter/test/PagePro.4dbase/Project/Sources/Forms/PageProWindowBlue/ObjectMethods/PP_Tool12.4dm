// modified 08.06.2011/16:22/SDS Software GmbH/Mußhoff

If (PPro_GetPropertyNumber(PP_ExtArea; PP_ExtArea; PProp_Drawingmode:K7906:127)=7)  // values are shown
	PPro_ReportProperty("Normal"; PP_ExtArea)  //reset
	PPro_ReportProperty("Values"; PP_ExtArea)  //force update
End if 

Self:C308->:=0
