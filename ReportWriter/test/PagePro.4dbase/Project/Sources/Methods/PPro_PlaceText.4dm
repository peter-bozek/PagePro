//%attributes = {}
// PPro_PlaceText (
//      report;    report reference or 0 for component area
//      section;   Object ID of section or group where to place text
//      text; 
//      left;       relative position in section or group
//      top;        relative position in section or group
//      width;     
//      height;    
//      styleName;  name of the text style
//      position)  'front' (default) or 'back' 
//   -> picture object reference

C_LONGINT:C283($1; $0)  // report, section, created object
C_TEXT:C284($3)
C_REAL:C285($4; $5; $6; $7)  // pos and dimensions
C_TEXT:C284($8)  // style name
C_TEXT:C284($2; $9)  // 

If (False:C215)
	C_LONGINT:C283(PPro_PlaceText; $1; $0)  // report, section, created object
	C_TEXT:C284(PPro_PlaceText; $3)
	C_REAL:C285(PPro_PlaceText; $4; $5; $6; $7)  // pos and dimensions
	C_TEXT:C284(PPro_PlaceText; $8)  // style ID 
	C_TEXT:C284(PPro_PlaceText; $2; $9)  // 
End if 

C_PICTURE:C286($pict)
C_LONGINT:C283($err; $rep)
C_REAL:C285($width; $height)
C_TEXT:C284($t)

$0:=0

If ($1=0)
	$rep:=PP_ExtArea
Else 
	$rep:=$1
End if 
$t:=$3

$width:=120
$height:=16

C_LONGINT:C283($newObject)
C_TEXT:C284($objectType)
$objectType:=PPro_Text:K7905:6
C_LONGINT:C283($section)
$err:=RW_FindObjectByID($rep; $2; $section)
If ($err=0)
	$err:=RW_NewObject($rep; $newObject; $objectType; $section)
	
	C_LONGINT:C283($style)
	C_TEXT:C284($name)
	ARRAY LONGINT:C221($alStyles; 0)
	$err:=RW_GetObjects($rep; $rep; PPro_StyleSet:K7905:24; $alStyles)
	For ($i; 1; Size of array:C274($alStyles))
		$err:=RW_GetProperty($rep; $alStyles{$i}; PProp_Name:K7906:3; ->$name)
		If ($8=$name)
			$i:=Size of array:C274($alStyles)
			$style:=$alStyles{$i}
		End if 
	End for 
	
	$err:=RW_GetProperty($rep; $style; PProp_FontSize:K7906:67; ->$height)
	$width:=$height*Length:C16($t)
	If ($width>900)
		$width:=900
	End if 
	
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
		
		$err:=RW_SetProperty($rep; $newObject; "data"; ->$t)
		$err:=PPro_SetPropertyInteger($rep; $newObject; PProp_Styl:K7906:46; $style)
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

