//%attributes = {"invisible":true}
C_TEXT:C284($1)  // selector: text, html or xml

If (False:C215)
	C_TEXT:C284(PPro_ExportReport; $1)
End if 

C_LONGINT:C283($win)
C_TEXT:C284($sel; $ext)
$sel:=$1

Case of 
	: ($sel="Text")
		PP_ExportFlag:=PPro_Export_Text:K7904:3
		$ext:="text"
		
		$win:=Open form window:C675("ExportSettingsText"+PP_Environment; Sheet form window:K39:12)
		DIALOG:C40("ExportSettingsText"+PP_Environment)
		CLOSE WINDOW:C154($win)
		
	: ($sel="HTML")
		PP_ExportFlag:=PPro_Export_HTML:K7904:2
		$ext:="html"
		
		$win:=Open form window:C675("ExportSettings"+PP_Environment; Sheet form window:K39:12)
		DIALOG:C40("ExportSettings"+PP_Environment)
		CLOSE WINDOW:C154($win)
		
	: ($sel="XML")
		PP_ExportFlag:=PPro_Export_XML:K7904:1
		$ext:="xml"
		
		$win:=Open form window:C675("ExportSettings"+PP_Environment; Sheet form window:K39:12)
		DIALOG:C40("ExportSettings"+PP_Environment)
		CLOSE WINDOW:C154($win)
		
	: ($sel="RW")
		PP_ExportFlag:=0
		$ext:="xml"
		OK:=1
		
	: ($sel="PDF")
		PP_ExportFlag:=PP_DestinationPDF:K7907:5
		$ext:="PDF"
		OK:=1
		
End case 

If (OK=1)
	C_TIME:C306($hRef)
	$hRef:=Create document:C266(""; $ext)
	If (OK=1)
		CLOSE DOCUMENT:C267($hRef)
		If (PP_ExportFlag=PP_DestinationPDF:K7907:5)
			PPro_PrintReport(PP_ExtArea; PP_ExportFlag; Document)
		Else 
			C_TEXT:C284($report)
			C_LONGINT:C283($err)
			$err:=RW_SaveReport(PP_ExtArea; $report; 0x0000)  // save it into variable
			$err:=RW_Process_RW($report; PP_ExportFlag; Document)
		End if 
	End if 
End if 
