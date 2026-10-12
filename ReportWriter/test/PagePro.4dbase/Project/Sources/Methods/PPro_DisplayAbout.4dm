//%attributes = {}
C_LONGINT:C283($left; $top; $right; $bottom)
C_LONGINT:C283($oleft; $otop; $oright; $obottom)
C_LONGINT:C283($w; $h; $offset)

$w:=174
$h:=360
$offset:=150
PP_ButtonInfo:=0

OBJECT GET COORDINATES:C663(*; "PP_ButtonInfo"; $oleft; $otop; $oright; $obottom)
GET WINDOW RECT:C443($left; $top; $right; $bottom)
C_LONGINT:C283($win)
$win:=Open window:C153($left+$oright+$offset; $top+$obottom+$offset; $left+$oright+$offset+$w; $top+$obottom+$offset+$h; 32)
DIALOG:C40("About")
CLOSE WINDOW:C154($win)

