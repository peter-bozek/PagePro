If (Form event code:C388=On Begin Drag Over:K2:44)
	C_BLOB:C604($o)
	C_LONGINT:C283($itemRef)
	C_TEXT:C284($itemText)
	GET LIST ITEM:C378(PP_HierList; *; $itemRef; $itemText)
	C_TEXT:C284($type; $ref; $format; $index)
	GET LIST ITEM PARAMETER:C985(PP_HierList; *; "type"; $type)
	If ($type#"")
		C_TEXT:C284($root)
		$root:=DOM Create XML Ref:C861("root")
		GET LIST ITEM PARAMETER:C985(PP_HierList; *; "ref"; $ref)
		GET LIST ITEM PARAMETER:C985(PP_HierList; *; "format"; $format)
		GET LIST ITEM PARAMETER:C985(PP_HierList; *; "index"; $index)
		
		C_TEXT:C284($elem)
		$elem:=DOM Create XML element:C865($root; "item"; "text"; $itemText; "type"; $type; "ref"; $ref)
		If ($format#"")
			DOM SET XML ATTRIBUTE:C866($elem; "format"; $format)
		End if 
		
		If ($index#"")
			DOM SET XML ATTRIBUTE:C866($elem; "index"; $index)
		End if 
		
		C_TEXT:C284($node)
		DOM EXPORT TO VAR:C863($elem; $node)
		DOM CLOSE XML:C722($root)
		
		CLEAR PASTEBOARD:C402
		TEXT TO BLOB:C554($node; $o; UTF8 text without length:K22:17)
		APPEND DATA TO PASTEBOARD:C403("eu.pagepro4D.drag"; $o)
		
		C_TEXT:C284($content)
		If ($type="var")
			$content:=$content+"<%"+$ref
			If ($index#"")
				$content:=$content+"{"+$index+"}"
			End if 
		Else 
			$content:=$content+"<%="+$ref
		End if 
		If ($format#"")
			$content:=$content+";"+$format
		End if 
		$content:=$content+"%>"
		TEXT TO BLOB:C554($content; $o; UTF8 text without length:K22:17)
		APPEND DATA TO PASTEBOARD:C403("com.4d.private.text.utf8"; $o)
	End if 
	
End if 
