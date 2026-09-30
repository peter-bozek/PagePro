/*
 *  RWReportVariables.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 29.12.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

#pragma	once
inline	void	RWInitReportVariable (RWTextValue *mVarNames)
{
	// RWReportWriter variables
	mVarNames [RW_VarPage]		= "PAGE";
	mVarNames [RW_VarPages]		= "PAGES";
	mVarNames [RW_VarSubPage]	= "SUBPAGE";
	mVarNames [RW_VarSubPages]	= "SUBPAGES";
	mVarNames [RW_VarHorPage]	= "HORPAGE";
	mVarNames [RW_VarHorPages]	= "HORPAGES";
	mVarNames [RW_VarFrame]		= "FRAME";
	mVarNames [RW_VarFrames]	= "FRAMES";
	mVarNames [RW_VarDateTime]	= "DATETIME";
	mVarNames [RW_VarDate]		= "DATE";
	mVarNames [RW_VarTime]		= "TIME";
	mVarNames [RW_VarName]		= "NAME";
}

inline	void	SRInitReportVariable (RWTextValue *mVarNames)
{
	RWInitReportVariable (mVarNames);
	// SRReportWriter variables
//	mVarNames [RW_VarSRPage]	= "SRPage";
//	mVarNames [RW_VarSRDate]	= "SRDate";
//	mVarNames [RW_VarSRTime]	= "SRTime";
//	mVarNames [RW_VarSRRecord]	= "SRRecord";
	mVarNames [RW_VarRWDate]	= "RWDate";
	mVarNames [RW_VarRWTime]	= "RWTime";
	mVarNames [RW_VarRWRecord]	= "RWRecord";
}
