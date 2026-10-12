//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //are into which to open report
C_BOOLEAN:C305($0)  //success
If (False:C215)
	C_LONGINT:C283(PPro_OpenReportDlg; $1)
	C_BOOLEAN:C305(PPro_OpenReportDlg; $0)
End if 

$0:=False:C215
C_TEXT:C284($path; $report)
If (PP_DocumentFolder="")
	$path:=System folder:C487(Desktop:K41:16)
Else 
	$path:=PP_DocumentFolder
End if 
$path:=Select document:C905($path; ""; Get localized string:C991("PP_OpenDocument"); Alias selection:K24:10)
If (OK=0x0001)
	$path:=Document
	
	C_BLOB:C604($o)
	DOCUMENT TO BLOB:C525($path; $o)
	$report:=BLOB to text:C555($o; UTF8 text without length:K22:17)
	C_LONGINT:C283($err)
	If (RW_ParseReport($1; $report; 0x0000)=0)  // third arg means it is a an actual report
		PP_DocumentPath:=$path
		PP_DocumentFolder:=PPro_Util_PathToFolder(PP_DocumentPath)
		$err:=RW_GetObjects($1; $1; "selc"; PP_ItemsToSelect)
		PPro_InitDocument
		$0:=True:C214
	End if 
End if 