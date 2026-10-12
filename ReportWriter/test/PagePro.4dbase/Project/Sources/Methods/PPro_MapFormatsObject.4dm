//%attributes = {"invisible":true}
// Doing actual mapping of objects

C_LONGINT:C283($1; $2)
C_TEXT:C284($3; $4)

If (False:C215)
	C_LONGINT:C283(PPro_MapFormatsObject; $1; $2)
	C_TEXT:C284(PPro_MapFormatsObject; $3; $4)
End if 

C_LONGINT:C283($j)
C_TEXT:C284($type; $curr; $dec; $format)
$type:=PPro_GetPropertyString($1; $2; PProp_Kind:K7906:2)

GET SYSTEM FORMAT:C994(Currency symbol:K60:3; $curr)
GET SYSTEM FORMAT:C994(Decimal separator:K60:1; $dec)

ARRAY LONGINT:C221($alObjects; 0)

Case of 
	: (($type="var") | ($type="Field"))
		$format:=PPro_GetPropertyString($1; $2; PProp_Format:K7906:41)
		If ($format#"")
			If (($3#"") & ($3#$curr))
				$format:=Replace string:C233($format; $3; $curr)
			End if 
			If (($4#"") & ($4#$dec))
				$format:=Replace string:C233($format; $4; $dec)
			End if 
		End if 
		$err:=PPro_SetPropertyString($1; $2; PProp_Format:K7906:41; $format)
		
	: ($type="text")
		C_LONGINT:C283($err)
		$data:=PPro_GetPropertyString($1; $2; PProp_Data:K7906:53)
		
		C_TEXT:C284($data)
		ARRAY LONGINT:C221($start; 0)
		ARRAY LONGINT:C221($len; 0)
		C_LONGINT:C283($offset)
		$offset:=1
		While (Match regex:C1019("<%.*;(.*)%>"; $data; $offset; $start; $len))
			
			$len{0}:=$len{0}-$len{1}
			$format:=Substring:C12($data; $start{1}; $len{1})
			If ($format#"")
				If (($3#"") & ($3#$curr))
					$format:=Replace string:C233($format; $3; $curr)
				End if 
				If (($4#"") & ($4#$dec))
					$format:=Replace string:C233($format; $4; $dec)
				End if 
			End if 
			$data:=Substring:C12($data; 1; $start{1}-1)+$format+Substring:C12($data; $start{1}+$len{1})
			$offset:=$start{0}+$len{0}+Length:C16($format)
			
		End while 
		$err:=PPro_SetPropertyString($1; $2; PProp_Data:K7906:53; $data)
		
	: ($type="group")
		$err:=RW_GetObjects($1; $2; "objs"; $alObjects)
		For ($j; 0x0001; Size of array:C274($alObjects))
			PPro_MapFormatsObject($1; $alObjects{$j}; $3; $4)
		End for 
		
	: ($type="table")
		$err:=RW_GetObjects($1; $2; "colu"; $alObjects)
		For ($j; 0x0001; Size of array:C274($alObjects))
			PPro_MapFormatsObject($1; $alObjects{$j}; $3; $4)
		End for 
		
End case 
