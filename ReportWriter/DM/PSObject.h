/*
 *  PSObject.h
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 25.09.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */
#ifndef	_PSObject_h_
#define	_PSObject_h_

# include	"RWBaseTypes.h"

// forward declaration
class ExtendedExecute;

// Persistent State Object - Load from XML, Save to XML, manipulate properties

class	PSObject;
typedef	RWList<PSObject*>		PSObjList;			// list
typedef	RWArray<PSObject*>		PSObjListD;			// list with destruction of objects
typedef	RWArray<PSObjListD*>	PSObjTableD;		// table with destruction of objects
//typedef	RWMap<OSType, RWValue>	PSPropsMap;

class PSPropsMap : public map<OSType, RWValue>
{
public:
				bool			HasProperty (OSType id) const;
				bool			GetProperty (OSType id, RWValue &outValue) const;
				bool			GetPropertyRef (OSType id, RWValue &outValue) const;
				bool			SetProperty (OSType id, const RWValue &inValue);
				bool			SetPropertyRef (OSType id, const RWValue &inValue);
				bool			RemoveProperty (OSType id);
};


class	PSObject
{
public:
	enum	EObject_Kind	{	eObject_Group = 0, eObject_Line, eObject_Rect, eObject_Oval, eObject_Pict,
								eObject_Text, eObject_Var, eObject_Fld,
								eObject_Table, eObject_TblHdr, eObject_TblCol,
								eObject_Document, eObject_DataSource, eObject_Style, eObject_Section, eObject_Guide, eObject_Kind_Count };
	enum	EAlignment		{	eAlign_None = 0, eAlign_Left, eAlign_Center, eAlign_Right };
	enum	EDraw			{	eDraw_No = 0, eDraw_Yes = 1, eDraw_OnOverflow = 2, eDraw_Always = 3 };
	enum	EEmpty			{	eEmpty_Draw = 0, eEmpty_Remove, eEmpty_RemoveRow };
	enum	ERepeat			{	eRepeat_None = 0, eRepeat_Horizontally, eRepeat_Vertically };

	enum	EProperties_Kind
	{
		PSProps_Boolean = 0,
		PSProps_Integer,
		PSProps_Real,
		PSProps_XMLString,
		PSProps_String,
		PSProps_Rect,
		PSProps_Color,
		PSProps_List,
		PSProps_BLOB,
		PSProps_Objects,
		PSProps_OID
	};
	enum	EProperties_Handling
	{
		PSProps_None = 0,
		PSProps_Attribute,		// <table attrib="xxx">
		PSProps_Value,			// <text>string value</text>
		PSProps_OneChild,		// <field> <script> string value </script> </field>
		PSProps_Childs,			// <group> <object/> <object/> </group>
		PSProps_OneContainer,	// <styleset> <style>...</style> </styleset>
		PSProps_Container		// <table> <headers> <header>...</header> </headers> </table>
	};
	struct	PSPropertiesLimits
	{
		const char **	list;
		double			defF;
		double			minF;
		double			maxF;
	};
	struct	PSObjProps
	{
		OSType					id;
		bool					writable;
		EProperties_Handling	handling;
		EProperties_Kind		kind;
		const char	*			name;
		PSPropertiesLimits		limits;
		bool					skipOnWrite;
	};


public:
	inline						PSObject (EObject_Kind inKind);
	inline	virtual				~PSObject (void);

	static	const PSObjProps*	FindPropertyByID (OSType id, const PSObjProps *pes);
	static	const PSObjProps*	FindPropertyByName (RWStringView inName, const PSObjProps *pes);
	static		int				CountProperties (const PSObjProps *pes);

	inline		EObject_Kind	GetKind (void) const;
	virtual	const PSObjProps *	GetProperties (void) const = 0;
	virtual		bool			GetProperty (OSType id, RWValue &outValue);
	virtual		bool			SetProperty (OSType id, RWValue &inValue);
	virtual		PSObjList	*	GetObjects (OSType id);
				void			GetProperties (PSPropsMap &outMap) const;

				bool			GetProperty (OSType id, RWString &outValue);

	virtual		RWXmlNode		WriteXML (RWXmlNode inParent, const PSObjProps* pes = NULL);
	virtual		long			GetInternalID() {return mInternalID; };
private:
				void			SetProperty (const PSObjProps* pes, RWStringView inValue);

protected:
	static		bool			SetBooleanProperty (RWValue &inValue, bool &outValue);
	static		bool			SetIntegerProperty (RWValue &inValue, long &outValue, long inMin = LONG_MIN, long inMax = LONG_MAX);
	static		bool			SetIntegerProperty (RWValue &inValue, int &outValue, int inMin = INT_MIN, int inMax = INT_MAX);
	static		bool			SetRealProperty (RWValue &inValue, double &outValue, double inMin = -INFINITY, double inMax = INFINITY);
	static		bool			SetRealProperty (RWValue &inValue, float &outValue, double inMin = -INFINITY, double inMax = INFINITY);
	static		bool			SetXMLStringProperty (RWValue &inValue, RWString &outValue);
	static		bool			SetStringProperty (RWValue &inValue, RWString &outValue);
	static		bool			SetStringProperty (RWValue &inValue, ExtendedExecute &outScript);
	static		bool			SetRectProperty (RWValue &inValue, SRect &outValue);
	static		bool			SetColorProperty (RWValue &inValue, SRGBColor &outValue);
	static		long			SetListProperty (RWValue &inValue, const char ** inList);
	static		bool			SetListProperty (RWValue &inValue, const char ** inList, int &outValue);

	virtual		bool			GetObjects (OSType id, PSObjListD* &outList);
	virtual		void			LoadXML (RWXmlNode inNode, const PSObjProps* pes = NULL);
	virtual		void			LoadXMLObjects (const PSObjProps* pes, RWXmlNode inNode);
	virtual		bool			WriteXMLObjects (const PSObjProps* pes, RWXmlNode inNode);

	static		long			GetNextId() {return mObjectCounter++;}
public:
	static	const char	*	sKind[];

	static	const char	*	sAlignment[];
	static	const char	*	sAlignment2[];
	static	const char	*	sDraw[];
	static	const char	*	sEmpty[];
	static	const char	*	sRepeat[];
	static	const char	*	sRepeat2[];
	static	const char	*	sPictFormat[];
	static	const char	*	sCalcType[];
	// Style
	static	const char	*	sVAlignment[];
	static	const char	*	sJustification[];
	// Section
	static	const char	*	sPageThrow[];
	static	const char	*	sBreakType[];
//	static	const char	*	sBreakOnField[];
//	static	const char	*	sBreakOnVariable[];
//	static	const char	*	sBreakOnArray[];
	// 4D DataSource
	static	const char	*	s4DKind[];
	static	const char	*	sSource[];
	static	const char	*	sRelate[];
// 1..4
	static	const char	*	sMirror[];

protected:
	static const PSObjProps	sProperties[];
	EObject_Kind			mObjectKind;
	long					mInternalID; // pB added to map this to longint for 64 bit systems
	static long				mObjectCounter;
	static std::map<long, PSObject*>				sObjectMap;

};


inline	PSObject::EObject_Kind
PSObject::GetKind (void) const									{ return mObjectKind; }
inline	const PSObject::PSObjProps *
PSObject::GetProperties (void) const							{ return sProperties; }
inline
PSObject::PSObject (EObject_Kind inKind) : mObjectKind (inKind)	{ mInternalID = GetNextId ();sObjectMap[mInternalID] = this; }
inline
PSObject::~PSObject (void)	{ sObjectMap.erase(mInternalID); };

#endif
