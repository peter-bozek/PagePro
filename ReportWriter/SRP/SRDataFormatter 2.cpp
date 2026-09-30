#include "SRDataFormatter.h"
#include <string>

// SRDataFormatter.cpp
// Modernized implementation relying on RWValue and RWTextValue utilities
// to avoid legacy Unicode wrapper APIs.

namespace SRDataFormatter {

RWTextValue FormatVariable(const RWValue &inVar, const CText inFormat) {
    RWTextValue out;

    // If a format is provided, convert it to UTF-8 and pass to RWValue::GetTextValue.
    // RWValue::GetTextValue handles numbers, dates, times and other kinds.
    if (!inFormat.empty()) {
        CXMLText fmt = RWTextValue::UTF_16_to_UTF8(inFormat);
        inVar.GetTextValue(out, fmt.c_str());
    } else {
        inVar.GetTextValue(out, nullptr);
    }

    return out;
}

} // namespace SRDataFormatter
