//%attributes = {"shared":true}
//%attributes = {"lang":"en","shared":true} comment added and reserved by 4D.
// modified 07.05.2012/08:13/SDS Software GmbH/Frank (2012.1) (FORM) ($ref stat $name in Textobjekten einsetzen)

C_REAL:C285($1; $2)
C_LONGINT:C283($3)

If (False:C215)
	C_REAL:C285(PPro_BlueHandler; $1; $2)
	C_LONGINT:C283(PPro_BlueHandler; $3)
End if 

C_REAL:C285($posX; $posY)
C_LONGINT:C283($obj; $err; $offset)
C_BLOB:C604($o)

$posX:=$1
$posY:=$2
$obj:=$3

C_TEXT:C284($targetKind)
If ($err=0)
	$targetKind:=PPro_GetPropertyString(PP_ExtArea; $obj; PProp_Kind:K7906:2)
End if 

GET PASTEBOARD DATA:C401("eu.sdsbestatter.drag"; $o)

If (BLOB size:C605($o)>0)
	
	C_TEXT:C284($Text_T)
	C_TEXT:C284($ref; $format; $name; $indexStr; $type; $node)
	C_TEXT:C284($itemRef)
	C_TEXT:C284($attrib; $value)
	C_LONGINT:C283($index)
	C_REAL:C285($width; $height)
	
	BLOB TO VARIABLE:C533($o; $type; $offset)  // "var" or "field"
	BLOB TO VARIABLE:C533($o; $ref; $offset)  // Reference: 
	BLOB TO VARIABLE:C533($o; $format; $offset)  // Format: ###.##0,00 and so on
	BLOB TO VARIABLE:C533($o; $name; $offset)  // Name: Alias name for var/field like "Beisetzung am"
	
	BLOB TO VARIABLE:C533($o; $Text_T; $offset)  // Width of new object
	$width:=Num:C11($Text_T)
	If ($width=0)
		$width:=100
	End if 
	
	BLOB TO VARIABLE:C533($o; $Text_T; $offset)  // Height of new object
	$height:=Num:C11($Text_T)
	If ($height=0)
		$height:=20
	End if 
	
	$index:=-2
	If ($offset<BLOB size:C605($o))
		BLOB TO VARIABLE:C533($o; $Text_T; $offset)
		$index:=Num:C11($Text_T)
	End if 
	
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
			$content:=$content+"<%="+$ref  // Frank 07.05.12
		End if 
		If ($format#"")
			// $content:=$content+";\""+$format+"\""
			$content:=$content+";"+$format  // Frank 23.03.12
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
