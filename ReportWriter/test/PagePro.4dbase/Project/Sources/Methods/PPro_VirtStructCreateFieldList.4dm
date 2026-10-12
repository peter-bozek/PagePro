//%attributes = {"invisible":true}
C_LONGINT:C283($0)  //returns default  list of fields

If (False:C215)
	C_LONGINT:C283(PPro_VirtStructCreateFieldList; $0)
End if 

C_LONGINT:C283($table; $field)
C_LONGINT:C283($hList; $hSubList)
$hList:=New list:C375

// first check if virtual structure is defined

ARRAY TEXT:C222($titles; 0x0000)
ARRAY LONGINT:C221($tableNum; 0x0000)
GET TABLE TITLES:C803($titles; $tableNum)

If (Size of array:C274($titles)>0)
	ARRAY TEXT:C222($fieldTitles; 0x0000)
	ARRAY LONGINT:C221($fieldNum; 0x0000)
	
	C_LONGINT:C283($iTable)
	For ($iTable; 1; Size of array:C274($titles))
		$hSubList:=New list:C375
		
		GET FIELD TITLES:C804(Table:C252($tableNum{$iTable})->; $fieldTitles; $fieldNum)
		C_LONGINT:C283($iField)
		For ($iField; 1; Size of array:C274($fieldTitles))
			APPEND TO LIST:C376($hSubList; $fieldTitles{$iField}; $tableNum{$iTable} << 0x0010+$fieldNum{$iField})
			SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "["+Table name:C256($tableNum{$iTable})+"]"+Field name:C257($tableNum{$iTable}; $fieldNum{$iField}))
		End for 
		APPEND TO LIST:C376($hList; $titles{$iTable}; $tableNum{$iTable}; $hSublist; False:C215)
	End for 
	
Else 
	
	For ($table; 1; Get last table number:C254)
		If (Is table number valid:C999($table))
			$hSubList:=New list:C375
			For ($field; 1; Get last field number:C255($table))
				If (Is field number valid:C1000($table; $field))
					APPEND TO LIST:C376($hSubList; Field name:C257($table; $field); $table << 0x0010+$field)
					SET LIST ITEM PARAMETER:C986($hSubList; 0; "varName"; "["+Table name:C256($table)+"]"+Field name:C257($table; $field))
				End if 
			End for 
			APPEND TO LIST:C376($hList; Table name:C256($table); $table; $hSublist; False:C215)
		End if 
	End for 
	
End if 

$0:=$hList
