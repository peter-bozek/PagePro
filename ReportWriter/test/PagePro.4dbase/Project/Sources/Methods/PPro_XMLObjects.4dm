//%attributes = {"invisible":true}
C_LONGINT:C283($1; $rep)
C_TEXT:C284($2; $sel)
$rep:=$1
$sel:=$2

C_LONGINT:C283($err; $obj; $i; $parent; $section)
C_REAL:C285($shift)
C_TEXT:C284($text; $kind)
C_BOOLEAN:C305($bool)

ARRAY LONGINT:C221($alObjects; 0x0000)
$err:=RW_GetObjects($rep; $rep; "selc"; $alObjects)
If (Size of array:C274($alObjects)>0x0000)
	$err:=RW_GetProperty($rep; $alObjects{0x0001}; "kind"; ->$kind)
	Case of 
		: ($sel="d")  // duplicate
			If ($kind="Report")
				// how to duplicate a report?!?
				BEEP:C151
			Else 
				$shift:=10
				$err:=RW_Area_SaveUndo($rep; 4)
				For ($i; 0x0001; Size of array:C274($alObjects))
					$err:=RW_GetObjectXML($rep; $alObjects{$i}; $text)
					$err:=RW_GetParent($rep; $alObjects{$i}; $parent)
					$bool:=False:C215
					$err:=RW_SetProperty($rep; $alObjects{$i}; "selc"; ->$bool)
					$err:=RW_NewObjectFromXML($rep; $obj; $text; $parent)
					$bool:=True:C214
					$err:=RW_SetProperty($rep; $obj; "selc"; ->$bool)
					$err:=RW_SetProperty($rep; $obj; "movH"; ->$shift)
					$err:=RW_SetProperty($rep; $obj; "movV"; ->$shift)
				End for 
				$err:=RW_Area_SaveUndo($rep; 0)
			End if 
			
		: (($sel="c") | ($sel="x"))  // copy/cut
			// if first selected object is a report or section, copy them
			If (($kind="Report") | ($kind="Section"))
				$err:=RW_GetObjectXML($rep; $alObjects{0x0001}; $text)
				SET TEXT TO PASTEBOARD:C523($text)
				If ($sel="x")
					If ($kind="Report")
						BEEP:C151
					Else 
						$err:=RW_Area_SaveUndo($rep; 5)
						$err:=RW_DeleteObject($rep; $alObjects{0x0001})
						$err:=RW_Area_SaveUndo($rep; 0)
					End if 
				End if 
			Else 
				If ($sel="x")
					$err:=RW_Area_SaveUndo($rep; 5)
				End if 
				// always create a special section for objects
				$err:=RW_NewObject($rep; $parent; "scrs"; 0x0000)  // in simple mode body is not allowed
				//$text:="Invissible_Scrap_Section"
				//$err:=RW_SetProperty ($rep;$parent;"name";->$text)
				$bool:=False:C215
				//$err:=RW_SetProperty ($rep;$parent;"selc";->$bool)
				//$err:=RW_SetProperty ($rep;$parent;"visi";->$bool)
				For ($i; 0x0001; Size of array:C274($alObjects))
					$err:=RW_GetObjectXML($rep; $alObjects{$i}; $text)
					$err:=RW_NewObjectFromXML($rep; $obj; $text; $parent)
					$err:=RW_SetProperty($rep; $obj; "selc"; ->$bool)
					If ($sel="x")
						$err:=RW_DeleteObject($rep; $alObjects{$i})
					End if 
				End for 
				$err:=RW_GetObjectXML($rep; $parent; $text)
				SET TEXT TO PASTEBOARD:C523($text)
				$err:=RW_DeleteObject($rep; $parent)
				If ($sel="x")
					$err:=RW_Area_SaveUndo($rep; 0)
				End if 
			End if 
			
		: ($sel="v")  // paste
			$text:=Get text from pasteboard:C524
			If ($text#"")
				Case of 
					: (Substring:C12($text; 1; 7)="<Report")
						BEEP:C151
					: ((Substring:C12($text; 1; 6)="<Scrap") | (Substring:C12($text; 1; 5)="<Page") | (Substring:C12($text; 1; 5)="<Body") | (Substring:C12($text; 1; 6)="<Break") | (Substring:C12($text; 1; 7)="<Header") | (Substring:C12($text; 1; 7)="<Footer"))
						$err:=RW_Area_SaveUndo($rep; 6)
						// it should be a Section...
						$err:=RW_NewObjectFromXML($rep; $section; $text; 0x0000)
						If (($err=0x0000) & ($section#0x0000))
							$bool:=False:C215
							$err:=RW_SetProperty($rep; $alObjects{0x0001}; "selc"; ->$bool)
							//$err:=RW_GetProperty ($rep;$section;"name";->$text)
							//If ($text="Invissible_Scrap_Section")
							If (Substring:C12($text; 1; 6)="<Scrap")
								$parent:=$alObjects{0x0001}
								$err:=RW_GetProperty($rep; $parent; "kind"; ->$text)
								While (($text#"Section") & ($text#"Group") & ($parent#0))
									$err:=RW_GetParent($rep; $parent; $parent)
									If ($parent=0)
										BEEP:C151
									Else 
										$err:=RW_GetProperty($rep; $parent; "kind"; ->$text)
									End if 
								End while 
								// v1.2.6
								// check parent height
								If (($parent#0) & ($text#"Report"))  // 1.4
									C_LONGINT:C283($height; $top; $objHeight)
									$err:=RW_GetProperty($rep; $parent; "posh"; ->$height)
									$err:=RW_GetObjects($rep; $section; "objs"; $alObjects)
									For ($i; 0x0001; Size of array:C274($alObjects))
										$err:=RW_GetObjectXML($rep; $alObjects{$i}; $text)
										$err:=RW_NewObjectFromXML($rep; $obj; $text; $parent)
										$err:=RW_GetProperty($rep; $obj; "post"; ->$top)
										If ($top>$height)
											$err:=RW_GetProperty($rep; $obj; "posh"; ->$objHeight)
											If ($objHeight>$height)
												$objHeight:=$height
											End if 
											$top:=-($top-($height-$objHeight))
											$err:=RW_SetProperty($rep; $obj; "movV"; ->$top)
										End if 
										$bool:=True:C214
										$err:=RW_SetProperty($rep; $obj; "selc"; ->$bool)
									End for 
								End if 
								
								$err:=RW_DeleteObject($rep; $section)
							End if 
						Else 
							BEEP:C151
						End if 
						$err:=RW_Area_SaveUndo($rep; 0)
					Else   // of case 
						$err:=RW_Area_SaveUndo($rep; 6)
						// something else...
						$section:=$alObjects{0x0001}
						$err:=RW_NewObjectFromXML($rep; $obj; $text; $section)
						If (($err=0x0000) & ($obj#0x0000))
							$bool:=False:C215
							$err:=RW_SetProperty($rep; $alObjects{0x0001}; "selc"; ->$bool)
							$bool:=True:C214
							$err:=RW_SetProperty($rep; $obj; "selc"; ->$bool)
						Else 
							If ($kind="Text")
								$err:=PPro_SetPropertyString($rep; $obj; PProp_Data:K7906:53; $text)
							Else 
								BEEP:C151
							End if 
						End if 
						$err:=RW_Area_SaveUndo($rep; 0)
						
				End case 
			Else 
				If ($kind="Section")
					$section:=$alObjects{0x0001}
					
					C_PICTURE:C286($pict)
					GET PICTURE FROM PASTEBOARD:C522($pict)
					If (Picture size:C356($pict)>0)
						$err:=RW_Area_SaveUndo($rep; 7)
						
						C_LONGINT:C283($width; $height)
						PICTURE PROPERTIES:C457($pict; $width; $height)
						If (($width+$height)>0)
							C_LONGINT:C283($newObject)
							C_TEXT:C284($objectType)
							$objectType:=PPro_Picture:K7905:5
							$err:=RW_NewObject($rep; $newObject; $objectType; $section)
							C_TEXT:C284($rect)
							$err:=PPro_Util_CoordToRect(->$rect; 6; 6; 6+$width; 6+$height)
							$err:=PPro_SetPropertyString($rep; $newObject; PProp_Rect:K7906:22; $rect)
							C_BLOB:C604($o)
							VARIABLE TO BLOB:C532($pict; $o)
							$err:=RW_SetProperty($rep; $newObject; "data"; ->$o)
						End if 
						
						$err:=RW_Area_SaveUndo($rep; 0)
					End if 
				End if 
			End if 
		Else 
			BEEP:C151
	End case 
End if 