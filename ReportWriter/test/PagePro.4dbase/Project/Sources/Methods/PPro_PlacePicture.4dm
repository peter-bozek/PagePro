//%attributes = {"shared":true}
// PPro_PlacePicture (
//      report;    report reference or 0 for component area
//      section;   Object ID of section or group where to place picture
//      picture; 
//      left;       relative position in section or group
//      top;        relative position in section or group
//      width;     (0 for auto)
//      height;    (0 for auto)
//      format;    (0 - 4) 
//      position)  'front' (default) or 'back' 
//   -> picture object reference

C_LONGINT:C283($1; $0)  // report, section, created object
C_PICTURE:C286($3)
C_REAL:C285($4; $5; $6; $7)  // pos and dimensions
C_LONGINT:C283($8)  // format between 0 and 4
C_TEXT:C284($2; $9)  // 'front' or 'back'

If (False:C215)
	C_LONGINT:C283(PPro_PlacePicture; $1; $0)  // report, created object
	C_PICTURE:C286(PPro_PlacePicture; $3)
	C_REAL:C285(PPro_PlacePicture; $4; $5; $6; $7)  // pos and dimensions
	C_LONGINT:C283(PPro_PlacePicture; $8)  // format between 0 and 4 
	C_TEXT:C284(PPro_PlacePicture; $2; $9)  // section ID, 'front' or 'back' 
End if 

C_PICTURE:C286($pict)
C_LONGINT:C283($err; $rep)
C_REAL:C285($width; $height)

$0:=0

If ($1=0)
	$rep:=PP_ExtArea
Else 
	$rep:=$1
End if 
$pict:=$3

PICTURE PROPERTIES:C457($pict; $width; $height)
If (($width+$height)>0)  //   picture is non-empty
	C_LONGINT:C283($newObject)
	C_TEXT:C284($objectType)
	$objectType:=PPro_Picture:K7905:5
	
	C_LONGINT:C283($section)
	$err:=RW_FindObjectByID($rep; $2; $section)
	If ($err=0)
		$err:=RW_NewObject($rep; $newObject; $objectType; $section)
		
		If ($err=0)
			C_TEXT:C284($rect)
			If ($6#0)
				$width:=$6
			End if 
			If ($7#0)
				$height:=$7
			End if 
			$err:=PPro_Util_CoordToRect(->$rect; $4; $5; $4+$width; $5+$height)
			$err:=PPro_SetPropertyString($rep; $newObject; PProp_Rect:K7906:22; $rect)
			C_BLOB:C604($o)
			VARIABLE TO BLOB:C532($pict; $o)
			$err:=RW_SetProperty($rep; $newObject; "data"; ->$o)
			$err:=PPro_SetPropertyInteger($rep; $newObject; PProp_Format:K7906:41; $8)
			If ($9="back")
				C_LONGINT:C283($parent; $old; $new)
				ARRAY LONGINT:C221($aObjects; 0x0000)
				$err:=RW_GetObjects($rep; $section; "objs"; $aObjects)
				$old:=Find in array:C230($aObjects; $newObject)
				If ($old>0)
					DELETE FROM ARRAY:C228($aObjects; $old; 1)
					INSERT IN ARRAY:C227($aObjects; 1; 1)
					$aObjects{1}:=$newObject
					C_LONGINT:C283($i)
					For ($i; 1; Size of array:C274($aObjects))
						PPro_SetPropertyInteger($rep; $aObjects{$i}; PProp_PrintOrder:K7906:4; $i)
					End for 
				End if 
			End if 
			$0:=$newObject
		End if 
		
	End if 
End if 

