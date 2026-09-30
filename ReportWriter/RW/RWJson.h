/*
 *  RWJson.h
 *  ReportWriter
 *
 *  JSON support over RapidJSON. Values hold UTF-16 (RWString) natively;
 *  output is produced either as UTF-8 bytes (files, BLOBs) or as UTF-16
 *  (4D text). Include RapidJSON only through this header so the
 *  configuration below is the same everywhere.
 */

#ifndef	_RWJson_h_
# define	_RWJson_h_

# include	"RWString.h"

# define	RAPIDJSON_HAS_STDSTRING		1
# include	"rapidjson/document.h"
# include	"rapidjson/prettywriter.h"
# include	"rapidjson/stringbuffer.h"
# include	"rapidjson/writer.h"

typedef	rapidjson::UTF16<char16_t>					RWJsonEncoding;
typedef	rapidjson::GenericDocument<RWJsonEncoding>	RWJsonDocument;
typedef	rapidjson::GenericValue<RWJsonEncoding>		RWJsonValue;
typedef	RWJsonDocument::AllocatorType				RWJsonAllocator;

// streaming writers (e.g. for exports), fed with UTF-16 strings
typedef	rapidjson::StringBuffer												RWJsonUTF8Buffer;
typedef	rapidjson::GenericStringBuffer<RWJsonEncoding>						RWJsonUTF16Buffer;
typedef	rapidjson::Writer<RWJsonUTF8Buffer, RWJsonEncoding, rapidjson::UTF8<>>	RWJsonUTF8Writer;
typedef	rapidjson::Writer<RWJsonUTF16Buffer, RWJsonEncoding, RWJsonEncoding>	RWJsonUTF16Writer;
typedef	rapidjson::PrettyWriter<RWJsonUTF8Buffer, RWJsonEncoding, rapidjson::UTF8<>>	RWJsonUTF8PrettyWriter;
typedef	rapidjson::PrettyWriter<RWJsonUTF16Buffer, RWJsonEncoding, RWJsonEncoding>	RWJsonUTF16PrettyWriter;

namespace	RWJson
{
	// string value copied into the document's allocator
	inline	RWJsonValue		String (RWStringView inText, RWJsonAllocator &inAllocator)
	{
		return RWJsonValue (inText.data(), rapidjson::SizeType (inText.size()), inAllocator);
	}

	template <class TargetEncoding, class Buffer>
	void					Serialize (const RWJsonValue &inValue, Buffer &ioBuffer, bool inPretty)
	{
		if (inPretty)
		{
			rapidjson::PrettyWriter<Buffer, RWJsonEncoding, TargetEncoding>	writer (ioBuffer);
			writer.SetIndent ('\t', 1);
			inValue.Accept (writer);
		}
		else
		{
			rapidjson::Writer<Buffer, RWJsonEncoding, TargetEncoding>	writer (ioBuffer);
			inValue.Accept (writer);
		}
	}

	inline	std::string		ToUTF8 (const RWJsonValue &inValue, bool inPretty = false)
	{
		RWJsonUTF8Buffer	buffer;
		Serialize<rapidjson::UTF8<>> (inValue, buffer, inPretty);
		return std::string (buffer.GetString(), buffer.GetLength());
	}

	inline	RWString		ToString (const RWJsonValue &inValue, bool inPretty = false)
	{
		RWJsonUTF16Buffer	buffer;
		Serialize<RWJsonEncoding> (inValue, buffer, inPretty);
		return RWString (buffer.GetString(), buffer.GetLength());
	}
}

#endif
