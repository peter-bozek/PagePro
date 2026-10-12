//%attributes = {}
C_LONGINT:C283($1)
C_REAL:C285($2; $0)
C_TEXT:C284($3)
C_BOOLEAN:C305($4)

If (False:C215)
	C_LONGINT:C283(PPro_SnapCoordinate; $1)
	C_REAL:C285(PPro_SnapCoordinate; $2; $0)
	C_TEXT:C284(PPro_SnapCoordinate; $3)
	C_BOOLEAN:C305(PPro_SnapCoordinate; $4)
End if 

C_REAL:C285($unit; $r; $r1)
C_LONGINT:C283($err; $i)

$unit:=1

If (Count parameters:C259=4)
	If ($4)
		If (PPro_GetPropertyInteger($1; $1; PProp_RulerUnit:K7906:115)=2)
			$unit:=72/25.4
		End if 
	End if 
End if 

C_REAL:C285($r)

$r:=$1

// rounding 
C_LONGINT:C283($round)
$round:=PPro_GetPropertyInteger($1; $1; PProp_Rounding:K7906:125)
Case of 
	: ($round=1)
		$round:=0
	: ($round=10)
		$round:=1
	: ($round=100)
		$round:=2
	Else 
		$round:=0
End case 

$r:=Round:C94($r/$unit; $round)*$unit

// snapping to grid
If (PPro_GetPropertyInteger($1; $1; PProp_SnapToGrid:K7906:118)=1)
	C_REAL:C285($gridSize)
	$gridSize:=PPro_GetPropertyNumber($1; $1; PProp_GridSize:K7906:117)
	If ($gridSize>0)
		$r1:=Round:C94($r/$gridSize; 0)*$gridSize
		If (Abs:C99($r-$r1)<PPro_GetPropertyNumber($1; $1; PProp_Proximity:K7906:126))
			$r:=$r1
		End if 
	End if 
End if 

If (Count parameters:C259>2)
	If (PPro_GetPropertyInteger($1; $1; PProp_SnapToGuides:K7906:120)=1)
		
		ARRAY LONGINT:C221($alObjects; 0x0000)
		$err:=RW_GetObjects($1; $1; PPro_Guides:K7905:20; $alObjects)
		If ($err=0)
			Case of 
				: ($3="hor")
					For ($i; 0x0001; Size of array:C274($alObjects))
						If (PPro_GetPropertyInteger($1; $alObjects{$i}; PProp_Type:K7906:17)=1)  // snap to vertical guides
							If (PPro_GetPropertyInteger($1; $alObjects{$i}; PProp_Visible:K7906:18)=1)
								$r1:=PPro_GetPropertyNumber($1; $alObjects{$i}; PProp_Data:K7906:53)
								If (Abs:C99($r-$r1)<PPro_GetPropertyNumber($1; $1; PProp_Proximity:K7906:126))
									$r:=$r1
									$i:=Size of array:C274($alObjects)
								End if 
							End if 
						End if 
					End for 
					
				: ($3="vert")
					For ($i; 0x0001; Size of array:C274($alObjects))
						If (PPro_GetPropertyInteger($1; $alObjects{$i}; PProp_Type:K7906:17)=0)  // snap to hor guides
							If (PPro_GetPropertyInteger($1; $alObjects{$i}; PProp_Visible:K7906:18)=1)
								$r1:=PPro_GetPropertyNumber($1; $alObjects{$i}; PProp_Data:K7906:53)
								If (Abs:C99($r-$r1)<PPro_GetPropertyNumber($1; $1; PProp_Proximity:K7906:126))
									$r:=$r1
									$i:=Size of array:C274($alObjects)
								End if 
							End if 
						End if 
					End for 
					
			End case 
		End if 
		
	End if 
End if 

$0:=$r