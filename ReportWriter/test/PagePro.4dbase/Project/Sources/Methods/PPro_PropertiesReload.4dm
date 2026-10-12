//%attributes = {"shared":true}
C_LONGINT:C283($1; $2)  //area, object

If (False:C215)
	C_LONGINT:C283(PPro_PropertiesReload; $1; $2)  //area
End if 

C_LONGINT:C283($err; $size; $i; $idx; $pos)
C_TEXT:C284($localized)

ARRAY TEXT:C222($PP_PropID; 0x0000)
ARRAY TEXT:C222($PP_PropValue; 0x0000)
ARRAY TEXT:C222($PP_PropName; 0x0000)
$err:=RW_GetProperties($1; $2; $PP_PropID; $PP_PropValue; $PP_PropName)

// first put properties in hierarchy according to localized string array
STRING LIST TO ARRAY:C511(13006; PP_PropertyGroup)
$size:=Size of array:C274(PP_PropertyGroup)
ARRAY TEXT:C222(PP_PropID; $size)
ARRAY TEXT:C222(PP_PropValue; $size)
ARRAY TEXT:C222(PP_PropName; $size)
ARRAY BOOLEAN:C223($hasValue; $size)

C_LONGINT:C283($i; $pos)
C_TEXT:C284($localized)
For ($i; 1; Size of array:C274($PP_PropName))
	$localized:=Get localized string:C991("PP_"+$PP_PropName{$i})
	$idx:=Find in array:C230(PP_PropertyGroup; $localized)
	If ($idx>0)
		$pos:=Position:C15(";"; $localized)
		If ($pos>0)
			PP_PropertyGroup{$idx}:=Substring:C12($localized; 1; $pos-1)
			PP_PropName{$idx}:=Substring:C12($localized; $pos+1)
		End if 
		PP_PropID{$idx}:=$PP_PropID{$i}
		PP_PropValue{$idx}:=$PP_PropValue{$i}
		$hasValue{$idx}:=True:C214
	Else 
		//property that is not in list of localized names
		//will be included at the bottom
		
		$size:=Size of array:C274(PP_PropertyGroup)+1
		INSERT IN ARRAY:C227(PP_PropertyGroup; $size; 1)
		INSERT IN ARRAY:C227(PP_PropID; $size; 1)
		INSERT IN ARRAY:C227(PP_PropValue; $size; 1)
		INSERT IN ARRAY:C227(PP_PropName; $size; 1)
		INSERT IN ARRAY:C227($hasValue; $size; 1)
		PP_PropertyGroup{$size}:="Other"
		PP_PropID{$size}:=$PP_PropID{$i}
		PP_PropValue{$size}:=$PP_PropValue{$i}
		PP_PropName{$size}:=$PP_PropName{$i}
		$hasValue{$size}:=True:C214
	End if 
End for 
For ($i; Size of array:C274($hasValue); 1; -1)
	If ($hasValue{$i}=False:C215)
		DELETE FROM ARRAY:C228(PP_PropertyGroup; $i; 1)
		DELETE FROM ARRAY:C228(PP_PropID; $i; 1)
		DELETE FROM ARRAY:C228(PP_PropValue; $i; 1)
		DELETE FROM ARRAY:C228(PP_PropName; $i; 1)
	End if 
End for 
REDRAW:C174(PP_PropertyPalette)
