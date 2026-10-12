Case of 
	: (Form event code:C388=On Load:K2:1)
		
		
		STRING LIST TO ARRAY:C511(13016; PP_ReportTypes)
		PP_Add2Section:=0
		STRING LIST TO ARRAY:C511(13018; PP_BreakOnPopup)
		
		// modified 09.06.2011/14:11/SDS Software GmbH/Mußhoff
		PP_BreakOnPopup:=1
		PP_Source:=""
		PP_ReportTypes:=1  // Header
		
		If ((PP_ReportTypes=2) | (PP_ReportTypes=3))
			OBJECT SET ENABLED:C1123(PP_BreakOnPopup; True:C214)
			OBJECT SET ENABLED:C1123(PP_ColumnPopup; True:C214)
			OBJECT SET ENTERABLE:C238(PP_Source; True:C214)
		Else 
			OBJECT SET ENABLED:C1123(PP_BreakOnPopup; False:C215)
			OBJECT SET ENABLED:C1123(PP_ColumnPopup; False:C215)
			OBJECT SET ENTERABLE:C238(PP_Source; False:C215)
		End if 
		// end
		
	: (Form event code:C388=On Clicked:K2:4)
		
		If ((PP_ReportTypes=2) | (PP_ReportTypes=3))
			OBJECT SET ENABLED:C1123(PP_BreakOnPopup; True:C214)
			OBJECT SET ENABLED:C1123(PP_ColumnPopup; True:C214)
			OBJECT SET ENTERABLE:C238(PP_Source; True:C214)
		Else 
			OBJECT SET ENABLED:C1123(PP_BreakOnPopup; False:C215)
			OBJECT SET ENABLED:C1123(PP_ColumnPopup; False:C215)
			OBJECT SET ENTERABLE:C238(PP_Source; False:C215)
		End if 
		
		
End case 