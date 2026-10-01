//%attributes = {"invisible":true}
// builds the test report through the editor API (sections, objects, properties);
// "rect" is left;top;right;bottom, objects in a group relative to the group
// and returns its reference; each step's error code goes to results.txt
#DECLARE()->$ref : Integer

var $err; $hdr; $body; $ftr; $obj; $grp : Integer

$err:=RW_NewReport($ref; ""; 0)
RWT_Result("build.new"; String($err))

$err:=RW_NewObject($ref; $hdr; "HDrs"; $ref)
RWT_Result("build.header"; String($err)+" "+String(RWT_SetReal($ref; $hdr; "high"; 50)))
$err:=RW_NewObject($ref; $body; "body"; $ref)
RWT_Result("build.body"; String($err)+" "+String(RWT_SetReal($ref; $body; "high"; 300)))
$err:=RW_NewObject($ref; $ftr; "FOOs"; $ref)
RWT_Result("build.footer"; String($err)+" "+String(RWT_SetReal($ref; $ftr; "high"; 30)))

// header: title, bold 18 pt
$err:=RW_NewObject($ref; $obj; "TEXT"; $hdr)
RWT_Result("build.title"; String($err)+" "+String(RWT_SetText($ref; $obj; "data"; "Faktúra – Žltý kôň úpel ďábelské ódy"))+" "+String(RWT_SetText($ref; $obj; "rect"; "20;10;400;40"))+" "+String(RWT_SetReal($ref; $obj; "size"; 18))+" "+String(RWT_SetBool($ref; $obj; "styB"; True))+" "+String(RWT_SetText($ref; $obj; "name"; "Title")))

// body: dynamic text with 4D variables, attributed (styled) text, a variable object
$err:=RW_NewObject($ref; $obj; "TEXT"; $body)
RWT_Result("build.dynamic"; String($err)+" "+String(RWT_SetText($ref; $obj; "data"; "Name: <%vName%>, note: <%vNote%>"))+" "+String(RWT_SetBool($ref; $obj; "dyna"; True))+" "+String(RWT_SetText($ref; $obj; "rect"; "20;10;400;30"))+" "+String(RWT_SetText($ref; $obj; "name"; "Customer")))

$err:=RW_NewObject($ref; $obj; "VARI"; $body)
RWT_Result("build.variable"; String($err)+" "+String(RWT_SetText($ref; $obj; " src"; "vAmount"))+" "+String(RWT_SetText($ref; $obj; " fmt"; "###,###,##0.00"))+" "+String(RWT_SetText($ref; $obj; "rect"; "20;40;200;60"))+" "+String(RWT_SetText($ref; $obj; "name"; "Amount")))

$err:=RW_NewObject($ref; $obj; "TEXT"; $body)
RWT_Result("build.wrapped"; String($err)+" "+String(RWT_SetText($ref; $obj; "data"; "Dlhý text, ktorý sa zalomí do viacerých riadkov: Príliš žluťoučký kůň úpěl ďábelské ódy. "*3))+" "+String(RWT_SetText($ref; $obj; "rect"; "20;70;250;150")))

// shapes
$err:=RW_NewObject($ref; $obj; "RECT"; $body)
RWT_Result("build.rect"; String($err)+" "+String(RWT_SetText($ref; $obj; "rect"; "20;160;200;220"))+" "+String(RWT_SetReal($ref; $obj; "thic"; 2))+" "+String(RWT_SetText($ref; $obj; "lclr"; "#ffff0000")))
$err:=RW_NewObject($ref; $obj; "OVAL"; $body)
RWT_Result("build.oval"; String($err)+" "+String(RWT_SetText($ref; $obj; "rect"; "220;160;400;220"))+" "+String(RWT_SetBool($ref; $obj; "fill"; True))+" "+String(RWT_SetText($ref; $obj; "fclr"; "#ff00a0ff")))
$err:=RW_NewObject($ref; $obj; "LINE"; $body)
RWT_Result("build.line"; String($err)+" "+String(RWT_SetText($ref; $obj; "rect"; "20;230;400;230")))

// group with a text inside
$err:=RW_NewObject($ref; $grp; "GRP#"; $body)
RWT_Result("build.group"; String($err)+" "+String(RWT_SetText($ref; $grp; "rect"; "20;240;400;280")))
$err:=RW_NewObject($ref; $obj; "TEXT"; $grp)
RWT_Result("build.grouptext"; String($err)+" "+String(RWT_SetText($ref; $obj; "data"; "In a group"))+" "+String(RWT_SetText($ref; $obj; "rect"; "0;0;280;20"))+" "+String(RWT_SetText($ref; $obj; "name"; "GroupText")))

// footer
$err:=RW_NewObject($ref; $obj; "TEXT"; $ftr)
RWT_Result("build.footertext"; String($err)+" "+String(RWT_SetText($ref; $obj; "data"; "Footer"))+" "+String(RWT_SetText($ref; $obj; "rect"; "20;0;200;20"))+" "+String(RWT_SetText($ref; $obj; "name"; "FooterText")))
