//%attributes = {"invisible":true}
C_REAL:C285($1; $2)
C_LONGINT:C283($3)

If (False:C215)
	C_REAL:C285(PPro_NativeDataDrop; $1; $2)
	C_LONGINT:C283(PPro_NativeDataDrop; $3)
End if 

C_REAL:C285($posX; $posY)
C_LONGINT:C283($obj; $err)
C_BLOB:C604($o)

$posX:=$1
$posY:=$2
$obj:=$3

GET PASTEBOARD DATA:C401("eu.pagepro4D.drag"; $o)

If (BLOB size:C605($o)>0)
	C_TEXT:C284($targetKind)
	If ($err=0)
		$targetKind:=PPro_GetPropertyString(PP_ExtArea; $obj; PProp_Kind:K7906:2)
	End if 
	
	C_TEXT:C284($ref; $format; $name; $type; $node)
	
	$node:=BLOB to text:C555($o; UTF8 text without length:K22:17)
	$node:=PPro_Util_Fix4DNode($node)
	
	C_TEXT:C284($itemRef)
	$itemRef:=DOM Parse XML variable:C720($node)
	C_LONGINT:C283($count; $i)
	$count:=DOM Count XML attributes:C727($itemRef)
	C_TEXT:C284($attrib; $value)
	C_LONGINT:C283($index)
	$index:=-2
	C_REAL:C285($width; $height)
	$width:=60
	$height:=15
	For ($i; 1; $count)
		DOM GET XML ATTRIBUTE BY INDEX:C729($itemRef; $i; $attrib; $value)
		Case of 
			: ($attrib="type")
				$type:=$value
			: ($attrib="ref")
				$ref:=$value
			: ($attrib="text")
				$name:=$value
			: ($attrib="format")
				$format:=$value
			: ($attrib="index")
				If ($value#"")
					$index:=Num:C11($value)
				End if 
			: ($attrib="width")
				If ($value#"")
					$width:=Num:C11($value)
				End if 
			: ($attrib="height")
				If ($value#"")
					$height:=Num:C11($value)
				End if 
		End case 
	End for 
	
	C_BOOLEAN:C305($isHandled)
	$isHandled:=False:C215
	If ((($targetKind="var") | ($targetKind="field") | ($targetKind="column")) & (($type="var") | ($type="field")))
		$isHandled:=True:C214
		C_TEXT:C284($objectType)
		Case of 
			: ($type="var")
				$objectType:=PPro_Variable:K7905:7
			: ($type="field")
				$objectType:=PPro_Field:K7905:8
			Else 
				$objectType:=$targetKind
		End case 
		
		If ($targetKind#"column")
			$err:=PPro_SetPropertyString(PP_ExtArea; $obj; PProp_Kind:K7906:2; $objectType)
		End if 
		$err:=PPro_SetPropertyString(PP_ExtArea; $obj; PProp_Alias:K7906:81; $name)
		$err:=PPro_SetPropertyString(PP_ExtArea; $obj; PProp_Format:K7906:41; $format)
		$err:=PPro_SetPropertyString(PP_ExtArea; $obj; PProp_Source:K7906:80; $ref)
		If ($index#-2)
			$err:=PPro_SetPropertyInteger(PP_ExtArea; $newObject; PProp_Element:K7906:82; $index)
		End if 
	End if 
	
	If ($targetKind="text")
		$isHandled:=True:C214
		C_TEXT:C284($content)
		$content:=PPro_GetPropertyString(PP_ExtArea; $obj; PProp_Data:K7906:53)
		If ($type="var")
			$content:=$content+"<%"+$name
			If ($index#-2)
				$content:=$content+"{"+String:C10($index)+"}"
			End if 
		Else 
			$content:=$content+"<%="+$name
		End if 
		If ($format#"")
			$content:=$content+";\""+$format+"\""
		End if 
		$content:=$content+"%>"
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
			Case of 
				: ($type="var")
					$objectType:=PPro_Variable:K7905:7
				: ($type="field")
					$objectType:=PPro_Field:K7905:8
				: ($type="pict")
					$objectType:=PPro_Picture:K7905:5
			End case 
			
			$err:=RW_NewObject(PP_ExtArea; $newObject; $objectType; $obj)
			$err:=PPro_Util_CoordToRect(->$rect; $posX; $posY; $posX+$width; $posY+$height)
			$err:=PPro_SetPropertyString(PP_ExtArea; $newObject; PProp_Rect:K7906:22; $rect)
			
			$err:=PPro_SetPropertyString(PP_ExtArea; $newObject; PProp_Alias:K7906:81; $name)
			$err:=PPro_SetPropertyString(PP_ExtArea; $newObject; PProp_Format:K7906:41; $format)
			$err:=PPro_SetPropertyString(PP_ExtArea; $newObject; PProp_Source:K7906:80; $ref)
			If ($index#-2)
				$err:=PPro_SetPropertyInteger(PP_ExtArea; $newObject; PProp_Element:K7906:82; $index)
			End if 
		Else 
			TRACE:C157  // position with minus?
		End if 
	End if 
	
End if 
