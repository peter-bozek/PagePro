/*
 *  PSObject.cpp
 *  ReportWriter
 *
 *  Created by Miloslav Bystrický on 13.10.2009.
 *  Copyright 2009 INFORCE Bratislava spol. s r. o.. All rights reserved.
 *
 */

# include	"PSObject.h"
# include	"PSObjProps.h"
# include	<sstream>
# include	"ExtendedExecute.h"

long                        PSObject::mObjectCounter = 1024;
std::map<long, PSObject*>	PSObject::sObjectMap;


// ---------------------------------------------------------------------------
// HasProperty														  [public]
// ---------------------------------------------------------------------------

bool
PSPropsMap::HasProperty (OSType id)
const
{
	return find (id) != end();
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
PSPropsMap::GetProperty (OSType id, RWValue &outValue)
const
{
	bool			found = false;
	const_iterator	it = find (id);
	
	if (it != end())
	{
		found = true;
		outValue.Clone (it->second);
	}
	
	return found;
}


// ---------------------------------------------------------------------------
// GetPropertyRef													  [public]
// ---------------------------------------------------------------------------

bool
PSPropsMap::GetPropertyRef (OSType id, RWValue &outValue)
const
{
	bool			found = false;
	const_iterator	it = find (id);
	
	if (it != end())
	{
		found = true;
		outValue.Attach (it->second);
	}
	
	return found;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
PSPropsMap::SetProperty (OSType id, const RWValue &inValue)
{
	bool		found = false;
	iterator	it = find (id);

	if (it != end())
	{
		found = true;
		it->second.Clone (inValue);
	}
	else
	{
		value_type	value (id, inValue);	// constructor will call Clone
		insert (value);
	}

	return found;
}


// ---------------------------------------------------------------------------
// SetPropertyRef													  [public]
// ---------------------------------------------------------------------------

bool
PSPropsMap::SetPropertyRef (OSType id, const RWValue &inValue)
{
	bool		found = false;
	iterator	it = find (id);
	
	if (it != end())
	{
		found = true;
		it->second.Attach (inValue);
	}
	else
	{
		value_type	value (id, inValue);	// constructor will call Clone
		insert (value);
	}
	
	return found;
}


// ---------------------------------------------------------------------------
// RemoveProperty													  [public]
// ---------------------------------------------------------------------------

bool
PSPropsMap::RemoveProperty (OSType id)
{
	bool		found = false;
	iterator	it = find (id);
	
	if (it != end())
	{
#if	0 && TARGET_DEBUG
		it->second.SetText ("<no intersection>");
#else
		found = true;
		erase (it);
#endif
	}
	
	return found;
}

# pragma mark	-

const char *PSObject::sKind[]			= { "Group", "Line", "Rect", "Oval", "Pict", "Text", "Var", "Field",
											"Table", "Header", "Column",
											"Report", "DataSource", "Style", "Section", "Guide", NULL };
const char *PSObject::sAlignment[]		= { "none", "left", "center", "right", NULL };
const char *PSObject::sAlignment2[]		= { "none", "left", "middle", "right", NULL };
const char *PSObject::sDraw[]			= { "no", "yes", "on overflow", "always", NULL };
const char *PSObject::sEmpty[]			= { "draw", "remove", "remove row", NULL };
const char *PSObject::sRepeat[]			= { "none", "horizontal", "vertical", NULL };
const char *PSObject::sRepeat2[]		= { "none", "horizontally", "vertically", NULL };
const char *PSObject::sPictFormat[]		= { "Normal", "Centered", "ScaledToFit", "ScaledProp", "ScaledPropCentered", NULL };
const char *PSObject::sCalcType[]		= { "none", "total", "min", "avg", "max", "count", "stdvar", "stddev", NULL };

const char *PSObject::sVAlignment[]		= { "default", "top", "bottom", "center", NULL };
const char *PSObject::sJustification[]	= { "default", "left", "right", "center", "justify", "fulljustify", NULL };

const char *PSObject::sPageThrow[]		= { "none", "before", "after", NULL };
const char *PSObject::sBreakType[]		= { "none", "Field", "Variable", "Array", NULL };
//const char *PSObject::sBreakOnField[]	= { "breakOnField", NULL };
//const char *PSObject::sBreakOnVariable[]= { "breakOnVariable", NULL };
//const char *PSObject::sBreakOnArray[]	= { "breakOnArray", NULL };

const char *PSObject::s4DKind[]			= { "4D", NULL };
const char *PSObject::sSource[]			= { "undefined", "table", "fixed", "variable", "array", NULL };
const char *PSObject::sRelate[]			= { "no", "automatic", "manual", NULL };
// v1.4
const char *PSObject::sMirror[]			= { "no", "horizontal", "vertical", NULL };


// ---------------------------------------------------------------------------
// FindPropertyByID											 [static] [public]
// ---------------------------------------------------------------------------

const PSObject::PSObjProps*
PSObject::FindPropertyByID (OSType id, const PSObjProps *pes)
{
	for ( ; pes->name != NULL; pes++)
		if (pes->id == id)
			return pes;
	return NULL;
}


// ---------------------------------------------------------------------------
// FindPropertyByName										 [static] [public]
// ---------------------------------------------------------------------------

const PSObject::PSObjProps*
PSObject::FindPropertyByName (RWStringView inName, const PSObjProps *pes)
{
	for ( ; pes->name != NULL; pes++)
		if (RWStr::EqualsNoCase (inName, pes->name))
			return pes;
	return NULL;
}


// ---------------------------------------------------------------------------
// CountProperties											 [static] [public]
// ---------------------------------------------------------------------------

int
PSObject::CountProperties (const PSObjProps *pes)
{
	int	count = 0;
	for ( ; pes->name != NULL; pes++)
		count++;
	return count;
}


// ---------------------------------------------------------------------------
// GetProperty														  [public]
// ---------------------------------------------------------------------------

bool
PSObject::GetProperty (OSType id, RWValue &outValue)
{
	switch (id)
	{
		case PSObjPropOID:
			outValue.SetInteger ((long) this);
			break;

		case PSObjPropKind:
			outValue.SetText (RWStr::FromASCII (sKind [mObjectKind]));
			break;

		case PSObjPropXML:
		{
			RWXmlDocument	xml;
			WriteXML (xml.Node());
			outValue.SetText (xml.SaveString (false));
			return true;
		}

		default:
			return false;
			break;
	}

	return true;
}


bool
PSObject::GetProperty (OSType id, RWString &outValue)
{
	RWValue	value;

	if (GetProperty (id, value))
	{
		value.GetTextValue (outValue, NULL);
		return true;
	}
	return false;
}


// ---------------------------------------------------------------------------
// SetProperty														  [public]
// ---------------------------------------------------------------------------

bool
PSObject::SetProperty (OSType id, RWValue &inValue)
{
	switch (id)
	{
		case PSObjPropXML:
			if (inValue.GetKind() == RWValue::eValue_Text)
			{
				RWXmlDocument	xml;
				RWXmlResult		result = xml.LoadString (inValue.GetText());
				if (!result)
				{
					printf ("PSObject::SetProperty:: Could not load XML. Error='%s'.\n", RWStr::ToUTF8 (result.description).c_str());
					break;
				}
				LoadXML (xml.Root());
				return true;
			}
			break;
	}
	return false;
}


// ---------------------------------------------------------------------------
// GetObjects														  [public]
// ---------------------------------------------------------------------------

PSObjList	*
PSObject::GetObjects (OSType id)
{
	return NULL;
}


// ---------------------------------------------------------------------------
// GetObjects													   [protected]
// ---------------------------------------------------------------------------

bool
PSObject::GetObjects (OSType id, PSObjListD* &outList)
{
//	outList = NULL;
	return false;
}



// ---------------------------------------------------------------------------
// SetBooleanProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetBooleanProperty (RWValue &inValue, bool &outValue)
{
	if (inValue.CoerceValue (RWValue::eValue_Boolean))
		outValue = inValue.GetBoolean();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetIntegerProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetIntegerProperty (RWValue &inValue, long &outValue, long inMin, long inMax)
{
	if (inValue.CoerceValue (RWValue::eValue_Integer) && inValue.GetInteger() >= inMin && inValue.GetInteger() <= inMax)
		outValue = inValue.GetInteger();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetIntegerProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetIntegerProperty (RWValue &inValue, int &outValue, int inMin, int inMax)
{
	if (inValue.CoerceValue (RWValue::eValue_Integer) && inValue.GetInteger() >= inMin && inValue.GetInteger() <= inMax)
		outValue = inValue.GetInteger();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetRealProperty										   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetRealProperty (RWValue &inValue, double &outValue, double inMin, double inMax)
{
	if (inValue.CoerceValue (RWValue::eValue_Real) && inValue.GetReal() >= inMin && inValue.GetReal() <= inMax)
		outValue = inValue.GetReal();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetRealProperty										   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetRealProperty (RWValue &inValue, float &outValue, double inMin, double inMax)
{
	if (inValue.CoerceValue (RWValue::eValue_Real) && inValue.GetReal() >= inMin && inValue.GetReal() <= inMax)
		outValue = (float) inValue.GetReal();
	else
		return false;
	return true;
}



// ---------------------------------------------------------------------------
// SetXMLStringProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetXMLStringProperty (RWValue &inValue, RWString &outValue)
{
	if (inValue.GetKind() != RWValue::eValue_Text)
		return false;
	outValue = inValue.GetText();
	return true;
}


// ---------------------------------------------------------------------------
// SetStringProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetStringProperty (RWValue &inValue, RWString &outValue)
{
	if (inValue.GetKind() != RWValue::eValue_Text)
		return false;
	outValue = inValue.GetText();
	return true;
}

// ---------------------------------------------------------------------------
// SetStringProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetStringProperty (RWValue &inValue, ExtendedExecute &outScript)
{
	if (inValue.CoerceValue (RWValue::eValue_Text))
		outScript = inValue.GetText();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetRectProperty										   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetRectProperty (RWValue &inValue, SRect &outValue)
{
	if (inValue.GetKind() != RWValue::eValue_Text)
		return false;
	outValue = inValue.GetText();
	return true;
}


// ---------------------------------------------------------------------------
// SetColorProperty										   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetColorProperty (RWValue &inValue, SRGBColor &outValue)
{
	if (inValue.GetKind() == RWValue::eValue_Text)
		outValue = inValue.GetText();
	else if (inValue.CoerceValue (RWValue::eValue_Integer))
		outValue = (unsigned long) inValue.GetInteger();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// ListIndex															[local]
// ---------------------------------------------------------------------------
// inIndex if it is a valid index into the NULL terminated inList, -1 otherwise

static	long
ListIndex (long inIndex, const char **inList)
{
	if (inIndex < 0 || inList == NULL)
		return -1;
	for (long i = 0; inList[i] != NULL; i++)
		if (i == inIndex)
			return inIndex;
	return -1;
}


// ---------------------------------------------------------------------------
// SetListProperty										   [static][protected]
// ---------------------------------------------------------------------------
// the list item's name, or its index

long
PSObject::SetListProperty (RWValue &inValue, const char ** inList)
{
	long	lVal = -1;
	if (inValue.GetKind() == RWValue::eValue_Text)
		lVal = RWTools::FindInList (inValue.GetText(), inList);
	if (lVal == -1 && inValue.CoerceValue (RWValue::eValue_Integer))
		lVal = ListIndex (inValue.GetInteger(), inList);
	return lVal;
}


// ---------------------------------------------------------------------------
// SetListProperty										   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetListProperty (RWValue &inValue, const char ** inList, int &outValue)
{
	long	lVal = SetListProperty (inValue, inList);
	if (lVal >= 0)
	{
		outValue = int (lVal);
		return true;
	}

	return false;
}


// ---------------------------------------------------------------------------
// SetProperty														 [private]
// ---------------------------------------------------------------------------
// property from its XML text

void
PSObject::SetProperty (const PSObjProps* pes, RWStringView inValue)
{
	RWValue	value;
	switch (pes->kind)
	{
		case PSProps_Boolean:
			value.SetBoolean (RWStr::ToInteger (inValue).value_or (0) != 0);
			break;

		case PSProps_Integer:
		{
			long	lVal = (long) RWStr::ToInteger (inValue).value_or (0);
			if (lVal >= pes->limits.minF && lVal <= pes->limits.maxF)
				value.SetInteger (lVal);
			break;
		}

		case PSProps_Real:
		{
			double	fVal = RWStr::ToDouble (inValue).value_or (0);
			if (fVal >= pes->limits.minF && fVal <= pes->limits.maxF)
				value.SetReal (fVal);
			break;
		}

		case PSProps_XMLString:
		case PSProps_String:
		case PSProps_Rect:
		case PSProps_Color:
			value.SetText (RWString (inValue));
			break;

		case PSProps_List:
		{
			long	lVal = RWTools::FindInList (inValue, pes->limits.list);
			if (lVal < 0)
			{
				std::optional<long long>	index = RWStr::ToInteger (inValue);
				if (index)
					lVal = ListIndex ((long) *index, pes->limits.list);
			}
			if (lVal >= 0)
				value.SetInteger (lVal + pes->limits.minF);
			break;
		}

		case PSProps_BLOB:
		case PSProps_Objects:
			// child needs to implement
			// probably nonsense to have big data in attributes...
			break;

		case PSProps_OID:
			// ignored
			break;
	}
	if (value.GetKind() != RWValue::eValue_Undefined)
		SetProperty (pes->id, value);

	return;
}


// ---------------------------------------------------------------------------
// LoadXML														   [protected]
// ---------------------------------------------------------------------------

void
PSObject::LoadXML (RWXmlNode inNode, const PSObjProps* ppes)
{
	if (ppes == NULL)
		ppes = GetProperties();
	const PSObjProps*	pes = ppes;
	bool				loadChilds = false;

	for ( ; pes->id && pes->name != NULL; pes++)
	{
		if (pes->handling == PSProps_None || not pes->writable)
			continue;

		RWString	name = RWStr::FromASCII (pes->name);

		if (pes->handling == PSProps_Attribute)
		{
			RWString	value = inNode.Attr (name);
			if (!value.empty())
				SetProperty (pes, value);
		}
		else
		{
			RWXmlNode	elem = inNode;
			if (pes->handling == PSProps_OneChild || pes->handling == PSProps_OneContainer)
			{
				elem = inNode.Child (name);
				if (!elem)
					continue;
				if (pes->kind == PSProps_Objects || pes->kind == PSProps_BLOB)
				{
					LoadXMLObjects (pes, elem);
					continue;
				}
			}
			else if ((pes->kind == PSProps_Objects) && ((pes->handling == PSProps_Childs) || (pes->handling == PSProps_Container)))
				// pB changed 2011-9
			{
				loadChilds = true;
				continue;
			}

assert (pes->kind < PSProps_BLOB);

			RWString	text = RWTools::ParseIntoText (elem);
			if (!text.empty())
			{
				RWValue	value;
				value.SetText (std::move (text));
				SetProperty (pes->id, value);
			}
		}
	}

	if (loadChilds)
	{
		for (RWXmlNode elem : inNode.Children())
		{
			pes = FindPropertyByName (elem.Name(), ppes);
			if (pes && pes->kind == PSProps_Objects && (pes->handling == PSProps_Childs || pes->handling == PSProps_Container))
				LoadXMLObjects (pes, elem);
		}
	}

	return;
}


// ---------------------------------------------------------------------------
// LoadXMLObjects												   [protected]
// ---------------------------------------------------------------------------

void
PSObject::LoadXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------
// Writes the object as a child element of inParent (named after its kind) and
// returns it. Property tables without a kind entry write into inParent itself.

RWXmlNode
PSObject::WriteXML (RWXmlNode inParent, const PSObjProps* ppes)
{
	if (ppes == NULL)
		ppes = GetProperties();
	const PSObjProps*	pes = FindPropertyByID (PSObjPropKind, ppes);

	RWXmlNode	me = inParent;
	if (pes)
		me = inParent.Append (RWStr::FromASCII (pes->limits.list ? pes->limits.list [GetKind()] : pes->name));

	for (pes = ppes; pes->name != NULL; pes++)
	{
		if (pes->handling == PSProps_None || pes->skipOnWrite)
			continue;
		if (pes->id == PSObjPropOID || pes->id == PSObjPropKind)
			continue;

		RWValue	value;
		if (pes->kind != PSProps_Objects && not GetProperty (pes->id, value))
			continue;

		RWString	name = RWStr::FromASCII (pes->name);

		switch (pes->kind)
		{
			case PSProps_Boolean:
				if (value.GetKind() == RWValue::eValue_Boolean && value.GetBoolean() != (pes->limits.defF != 0))
					me.SetAttrBool (name, value.GetBoolean());
				break;

			case PSProps_Integer:
				if (value.GetKind() == RWValue::eValue_Integer && value.GetInteger() != pes->limits.defF)
					me.SetAttrInt (name, value.GetInteger());
				break;

			case PSProps_Real:
				if (value.GetKind() == RWValue::eValue_Real && value.GetReal() != pes->limits.defF)
					me.SetAttrDouble (name, value.GetReal(), "%.15g");
				break;

			case PSProps_XMLString:
			case PSProps_String:
				if (value.GetKind() == RWValue::eValue_Text)	// ••• TODO ••• should we write empty string properties?!?
				{
					if (pes->handling >= PSProps_OneChild)
						RWTools::WriteText (me.Append (name), value.GetText());
					else if (pes->handling == PSProps_Value)
						RWTools::WriteText (me, value.GetText());
					else // if (pes->handling == PSProps_Attribute
						me.SetAttr (name, value.GetText());
				}
				break;

			case PSProps_Rect:
			case PSProps_Color:
				if (value.GetKind() == RWValue::eValue_Text)
					me.SetAttr (name, value.GetText());
				break;

			case PSProps_List:
				if (value.GetKind() == RWValue::eValue_Integer && value.GetInteger() != pes->limits.defF)
				{
					long	index = ListIndex (long (value.GetInteger() - pes->limits.minF), pes->limits.list);
					if (index >= 0)
						me.SetAttr (name, RWStr::FromASCII (pes->limits.list [index]));
				}
				else if (value.GetKind() == RWValue::eValue_Text)
				{
					long	lVal = RWTools::FindInList (value.GetText(), pes->limits.list);
					if (lVal >= 0 && lVal != pes->limits.defF)
						me.SetAttr (name, value.GetText());
				}
				break;

			case PSProps_BLOB:
			{
				// the container is kept only when the object writes something into it
				RWXmlNode	container = me.Append (name);
				if (!WriteXMLObjects (pes, container))
					me.Remove (container);
				break;
			}

			case PSProps_Objects:
			{
assert (pes->handling >= PSProps_OneChild);

				bool		ownContainer = (pes->handling == PSProps_OneContainer || pes->handling == PSProps_Container);
				RWXmlNode	container = ownContainer ? me.Append (name) : me;
				if (WriteXMLObjects (pes, container))
				{
					PSObjListD*	objects = NULL;
					if (GetObjects (pes->id, objects))
					{
						for (PSObject *obj : *objects)
							obj->WriteXML (container);
					}
				}
				else if (ownContainer)
					me.Remove (container);
				break;
			}

			case PSProps_OID:	// ignored
				break;
		}
	}

	return me;
}


// ---------------------------------------------------------------------------
// WriteXMLObjects												   [protected]
// ---------------------------------------------------------------------------

bool
PSObject::WriteXMLObjects (const PSObjProps* pes, RWXmlNode inNode)
{
	return true;
}
