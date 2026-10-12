//%attributes = {"shared":true}
C_LONGINT:C283($1)

If (False:C215)
	C_LONGINT:C283(PPro_SubformVisibility; $1)
End if 

C_LONGINT:C283($area; $err)
C_TEXT:C284($text)

$area:=$1

OBJECT SET VISIBLE:C603(*; "PP_Subform@"; False:C215)
OBJECT SET VISIBLE:C603(PP_CurrentObject; False:C215)

If ((PP_Dialogs=1) & (PP_CurrentObject#0))
	$text:=PPro_GetPropertyString($area; PP_CurrentObject; "kind")
	Case of 
		: ($text="Text")
			OBJECT SET VISIBLE:C603(*; "PP_SubformText"; True:C214)
			// PPro_TextTabsVisibility (1)
		: (($text="Var") | ($text="Field"))
			C_TEXT:C284($repeating)
			$repeating:=PPro_GetPropertyString($area; PP_CurrentObject; "repe")
			If (($repeating="") | ($repeating="none"))
				OBJECT SET VISIBLE:C603(*; "PP_SubformVar"; True:C214)
			Else 
				OBJECT SET VISIBLE:C603(*; "PP_SubformRepeat"; True:C214)
			End if 
			
		: ($text="line")
			OBJECT SET VISIBLE:C603(*; "PP_SubformLine@"; True:C214)
		: ($text="Rect")
			OBJECT SET VISIBLE:C603(*; "PP_SubformRect"; True:C214)
		: ($text="Oval")
			OBJECT SET VISIBLE:C603(*; "PP_SubformOval"; True:C214)
		: ($text="Pict")
			OBJECT SET VISIBLE:C603(*; "PP_SubformPict"; True:C214)
		: ($text="Guide")
			OBJECT SET VISIBLE:C603(*; "PP_SubformGuide"; True:C214)
		: ($text="Table")
			OBJECT SET VISIBLE:C603(*; "PP_SubformTable"; True:C214)
			
		: ($text="Style")
			OBJECT SET VISIBLE:C603(*; "PP_SubformStyle"; True:C214)
		: ($text="Report")
			OBJECT SET VISIBLE:C603(*; "PP_SubformReport"; True:C214)
		: ($text="DataSource")
			OBJECT SET VISIBLE:C603(*; "PP_SubformDataSource"; True:C214)
		: ($text="Header")
			OBJECT SET VISIBLE:C603(*; "PP_SubformTableHeader"; True:C214)
		: ($text="Column")
			OBJECT SET VISIBLE:C603(*; "PP_SubformColumn"; True:C214)
		: ($text="Group")
			OBJECT SET VISIBLE:C603(*; "PP_SubformGroup"; True:C214)
			
		: ($text="Section")
			C_TEXT:C284($type)
			$type:=PPro_GetPropertyString($area; PP_CurrentObject; PProp_Type:K7906:17)
			Case of 
				: ($type="header")
					OBJECT SET VISIBLE:C603(*; "PP_SubformHeader"; True:C214)
				: ($type="breakHeader")
					OBJECT SET VISIBLE:C603(*; "PP_SubformBreakHeader"; True:C214)
				: ($type="body")
					OBJECT SET VISIBLE:C603(*; "PP_SubformBody"; True:C214)
				: ($type="page")
					OBJECT SET VISIBLE:C603(*; "PP_SubformPage"; True:C214)
				: ($type="breakFooter")
					OBJECT SET VISIBLE:C603(*; "PP_SubformBreakFooter"; True:C214)
				: ($type="footer")
					OBJECT SET VISIBLE:C603(*; "PP_SubformFooter"; True:C214)
				: ($type="watermark")
					OBJECT SET VISIBLE:C603(*; "PP_SubformWatermark"; True:C214)
			End case 
	End case 
	
End if 