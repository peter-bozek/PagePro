If (Form event code:C388=On Drag Over:K2:13)
	$0:=PPro_OnDragOver
Else 
	PPro_ExtAreaScript
End if 

GOTO OBJECT:C206(PP_ExtArea)
