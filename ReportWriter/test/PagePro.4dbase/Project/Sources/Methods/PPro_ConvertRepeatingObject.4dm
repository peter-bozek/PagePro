//%attributes = {"invisible":true}
C_LONGINT:C283($1; $2)  // area and repeating object to be changed to table
C_LONGINT:C283($0)  // reference to table

If (False:C215)
	C_LONGINT:C283(PPro_ConvertRepeatingObject; $0; $1; $2)
End if 

C_LONGINT:C283($area; $object; $table; $column; $parent; $offset; $error; $intVal)
C_REAL:C285($numVal)
C_TEXT:C284($type; $value)

$area:=$1
$object:=$2

$0:=$2  // return back current object
$type:=PPro_GetPropertyString($area; $object; PProp_Kind:K7906:2)
If (($type="var") | ($type="Field"))
	$type:=PPro_GetPropertyString($area; $object; "repe")
	// only vertically repeating object can be converted to table
	If ($type="vertical")
		$error:=RW_GetParent($area; $object; $parent)
		
		$error:=RW_Area_SaveUndo($area; 1)
		// create table
		$error:=RW_NewObject($area; $table; PPro_Table:K7905:9; $parent)
		If ($error=0)
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
			$error:=PPro_SetPropertyNumber($area; $table; PProp_ObjectHeight:K7906:28; $offset+2)
			$error:=PPro_SetPropertyNumber($area; $table; PProp_Height:K7906:7; $offset)  // line height
			
			// increase with by 2
			$numVal:=PPro_GetPropertyNumber($area; $object; PProp_ObjectWidth:K7906:27)
			$error:=PPro_SetPropertyNumber($area; $table; PProp_ObjectWidth:K7906:27; $numVal+2)
			
			// offset by 1 top and left
			$intVal:=-1
			$error:=PPro_SetPropertyInteger($area; $table; PProp_MoveHorizontally:K7906:29; $intVal)
			$intVal:=-1
			$error:=PPro_SetPropertyInteger($area; $table; PProp_MoveVertically:K7906:30; $intVal)
			
			// set it to no header, 1 column
			$intVal:=1
			$error:=PPro_SetPropertyInteger($area; $table; PPRop_Columns:K7906:55; $intVal)
			$error:=PPro_SetPropertyInteger($area; $table; PProp_DrawColumns:K7906:59; $intVal)
			$intVal:=0
			$error:=PPro_SetPropertyInteger($area; $table; PProp_Headers:K7906:56; $intVal)
			$error:=PPro_SetPropertyInteger($area; $table; PProp_DrawHeaders:K7906:58; $intVal)
			
			// frame
			$intVal:=1
			$error:=PPro_SetPropertyInteger($area; $table; PProp_Frame:K7906:42; $intVal)
			$numVal:=1  // add 1 point margin around column 
			$error:=PPro_SetPropertyNumber($area; $table; PProp_FrameOffset:K7906:43; $numVal)
			$numVal:=0  // but do not draw frame 
			$error:=PPro_SetPropertyNumber($area; $table; PProp_FrameThickness:K7906:44; $numVal)
			
			// get column object
			ARRAY LONGINT:C221($alColumns; 0)
			$error:=RW_GetObjects($area; $table; "colu"; $alColumns)
			If (($error=0) & (Size of array:C274($alColumns)=1))
				$column:=$alColumns{1}
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
				
				$numVal:=PPro_GetPropertyNumber($area; $object; PProp_ObjectWidth:K7906:27)
				$error:=PPro_SetPropertyNumber($area; $column; PProp_Width:K7906:6; $numVal)
				
				$intVal:=0
				$error:=PPro_SetPropertyInteger($area; $column; PProp_Grid:K7906:62; $intVal)
				$0:=$table
			End if 
			
			$error:=RW_DeleteObject($area; $object)
		End if 
		
		$error:=RW_Area_SaveUndo($area; 0)
		
	End if 
End if 