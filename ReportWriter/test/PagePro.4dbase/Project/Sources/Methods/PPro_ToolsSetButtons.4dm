//%attributes = {"shared":true}
C_LONGINT:C283($1; $2)

If (False:C215)
	C_LONGINT:C283(PPro_ToolsSetButtons; $1; $2)
End if 

C_BOOLEAN:C305($b)

PP_ToolCursor:=0
PP_ToolText:=0
PP_ToolVariable:=0
PP_ToolField:=0
PP_ToolRect:=0
PP_ToolOval:=0
PP_ToolLine:=0
PP_ToolTable:=0
PP_ToolPicture:=0

Case of 
	: ($1=PProTool_Select:K7902:1)
		PP_ToolCursor:=1
	: ($1=PProTool_Text:K7902:6)
		PP_ToolText:=1
	: ($1=PProTool_Variable:K7902:7)
		PP_ToolVariable:=1
	: ($1=PProTool_Field:K7902:8)
		PP_ToolField:=1
	: ($1=PProTool_Rectangle:K7902:3)
		PP_ToolRect:=1
	: ($1=PProTool_Oval:K7902:4)
		PP_ToolOval:=1
	: ($1=PProTool_Line:K7902:2)
		PP_ToolLine:=1
	: ($1=PProTool_Table:K7902:9)
		PP_ToolTable:=1
	: ($1=PProTool_Picture:K7902:5)
		PP_ToolPicture:=1
End case 
$b:=PPro_EditorCallback("tool"; ""; $2; $1)
