//%attributes = {"invisible":true}
C_LONGINT:C283($1)
C_BOOLEAN:C305($0)  //success
C_TEXT:C284($2)

If (False:C215)
	C_LONGINT:C283(PPro_NewReportDlg; $1)
	C_BOOLEAN:C305(PPro_NewReportDlg; $0)  //success
	C_TEXT:C284(PPro_NewReportDlg; $2)  // Type of report ("new" "static" or "dynamic")
End if 

C_TEXT:C284($text; $sValue; $ReportType)
C_LONGINT:C283($err; $newSection; $newStyle; $lValue; $win)

$ReportType:=$2
$text:=""
$err:=RW_ParseReport($1; $text; 0x0000)  // parsing empty report is area cleared

//default properties of new report
//Report is by default created as A4 portrait with 12 point margina nd physical coordinates

$sValue:="New Report"
$err:=RW_SetProperty($1; $1; PProp_Name:K7906:3; ->$sValue)
$sValue:="595"
$err:=RW_SetProperty($1; $1; PProp_Width:K7906:6; ->$sValue)
$sValue:="842"
$err:=RW_SetProperty($1; $1; PProp_Height:K7906:7; ->$sValue)
$sValue:="12,12,12,12"
$err:=RW_SetProperty($1; $1; PProp_Margins:K7906:9; ->$sValue)
$sValue:="1"
$err:=RW_SetProperty($1; $1; PProp_ShowMargins:K7906:113; ->$sValue)
$err:=PPro_SetPropertyNumber($1; $1; PProp_PhysicalPaper:K7906:8; 1)
$err:=PPro_SetPropertyNumber($1; $1; PProp_ShowObjectBorders:K7906:122; 1)

//create defult report parts
//$err:=RW_NewObject ($1;$newObject;PPro_DataSource ;$1)
//$err:=RW_NewObject ($1;$newObject;PPro_StyleSet ;$1)

//create default style
$err:=RW_NewObject($1; $newStyle; PPro_Style:K7905:25; 0)
$sValue:="Default"
$err:=RW_SetProperty($1; $newStyle; PProp_Name:K7906:3; ->$sValue)
$sValue:="Tahoma"
$err:=RW_SetProperty($1; $newStyle; PProp_FontName:K7906:66; ->$sValue)
$sValue:="12"
$err:=RW_SetProperty($1; $newStyle; PProp_FontSize:K7906:67; ->$sValue)
$lValue:=PPro_Util_ComponentsToColor(0; 0; 0; 1)  //non-transparent black
$err:=RW_SetProperty($1; $newStyle; PProp_TextColor:K7906:71; ->$lValue)
$lValue:=0  //will become default style for all newly created objects
$err:=RW_SetProperty($1; $newStyle; PProp_ID:K7906:16; ->$lValue)


//check type of report
// modified 02.06.2011/10:33/SDS Software GmbH/Mu√ühoff
If (($ReportType="dynamic") | ($ReportType="new"))
	$win:=Open form window:C675("NewDocument"+PP_Environment; Sheet form window:K39:12)
	DIALOG:C40("NewDocument"+PP_Environment)
	CLOSE WINDOW:C154($win)
Else 
	PP_Type1:=0
	PP_Type2:=1
End if 
// end

If (PP_Type1=1)
	//create body section
	$newSection:=PPro_ReportAddSection("body"; $1; 0x000F; "Body"; 144)
	
	If ((PP_ReportTypes=2) | (PP_ReportTypes=4))
		$newSection:=PPro_ReportAddSection("header"; $1; 0x0003; "Odd Header"; 48)
		$newSection:=PPro_ReportAddSection("header"; $1; 0x0004; "Even Header"; 48)
	Else 
		$newSection:=PPro_ReportAddSection("header"; $1; 0x000F; "Header"; 72)
	End if 
	
	If ((PP_ReportTypes=2) | (PP_ReportTypes=4))
		$newSection:=PPro_ReportAddSection("footer"; $1; 0x0003; "Odd Footer"; 48)
		$newSection:=PPro_ReportAddSection("footer"; $1; 0x0004; "Even Footer"; 48)
	Else 
		$newSection:=PPro_ReportAddSection("footer"; $1; 0x000F; "Footer"; 72)
	End if 
	
	If (PP_ReportTypes>2)
		$newSection:=PPro_ReportAddSection("breakHeader"; $1; 0x000F; "Break Header"; 48)
		$newSection:=PPro_ReportAddSection("breakFooter"; $1; 0x000F; "Break Footer"; 48)
		If (PP_ReportTypes=5)
			$newSection:=PPro_ReportAddSection("breakHeader"; $1; 0x000F; "Break Header 1"; 48)
			$newSection:=PPro_ReportAddSection("breakFooter"; $1; 0x000F; "Break Footer 1"; 48)
		End if 
	End if 
	
Else 
	//static report
	$sValue:="1"
	$err:=RW_SetProperty($1; $1; "dyna"; ->$sValue)
	$newSection:=PPro_ReportAddSection("page"; $1; 0x000F; "Page"; 0)
	
End if 

C_TEXT:C284($report)
$err:=RW_SaveReport(PP_ExtArea; $report; 0x0000)  // third arg means it is a variable
PP_Report:=$report

PPro_InitDocument

$0:=($err=0)  //always true