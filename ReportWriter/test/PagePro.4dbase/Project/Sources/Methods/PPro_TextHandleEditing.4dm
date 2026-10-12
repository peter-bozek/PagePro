//%attributes = {"invisible":true}
//code handles editing of Text, variabe, field and header fields

C_POINTER:C301($1)
C_LONGINT:C283($2; $3)

If (False:C215)
	C_POINTER:C301(PPro_TextHandleEditing; $1)
	C_LONGINT:C283(PPro_TextHandleEditing; $2; $3)
End if 

C_LONGINT:C283($top; $left; $bottom; $right)
C_LONGINT:C283($topMargin; $leftMargin; $bottomMargin; $rightMargin)
C_LONGINT:C283($oleft; $otop; $oright; $obottom)
C_LONGINT:C283($area; $currentObject)

$area:=$2
$currentObject:=$3

$err:=RW_GetObjectRect($area; $currentObject; $left; $top; $right; $bottom)  // adjusted for scale
// limit the rect to on-screen/on-area!!!
// OBJECT GET COORDINATES($1->;$oleft;$otop;$oright;$obottom)
RW_GetEditorRect($area; $oleft; $otop; $oright; $obottom)

C_LONGINT:C283($w; $h)
$w:=$right-$left
PP_EditWidth:=$w
$h:=$bottom-$top
If ($w<130)
	$w:=130
	$right:=$left+$w
End if 

PP_EditHeight:=$h
If ($h<14)
	$h:=14
	$right:=$left+$w
End if 

// adjust size by margins
$topMargin:=26
$leftMargin:=4
$rightMargin:=20
$bottomMargin:=6

$top:=$top-$topMargin
$left:=$left-$leftMargin

$right:=$right+$rightMargin
$bottom:=$bottom+$bottomMargin

C_LONGINT:C283($win)
GET WINDOW RECT:C443($oleft; $otop; $oright; $obottom)
$win:=Open window:C153($oleft+$left; $otop+$top; $oleft+$right; $otop+$bottom; 2)

C_LONGINT:C283($value; $err)
$err:=RW_GetProperty($area; $currentObject; PProp_AttributedText:K7906:48; ->$value)

PPro_EditMenu("set")
If (($err=0) & ($value=1))
	DIALOG:C40("EditBox"+PP_Environment)
Else 
	DIALOG:C40("EditBoxPlain"+PP_Environment)
End if 
CLOSE WINDOW:C154($win)
PPro_EditMenu("restore")
