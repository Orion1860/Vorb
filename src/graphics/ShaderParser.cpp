#include "stdafx.h"
#include "graphics/ShaderParser.h"

#include <sstream>

#include "io/IOManager.h"

std::map<nString, vg::Semantic> vg::ShaderParser::m_semantics;
Event<nString> vg::ShaderParser::onParseError;

inline bool isWhitespace(char c) {
    return (c == '\n' || c == '\0' || c == '\t' || c == '\r');
}

inline void skipWhitespace(const nString& s, size_t& i) {
    while (isWhitespace(s[i]) && i < s.size()) i++;
}

inline bool isNumeric(char c) {
    return (c >= '0' && c <= '9');
}

void vg::ShaderParser::parseVertexShader(const nString& inputCode, OUT nString& resultCode,
                                         OUT std::vector<nString>& attributeNames,
                                         OUT std::vector<VGSemantic>& semantics,
                                         vio::IOManager* iom /*= nullptr*/) {
    if (m_semantics.empty()) initSemantics();

    vio::IOManager ioManager;
    if (!iom) iom = &ioManager;
    nString data;

    // Anticipate final code size
    resultCode.reserve(inputCode.size());

    for (size_t i = 0; i < inputCode.size(); i++) {
        char c = inputCode[i];
        if (c == '#') {
            nString include = tryParseInclude(inputCode, i);
            if (include.size()) {
                // Replace the include with the file contents
                if (ioManager.readFileToString(include, data)) {
                    resultCode += data;
                    continue; // Skip over last "
                } else {
                    onParseError("Failed to open file " + include);
                }
            }
        } else if (c == 'i') {
            // Attempt to parse as an attribute
            VGSemantic semantic;
            nString attribute = tryParseAttribute(inputCode, i, semantic);
            if (attribute.size()) {
                attributeNames.push_back(attribute);
                semantics.push_back(semantic);
            }
        }
        resultCode += c;
    }
}

void vg::ShaderParser::parseFragmentShader(const nString& inputCode, OUT nString& resultCode, vio::IOManager* iom /*= nullptr*/) {
    if (m_semantics.empty()) initSemantics();

    vio::IOManager ioManager;
    if (!iom) iom = &ioManager;
    nString data;

    // Anticipate final code size
    resultCode.reserve(inputCode.size());

    for (size_t i = 0; i < inputCode.size(); i++) {
        char c = inputCode[i];
        if (c == '#') {
            nString include = tryParseInclude(inputCode, i);
            if (include.size()) {
                // Replace the include with the file contents
                if (ioManager.readFileToString(include, data)) {
                    resultCode += data;
                    continue; // Skip over last "
                } else {
                    onParseError("Failed to open file " + include);
                }
            }
        } 
        resultCode += c;
    }
}

void vorb::graphics::ShaderParser::initSemantics() {
    m_semantics["COLOR"] = SEM_COLOR;
    m_semantics["POSITION"] = SEM_POSITION;
    m_semantics["TEXCOORD"] = SEM_TEXCOORD;
    m_semantics["NORMAL"] = SEM_NORMAL;
    m_semantics["BINORMAL"] = SEM_BINORMAL;
    m_semantics["TANGENT"] = SEM_TANGENT;
    m_semantics["FOG"] = SEM_FOG;
    m_semantics["BLENDINDICES"] = SEM_BLENDINDICES;
    m_semantics["BLENDWEIGHT"] = SEM_BLENDWEIGHT;
    m_semantics["PSIZE"] = SEM_PSIZE;
    m_semantics["TESSFACTOR"] = SEM_TESSFACTOR;
}

nString vg::ShaderParser::tryParseInclude(const nString& s, size_t& i) {
    size_t startI = i;
    static const char INCLUDE_STR[10] = "#include";
    // Check that #include is correct
    for (int j = 0; INCLUDE_STR[j] != '\0'; j++) {
        if (i == s.size()) { i = startI; return ""; }
        if (s[i++] != INCLUDE_STR[j]) { i = startI; return ""; }
    }

    skipWhitespace(s, i);
    if (i == s.size()) { i = startI; return ""; }

    if (s[i++] != '\"') { i = startI; return ""; }
    // Grab the include string
    nString include = "";
    while (s[i] != '\"') {
        // Check for invalid characters in path
        if (isWhitespace(s[i])) { i = startI; return ""; }
        include += s[i++];
    }
    return include;
}

nString vg::ShaderParser::tryParseAttribute(const nString& s, size_t i, VGSemantic& semantic) {
    static const char IN_STR[4] = "in ";
    static const char SEM_STR[5] = "SEM ";
    semantic = vg::Semantic::SEM_INVALID;
    // Check that in is correct
    for (int j = 0; IN_STR[j] != '\0'; j++) {
        if (i == s.size()) return "";
        if (s[i++] != IN_STR[j]) return "";
    }

    skipWhitespace(s, i);
    if (i == s.size()) return "";

    // Skip the type
    while (!isWhitespace(s[i])) {
        // Check for null character
        if (s[i] == '\0') return "";
        i++;
    }

    skipWhitespace(s, i);
    if (i == s.size()) return "";

    // Read the name
    nString name = "";
    while (s[i] != ';' && !isWhitespace(s[i])) {
        // Check for null character
        if (s[i] == '\0') return "";
        name += s[i++];
    }

    // Now to look for semantic comment

    skipWhitespace(s, i);
    if (i == s.size()) return name;

    // Check for comment
    if (s[i++] != '/') return name;
    if (i == s.size()) return name;
    if (s[i++] != '/') return name;
    if (i == s.size()) return name;

    skipWhitespace(s, i);
    if (i == s.size()) return name;

    // Check for SEM
    for (int j = 0; SEM_STR[j] != '\0'; j++) {
        if (i == s.size()) return name;
        if (s[i++] != SEM_STR[j]) return name;
    }

    skipWhitespace(s, i);
    if (i == s.size()) return name;

    // Read the semantic
    nString semanticName = "";
    while (!isWhitespace(s[i]) && !isNumeric(s[i])) {
        // Check for null character
        if (s[i] == '\0') return name;
        semanticName += s[i++];
    }
    auto& it = m_semantics.find(semanticName);
    if (it == m_semantics.end()) return name;

    // Get the number
    nString numberString = "";
    for (int j = 0; i != s.size() && isNumeric(s[i]); j++) {
        numberString += s[i];
        i++;
    }
    int number = 0;
    if (numberString.size()) {
        std::istringstream(numberString) >> number;
    }

    // Calculate the semantic ID
    semantic = (VGSemantic)it->second + number * Semantic::SEM_NUM_SEMANTICS + 1;
    return name;
}
