//%attributes = {"invisible":true}
C_LONGINT:C283($1)  //area
C_REAL:C285($2)

If (False:C215)
	C_LONGINT:C283(PPro_ScaleTo; $1)  //area
	C_REAL:C285(PPro_ScaleTo; $2)
End if 

C_REAL:C285($scale)
C_LONGINT:C283($err; $l; $area)

$area:=$1
Case of 
	: ($2>0)
		$scale:=$2
	: ($2=-1)  // scale to fit
		C_REAL:C285($oleft; $otop; $oright; $obottom; $reportWidth; $reportHeight; $width; $height; $hScale; $vScale)
		RW_GetEditorRect($area; $oleft; $otop; $oright; $obottom)
		$width:=$oright-$oleft
		$height:=$obottom-$otop
		$err:=RW_GetProperty($area; $area; "widt"; ->$reportWidth)
		$err:=RW_GetProperty($area; $area; "high"; ->$reportHeight)
		If ($reportWidth#0)
			$hScale:=$width/$reportWidth
		End if 
		If ($reportHeight#0)
			$vScale:=$height/$reportHeight
		End if 
		If ($hScale>$vScale)
			$scale:=$vScale
		Else 
			$scale:=$hscale
		End if 
		
		
	Else 
		//special case - scale to screen resolution
		$scale:=PPro_GetPropertyNumber($area; $area; "dpiX")/72
End case 

$err:=RW_SetProperty($area; $area; "scal"; ->$scale)
$err:=RW_GetProperty($area; $area; "scal"; ->$scale)
PP_Scaling:=Round:C94($scale*100; 0x0002)
