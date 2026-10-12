//%attributes = {"invisible":true}
// uploads pictures from selected folder t picture library
// the name of picture file need to be Nnnnn_Ppppppp, 
// where Nnnnn is pictire ID and Pppppp is picture name

C_TEXT:C284($folder)

$folder:=Select folder:C670("Pictures")

If ($folder#"")
	ARRAY TEXT:C222($files; 0)
	DOCUMENT LIST:C474($folder; $files)
	
	C_LONGINT:C283($i)
	For ($i; 1; Size of array:C274($files))
		If ($files{$i}#".@")
			C_LONGINT:C283($num)
			$num:=Num:C11(Substring:C12($files{$i}; 1; 5))
			C_TEXT:C284($name)
			$name:=Substring:C12($files{$i}; 7)
			C_LONGINT:C283($pos)
			$pos:=Position:C15("."; $name)
			If ($pos>0)
				$name:=Substring:C12($name; 1; $pos-1)
			End if 
			C_PICTURE:C286($pict)
			READ PICTURE FILE:C678($folder+$files{$i}; $pict)
			SET PICTURE TO LIBRARY:C566($pict; $num; $name)
		End if 
	End for 
End if 
