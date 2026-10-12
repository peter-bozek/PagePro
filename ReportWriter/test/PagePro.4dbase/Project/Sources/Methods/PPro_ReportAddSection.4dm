//%attributes = {"invisible":true}
C_TEXT:C284($1)
C_LONGINT:C283($2)
C_LONGINT:C283($3)
C_TEXT:C284($4)
C_LONGINT:C283($5)
C_LONGINT:C283($0)

If (False:C215)
	C_TEXT:C284(PPro_ReportAddSection; $1)
	C_LONGINT:C283(PPro_ReportAddSection; $2)
	C_LONGINT:C283(PPro_ReportAddSection; $3)
	C_TEXT:C284(PPro_ReportAddSection; $4)
	C_LONGINT:C283(PPro_ReportAddSection; $5)
	C_LONGINT:C283(PPro_ReportAddSection; $0)
End if 

C_LONGINT:C283($area; $flag; $height; $err; $newSection; $lValue)
C_TEXT:C284($name; $sel)

$sel:=$1
$area:=$2
$flag:=$3
$name:=$4
$height:=$5

If ($name="")
	Case of 
		: ($sel="body")
			$name:="Body"
		: ($sel="page")
			$name:="Page"
		: ($sel="header")
			$name:="Header"
		: ($sel="footer")
			$name:="Footer"
		: ($sel="breakHeader")
			$name:="Break Header"
		: ($sel="BreakFooter")
			$name:="Break Footer"
		: ($sel="watermark")
			$name:="Watermark"
	End case 
End if 


Case of 
	: ($sel="body")
		$err:=RW_NewObject($area; $newSection; PPro_Body:K7905:15; 0)
	: ($sel="page")
		$err:=RW_NewObject($area; $newSection; PPro_Page:K7905:14; 0)
	: ($sel="header")
		$err:=RW_NewObject($area; $newSection; PPro_Header:K7905:12; 0)
	: ($sel="footer")
		$err:=RW_NewObject($area; $newSection; PPro_Footer:K7905:17; 0)
	: ($sel="breakHeader")
		$err:=RW_NewObject($area; $newSection; PPro_BreakHeader:K7905:13; 0)
	: ($sel="breakFooter")
		$err:=RW_NewObject($area; $newSection; PPro_BreakFooter:K7905:16; 0)
	: ($sel="watermark")
		$err:=RW_NewObject($area; $newSection; PPro_Watermark:K7905:18; 0)
		
	Else 
		TRACE:C157
End case 

$err:=RW_SetProperty($area; $newSection; PProp_Name:K7906:3; ->$name)
If ($height>0)
	C_BOOLEAN:C305($fixedHeight)
	$err:=RW_SetProperty($area; $newSection; "posh"; ->$height)
	If (($sel="body") | ($sel="page"))
		$fixedHeight:=False:C215
	Else 
		$fixedHeight:=True:C214
	End if 
	$err:=RW_SetProperty($area; $newSection; "expV"; ->$fixedHeight)
End if 

//by default are all properties on - disable if needed
$lValue:=0
If (Not:C34($flag ?? 0))
	$err:=RW_SetProperty($area; $newSection; "pagF"; ->$lValue)
End if 
If (Not:C34($flag ?? 1))
	$err:=RW_SetProperty($area; $newSection; "pagO"; ->$lValue)
End if 
If (Not:C34($flag ?? 2))
	$err:=RW_SetProperty($area; $newSection; "pagE"; ->$lValue)
End if 
If (Not:C34($flag ?? 3))
	$err:=RW_SetProperty($area; $newSection; "pagL"; ->$lValue)
End if 

$0:=$newSection