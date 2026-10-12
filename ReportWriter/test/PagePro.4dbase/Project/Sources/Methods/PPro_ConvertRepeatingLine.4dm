//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2)  // area and repeating object to be changed to table
C_LONGINT:C283($0)  // reference to table

If (False:C215)
	C_LONGINT:C283(PPro_ConvertRepeatingLine; $0; $1; $2)
End if 

C_LONGINT:C283($area; $object; $table; $column; $parent; $offset; $error; $intVal)
C_REAL:C285($numVal)
C_TEXT:C284($type; $value)

$area:=$1
$object:=$2
$0:=$2
$type:=PPro_GetPropertyString($area; $object; PProp_Kind:K7906:2)
If (($type="var") | ($type="Field"))
	$type:=PPro_GetPropertyString($area; $object; "repe")
	// only vertically repeating object can be converted to table
	If ($type="vertical")
		// we need to select all repeating objects with the same top
		
		$error:=RW_GetParent($area; $object; $parent)
		ARRAY LONGINT:C221($alObjects; 0)
		$error:=RW_GetObjects($area; $parent; PPro_AllObjects:K7905:26; $alObjects)
		C_LONGINT:C283($idx; $obj)
		ARRAY REAL:C219($arLeft; Size of array:C274($alObjects))
		ARRAY REAL:C219($arWidth; Size of array:C274($alObjects))
		ARRAY LONGINT:C221($alJust; Size of array:C274($alObjects))
		
		For ($idx; Size of array:C274($alObjects); 1; -1)
			$obj:=$alObjects{$idx}
			$type:=PPro_GetPropertyString($area; $obj; PProp_Kind:K7906:2)
			If (($type="var") | ($type="Field"))
				$type:=PPro_GetPropertyString($area; $obj; "repe")
				// only vertically repeating object can be converted to table
				If ($type="vertical")
					$numVal:=PPro_GetPropertyNumber($area; $obj; PProp_Left:K7906:25)
					$arLeft{$idx}:=$numVal
					$numVal:=PPro_GetPropertyNumber($area; $obj; PProp_ObjectWidth:K7906:27)
					$arWidth{$idx}:=$numVal
					$value:=PPro_GetPropertyString($area; $obj; PProp_TextJustification:K7906:73)
					Case of 
						: ($value="default")
						: ($value="left")
						: ($value="right")
							$alJust{$idx}:=1
						Else 
							$alJust{$idx}:=2
					End case 
					
				Else 
					DELETE FROM ARRAY:C228($alObjects; $idx; 1)
					DELETE FROM ARRAY:C228($arLeft; $idx; 1)
					DELETE FROM ARRAY:C228($arWidth; $idx; 1)
					DELETE FROM ARRAY:C228($alJust; $idx; 1)
				End if 
			Else 
				DELETE FROM ARRAY:C228($alObjects; $idx; 1)
				DELETE FROM ARRAY:C228($arLeft; $idx; 1)
				DELETE FROM ARRAY:C228($arWidth; $idx; 1)
				DELETE FROM ARRAY:C228($alJust; $idx; 1)
			End if 
		End for 
		
		SORT ARRAY:C229($arLeft; $alObjects; $arWidth; $alJust)
		
		If (Size of array:C274($alObjects)>0)
			
			$error:=RW_Area_SaveUndo($area; 1)
			// create table
			$error:=RW_NewObject($area; $table; PPro_Table:K7905:9; $parent)
			If ($error=0)
				
				C_LONGINT:C283($last)
				$last:=Size of array:C274($alObjects)
				// build a new table by copying properties from var
				// all properties can be coerced to string value
				
				$value:=PPro_GetPropertyString($area; $object; PProp_ID:K7906:16)
				$error:=PPro_SetPropertyString($area; $table; PProp_ID:K7906:16; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_PrintOrder:K7906:4)
				$error:=PPro_SetPropertyString($area; $table; PProp_PrintOrder:K7906:4; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_Visible:K7906:18)
				$error:=PPro_SetPropertyString($area; $table; PProp_Visible:K7906:18; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_Locked:K7906:19)
				$error:=PPro_SetPropertyString($area; $table; PProp_Locked:K7906:19; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_Alignment:K7906:33)
				$error:=PPro_SetPropertyString($area; $table; PProp_Alignment:K7906:33; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_FixedVerticalPos:K7906:31)
				$error:=PPro_SetPropertyString($area; $table; PProp_FixedVerticalPos:K7906:31; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_Draw:K7906:52)
				$error:=PPro_SetPropertyString($area; $table; PProp_Draw:K7906:52; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_Styl:K7906:46)
				$error:=PPro_SetPropertyString($area; $table; PProp_Styl:K7906:46; $value)
				
				$value:=PPro_GetPropertyString($area; $object; PProp_Rect:K7906:22)
				$error:=PPro_SetPropertyString($area; $table; PProp_Rect:K7906:22; $value)
				
				
				// height is variable height plus offset
				$offset:=PPro_GetPropertyNumber($area; $object; "repo")
				$offset:=$offset+PPro_GetPropertyNumber($area; $object; PProp_ObjectHeight:K7906:28)
				$error:=PPro_SetPropertyNumber($area; $table; PProp_ObjectHeight:K7906:28; $offset+4)
				$error:=PPro_SetPropertyNumber($area; $table; PProp_Height:K7906:7; $offset)  // line height
				
				// set table width
				$error:=PPro_SetPropertyNumber($area; $table; PProp_Left:K7906:25; $arLeft{1}-2)
				$error:=PPro_SetPropertyNumber($area; $table; PProp_Right:K7906:26; $arLeft{$last}+$arWidth{$last}+2)
				
				
				// set it to no header, $last columns
				$intVal:=1
				$error:=PPro_SetPropertyInteger($area; $table; PPRop_Columns:K7906:55; $last)
				$error:=PPro_SetPropertyInteger($area; $table; PProp_DrawColumns:K7906:59; $intVal)
				$intVal:=0
				$error:=PPro_SetPropertyInteger($area; $table; PProp_Headers:K7906:56; $intVal)
				$error:=PPro_SetPropertyInteger($area; $table; PProp_DrawHeaders:K7906:58; $intVal)
				
				// frame
				$intVal:=1
				$error:=PPro_SetPropertyInteger($area; $table; PProp_Frame:K7906:42; $intVal)
				$numVal:=1  // add 2 point margin around columns 
				$error:=PPro_SetPropertyNumber($area; $table; PProp_FrameOffset:K7906:43; $numVal)
				$numVal:=0  // but do not draw frame 
				$error:=PPro_SetPropertyNumber($area; $table; PProp_FrameThickness:K7906:44; $numVal)
				
				// get column object
				ARRAY LONGINT:C221($alColumns; 0)
				$error:=RW_GetObjects($area; $table; "colu"; $alColumns)
				If (Size of array:C274($alColumns)=Size of array:C274($alObjects))
					C_LONGINT:C283($col)
					C_LONGINT:C283($nextColumn)
					$nextColumn:=$arLeft{1}
					For ($col; 1; Size of array:C274($alColumns))
						$column:=$alColumns{$col}
						$object:=$alObjects{$col}
						
						$intVal:=PPro_GetPropertyInteger($area; $object; PProp_Styl:K7906:46)
						$error:=PPro_SetPropertyInteger($area; $column; PProp_Styl:K7906:46; $intVal)
						
						$value:=PPro_GetPropertyString($area; $object; PProp_Source:K7906:80)
						$error:=PPro_SetPropertyString($area; $column; PProp_Source:K7906:80; $value)
						
						$value:=PPro_GetPropertyString($area; $object; PProp_Alias:K7906:81)
						$error:=PPro_SetPropertyString($area; $column; PProp_Alias:K7906:81; $value)
						
						$value:=PPro_GetPropertyString($area; $object; PProp_Format:K7906:41)
						$error:=PPro_SetPropertyString($area; $column; PProp_Format:K7906:41; $value)
						
						$value:=PPro_GetPropertyString($area; $object; PProp_Script:K7906:54)
						$error:=PPro_SetPropertyString($area; $column; PProp_Script:K7906:54; $value)
						
						$numVal:=-1  // but do not draw frame 
						$error:=PPro_SetPropertyNumber($area; $table; PProp_Top:K7906:23; $numVal)
						
						Case of 
							: ($col=$last)
								$numVal:=$arLeft{$col}+$arWidth{$col}
							: (($alJust{$col}=0) & ($alJust{$col+1}=0))
								$numVal:=$arLeft{$col+1}
							: ($alJust{$col}=0)
								$numVal:=($arLeft{$col+1}+$arLeft{$col}+$arWidth{$col})/2
							: ($alJust{$col+1}=0)
								$numVal:=$arLeft{$col+1}
							Else 
								$numVal:=($arLeft{$col+1}+$arLeft{$col}+$arWidth{$col})/2
						End case 
						$error:=PPro_SetPropertyNumber($area; $column; PProp_Width:K7906:6; $numVal-$nextColumn)
						$nextColumn:=$numVal
						
						$intVal:=0
						$error:=PPro_SetPropertyInteger($area; $column; PProp_Grid:K7906:62; $intVal)
						
						$error:=RW_DeleteObject($area; $object)
						
					End for 
					$0:=$table
					
				End if 
				
			End if 
			
			$error:=RW_Area_SaveUndo($area; 0)
		End if 
	End if 
End if 