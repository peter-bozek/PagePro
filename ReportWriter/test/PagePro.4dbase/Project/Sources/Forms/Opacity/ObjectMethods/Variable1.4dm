Case of 
	: (Form event code:C388=On Data Change:K2:15)
		ACCEPT:C269
		
	: (Form event code:C388=On After Keystroke:K2:26)
		PP_Opacity:=Num:C11(Get edited text:C655)
		If (PP_Opacity<0)
			PP_Opacity:=0
		End if 
		If (PP_Opacity>100)
			PP_Opacity:=100
		End if 
		
		C_TEXT:C284($root; $elem)
		$root:=DOM Create XML Ref:C861("svg")
		$elem:=DOM Create XML element:C865($root; "svg/path"; "d"; "M 0 24 L 100 0 L 100 24 z"; "fill"; "grey"; "stroke"; "blue")
		$elem:=DOM Create XML element:C865($root; "svg/rect"; "x"; String:C10(PP_Opacity); "y"; "0"; "width"; String:C10(100-PP_Opacity); "height"; "24"; "fill"; "white"; "stroke"; "none"; "fill-opacity"; "0.5")
		SVG EXPORT TO PICTURE:C1017($root; PP_Opacity_SVG)
		DOM CLOSE XML:C722($root)
		
		
End case 