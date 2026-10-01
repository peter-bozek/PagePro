//%attributes = {"invisible":true}
// ReportWriter plugin tests (phase 10): tests/4D/run_4d_tests.sh <bundle> <arch> <out>
// Writes results.txt (name = value lines) and result files; tests/4D/compare.py compares two runs.

var $ref; $ref2; $err; $session; $obj; $i : Integer
var $xml; $xml2; $text; $path : Text
var $flagsPDF : Integer
ARRAY TEXT($props; 0)
ARRAY TEXT($values; 0)
ARRAY LONGINT($objects; 0)

RWT_Declare
RWT_Init
TEXT TO DOCUMENT(vRWT_Out+"results.txt"; ""; "UTF-8-no-bom")

RWT_Result("version"; RW_GetVersion)
$err:=RW_Register("TEST")

vName:="Peter Novák"
vNote:="a < b & c > d"
vAmount:=1234567.891

// 1. report built through the editor API
$ref:=RWT_BuildReport
$err:=RW_SaveReport($ref; $xml; 0)
RWT_Result("save"; String($err))
RWT_Save("report.xml"; $xml)

// 2. reading back through the API
$err:=RW_FindObjectByID($ref; "Text_2"; $obj)
RWT_Result("find Text_2"; String($err)+" "+String(Num($obj#0)))
$err:=RW_GetProperties($ref; $obj; $props; $values)
RWT_Result("properties Text_2"; String($err)+" "+String(Size of array($props)))
For ($i; 1; Size of array($props))
	RWT_Result("  Text_2."+$props{$i}; $values{$i})
End for
$err:=RW_GetObjectXML($ref; $obj; $text)
RWT_Result("object xml"; String($err))
RWT_Save("object.xml"; $text)
$err:=RW_GetObjects($ref; $ref; "body"; $objects)
RWT_Result("objects of report (body)"; String($err)+" "+String(Size of array($objects)))

// 3. round trip: parse the saved XML into another report, save again
$err:=RW_NewReport($ref2; $xml; 0)
RWT_Result("new from xml"; String($err))
$err:=RW_SaveReport($ref2; $xml2; 0)
RWT_Save("report_roundtrip.xml"; $xml2)
RWT_Result("roundtrip identical"; String(Num($xml=$xml2)))
$err:=RW_ParseReport($ref2; $xml; 0)
RWT_Result("parse into existing"; String($err))

// file variant (bit 0): HFS path from 4D
$path:=vRWT_Out+"report_file.xml"
$err:=RW_SaveReport($ref; $path; 1)
RWT_Result("save to file"; String($err)+" "+String(Num(Test path name($path)=Is a document)))
$err:=RW_ParseReport($ref2; $path; 1)
RWT_Result("parse from file"; String($err))

// 5. styled text commands
$text:=""
$err:=RW_AddStyle($text; "Hello world"; 1; 6; 1; "Arial"; 14; 0x00FF0000; 1; 1; 0; 0)
RWT_Result("add style"; String($err))
RWT_Save("styled_add.txt"; $text)
$xml2:=$text
$err:=RW_RemoveStyle($text; $xml2; 1; 3; 1; ""; 0; 0; 0; 1; 0)
RWT_Result("remove style"; String($err))
RWT_Save("styled_remove.txt"; $text)

// 6. processing and exports (RW_Process_RW); flag values from ET/ETReport.h
$err:=RW_Process_RW($xml; 0; vRWT_Out+"processed.rwxml")
RWT_Result("process rwxml"; String($err))
// static, body, headers, totals + format
$err:=RW_Process_RW($xml; 0x7020+0x0800; vRWT_Out+"export.txt")
RWT_Result("export text"; String($err))
$err:=RW_Process_RW($xml; 0x7020+0x0400; vRWT_Out+"export.html")
RWT_Result("export html"; String($err))
$err:=RW_Process_RW($xml; 0x7020+0x0200; vRWT_Out+"export_xml.xml")
RWT_Result("export xml"; String($err))
// JSON (new): an older plugin writes the file anyway, as its fallback format - only compare in new runs
EXECUTE FORMULA("vRWT_Result:=RW_Process_RW(\""+Replace string($xml; "\""; "\\\"")+"\"; 0x7020+0x8000; \""+vRWT_Out+"new_export.json\")")
RWT_Result("export json (new)"; String(vRWT_Result))

// 7. JSON commands (new): text, file, 4D object - each must give back the same XML
vRWT_Result:=-99
EXECUTE FORMULA("vRWT_Result:=RW_SaveReportJSON("+String($ref)+"; vRWT_JSON; 0)")
RWT_Result("json save (new)"; String(vRWT_Result))
If (vRWT_JSON#"")
	RWT_Save("new_report.json"; vRWT_JSON)
End if
vRWT_Result:=-99
EXECUTE FORMULA("vRWT_Result:=RW_ParseReportJSON("+String($ref2)+"; vRWT_JSON; 0)")
$err:=RW_SaveReport($ref2; $xml2; 0)
RWT_Result("json parse (new)"; String(vRWT_Result)+" same xml "+String(Num($xml2=$xml)))
vRWT_JSON:=vRWT_Out+"new_report_file.json"
vRWT_Result:=-99
EXECUTE FORMULA("vRWT_Result:=RW_SaveReportJSON("+String($ref)+"; vRWT_JSON; 1)")
RWT_Result("json save file (new)"; String(vRWT_Result))
vRWT_Result:=-99
EXECUTE FORMULA("vRWT_Result:=RW_ParseReportJSON("+String($ref2)+"; vRWT_JSON; 1)")
$err:=RW_SaveReport($ref2; $xml2; 0)
RWT_Result("json parse file (new)"; String(vRWT_Result)+" same xml "+String(Num($xml2=$xml)))
vRWT_Object:=Null
EXECUTE FORMULA("vRWT_Object:=RW_SaveReportObject("+String($ref)+")")
If (vRWT_Object#Null)
	RWT_Result("object save (new)"; String(vRWT_Object.tag)+" children "+String(vRWT_Object.children.length))
	vRWT_Object.attributes.name:="Changed in 4D"
	vRWT_Result:=-99
	EXECUTE FORMULA("vRWT_Result:=RW_ParseReportObject("+String($ref2)+"; vRWT_Object)")
	$err:=RW_SaveReport($ref2; $xml2; 0)
	RWT_Result("object parse (new)"; String(vRWT_Result)+" name changed "+String(Num(Position("name=\"Changed in 4D\""; $xml2)>0)))
Else
	RWT_Result("object save (new)"; "Null")
End if
vRWT_Result:=-99
EXECUTE FORMULA("vRWT_Result:=RW_ParseReportJSON("+String($ref2)+"; \"{\\\"tag\\\":1}\"; 0)")
RWT_Result("json invalid (new)"; String(vRWT_Result))

// 8. PDF: eDestinationPDF (4) + default page / job setup, no progress
$flagsPDF:=4+0x0020+0x0200+0x4000
RWT_Log("printing report.pdf")
$err:=RW_Print($xml; 0; $flagsPDF; vRWT_Out+"report.pdf"; 0; "")
RWT_Result("print pdf"; String($err)+" "+String(Num(Test path name(vRWT_Out+"report.pdf")=Is a document)))
RWT_Log("printing processed.pdf")
$err:=RW_Print_RW(vRWT_Out+"processed.rwxml"; 1; $flagsPDF; vRWT_Out+"processed.pdf"; 0; "")
RWT_Result("print rwxml pdf"; String($err))
RWT_Log("printing session.pdf")
$err:=RW_OpenSession($session; $flagsPDF; vRWT_Out+"session.pdf"; $xml; "Session test"; "")
RWT_Result("open session"; String($err)+" "+String(Num($session#0)))
$err:=RW_Print($xml; 0; 0; ""; $session; "")
RWT_Result("session report 1"; String($err))
vName:="Second report"
$err:=RW_Print($xml; 0; 0; ""; $session; "")
RWT_Result("session report 2"; String($err))
$err:=RW_CloseSession($session)
RWT_Result("close session"; String($err)+" "+String(Num(Test path name(vRWT_Out+"session.pdf")=Is a document)))

// 10. files in a folder with non-ASCII characters (4D passes HFS paths on the Mac)
var $folder : Text
$folder:=vRWT_Out+"Súbory ľščť "+Char(0x00DF)+":"
CREATE FOLDER($folder)
$err:=RW_SaveReport($ref; $folder+"správa.xml"; 1)
RWT_Result("unicode path save"; String($err)+" "+String(Num(Test path name($folder+"správa.xml")=Is a document)))
$err:=RW_ParseReport($ref2; $folder+"správa.xml"; 1)
RWT_Result("unicode path parse"; String($err))
$err:=RW_Print($xml; 0; $flagsPDF; $folder+"správa.pdf"; 0; "")
RWT_Result("unicode path pdf"; String($err)+" "+String(Num(Test path name($folder+"správa.pdf")=Is a document)))
vRWT_JSON:=$folder+"správa.json"
vRWT_Result:=-99
EXECUTE FORMULA("vRWT_Result:=RW_SaveReportJSON("+String($ref)+"; vRWT_JSON; 1)")
RWT_Result("unicode path json (new)"; String(vRWT_Result)+" "+String(Num(Test path name($folder+"správa.json")=Is a document)))

// 9. editing: delete an object (last - the old plugin crashes here: RW_DeleteObject
// on a report without an editor area dereferenced a NULL area)
$err:=RW_ParseReport($ref2; $xml; 0)
$err:=RW_FindObjectByID($ref2; "Line_1"; $obj)
$err:=RW_DeleteObject($ref2; $obj)
RWT_Result("delete object"; String($err)+" "+String($obj))
$err:=RW_SaveReport($ref2; $xml2; 0)
RWT_Save("report_deleted.xml"; $xml2)

$err:=RW_DeleteReport($ref2)
$err:=RW_DeleteReport($ref)
RWT_Result("4D errors"; String(vRWT_Errors))
RWT_Log("done")
QUIT 4D
