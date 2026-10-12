//%attributes = {"invisible":true}

C_BOOLEAN:C305($b)

If (PPro_EditorCallback("window"; "init"; 0; 0))
	PP_WindowRef:=Open form window:C675("PagePro"; PP_WindowType; PP_WindowTop; PP_WindowLeft; *)
	$b:=PPro_EditorCallback("window"; "open"; PP_WindowRef; 0)
End if 