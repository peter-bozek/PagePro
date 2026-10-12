//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2)
C_BOOLEAN:C305($0)

If (False:C215)
	C_LONGINT:C283(PPro_ConfirmDelete; $1; $2)
	C_BOOLEAN:C305(PPro_ConfirmDelete; $0)
End if 

C_LONGINT:C283($area; $object)
C_TEXT:C284($text)
C_LONGINT:C283($value; $err)

$area:=$1
$object:=$2
$0:=True:C214

$err:=RW_GetProperty($area; $object; "kind"; ->$text)
Case of 
		//following objects can be deleted
	: ($text="Text")
	: ($text="Var")
	: ($text="Field")
	: ($text="Header")
	: ($text="Column")
	: ($text="Table")
	: ($text="Pict")
	: ($text="Oval")
	: ($text="Rect")
	: ($text="Pict")
	: ($text="Guide")
		
		//following object must not be deleted
	: ($text="Report")
		$0:=False:C215
	: ($text="DataSource")
		$0:=False:C215
		
		// complex cases
	: ($text="Group")
		ARRAY LONGINT:C221($alObjects; 0)
		$err:=RW_GetObjects($area; $object; "objs"; $alObjects)
		If (Size of array:C274($alObjects)>0)
			$text:=Get localized string:C991("PP_GroupDeleteConfirm")
			CONFIRM:C162($text)
			$0:=(OK=1)
		End if 
		
	: ($text="Style")
		$err:=RW_GetProperty($area; $object; "  id"; ->$value)
		If ($value=0)
			//cannot delete default style
			$0:=False:C215
			$text:=Get localized string:C991("PP_StyleDeleteMsg")
			ALERT:C41($text)
		Else 
			$err:=RW_GetProperty($area; $object; "baid"; ->$value)
			If ($value>-1)
				//cannot delete object style
				$0:=False:C215
				$text:=Get localized string:C991("PP_ObjectDeleteMsg")
				ALERT:C41($text)
			End if 
		End if 
		
	: ($text="Section")
		$err:=RW_GetProperty($area; $object; "name"; ->$text)
		If (($text="Body") | ($text="Page"))
			//section cannot be deleted
			$0:=False:C215
			$text:=Get localized string:C991("PP_SectionDeleteMsg")
			ALERT:C41($text)
		Else 
			ARRAY LONGINT:C221($alObjects; 0)
			$err:=RW_GetObjects($area; $object; "objs"; $alObjects)
			If (Size of array:C274($alObjects)>0)
				$text:=Get localized string:C991("PP_SectionDeleteConfirm")
				CONFIRM:C162($text)
				$0:=(OK=1)
			End if 
		End if 
End case 