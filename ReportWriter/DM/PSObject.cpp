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
		it->second.SetXMLText ("<no intersection>");
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
PSObject::FindPropertyByName (const CXMLText inName, const PSObjProps *pes)
{
	for ( ; pes->name != NULL; pes++)
		if (STR_EQUALS (pes->name, inName))
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
			outValue.SetXMLText (sKind [mObjectKind]);
			break;

		case PSObjPropXML:
		{
			XMLDocument	xml;
			WriteXML (&xml);
			ostringstream	ostr;
			ostr << xml;
			outValue.SetXMLText (strdup (ostr.str().c_str()), true);
			return true;
		}

		default:
			return false;
			break;
	}

	return true;
}


bool
PSObject::GetProperty (OSType id, RWTextValue &outValue)
{
/*
	if (id == PSProps_OID)
	{
		char	buf [16];
		snprintf (buf, sizeof (buf), "oid:%lx", (long) this);
		outValue = (const UTF8Char *) buf;
		return true;
	}
	else
*/
	{
		RWValue	value;

		if (GetProperty (id, value))
		{
			value.GetTextValue (outValue, NULL);
			return true;
		}
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
			if (inValue.CoerceValue (RWValue::eValue_XMLText))
			{
				XMLDocument	xml;
				xml.Parse (inValue.GetXMLText());
				if (xml.Error())
				{
					printf ("PSObject::SetProperty:: Could not load XML. Error='%s'.\n", xml.ErrorDesc());
					break;
				}
				LoadXML (xml.RootElement());
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
PSObject::SetXMLStringProperty (RWValue &inValue, CXMLText &outValue)
{
	if (inValue.CoerceValue (RWValue::eValue_XMLText))
	{
		if (outValue)
			free (outValue);
		outValue = reinterpret_cast <CXMLText> (strdup (reinterpret_cast <const char*> (inValue.GetXMLText())));
	}
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetStringProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetStringProperty (RWValue &inValue, RWTextValue &outValue)
{
	if (inValue.CoerceValue (RWValue::eValue_Text))
		outValue = inValue.GetText();
	else
		return false;
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
// SetStringProperty									   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetRectProperty (RWValue &inValue, SRect &outValue)
{
	if (inValue.CoerceValue (RWValue::eValue_XMLText))
		outValue = inValue.GetXMLText();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetColorProperty										   [static][protected]
// ---------------------------------------------------------------------------

bool
PSObject::SetColorProperty (RWValue &inValue, SRGBColor &outValue)
{
	if (inValue.CoerceValue (RWValue::eValue_XMLText))
		outValue = inValue.GetXMLText();
	else if (inValue.CoerceValue (RWValue::eValue_Integer))
		outValue = (unsigned long) inValue.GetInteger();
	else
		return false;
	return true;
}


// ---------------------------------------------------------------------------
// SetListProperty										   [static][protected]
// ---------------------------------------------------------------------------

long
PSObject::SetListProperty (RWValue &inValue, const char ** inList)
{
	long				lVal = -1;
	if (inValue.CoerceValue (RWValue::eValue_XMLText))
		lVal = RWTools::FindInList (inValue.GetXMLText(), inList);
//	else if (inValue.CoerceValue (RWValue::eValue_Integer))
	if (lVal == -1 && inValue.CoerceValue (RWValue::eValue_Integer))
	{
		lVal = (unsigned long) inValue.GetInteger();
		if (lVal >= 0)
		{
			int	count = 0;
			while (count <= lVal && *inList != NULL)
			{
				count++;
				inList++;
			}
			if (count <= lVal)
				lVal = -1;
		}
	}
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

void
PSObject::SetProperty (const PSObjProps* pes, const CXMLText inValue)
{
	RWValue	value;
	long	lVal;
	switch (pes->kind)
	{
		case PSProps_Boolean:
			lVal = 0;
			sscanf (inValue, "%li", &lVal);
			value.SetBoolean (lVal != 0);
			break;

		case PSProps_Integer:
			lVal = 0;
			sscanf (inValue, "%li", &lVal);
			if (lVal >= pes->limits.minF && lVal <= pes->limits.maxF)
				value.SetInteger (lVal);
			break;

		case PSProps_Real:
		{
			double	fVal = 0;
			sscanf (inValue, "%lg", &fVal);
			if (fVal >= pes->limits.minF && fVal <= pes->limits.maxF)
				value.SetReal (fVal);
			break;
		}

		case PSProps_XMLString:
			value.SetXMLText (inValue);
			break;

		case PSProps_String:
		{
			CText	t (reinterpret_cast <const UTF8Char*> (inValue), CText::_nullTerminated_);
			value.SetText (t.Release(), true);
			break;
		}

		case PSProps_Rect:
		case PSProps_Color:
			value.SetXMLText (inValue);
			break;

		case PSProps_List:
			lVal = RWTools::FindInList (inValue, pes->limits.list);
			if (lVal >= 0)
				value.SetInteger (lVal + pes->limits.minF);
			else if (sscanf (inValue, "%li", &lVal) == 1)
			{
				if (lVal >= 0)
				{
					int	count = 0;
					const char ** inList = pes->limits.list;
					while (count <= lVal && *inList != NULL)
					{
						count++;
						inList++;
					}
					if (count > lVal)
						value.SetInteger (lVal + pes->limits.minF);
				}
			}
			break;

		case PSProps_BLOB:
		case PSProps_Objects:
			// child needs to implement
			// probably nonsense to have big data in attributes...
			break;

		case PSProps_OID:
			// ignored
			break;
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
PSObject::LoadXML (XMLElement *inNode, const PSObjProps* ppes)
{
	if (ppes == NULL)
		ppes = GetProperties();
	const PSObjProps*	pes = ppes;
	bool				loadChilds = false;

	for ( ; pes->id && pes->name != NULL; pes++)
	{
		if (pes->handling == PSProps_None || not pes->writable)
			continue;

		if (pes->handling == PSProps_Attribute)
		{
			const CXMLText	value = inNode->Attribute (pes->name);
			if (value && *value)
				SetProperty (pes, value);
		}
		else
		{
            XMLElement	*elem = inNode;
			if (pes->handling == PSProps_OneChild || pes->handling == PSProps_OneContainer)
			{
				elem = inNode->FirstChildElement (pes->name);
				if (elem == NULL)
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

			CText	text = RWTools::ParseIntoText (elem);
			if (text && *text)
			{
				RWValue	value;
				value.SetText (text, true);
				SetProperty (pes->id, value);
			}
		}
	}

	if (loadChilds)
	{
		XMLNode		*node, *next;
        XMLElement	*elem;
		for (node = inNode->FirstChildElement() ; node; node = next )
		{
			next = node->NextSibling();
			elem = node->ToElement();
			if (elem == NULL)
				continue;
			pes = FindPropertyByName (elem->Value(), ppes);
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
PSObject::LoadXMLObjects (const PSObjProps* pes, XMLElement *inNode)
{
}


// ---------------------------------------------------------------------------
// WriteXML															  [public]
// ---------------------------------------------------------------------------

XMLElement*
PSObject::WriteXML (XMLNode *inParent, const PSObjProps* ppes)
{
	if (ppes == NULL)
		ppes = GetProperties();
	const PSObjProps*	pes = FindPropertyByID (PSObjPropKind, ppes);

    XMLElement	*me = NULL, *container;
	{
        XMLElement	lme (pes? (pes->limits.list ? pes->limits.list [GetKind()] : pes->name) : "");
		if (!pes)
			me = inParent->ToElement();
		if (me == NULL)
			me = inParent->InsertEndChild (lme)->ToElement();
	}

	for (pes = ppes; pes->name != NULL; pes++)
	{
		if (pes->handling == PSProps_None || pes->skipOnWrite)
			continue;
		if (pes->id == PSObjPropOID || pes->id == PSObjPropKind)
			continue;

		RWValue	value;
		if (pes->kind != PSProps_Objects && not GetProperty (pes->id, value))
			continue;

		switch (pes->kind)
		{
			case PSProps_Boolean:
				if (value.GetKind() == RWValue::eValue_Boolean && value.GetBoolean() != (pes->limits.defF != 0))
					me->SetAttribute (pes->name, int (value.GetBoolean()));
				break;

			case PSProps_Integer:
				if (value.GetKind() == RWValue::eValue_Integer && value.GetInteger() != pes->limits.defF)
					me->SetAttribute (pes->name, value.GetInteger());
				break;

			case PSProps_Real:
				if (value.GetKind() == RWValue::eValue_Real && value.GetReal() != pes->limits.defF)
					me->SetAttribute (pes->name, value.GetReal());
				break;

			case PSProps_XMLString:
			case PSProps_String:
				if (value.CoerceValue (RWValue::eValue_XMLText))
				{
					if (value.GetXMLText() != NULL)	// ••• TODO ••• should we write empty string properties?!?
					{
						if (pes->handling >= PSProps_OneChild)
						{
							{
                                XMLElement	elem (pes->name);
								container = me->InsertEndChild (elem)->ToElement();
							}
							RWTools::WriteText (container, value.GetXMLText());
						}
						else if (pes->handling == PSProps_Value)
						{
							RWTools::WriteText (me, value.GetXMLText());
						}
						else // if (pes->handling == PSProps_Attribute
						{
							me->SetAttribute (pes->name, value.GetXMLText());
						}
					}
				}
				break;

			case PSProps_Rect:
				if (value.CoerceValue (RWValue::eValue_XMLText))
					me->SetAttribute (pes->name, value.GetXMLText());
				break;

			case PSProps_Color:
				if (value.CoerceValue (RWValue::eValue_XMLText))
//					if (! STR_EQUALS (value.GetXMLText(), (const char *) cBlackColor))
						me->SetAttribute (pes->name, value.GetXMLText());
				break;

			case PSProps_List:
				if (value.GetKind() == RWValue::eValue_Integer && value.GetInteger() != pes->limits.defF)
					me->SetAttribute (pes->name, pes->limits.list [int (value.GetInteger() - pes->limits.minF)]);
				else if (value.GetKind() == RWValue::eValue_XMLText)
				{
					long	lVal = RWTools::FindInList (value.GetXMLText(), pes->limits.list);
					if (lVal >= 0 && lVal != pes->limits.defF)
						me->SetAttribute (pes->name, value.GetXMLText());
				}
				break;

			case PSProps_BLOB:
			{
//				container = me;
                XMLElement	sub (pes->name);
//				if (pes->handling == PSProps_OneContainer || pes->handling == PSProps_Container)
					container = &sub;
				if (WriteXMLObjects (pes, container))
				{
					if (container != me)
						container = me->InsertEndChild (sub)->ToElement();
				}
				break;
			}

			case PSProps_Objects:
			{
assert (pes->handling >= PSProps_OneChild);

				container = me;
                XMLElement	sub (pes->name);
				if (pes->handling == PSProps_OneContainer || pes->handling == PSProps_Container)
					container = &sub;
				if (WriteXMLObjects (pes, container))
				{
					if (container != me)
						container = me->InsertEndChild (sub)->ToElement();
					
					PSObjListD*	objects = NULL;
					if (GetObjects (pes->id, objects))
					{
						PSObjListD::const_iterator	it;
						PSObject					*obj;
						for (it = objects->begin(); it != objects->end(); it++)
						{
							obj = *it;
							obj->WriteXML (container);
						}
					}
				}
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
PSObject::WriteXMLObjects (const PSObjProps* pes, XMLElement *inNode)
{
	return true;
}
