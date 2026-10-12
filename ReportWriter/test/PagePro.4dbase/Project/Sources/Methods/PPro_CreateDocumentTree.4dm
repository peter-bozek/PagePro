//%attributes = {"invisible":true}
//PPro_CreateDocumentTree
C_LONGINT:C283($0)  //error
C_LONGINT:C283($1)  //external area
C_POINTER:C301($2)  //reference to hList 
C_LONGINT:C283($3)  //element to select

If (False:C215)
	C_LONGINT:C283(PPro_CreateDocumentTree; $0)  //error
	C_LONGINT:C283(PPro_CreateDocumentTree; $1)  //external area
	C_POINTER:C301(PPro_CreateDocumentTree; $2)  //reference to hList 
	C_LONGINT:C283(PPro_CreateDocumentTree; $3)  //element to select
End if 

C_LONGINT:C283(PP_ExtArea)
C_LONGINT:C283($hList)
C_TEXT:C284($name)

C_LONGINT:C283($err; $i; $id; $lXMLStyles; $lXMLSections; $lSection; $base; $lGuides; $hOldList)
C_TEXT:C284($name; $type; $text)

PP_ExtArea:=$1
$hOldList:=$2->

$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "name"; ->$name)
If ($err=0x0000)
	
	$hList:=New list:C375
	
	// editor styles
	$lXMLStyles:=New list:C375
	ARRAY LONGINT:C221($alObjects; 0x0000)
	$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "STYL"; $alObjects)
	For ($i; 0x0001; Size of array:C274($alObjects))
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "id"; ->$id)
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "name"; ->$name)
		APPEND TO LIST:C376($lXMLStyles; $name; $alObjects{$i})
		SET LIST ITEM PROPERTIES:C386($lXMLStyles; 0; False:C215; 0; Use PicRef:K28:4+12103)
	End for 
	$text:=Get localized string:C991("PP_EditorStyles")
	APPEND TO LIST:C376($hList; $text; PPro_Util_TypeToLong("STYL"); $lXMLStyles; PPro_CheckSublistState($hOldList; PPro_Util_TypeToLong("STYL")))
	SET LIST ITEM PROPERTIES:C386($hList; 0; False:C215; 0; Use PicRef:K28:4+12113)
	
	// Report Root
	$err:=RW_GetProperty(PP_ExtArea; PP_ExtArea; "name"; ->$name)
	If ($name="")
		$name:="New Report"
	End if 
	$text:=Get localized string:C991("PP_Report")
	APPEND TO LIST:C376($hList; $text+" - "+$name; PP_ExtArea)  // PPro_Util_TypeToLong ("docu"))
	SET LIST ITEM PROPERTIES:C386($hList; 0; False:C215; 0; Use PicRef:K28:4+12101)
	
	// DataSource
	ARRAY LONGINT:C221($alObjects; 0x0000)
	$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "DATA"; $alObjects)
	If (($err=0x0000) & (Size of array:C274($alObjects)=0x0001))
		$text:=Get localized string:C991("PP_DataSource")
		APPEND TO LIST:C376($hList; $text; $alObjects{0x0001})
		SET LIST ITEM PROPERTIES:C386($hList; 0; False:C215; 0; Use PicRef:K28:4+12104)
	End if 
	
	// styles
	$lXMLStyles:=New list:C375
	ARRAY LONGINT:C221($alObjects; 0x0000)
	$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "STL#"; $alObjects)
	For ($i; 0x0001; Size of array:C274($alObjects))
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "id"; ->$id)
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "name"; ->$name)
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "baid"; ->$base)
		If ($base=-1)
			APPEND TO LIST:C376($lXMLStyles; $name; $alObjects{$i})
			SET LIST ITEM PROPERTIES:C386($lXMLStyles; 0; False:C215; 0; Use PicRef:K28:4+12103)
		End if 
	End for 
	$text:=Get localized string:C991("PP_GlobalStyles")
	APPEND TO LIST:C376($hList; $text; PPro_Util_TypeToLong("styl"); $lXMLStyles; PPro_CheckSublistState($hOldList; PPro_Util_TypeToLong("styl")))
	SET LIST ITEM PROPERTIES:C386($hList; 0; False:C215; 0; Use PicRef:K28:4+12113)
	
	// Guides
	$lGuides:=New list:C375
	ARRAY LONGINT:C221($alObjects; 0x0000)
	$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; PPro_Guides:K7905:20; $alObjects)
	For ($i; 0x0001; Size of array:C274($alObjects))
		If (PPro_GetPropertyInteger(PP_ExtArea; $alObjects{$i}; PProp_Type:K7906:17)=0)
			$name:=Get localized string:C991("PP_GuideTypeHor")
		Else 
			$name:=Get localized string:C991("PP_GuideTypeVert")
		End if 
		$name:=$name+" ("+String:C10(PPro_GetPropertyNumber(PP_ExtArea; $alObjects{$i}; PProp_Data:K7906:53))+")"
		
		APPEND TO LIST:C376($lGuides; $name; $alObjects{$i})
		SET LIST ITEM PROPERTIES:C386($lGuides; 0; False:C215; 0; Use PicRef:K28:4+12114)
		
	End for 
	$text:=Get localized string:C991("PP_Guides")
	APPEND TO LIST:C376($hList; $text; PPro_Util_TypeToLong("GUID"); $lGuides; PPro_CheckSublistState($hOldList; PPro_Util_TypeToLong("GUID")))
	SET LIST ITEM PROPERTIES:C386($hList; 0; False:C215; 0; Use PicRef:K28:4+12114)
	
	// sections
	$lXMLSections:=New list:C375
	ARRAY LONGINT:C221($alObjects; 0x0000)
	$err:=RW_GetObjects(PP_ExtArea; PP_ExtArea; "body"; $alObjects)
	For ($i; 0x0001; Size of array:C274($alObjects))
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "type"; ->$type)
		$err:=RW_GetProperty(PP_ExtArea; $alObjects{$i}; "name"; ->$name)
		$lSection:=New list:C375
		APPEND TO LIST:C376($lXMLSections; $name; $alObjects{$i}; $lSection; False:C215)
		SET LIST ITEM PROPERTIES:C386($lXMLSections; 0; False:C215; 0; Use PicRef:K28:4+12102)
		PPro_LoadObjects(PP_ExtArea; $alObjects{$i}; $lSection)
	End for 
	$text:=Get localized string:C991("PP_Sections")
	APPEND TO LIST:C376($hList; $text; PPro_Util_TypeToLong("sect"); $lXMLSections; PPro_CheckSublistState($hOldList; PPro_Util_TypeToLong("sect")))
	SET LIST ITEM PROPERTIES:C386($hList; 0; False:C215; 0; Use PicRef:K28:4+12102)
	
	// hardcoded variable names...
	ARRAY TEXT:C222(PP_PropID; 0x0000)
	ARRAY TEXT:C222(PP_PropValue; 0x0000)
	ARRAY TEXT:C222(PP_PropName; 0x0000)
	
	If (Count parameters:C259>0x0002)
		SELECT LIST ITEMS BY REFERENCE:C630($hList; $3)
		PPro_PropertiesReload(PP_ExtArea; $3)
	Else 
		// show properties of the selected object(s)
		//ARRAY LONGINT(PP_ItemsToSelect;0x0000)
		//$err:=RW_GetObjects (PP_ExtArea;PP_ExtArea;"selc";PP_ItemsToSelect)
		SELECT LIST ITEMS BY REFERENCE:C630($hList; 0x0000; PP_ItemsToSelect)
		If (Size of array:C274(PP_ItemsToSelect)=0x0001)  // this is necessary - DataSource is not selectable
			PPro_PropertiesReload(PP_ExtArea; PP_ItemsToSelect{0x0001})
		Else 
			PPro_PropertiesReload(PP_ExtArea; 0)
		End if 
	End if 
	REDRAW:C174(PP_PropertyPalette)
End if 

If (Is a list:C621($hOldList))
	CLEAR LIST:C377($hOldList; *)
End if 

$0:=$err
$2->:=$hList