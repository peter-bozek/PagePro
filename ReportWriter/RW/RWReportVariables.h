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
	mVarNames [RW_VarPage]		= u"PAGE";
	mVarNames [RW_VarPages]		= u"PAGES";
	mVarNames [RW_VarSubPage]	= u"SUBPAGE";
	mVarNames [RW_VarSubPages]	= u"SUBPAGES";
	mVarNames [RW_VarHorPage]	= u"HORPAGE";
	mVarNames [RW_VarHorPages]	= u"HORPAGES";
	mVarNames [RW_VarFrame]		= u"FRAME";
	mVarNames [RW_VarFrames]	= u"FRAMES";
	mVarNames [RW_VarDateTime]	= u"DATETIME";
	mVarNames [RW_VarDate]		= u"DATE";
	mVarNames [RW_VarTime]		= u"TIME";
	mVarNames [RW_VarName]		= u"NAME";
}

inline	void	SRInitReportVariable (RWTextValue *mVarNames)
{
	RWInitReportVariable (mVarNames);
	// SRReportWriter variables
//	mVarNames [RW_VarSRPage]	= "SRPage";
//	mVarNames [RW_VarSRDate]	= "SRDate";
//	mVarNames [RW_VarSRTime]	= "SRTime";
//	mVarNames [RW_VarSRRecord]	= "SRRecord";
	mVarNames [RW_VarRWDate]	= u"RWDate";
	mVarNames [RW_VarRWTime]	= u"RWTime";
	mVarNames [RW_VarRWRecord]	= u"RWRecord";
}
