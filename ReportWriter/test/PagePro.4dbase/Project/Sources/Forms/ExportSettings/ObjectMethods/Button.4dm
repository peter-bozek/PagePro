//setup flags for export

If (PP_ExportBreaks=1)
	PP_ExportFlag:=PP_ExportFlag | PPro_Export_Breaks:K7904:5
End if 

If (PP_ExportHEaders=1)
	PP_ExportFlag:=PP_ExportFlag | PPro_Export_Headers:K7904:6
End if 

If (PP_ExportTexts=1)
	PP_ExportFlag:=PP_ExportFlag | PPro_Export_Texts:K7904:7
End if 

If (PP_ExportCVS=1)
	PP_ExportFlag:=PP_ExportFlag | PPro_Export_CVSFormat:K7904:4
End if 



