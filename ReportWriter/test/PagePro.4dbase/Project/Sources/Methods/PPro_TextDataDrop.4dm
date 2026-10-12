//%attributes = {"invisible":true}
C_REAL:C285($1; $2)
C_LONGINT:C283($3)

If (False:C215)
	C_REAL:C285(PPro_TextDataDrop; $1; $2)
	C_LONGINT:C283(PPro_TextDataDrop; $3)
End if 

C_REAL:C285($posX; $posY)
C_LONGINT:C283($obj; $err)
C_BLOB:C604($o)

$posX:=$1
$posY:=$2
$obj:=$3

C_TEXT:C284($text)
$text:=Get text from pasteboard:C524

If ($text#"")
	C_TEXT:C284($targetKind)
	$targetKind:=PPro_GetPropertyString(PP_ExtArea; $obj; PProp_Kind:K7906:2)
	
	C_BOOLEAN:C305($isHandled)
	$isHandled:=False:C215
	If ($targetKind="text")
		$isHandled:=True:C214
		C_TEXT:C284($content)
		$content:=PPro_GetPropertyString(PP_ExtArea; $obj; PProp_Data:K7906:53)
		$content:=$content+$text
		$err:=PPro_SetPropertyString(PP_ExtArea; $obj; PProp_Data:K7906:53; $content)
		
	End if 
	
	If (($isHandled=False:C215) & Not:C34(($targetKind="section") | ($targetKind="group")))
		$err:=RW_GetParent(PP_ExtArea; $obj; $obj)
		$targetKind:=PPro_GetPropertyString(PP_ExtArea; $obj; PProp_Kind:K7906:2)
	End if 
	
	If (($targetKind="section") | ($targetKind="group"))
		C_LONGINT:C283($top; $left)
		$top:=PPro_GetPropertyInteger(PP_ExtArea; $obj; PProp_Top:K7906:23)
		$left:=PPro_GetPropertyInteger(PP_ExtArea; $obj; PProp_Left:K7906:25)
		C_TEXT:C284($rect)
		$rect:=PPro_GetPropertyString(PP_ExtArea; $obj; PProp_Rect:K7906:22)
		$posY:=$posY-$top
		$posX:=$posX-$left
		
		If ($posY>0)
			C_LONGINT:C283($newObject)
			C_TEXT:C284($objectType)
			$objectType:=PPro_Text:K7905:6
			
			$err:=RW_NewObject(PP_ExtArea; $newObject; $objectType; $obj)
			$err:=PPro_Util_CoordToRect(->$rect; $posX; $posY; $posX+60; $posY+15)
			$err:=PPro_SetPropertyString(PP_ExtArea; $newObject; PProp_Rect:K7906:22; $rect)
			$err:=PPro_SetPropertyString(PP_ExtArea; $newObject; PProp_Data:K7906:53; $text)
			
		Else 
			TRACE:C157  // position with minus?
		End if 
	End if 
	
End if 
