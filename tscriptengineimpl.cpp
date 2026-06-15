/* ========================================================================= *
 *                                                                           *
 *                             TScriptEngine                                 *
 *                      Copyright (c) 2025-2026, Zhihai TU                   *
 *                          All rights reserved.                             *
 *                            tuzhihai@126.com                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of TScriptEngine.                                       *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
 *                                                                           *
 * ========================================================================= */



#include "tscriptengineimpl.h"

namespace TScript {

const char * signall = u8"+-*/%&|!=()[]{}<>?:.,;'\"`~@#$^";

const char * signcList = u8"+,101;-,102;*,103;/,104;%,105;=,106;!,107;>,108;<,109";
const char * signccList = u8"<<,201;>>,202;+=,203;-=,204;*=,205;/=,206;%=,207;==,208;&&,209;||,210;!=,211;>=,212;<=,213;++,214;--,215";
const char * signcccList = u8"<<=,301;>>=,302;&&=,303;||=,304";
const char * signccsetList = u8"=;++;--;+=;-=;*=;/=;%=;>>=;<<=;&&=;||=";
const char * multisignList = u8"+-*/%&|!=<>";

/****************************************** Function Definiation Begin *******************************************/
char convertChar(char p) {
    char c = p;
    if(c == 't') {
        c = '\t';
    } else if(c == 'r') {
        c = '\r';
    } else if(c == 'n') {
        c = '\n';
    } else if(c == '\\') {
        c = '\\';
    }
    return c;
}

std::string charConvertHandwriteStr(char p) {
    char c = p;
    if(c == '\t') {
        return "\\t";
    } else if(c == '\r') {
        return "\\r";
    } else if(c == '\n') {
        return "\\n";
    } else if(c == '\\') {
        return "\\\\";
    } else if(c == '\'') {
        return "\\\'";
    } else if(c == '\"') {
        return "\\\"";
    }
    std::string s;
    s.push_back(c);
    return s;
}
/****************************************** Function Definiation End *********************************************/
std::string TScriptObject::getObjectName() {
    return "NativeObject";
}
bool TScriptObject::set(const std::string & name, const TScriptValue & value) {
    throw TScriptException(getObjectName() + " has no property " + name);
}
TScriptValue TScriptObject::get(const std::string & name) {
    throw TScriptException(getObjectName() + " has no property " + name);
}

void relocationSymbol(TScriptTokenLocations & scriptTokenLocations, TScriptTreeNode & scriptTreeNode) {
    TScriptExpression & expression = scriptTreeNode.get();
    if(expression.isFunc()) {
        if(TCollectHelper::contains(scriptTokenLocations.classLocationMap,expression.getToken())) {
            expression = scriptTokenLocations.classLocationMap[expression.getToken()];
        } else if(TCollectHelper::contains(scriptTokenLocations.funcLocationMap,expression.getToken())) {
            expression = scriptTokenLocations.funcLocationMap[expression.getToken()];
        }
    } else if(expression.isToken()) {
        if(TCollectHelper::contains(scriptTokenLocations.valueLocationMap,expression.getToken())) {
            expression = scriptTokenLocations.valueLocationMap[expression.getToken()];
        }
    }
    int paramCount = scriptTreeNode.getParamCount();
    for(int i=0;i<paramCount;i++) {
        relocationSymbol(scriptTokenLocations,scriptTreeNode[i]);
    }
}

/****************************************** Implementation Begin *************************************************/

/****************************************** TScriptException Begin ***********************************************/
TScriptException::TScriptException(const std::string& msg) : std::exception(),error(msg) {}

const std::string & TScriptException::message() {
    return error;
}
TScriptException & TScriptException::operator = (const TScriptException & ref) {
    if(&ref != this) {
        error = ref.error;
    }
    return *this;
}
/****************************************** TScriptException End *************************************************/


/******************************************* TScriptFile Begin ***************************************************/
TScriptFile::TScriptFile() {
}
TScriptFile::~TScriptFile() {
    if(file.is_open()) {
        file.close();
    }
}
size_t TScriptFile::length(const std::string & filepath) {
    std::fstream file(filepath, std::ios::in | std::ios::binary | std::ios::ate);
    size_t size = 0;
    if(file.is_open()) {
        size = file.tellg();
        file.close();
    }
    return size;
}
bool TScriptFile::exists(const std::string & filepath) {
    std::fstream f(filepath);
    return f.good();
}
std::string TScriptFile::readText(const std::string & filepath) {
    std::fstream file(filepath, std::ios::in);
    std::string result;
    if(file.is_open()) {
        file.seekg(0, std::ios::beg);
        std::string line;
        while(getline(file,line)) {
            result.append(line);
            result.push_back('\n');
        }
        file.close();
    }
    return result;
}
bool TScriptFile::writeText(const std::string & filepath, const std::string & text, bool isAppend) {
    std::fstream file(filepath, (isAppend)?(std::ios::out | std::ios::app | std::ios::ate):(std::ios::out | std::ios::trunc));
    if(file.is_open()) {
        file.write(text.data(), text.size());
        file.close();
        return true;
    }
    return false;
}
TScriptByteArray TScriptFile::readData(const std::string & filepath, long long pos, long long len) {
    TScriptByteArray ba;
    if(len == 0) {
        return ba;
    }
    std::fstream file(filepath, std::ios::in | std::ios::binary);
    if(file.is_open()) {
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        long long totalLen = size;
        if(len < 0) {
            if(pos >= 0) {
                totalLen = size - pos;
            } else {
                totalLen = -pos;
            }
        } else {
            if(pos >= 0) {
                totalLen = std::min(totalLen,len);
            } else {
                totalLen = std::min(totalLen, -pos);
            }
        }

        ba.expand(totalLen);
        if(pos >= 0) {
            file.seekg(pos, std::ios::beg);
        } else {
            file.seekg(pos, std::ios::end);
        }
        file.read((char *)ba.getData(), ba.length());
    }
    return ba;
}
bool TScriptFile::writeData(const std::string & filepath, const TScriptByteArray & byteArray, bool isAppend) {
    std::fstream file(filepath, (isAppend)?(std::ios::out | std::ios::app | std::ios::ate | std::ios::binary):(std::ios::out | std::ios::trunc | std::ios::binary));
    if(file.is_open()) {
        file.write((char*)byteArray.getData(), byteArray.length());
        file.close();
        return true;
    }
    return false;
}

void TScriptFile::mkdirs(const std::string & path) {
    //TODO
}
bool TScriptFile::isAbsolutePath(const std::string & path) {
    if(path.size() > 0) {
        if(path.at(0) == '/' || path.at(0) == '\\') {
            return true;
        }
        if(path.size() > 1) {
            if(path.at(1) == ':') {
                return true;
            }
        }
    }
    return false;
}
std::string TScriptFile::getAbsoluteFile(const std::string & file) {
    std::string s = file;
    if(s.length() == 0) {
        return s;
    }
    TStringHelper::replace(TStringHelper::replace(s, "\\", "/"),"/./","/");
    while(TStringHelper::strfind(s,"//") >= 0) {
        TStringHelper::replace(s, "//", "/");
    }
    int pos;
    while((pos = TStringHelper::strfind(s,"/../")) > 0) {
        int endpos = pos + 3;
        int prevpos = s.rfind('/',pos - 1);
        if(prevpos >= 0) {
            s.erase(prevpos,endpos - prevpos);
        } else {
            s.erase(0,endpos);
        }
    }
    if(s.size() >= 2) {
        if(s.at(1) == ':') {
            if(s.at(1) >= 'A' && s.at(1) <= 'Z') {
                s[1] = s.at(1) - 'A' + 'a';
            }
        }
    }
    while(TStringHelper::strfind(s,"//") >= 0) {
        TStringHelper::replace(s, "//", "/");
    }
    return s;
}
/**
 * @brief TScriptFile::getAbsolutePath
 *  convert '\\' to '/'
 * @param path
 * @return
 */
std::string TScriptFile::getAbsolutePath(const std::string & path) {
    std::string s = path;
    if(s.length() == 0) {
        return s;
    }
    TStringHelper::replace(TStringHelper::replace(s, "\\", "/"),"/./","/");
    while(TStringHelper::strfind(s,"//") >= 0) {
        TStringHelper::replace(s, "//", "/");
    }
    int pos;
    while((pos = TStringHelper::strfind(s,"/../")) > 0) {
        int endpos = pos + 3;
        int prevpos = s.rfind('/',pos - 1);
        if(prevpos >= 0) {
            s.erase(prevpos,endpos - prevpos);
        } else {
            s.erase(0,endpos);
        }
    }
    if(s.size() >= 2) {
        if(s.at(1) == ':') {
            if(s.at(1) >= 'A' && s.at(1) <= 'Z') {
                s[1] = s.at(1) - 'A' + 'a';
            }
        }
    }
    s.push_back('/');
    while(TStringHelper::strfind(s,"//") >= 0) {
        TStringHelper::replace(s, "//", "/");
    }
    return s;
}
std::string TScriptFile::getParentPath(const std::string & file) {
    std::string s = getAbsolutePath(file);
    int pos = s.rfind('/');
    if(pos < 0) {
        return "";
    } else  {
        return s.substr(0, pos + 1);
    }
}
std::string TScriptFile::getSimpleFileName(const std::string & file) {
    std::string s = getAbsolutePath(file);
    int pos = s.rfind('/');
    if(pos < 0) {
        return s;
    } else  {
        return s.substr(pos + 1);
    }
}
/******************************************* TScriptFile End *****************************************************/


/******************************************* TStringHelper Begin *************************************************/
std::string TStringHelper::fromWString(std::wstring w) {
    std::wstring_convert< std::codecvt_utf8_utf16<wchar_t> > converter;
     return converter.to_bytes(w);
}
std::wstring TStringHelper::toWString(const std::string s) {
    std::wstring_convert< std::codecvt_utf8_utf16<wchar_t> > converter;
    return converter.from_bytes(s);
}
std::string TStringHelper::toLower(const std::string & s) {
    std::string r;
    for(char c: s) {
        if(c >= 'A' && c <= 'Z') {
            c = c -'A' + 'a';
        }
        r.push_back(c);
    }
    return r;
}
std::string TStringHelper::toUpper(const std::string & s) {
    std::string r;
    for(char c: s) {
        if(c >= 'a' && c <= 'z') {
            c = c -'a' + 'A';
        }
        r.push_back(c);
    }
    return r;
}
int TStringHelper::toInt(const std::string & s) {
    return atoi(s.data());
}
long long TStringHelper::toLongLong(const std::string & s) {
    return atoll(s.data());
}
double TStringHelper::toDouble(const std::string & s) {
    return atof(s.data());
}
std::string TStringHelper::number(int v, int radix) {
    char s[20];
    _ltoa_s(v, s, sizeof (s) - 1, radix);
    return s;
}
std::string TStringHelper::number(bool b) {
    return (b)?"true":"false";
}
std::string TStringHelper::number(long long v, int radix) {
    char s[40];
    if(radix == 16) {
        sprintf(s, "%llx",v);
    } else {
        sprintf(s, "%lld",v);
    }
    return s;
}
std::string TStringHelper::number(double v) {
    char s[40];
    _gcvt_s(s, sizeof(s) - 1, v, 15);
    return s;
}
bool TStringHelper::startsWith(const std::string & s, const std::string & sub) {
    return startsWith(s, sub.data());
}

bool TStringHelper::startsWith(const std::string & s, const char * sub) {
    int slen = s.length();
    int sublen = strlen(sub);
    if(sublen > slen) {
        return false;
    }
    for(int i=0;i<sublen;i++) {
        if(s[i] != sub[i]) {
            return false;
        }
    }
    return true;
}
bool TStringHelper::endsWith(const std::string & s, const std::string & sub) {
    return endsWith(s,sub.data());
}
bool TStringHelper::endsWith(const std::string & s, const char * sub) {
    int slen = s.length();
    int sublen = strlen(sub);
    if(sublen > slen) {
        return false;
    }
    int s_start = slen - sublen;
    for(int i=0;i<sublen;i++) {
        if(s[s_start + i] != sub[i]) {
            return false;
        }
    }
    return true;
}
int TStringHelper::wstrfind(const std::wstring & s, char c, int offset, int end) {
    if(end < 0) end = s.size();
    else end = std::min(end, (int)s.size());
    offset = std::max(offset, 0);
    int lastIdx = end - 1;
    const wchar_t * p = s.c_str();
    for(int i=offset;i<=lastIdx;i++) {
        if(p[i] == c) {
            return i;
        }
    }
    return -1;

}
int TStringHelper::wstrfind(const std::wstring & s, const std::wstring & sub, int offset, int end) {
    if(end < 0) end = s.size();
    else end = std::min(end, (int)s.size());
    offset = std::max(offset, 0);
    int lastIdx = end - sub.size();
    const wchar_t * p = s.c_str();
    const wchar_t * q = sub.c_str();
    for(int i=offset;i<=lastIdx;i++) {
        bool cmp = 0;
        for(int t=0;t<sub.size();t++) {
            if(p[i + t] != q[t]) {
                cmp = 1;
            }
        }
        if(cmp == 0) {
            return i;
        }
    }
    return -1;

}
int TStringHelper::strfind(const std::string & s, char c, int offset, int end) {
    if(end < 0) end = s.size();
    else end = std::min(end, (int)s.size());
    offset = std::max(offset, 0);
    int lastIdx = end - 1;
    const char * p = s.c_str();
    for(int i=offset;i<=lastIdx;i++) {
        if(p[i] == c) {
            return i;
        }
    }
    return -1;

}
int TStringHelper::strfind(const std::string & s, const std::string & sub, int offset, int end) {
    if(end < 0) end = s.size();
    else end = std::min(end, (int)s.size());
    offset = std::max(offset, 0);
    int lastIdx = end - sub.size();
    const char * p = s.c_str();
    for(int i=offset;i<=lastIdx;i++) {
        if(strncmp(&p[i],sub.c_str(),sub.size()) == 0) {
            return i;
        }
    }
    return -1;

}

int TStringHelper::strlastfind(const std::string & s, char c, int offset) {
    if(offset < 0) offset = s.size() - 1;
    offset = std::min(offset, (int)s.size() - 1);
    const char * p = s.c_str();
    for(int i=offset;i>=0;i--) {
        if(p[i] == c) {
            return i;
        }
    }
    return -1;
}
int TStringHelper::strlastfind(const std::string & s, const std::string & sub, int offset) {
    if(offset < 0) offset = s.size() - 1;
    offset = std::min(offset, (int)s.size() - 1) - sub.size() + 1;
    const char * p = s.c_str();
    for(int i=offset;i>=0;i--) {
        if(strncmp(&p[i],sub.c_str(),sub.size()) == 0) {
            return i;
        }
    }
    return -1;
}

std::string & TStringHelper::trimLeft(std::string & s) {
    int n = 0;
    char c;
    while(n < s.length()) {
        c = s[n];
        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            n ++;
        } else {
            break;
        }
    }
    if(n > 0) {
        s.erase(0,n);
    }
    return s;
}
std::string & TStringHelper::trimRight(std::string & s) {
    int n = 0;
    int slen = s.length();
    char c;
    while(n < s.length()) {
        c = s[slen - 1 - n];
        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            n ++;
        } else {
            break;
        }
    }
    if(n > 0) {
        s.erase(slen - n);
    }
    return s;
}
std::string & TStringHelper::trim(std::string & s) {
    return trimLeft(trimRight(s));
}
std::wstring & TStringHelper::wtrimLeft(std::wstring & s) {
    int n = 0;
    char c;
    while(n < s.length()) {
        c = s[n];
        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            n ++;
        } else {
            break;
        }
    }
    if(n > 0) {
        s.erase(0,n);
    }
    return s;
}
std::wstring & TStringHelper::wtrimRight(std::wstring & s) {
    int n = 0;
    int slen = s.length();
    char c;
    while(n < s.length()) {
        c = s[slen - 1 - n];
        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            n ++;
        } else {
            break;
        }
    }
    if(n > 0) {
        s.erase(slen - n);
    }
    return s;
}
std::wstring & TStringHelper::wtrim(std::wstring & s) {
    return wtrimLeft(wtrimRight(s));
}
bool TStringHelper::isLatinString(std::wstring & s) {
    for(int i=0;i<s.size();i++) {
        if(!isChar(s.at(i))) return false;
    }
    return true;
}
bool TStringHelper::isChar(wchar_t c) {
    return (c & 0xFFFFFF80) == 0;
}
std::string TStringHelper::fromChar(char c) {
    char s[2];
    s[0] = c;
    s[1] = 0;
    return s;
}
std::string & TStringHelper::replace(std::string & s, std::string & sub, std::string & newstr) {
    return replace(s,sub.data(), newstr.data());
}

std::string & TStringHelper::replace(std::string & s, const char * sub, const char * newstr) {
    int slen = s.length();
    int sublen = strlen(sub);
    int stopIdx = slen - sublen + 1;
    int startIdx = 0;
    bool isMatch;
    std::string r;
    for(int i=0;i<=stopIdx;) {
        if(i >= stopIdx) {
            if(startIdx < slen) {
                r.append(s.substr(startIdx));
            }
            break;
        }
        isMatch = true;
        for(int n=0;n<sublen;n++) {
            if(s[i + n] != sub[n]) {
                isMatch = false;
                break;
            }
        }
        if(isMatch) {
            r.append(s.substr(startIdx, i - startIdx));
            r.append(newstr);
            startIdx = i + sublen;
            i = startIdx;
        } else {
            i ++;
        }
    }
    s.swap(r);
    return s;
}
std::vector<std::string> TStringHelper::split(const std::string & s, const std::string & sub) {
    return split(s,sub.data());
}
std::vector<std::string> TStringHelper::split(const std::string & s, const char * sub) {
    int slen = s.length();
    int sublen = strlen(sub);
    int stopIdx = slen - sublen + 1;
    int startIdx = 0;
    bool isMatch;
    std::vector<std::string> rv;
    for(int i=0;i<=stopIdx;) {
        if(i >= stopIdx) {
            if(startIdx < slen) {
                rv.push_back(s.substr(startIdx));
            }
            break;
        }
        isMatch = true;
        for(int n=0;n<sublen;n++) {
            if(s[i + n] != sub[n]) {
                isMatch = false;
                break;
            }
        }
        if(isMatch) {
            rv.push_back(s.substr(startIdx, i - startIdx));
            startIdx = i + sublen;
            i = startIdx;
        } else {
            i ++;
        }
    }
    return rv;
}
std::string TStringHelper::join(const std::vector<std::string> & vector, const std::string div) {

    std::string r;
    int count = 0;
    for(const std::string & s: vector) {
        if(count > 0) {
            r.append(div);
        }
        r.append(s);
        count ++;
    }
    return r;
}
/******************************************* TStringHelper End ***************************************************/



/******************************************* TScriptArray Begin **************************************************/
TScriptArray::TScriptArray() {
    hasToString = false;
}
TScriptArray::TScriptArray(const TScriptArray & ref)
{
    valueArray = ref.valueArray;
    hasToString = ref.hasToString;
    if(hasToString) sVal = ref.sVal;
}
TScriptArray::TScriptArray(const std::vector<TScriptValue> & ref) {
    hasToString = false;
    setArray(ref);
}
TScriptArray::TScriptArray(const std::list<TScriptValue> & ref) {
    hasToString = false;
    setArray(ref);
}

TScriptArray & TScriptArray::resize(int n) {
    for(int i=0;i<n;i++) {
        valueArray.push_back(TScriptValue());
    }
    return *this;
}
int TScriptArray::length() const {
    return valueArray.size();
}

TScriptArray & TScriptArray::clear() {
    hasToString = false;
    valueArray.clear();
    return *this;
}

int TScriptArray::indexOf(TScriptValue & ee) {
    for(int i=0;i<valueArray.size();i++) {
        if(ee == valueArray[i]) {
            return i;
        }
    }
    return -1;
}
const std::vector<TScriptValue> & TScriptArray::get() {
    return valueArray;
}
void TScriptArray::toList(std::list<TScriptValue> & list) {
    list.clear();
    for(int i=0;i<valueArray.size();i++) {
        list.push_back(valueArray[i]);
    }
}
TScriptValue & TScriptArray::at(int i) {
    if(i < 0 || i >= valueArray.size()) {
        throw TScriptException(u8"Array::at index out of range");
    }
    return valueArray[i];
}
TScriptValue & TScriptArray::back() {
    if(valueArray.size() == 0) {
        throw TScriptException(u8"Array::back Cannot retrive element from empty array");
    }
    return valueArray[valueArray.size() - 1];
}
TScriptValue & TScriptArray::front() {
    if(valueArray.size() == 0) {
        throw TScriptException(u8"Array::front Cannot retrive element from empty array");
    }
    return valueArray[0];
}

TScriptArray & TScriptArray::push_back(const TScriptValue & ee) {
    hasToString = false;
    valueArray.push_back(ee);
    return *this;
}
TScriptArray & TScriptArray::push_front(const TScriptValue & ee) {
    hasToString = false;
    valueArray.insert(valueArray.begin(), ee);
    return *this;
}
TScriptArray & TScriptArray::pop_back() {
    hasToString = false;
    valueArray.pop_back();
    return *this;
}
TScriptArray & TScriptArray::pop_front() {
    hasToString = false;
    valueArray.erase(valueArray.begin());
    return *this;
}
TScriptArray & TScriptArray::setArray(const std::vector<TScriptValue> & eeList) {
    hasToString = false;
    valueArray = eeList;
    return *this;
}
TScriptArray & TScriptArray::setArray(const std::list<TScriptValue> & eeList) {
    hasToString = false;
    valueArray.clear();

    for(std::list<TScriptValue>::const_iterator iter = eeList.begin(); iter != eeList.end(); iter ++) {
        valueArray.push_back(*iter);
    }
    return *this;
}
TScriptArray & TScriptArray::append(const TScriptValue & vv) {
    hasToString = false;
    valueArray.push_back(vv);
    return *this;
}
TScriptArray & TScriptArray::appendAll(const TScriptArray & ref) {
    if(&ref != this && ref.length() > 0) {
        hasToString = false;
        for(int i=0;i<ref.length();i++) {
            valueArray.push_back(ref.valueArray[i]);
        }
    }
    return *this;
}
TScriptArray & TScriptArray::appendAll(const std::vector<TScriptValue> & eeList) {
    if(eeList.size() > 0) {
        hasToString = false;
        for(std::vector<TScriptValue>::const_iterator iter=eeList.begin();iter != eeList.end();iter ++) {
            valueArray.push_back(*iter);
        }
    }
    return *this;
}
TScriptArray & TScriptArray::appendAll(const std::list<TScriptValue> & eeList) {
    if(eeList.size() > 0) {
        hasToString = false;
        for(std::list<TScriptValue>::const_iterator iter=eeList.begin();iter != eeList.end();iter ++) {
            valueArray.push_back(*iter);
        }
    }
    return *this;
}

TScriptArray & TScriptArray::removeAt(int i) {
    if(i < 0 || i >= valueArray.size()) {
        throw TScriptException(u8"Array::removeAt index out of range");
    }
    hasToString = false;
    valueArray.erase(valueArray.begin() + i);
    return *this;
}
TScriptArray & TScriptArray::remove(int pos, int n) {
    if(pos < 0 || pos >= valueArray.size()) {
        throw TScriptException(u8"Array::remove index out of range");
    }
    if(n == 0) {
        return *this;
    }
    int nn = valueArray.size() - pos;
    if( n > 0) {
        nn = std::min(nn, n);
    }
    hasToString = false;
    valueArray.erase(valueArray.begin() + pos, valueArray.begin() + pos + nn);
    return *this;
}
TScriptArray & TScriptArray::remove(TScriptValue & ee) {
    for(std::vector<TScriptValue>::iterator iter = valueArray.begin();iter != valueArray.end(); iter ++) {
        if(ee == *iter) {
            hasToString = false;
            valueArray.erase(iter);
            break;
        }
    }
    return *this;
}

TScriptValue & TScriptArray::operator [] (int i) {
    return at(i);
}

TScriptArray & TScriptArray::operator << (const TScriptValue & ee) {
    hasToString = false;
    valueArray.push_back(ee);
    return *this;
}
TScriptArray & TScriptArray::operator = (const TScriptArray & ref) {
    if(&ref != this) {
        valueArray = ref.valueArray;
        hasToString = ref.hasToString;
        if(hasToString) {
            sVal = ref.sVal;
        }
    }
    return *this;
}
TScriptArray & TScriptArray::operator = (const std::vector<TScriptValue> & ref) {
    setArray(ref);
    return *this;
}
TScriptArray & TScriptArray::operator = (const std::list<TScriptValue> & ref) {
    setArray(ref);
    return *this;
}
bool TScriptArray::operator == (const TScriptArray & ref) const {
    return equals(ref);
}
bool TScriptArray::equals(const TScriptArray & ref) const {
    if(&ref == this) {
        return true;
    }
    if(valueArray.size() != ref.length()) {
        return false;
    }
    for(int i=0;i<valueArray.size();i++) {
        if(valueArray[i] != ref.valueArray[i]) {
            return false;
        }
    }
    return true;
}
bool TScriptArray::operator != (const TScriptArray & ref) const {
    return !equals(ref);
}
int TScriptArray::compare(const TScriptArray & ref) const {
    int maxn = std::max(valueArray.size(), ref.valueArray.size());
    for(int i=0;i<maxn;i++) {
        if(i == valueArray.size()) {
            return -1;
        }
        if(i == ref.valueArray.size()) {
            return 1;
        }
        int r = valueArray[i].compare(ref.valueArray[i]);
        if(r != 0) return r;
    }
    return 0;
}
bool TScriptArray::operator < (const TScriptArray & ref) const {
    return compare(ref) < 0;
}
bool TScriptArray::operator > (const TScriptArray & ref) const {
    return compare(ref) > 0;
}

TScriptArray TScriptArray::slice(int pos, int n) {
    if(pos < 0 || pos >= valueArray.size()) {
        throw TScriptException(u8"Array::slice index out of range");
    }
    TScriptArray rr;
    if(n != 0) {
        int nn = valueArray.size() - pos;
        if( n > 0) {
            nn = std::min(nn, n);
        }
        for(int i=0;i<nn;i++) {
            rr << valueArray[pos + i];
        }
    }
    return rr;
}
TScriptArray & TScriptArray::swap(TScriptArray & ref) {
    if(&ref != this) {
        hasToString = false;
        valueArray.swap(ref.valueArray);
    }
    return *this;
}

TScriptArray & TScriptArray::sort(bool asc) {
    hasToString = false;
    std::sort(valueArray.begin(), valueArray.end());
    if(!asc) {
        std::reverse(valueArray.begin(), valueArray.end());
    }
    return *this;
}

std::string & TScriptArray::toString() {
    if(!hasToString) {
        hasToString = true;
        sVal.clear();
        sVal.push_back('[');
        for(int i=0;i<valueArray.size();i++) {
            if(i > 0) {
                sVal.push_back(',');
            }
            TScriptValue & v = valueArray[i];
            if(v.isString()) {
                sVal.push_back('"');
                sVal.append(TScriptHelper::getInstance()->strVal2Handwrite(v.getString()));
                sVal.push_back('"');
            } else if(v.isChar()) {
                sVal.push_back('\'');
                sVal.append(TScriptHelper::getInstance()->char2Handwrite(v.toChar()));
                sVal.push_back('\'');
            } else {
                sVal.append(valueArray[i].toString());
            }
        }
        sVal.push_back(']');
    }
    return sVal;
}
/******************************************* TScriptArray End ****************************************************/


/******************************************* TScriptMap Begin ****************************************************/
TScriptMap::TScriptMap() {
    hasToString = false;
}

TScriptMap::TScriptMap(const TScriptMap & ref) {
    valueMap = ref.valueMap;
    hasToString = ref.hasToString;
    if(hasToString) sVal = ref.sVal;
}


TScriptMap & TScriptMap::putAll(const TScriptMap & ref) {
    if(ref.length() > 0) {
        for(std::map<std::string,TScriptValue>::const_iterator iter = ref.valueMap.begin();iter != ref.valueMap.end(); iter ++) {
            valueMap[iter->first] = iter->second;
        }
        hasToString = false;
    }
    return *this;
}

TScriptMap & TScriptMap::put(const std::string & key, const TScriptValue & value) {
    valueMap[key] = value;
    return *this;
}

TScriptValue & TScriptMap::get(const std::string & key) {
    std::map<std::string,TScriptValue>::iterator iter = valueMap.find(key);
    return iter->second;
}

bool TScriptMap::contains(const std::string & key) const {
    return (valueMap.find(key) != valueMap.end());
}

int TScriptMap::length() const {
    return valueMap.size();
}
void TScriptMap::clear() {
    if(valueMap.size() > 0) {
        valueMap.clear();
        hasToString = false;
    }
}

std::vector<std::string> TScriptMap::keys() const {
    std::vector<std::string> ks;
    for(std::map<std::string,TScriptValue>::const_iterator iter = valueMap.begin();iter != valueMap.end(); iter ++) {
        ks.push_back(iter->first);
    }
    return ks;
}

TScriptValue & TScriptMap::operator[] (const std::string & key) {
    return get(key);
}
TScriptMap & TScriptMap::operator = (const TScriptMap & ref) {
    if(&ref != this) {
        valueMap = ref.valueMap;
        hasToString = ref.hasToString;
        if(hasToString) sVal = ref.sVal;
    }
    return *this;
}
bool TScriptMap::equals(const TScriptMap & ref) const {
    if(this == &ref) {
        return true;
    }
    if(valueMap.size() != ref.valueMap.size()) {
        return false;
    }
    for(std::map<std::string,TScriptValue>::const_iterator iter = valueMap.begin();iter != valueMap.end(); iter ++) {
        const std::string & k = iter->first;
        std::map<std::string,TScriptValue>::const_iterator viter = ref.valueMap.find(k);
        if(viter == ref.valueMap.end()) {
            return false;
        }
        if(iter->second != viter->second) {
            return false;
        }
    }
    return true;
}
bool TScriptMap::operator == (const TScriptMap & ref) const {
    return equals(ref);
}
bool TScriptMap::operator != (const TScriptMap & ref) const {
    return !equals(ref);
}
int TScriptMap::compare(const TScriptMap & ref) const {
    TScriptArray ks;
    for(std::map<std::string,TScriptValue>::const_iterator iter = valueMap.begin();iter != valueMap.end(); iter ++) {
        ks << iter->first;
    }
    for(std::map<std::string,TScriptValue>::const_iterator iter = ref.valueMap.begin();iter != ref.valueMap.end(); iter ++) {
        ks << iter->first;
    }
    ks.sort();
    for(int i=0;i<ks.length();i++) {
        TScriptValue & k = ks[i];
        if(i > 0) {
            if(ks[i] == ks[i - 1]) {
                continue;
            }
        }
        if(!contains(k.getConstString())) {
            return 1;
        }
        if(!ref.contains(k.getConstString())) {
            return -1;
        }
        const TScriptValue & va = valueMap.find(k.getConstString())->first;
        const TScriptValue & vb = ref.valueMap.find(k.getConstString())->first;
        int r = va.compare(vb);
        if(r != 0) return r;
    }
    return 0;
}
bool TScriptMap::operator < (const TScriptMap & ref) const {
    return compare(ref) < 0;
}
bool TScriptMap::operator > (const TScriptMap & ref) const {
    return compare(ref) > 0;
}
TScriptMap & TScriptMap::swap(TScriptMap & ref) {
    if(&ref != this) {
        hasToString = false;
        valueMap.swap(ref.valueMap);
        sVal.swap(ref.sVal);
        bool b = hasToString;
        hasToString = ref.hasToString;
        ref.hasToString = b;
    }
    return *this;
}
std::string & TScriptMap::toString() {
    if(!hasToString) {
        hasToString = true;
        sVal.clear();
        sVal.push_back('{');

        for(std::map<std::string,TScriptValue>::iterator iter = valueMap.begin();iter != valueMap.end(); iter ++) {
            if(iter != valueMap.begin()) {
                sVal.push_back(',');
            }
            const std::string & k = iter->first;
            sVal.push_back('\"');
            sVal.append(TScriptHelper::getInstance()->strVal2Handwrite(k));
            sVal.push_back('\"');
            sVal.append(": ");

            TScriptValue & value = iter->second;
            if(value.isChar()) {
                sVal.append("\'" + iter->second.toString() + "\'");
            } else if(value.isString()) {
                sVal.append("\"" + TScriptHelper::getInstance()->strVal2Handwrite(iter->second.getString()) + "\"");
            } else {
                sVal.append(iter->second.toString());
            }
        }
        sVal.push_back('}');
    }
    return sVal;
}
/******************************************* TScriptMap End ******************************************************/



/******************************************* TScriptObject Begin *************************************************/
TScriptClassObject::TScriptClassObject(){
    instanceObject = this;
}
TScriptClassObject::~TScriptClassObject() {
    TScriptFunction * scriptFunction = getEngine()->getInternalFunction("~" + className);
    if(scriptFunction != NULL) {
        std::vector<TScriptValue> paramList;
        TScriptFunctionEngine::applyScriptFunction(getEngine()->getStatementEngine(),*scriptFunction,paramList);
    }
    scriptClassEngine = nullptr;
}

void TScriptClassObject::init(const std::string & className, std::shared_ptr<TScriptObject> & parentObject) {
    this->className = className;
    this->superObject = parentObject;
    TScriptClassObject * p = (TScriptClassObject*)parentObject.get();
    while(p != NULL) {
        p->instanceObject = instanceObject;
        p = (TScriptClassObject*)p->superObject.get();
    }
}


bool TScriptClassObject::set(const std::string & name, const TScriptValue & value){
    TScriptClassObject * theObject = getInstanceObject();
    while(theObject != NULL) {
        if(theObject->getEngine()->hasVar(name)) {
            return theObject->getEngine()->getStatementEngine()->setVarVal(name,value);
        }
        theObject = (TScriptClassObject*)theObject->superObject.get();
    }

    throw TScriptException(getInstanceObject()->className + u8": no property " + name);
}
TScriptValue TScriptClassObject::get(const std::string & name){
    TScriptClassObject * theObject = getInstanceObject();
    while(theObject != NULL) {
        if(theObject->getEngine()->hasVar(name)) {
            return theObject->getEngine()->getStatementEngine()->getVarVal(name);
        }
        theObject = (TScriptClassObject*)theObject->superObject.get();
    }

    throw TScriptException(getInstanceObject()->className + u8": no property " + name);
}
TScriptValue TScriptClassObject::invoke(const std::string & method, std::vector<TScriptValue> & paramList) {
    TScriptClassObject * theObject = getInstanceObject();
    while(theObject != NULL) {
        TScriptFunction * scriptFunction = theObject->getEngine()->getInternalFunction(method);
        if(scriptFunction != NULL) {
            return TScriptFunctionEngine::applyScriptFunction(theObject->getEngine()->getStatementEngine(),*scriptFunction,paramList);
        }
        theObject = (TScriptClassObject*)theObject->superObject.get();
    }
    throw TScriptException(getInstanceObject()->className + u8": no method " + method);
}

std::string TScriptClassObject::getObjectName(){
    return className;
}

TScriptClassObject & TScriptClassObject::operator = ( const TScriptClassObject & ref){
    className = ref.className;
    superObject = ref.superObject;
    scriptClassEngine = ref.scriptClassEngine;
    return *this;
}
TScriptClassEngine * TScriptClassObject::getEngine()
{
    return scriptClassEngine.get();
}
void TScriptClassObject::setEngine(std::shared_ptr<TScriptClassEngine> engine) {
    scriptClassEngine = engine;
}
/******************************************* TScriptObject End ***************************************************/



/******************************************* TScriptValue Begin **************************************************/
const TScriptValue TScriptValue::V_INVALID = TScriptValue();
TScriptValue::TScriptValue() {
    hasToString = false;
    vtype = TXVALUE_INVALID;
}
TScriptValue::TScriptValue(int v) {
    hasToString = false;
    setInt(v);
}
TScriptValue::TScriptValue(bool v) {
    hasToString = false;
    setBool(v);
}
TScriptValue::TScriptValue(char v) {
    hasToString = false;
    setChar(v);
}
TScriptValue::TScriptValue(long long v) {
    hasToString = false;
    setLongLong(v);
}
TScriptValue::TScriptValue(double v) {
    hasToString = false;
    setDouble(v);
}
TScriptValue::TScriptValue(const std::string & v) {
    hasToString = false;
    setString(v);
}
TScriptValue::TScriptValue(const TScriptValue & v) {
    hasToString = false;
    vtype = v.vtype;
    if(v.vtype == TXVALUE_CHAR) cVal = v.cVal;
    if(v.vtype == TXVALUE_INT) iVal = v.iVal;
    if(v.vtype == TXVALUE_LONGLONG) lVal = v.lVal;
    if(v.vtype == TXVALUE_BOOL) iVal = v.iVal;
    if(v.vtype == TXVALUE_DOUBLE) dVal = v.dVal;
    if(v.vtype == TXVALUE_STRING) sVal = v.sVal;
    if(v.vtype == TXVALUE_ARRAY) vArray = v.vArray;
    if(v.vtype == TXVALUE_MAP) vMap = v.vMap;
    if(v.vtype == TXVALUE_OBJECT) vObj = v.vObj;
    if(v.vtype == TXVALUE_BYTEARRAY) vByteArray = v.vByteArray;
}
TScriptValue::TScriptValue(const std::vector<TScriptValue> & eeList) {
    hasToString = false;
    setArray(eeList);
}
TScriptValue::TScriptValue(const TScriptArray & vv) {
    hasToString = false;
    setArray(vv);
}
TScriptValue::TScriptValue(const TScriptMap & vv) {
    hasToString = false;
    setMap(vv);
}
TScriptValue::TScriptValue(const std::shared_ptr<TScriptByteArray> & vv) {
    hasToString = false;
    vtype = TXVALUE_BYTEARRAY;
    vByteArray = vv;
}
TScriptValue::TScriptValue(const std::shared_ptr<TScriptObject> & vv) {
    hasToString = false;
    vtype = TXVALUE_OBJECT;
    vObj = vv;
}
std::string TScriptValue::getTypeName(TXVALUE_TYPE type) {
    switch(type) {
    case TXVALUE_INVALID: return "invalid";
    case TXVALUE_NULL: return "null";
    case TXVALUE_BOOL: return "bool";
    case TXVALUE_INT: return "int";
    case TXVALUE_LONGLONG: return "long long";
    case TXVALUE_DOUBLE: return "double";
    case TXVALUE_CHAR: return "char";
    case TXVALUE_STRING: return "string";
    case TXVALUE_ARRAY: return "array";
    case TXVALUE_MAP: return "map";
    case TXVALUE_OBJECT: return "class object";
    case TXVALUE_BYTEARRAY: return "byte array";
    }
    return "unknown";
}
TScriptValue::TXVALUE_TYPE TScriptValue::getType() {
    return vtype;
}
bool TScriptValue::isInvalid() const {
    return (vtype == TXVALUE_INVALID);
}
bool TScriptValue::isNull() const {
    return vtype == TXVALUE_NULL;
}
bool TScriptValue::isBool() const {
    return vtype == TXVALUE_BOOL;
}
bool TScriptValue::isInt() const {
    return vtype == TXVALUE_INT;
}
bool TScriptValue::isLongLong() const {
    return vtype == TXVALUE_LONGLONG;
}
bool TScriptValue::isDouble() const {
    return vtype == TXVALUE_DOUBLE;
}
bool TScriptValue::isNumber() const {
    return vtype == TXVALUE_INT || vtype == TXVALUE_LONGLONG || vtype == TXVALUE_DOUBLE;
}
bool TScriptValue::isChar() const {
    return vtype == TXVALUE_CHAR;
}
bool TScriptValue::isString() const {
    return vtype == TXVALUE_STRING;
}
bool TScriptValue::isArray() const {
    return vtype == TXVALUE_ARRAY;
}
bool TScriptValue::isMap() const {
    return vtype == TXVALUE_MAP;
}
bool TScriptValue::isObject() const {
    return vtype == TXVALUE_OBJECT;
}
bool TScriptValue::isByteArray() const {
    return vtype == TXVALUE_BYTEARRAY;
}
bool TScriptValue::toBool() const {
    if(vtype == TXVALUE_BOOL) {
        return iVal != 0;
    } else if(vtype == TXVALUE_INT) {
        return iVal != 0;
    } else if(vtype == TXVALUE_LONGLONG) {
        return lVal != 0;
    } else if(vtype == TXVALUE_DOUBLE) {
        return dVal != 0;
    } else if(vtype == TXVALUE_STRING) {
        return  sVal == "true" || sVal == "TRUE";
    } else if(vtype == TXVALUE_CHAR) {
        return cVal == 't' || cVal == 'T';
    } else {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to bool");
    }
}
int TScriptValue::toInt() const {
    if(vtype == TXVALUE_INT) {
        return iVal;
    } else if(vtype == TXVALUE_BOOL) {
        return (iVal != 0)?1:0;
    } else if(vtype == TXVALUE_LONGLONG) {
        return (int)lVal;
    } else if(vtype == TXVALUE_DOUBLE) {
        return (int)dVal;
    } else if(vtype == TXVALUE_STRING) {
        return TStringHelper::toInt(sVal);
    } else if(vtype == TXVALUE_CHAR) {
        return cVal & 0x00FF;
    } else {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to int");
    }
}
long long TScriptValue::toLongLong() const {
    if(vtype == TXVALUE_LONGLONG) {
        return lVal;
    } else if(vtype == TXVALUE_BOOL) {
        return iVal;
    } else if(vtype == TXVALUE_INT) {
        return (iVal & 0x0FFFFFFFFFFFFFFFF);
    } else if(vtype == TXVALUE_DOUBLE) {
        return (long long)dVal;
    } else if(vtype == TXVALUE_STRING) {
        return TStringHelper::toLongLong(sVal);
    } else if(vtype == TXVALUE_CHAR) {
        return cVal & 0x00FF;
    } else {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to long long");
    }
}
double TScriptValue::toDouble() const {
    if(vtype == TXVALUE_DOUBLE) {
        return dVal;
    } else if(vtype == TXVALUE_BOOL) {
        return iVal;
    } else if(vtype == TXVALUE_INT) {
        return iVal;
    } else if(vtype == TXVALUE_LONGLONG) {
        return lVal;
    } else if(vtype == TXVALUE_STRING) {
        return TStringHelper::toDouble(sVal);
    } else if(vtype == TXVALUE_CHAR) {
        return cVal & 0x00FF;
    } else {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to double");
    }
}
char TScriptValue::toChar() const {
    if(vtype == TXVALUE_CHAR) {
        return cVal;
    } else if(vtype == TXVALUE_BOOL) {
        return (iVal != 0)?'t':'f';
    } else if(vtype == TXVALUE_INT) {
        return iVal & 0x00FF;
    } else if(vtype == TXVALUE_LONGLONG) {
        return lVal & 0x00FF;
    } else if(vtype == TXVALUE_DOUBLE) {
    } else if(vtype == TXVALUE_STRING) {
        if(sVal.length() != 1) {
            return sVal.at(0);
        }
    }
    throw TScriptException(getTypeName(vtype) + u8"cannot convert to char");
}
std::string & TScriptValue::toString() {
    char s[40];
    if(vtype == TXVALUE_STRING) {
        return sVal;
    } else if(vtype == TXVALUE_BOOL) {
        if(hasToString) {
            return sVal;
        }
        hasToString = true;
        sVal = (iVal != 0)?"true":"false";
        return sVal;
    } else if(vtype == TXVALUE_INT) {
        if(hasToString) {
            return sVal;
        }
        hasToString = true;
        _itoa_s(iVal, s, sizeof(s) - 1, 10);
        sVal = s;
        return sVal;
    } else if(vtype == TXVALUE_LONGLONG) {
        if(hasToString) {
            return sVal;
        }
        hasToString = true;
        sprintf(s, "%lld",lVal);
        sVal = s;
        return sVal;
    } else if(vtype == TXVALUE_DOUBLE) {
        if(hasToString) {
            return sVal;
        }
        hasToString = true;
        _gcvt_s(s,sizeof(s)-1, dVal, 15);
        sVal = s;
        return sVal;
    } else if(vtype == TXVALUE_NULL) {
        if(hasToString) {
            return sVal;
        }
        hasToString = true;
        sVal = "null";
        return sVal;
    } else if(vtype == TXVALUE_INVALID) {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to string");
    } else if(vtype == TXVALUE_CHAR) {
        if(hasToString) {
            return sVal;
        }
        hasToString = true;
        sVal.clear();
        sVal.push_back(cVal);
        return sVal;
    } else if(vtype == TXVALUE_ARRAY) {
        return vArray->toString();
    } else if(vtype == TXVALUE_MAP) {
        return vMap->toString();
    } else {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to string");
    }
}
const std::string & TScriptValue::getConstString() const {
    if(vtype == TXVALUE_STRING) {
        return sVal;
    } else {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to const string");
    }
}
std::string & TScriptValue::getString() {
    if(vtype == TXVALUE_STRING) {
        return sVal;
    } else {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to string");
    }
}
TScriptArray & TScriptValue::getArray() {
    if(vtype != TXVALUE_ARRAY) {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to array");
    }
    return *vArray;
}
TScriptMap & TScriptValue::getMap() {
    if(vtype != TXVALUE_MAP) {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to map");
    }
    return *vMap;
}
std::shared_ptr<TScriptObject> TScriptValue::getObject() {
    if(vtype != TXVALUE_OBJECT) {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to class object");
    }
    return vObj;
}
TScriptByteArray & TScriptValue::getByteArray() {
    if(vtype != TXVALUE_BYTEARRAY) {
        throw TScriptException(getTypeName(vtype) + u8"cannot convert to byte array");
    }
    return *vByteArray;
}
int TScriptValue::indexOf(const std::string & s, int start) const {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, indexOf is not supported");
    }
    return TStringHelper::strfind(sVal,s, start);
}
int TScriptValue::indexOf(const char c, int start) const {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, indexOf is not supported");
    }
    return TStringHelper::strfind(sVal,c, start);
}
int TScriptValue::lastIndexOf(const std::string & s, int start) const
{
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, lastIndexOf is not supported");
    }
    return sVal.rfind(s, start);
}
int TScriptValue::lastIndexOf(const char c, int start) const
{
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, lastIndexOf is not supported");
    }
    return sVal.rfind(c, start);
}
TScriptValue & TScriptValue::append(const std::string & s) {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, append is not supported");
    }
    sVal.append(s);
    return *this;
}
TScriptValue & TScriptValue::append(const char c) {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, append is not supported");
    }
    sVal.push_back(c);
    return *this;
}
TScriptValue & TScriptValue::push_front(const std::string & s) {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, push_front is not supported");
    }
    sVal.insert(0,s);
    return *this;
}
TScriptValue & TScriptValue::push_front(const char c) {
    char s[2];
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, push_front is not supported");
    }

    s[0] = c;
    s[1] = 0;
    sVal.insert(0,s);
    return *this;
}
TScriptValue & TScriptValue::push_back(const std::string & s) {
    return append(s);
}
TScriptValue & TScriptValue::push_back(const char c) {
    return append(c);
}
TScriptValue & TScriptValue::pop_back() {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, pop_back is not supported");
    }
    sVal.pop_back();
    return *this;
}
TScriptValue & TScriptValue::pop_front() {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, pop_back is not supported");
    }
    sVal.erase(0,1);
    return *this;
}
TScriptValue & TScriptValue::erase(int pos, int count)
{
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, erase is not supported");
    }
    sVal.erase(pos, count);
    return *this;
}
char TScriptValue::operator [](int pos) const {
    if(vtype != TXVALUE_STRING) {
        throw TScriptException(getTypeName(vtype) + u8" is not string, [] is not supported");
    }
    return sVal[pos];
}
TScriptValue & TScriptValue::setNull() {
    hasToString = false;
    vtype = TXVALUE_NULL;
    return *this;
}
TScriptValue & TScriptValue::setChar(char c) {
    hasToString = false;
    vtype = TXVALUE_CHAR;
    cVal = c;
    return *this;
}
TScriptValue & TScriptValue::setBool(bool v) {
    hasToString = false;
    vtype = TXVALUE_BOOL;
    iVal = (v)?1:0;
    return *this;
}
TScriptValue & TScriptValue::setInt(int v) {
    hasToString = false;
    vtype = TXVALUE_INT;
    iVal = v;
    return *this;
}
TScriptValue & TScriptValue::setLongLong(long long v) {
    hasToString = false;
    vtype = TXVALUE_LONGLONG;
    lVal = v;
    return *this;
}
TScriptValue & TScriptValue::setDouble(double v) {
    hasToString = false;
    vtype = TXVALUE_DOUBLE;
    dVal = v;
    return *this;
}
TScriptValue & TScriptValue::setString(const std::string & v) {
    hasToString = false;
    vtype = TXVALUE_STRING;
    sVal = v;
    return *this;
}
TScriptValue & TScriptValue::setArray(const std::vector<TScriptValue> & eeList) {
    hasToString = false;
    vtype = TXVALUE_ARRAY;
    if(vArray == nullptr) {
        vArray = std::shared_ptr<TScriptArray>(new TScriptArray());
    }
    *vArray = eeList;
    return *this;
}
TScriptValue & TScriptValue::setArray(const TScriptArray & v) {
    hasToString = false;
    vtype = TXVALUE_ARRAY;
    vtype = TXVALUE_ARRAY;
    if(vArray == nullptr) {
        vArray = std::shared_ptr<TScriptArray>(new TScriptArray());
    }
    *vArray = v;
    return *this;
}
TScriptValue & TScriptValue::setMap(const TScriptMap & v) {
    hasToString = false;
    vtype = TXVALUE_MAP;
    if(vMap == nullptr) {
        vMap = std::shared_ptr<TScriptMap>(new TScriptMap());
    }
    *vMap = v;
    return *this;
}
TScriptValue & TScriptValue::setObject(const std::shared_ptr<TScriptObject> & v) {
    hasToString = false;
    vtype = TXVALUE_OBJECT;
    vObj = v;
    return *this;
}
TScriptValue & TScriptValue::setByteArray(const std::shared_ptr<TScriptByteArray> & v) {
    hasToString = false;
    vtype = TXVALUE_BYTEARRAY;
    vByteArray = v;
    return *this;
}
TScriptValue & TScriptValue::clear() {
    hasToString = false;
    sVal = "";
    if(vArray != nullptr) {
        vArray->clear();
    }
    if(vMap != nullptr) {
        vMap->clear();
    }
    if(vByteArray != nullptr) {
        vByteArray->clear();
    }
    return *this;
}
TScriptValue & TScriptValue::operator << (const TScriptValue & v) {
    if(isString()) {
        if(v.isString()) {
            sVal.append(v.getConstString());
        } else if(v.isChar() || v.isInt() || v.isLongLong() || v.isDouble() || v.isBool() || v.isChar()) {
            TScriptValue nv = v;
            sVal.append(nv.toString());
        } else {
            throw TScriptException(getTypeName(vtype) + " < <" + getTypeName(v.vtype) + " is not supported");
        }
    } else if(isInt()) {
        if(v.isInt() || v.isLongLong() || v.isString()) {
            iVal <<= v.toInt();
        } else {
            throw TScriptException(getTypeName(vtype) + " << " + getTypeName(v.vtype) + " is not supported");
        }
    } else if(isLongLong()) {
        if(v.isInt() || v.isLongLong() || v.isString()) {
            lVal <<= v.toInt();
        } else {
            throw TScriptException(getTypeName(vtype) + " << " + getTypeName(v.vtype) + " is not supported");
        }
    } else {
        throw TScriptException(getTypeName(vtype) + " << " + getTypeName(v.vtype) + " is not supported");
    }
    return *this;
}
TScriptValue & TScriptValue::operator << (char v) {
    if(isString()) {
        sVal.push_back(v);
    } else {
        throw TScriptException(getTypeName(vtype) + " << char is not supported");
    }
    return *this;
}
TScriptValue & TScriptValue::operator << (const std::string & v) {
    if(isString()) {
        sVal.append(v);
    } else {
        throw TScriptException(getTypeName(vtype) + " << string is not supported");
    }
    return *this;
}

TScriptValue & TScriptValue::operator = (bool v) {
    setBool(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (int v) {
    setInt(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (long long v) {
    setLongLong(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (double v) {
    setDouble(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (char v) {
    setChar(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (const std::string & v) {
    setString(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (const TScriptArray & v) {
    setArray(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (const TScriptMap & v) {
    setMap(v);
    return *this;
}
TScriptValue & TScriptValue::operator = (const std::shared_ptr<TScriptObject> & v)
{
        hasToString = false;
        vtype = TXVALUE_OBJECT;
        vObj = v;
        return *this;
}
TScriptValue & TScriptValue::operator = (const std::shared_ptr<TScriptByteArray> & v)
{
        hasToString = false;
        vtype = TXVALUE_BYTEARRAY;
        vByteArray = v;
        return *this;
}
TScriptValue & TScriptValue::operator = (const TScriptValue & v) {
    vtype = v.vtype;
    hasToString = v.hasToString;
    if(v.vtype == TXVALUE_CHAR) cVal = v.cVal;
    if(v.vtype == TXVALUE_INT) iVal = v.iVal;
    if(v.vtype == TXVALUE_LONGLONG) lVal = v.lVal;
    if(v.vtype == TXVALUE_BOOL) iVal = v.iVal;
    if(v.vtype == TXVALUE_DOUBLE) dVal = v.dVal;
    if(hasToString || v.vtype == TXVALUE_STRING) sVal = v.sVal;
    if(v.vtype == TXVALUE_ARRAY) vArray = v.vArray;
    if(v.vtype == TXVALUE_MAP) vMap = v.vMap;
    if(v.vtype == TXVALUE_OBJECT) vObj = v.vObj;
    if(v.vtype == TXVALUE_BYTEARRAY) vByteArray = v.vByteArray;
    return *this;
}
int TScriptValue::compare(const TScriptValue & v) const {
    if(vtype == TXVALUE_INVALID || v.vtype == TXVALUE_INVALID) {
        throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
    }
    if(vtype == TXVALUE_NULL || v.vtype == TXVALUE_NULL) {
        if(vtype != TXVALUE_NULL) return 1;
        if(v.vtype != TXVALUE_NULL) return -1;
        return 0;
    }
    if(vtype == TXVALUE_ARRAY || v.vtype == TXVALUE_ARRAY) {
        if(vtype != TXVALUE_ARRAY || v.vtype != TXVALUE_ARRAY) {
            throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
        }
        return vArray->compare(*v.vArray);
    }
    if(vtype == TXVALUE_OBJECT || v.vtype == TXVALUE_OBJECT) {
        throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
    }
    if(vtype == TXVALUE_MAP || v.vtype == TXVALUE_MAP) {
        if(vtype != TXVALUE_MAP || v.vtype != TXVALUE_MAP) {
            throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
        }
        return vMap->compare(*v.vMap);
    }
    if(vtype == TXVALUE_STRING || v.vtype == TXVALUE_STRING) {
        if(vtype != TXVALUE_STRING) {
            return - v.compare(*this);
        }
        if(v.vtype == TXVALUE_STRING) {
            return sVal.compare(v.sVal);
        }
        if(v.vtype == TXVALUE_BOOL) {
            throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
        }
        if(v.vtype == TXVALUE_CHAR) {
            char s[2];s[0] = v.cVal;s[1] = 0;
            return sVal.compare(s);
        }
        if(v.vtype == TXVALUE_DOUBLE || (TStringHelper::strfind(sVal,'.')) > 0) {
            double n1 = toDouble();
            double n2 = v.toDouble();
            if(n1 < n2) return -1;
            if(n1 > n2) return 1;
            return 0;
        }
        if(v.vtype == TXVALUE_LONGLONG || sVal.size() > 9) {
            long long n1 = toLongLong();
            long long n2 = v.toLongLong();
            if(n1 < n2) return -1;
            if(n1 > n2) return 1;
            return 0;
        }
        if(v.vtype == TXVALUE_INT) {
            int n1 = toInt();
            int n2 = v.iVal;
            if(n1 < n2) return -1;
            if(n1 > n2) return 1;
            return 0;
        }
        throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
    }
    if(vtype == TXVALUE_BOOL || v.vtype == TXVALUE_BOOL) {
        if(vtype != TXVALUE_BOOL && v.vtype != TXVALUE_BOOL) {
            throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
        }
        bool n1 = toBool();
        bool n2 = toBool();
        if(n1 && n2) {
            return 0;
        }
        if(n1 == true) return 1;
        if(n2 == true) return -1;
        return 0;
    }
    if(vtype == TXVALUE_DOUBLE || v.vtype == TXVALUE_DOUBLE) {
        double n1 = toDouble();
        double n2 = v.toDouble();
        if(n1 < n2) return -1;
        if(n1 > n2) return 1;
        return 0;
    }
    if(vtype == TXVALUE_LONGLONG || v.vtype == TXVALUE_LONGLONG) {
        long long n1 = toLongLong();
        long long n2 = v.toLongLong();
        if(n1 < n2) return -1;
        if(n1 > n2) return 1;
        return 0;
    }
    if(vtype == TXVALUE_INT || v.vtype == TXVALUE_INT) {
        int n1 = toInt();
        int n2 = v.iVal;
        if(n1 < n2) return -1;
        if(n1 > n2) return 1;
        return 0;
    }
    if(vtype == TXVALUE_CHAR || v.vtype == TXVALUE_CHAR) {
        char n1 = toChar();
        char n2 = v.toChar();
        if(n1 < n2) return -1;
        if(n1 > n2) return 1;
        return 0;
    }
    throw TScriptException(getTypeName(vtype) + " compare with" + getTypeName(v.vtype) + " is not supported");
}
bool TScriptValue::operator == (const TScriptValue & ref) const {
    return equals(ref);
}
bool TScriptValue::operator != (const TScriptValue & ref) const {
    return !equals(ref);
}
bool TScriptValue::equals(const TScriptValue & ee) const {
    return compare(ee) == 0;
}

bool TScriptValue::operator < (const TScriptValue & v) const {
    return compare(v) < 0;
}
bool TScriptValue::operator > (const TScriptValue & v) const {
    return compare(v) > 0;
}

TScriptValue TScriptValue::fromString(const std::string & s) {
    TScriptValue r;
    if(s.at(0) >= '0' && s.at(0) <= '9') {
        if(TStringHelper::strfind(s,'.') > 0) {
            r = TStringHelper::toDouble(s);
        } else if(s.length() >= 9) {
            r = TStringHelper::toLongLong(s);
        } else {
            r = TStringHelper::toInt(s);
        }
    } else {
        if(s == "true") {
            r = true;
        } else if(s == "false") {
            r = false;
        } else if(s == "null") {
            r.setNull();
        }
    }
    return r;
}

/******************************************* TScriptValue End ****************************************************/


/****************************************** TScriptByteArray Begin ***********************************************/
TScriptByteArray::TScriptByteArray():buff(nullptr),buffSize(0),offset(0),dataLen(0)
{
}
TScriptByteArray::TScriptByteArray(const TScriptByteArray & ref){
    dataLen = ref.dataLen;
    buffSize = ref.buffSize;
    offset = 0;
    if(buffSize > 0) {
        buff = new uint8_t[buffSize];
        memcpy(buff, &ref.buff[offset], dataLen);
    }
}
TScriptByteArray::~TScriptByteArray() {
    if(buff != nullptr) {
        delete buff;
    }
}

uint8_t * TScriptByteArray::getData(int pos) const {
    if(pos < 0 || pos >= dataLen) {
        throw TScriptException(u8"ByteArray::getData index out of range");
    }
    return &buff[offset + pos];
}
bool TScriptByteArray::empty() const {
    return dataLen == 0;
}
int TScriptByteArray::length() const {
    return dataLen;
}
TScriptByteArray & TScriptByteArray::clear() {
    dataLen = 0;
    offset = 0;
    return *this;
}
TScriptByteArray & TScriptByteArray::operator = (const TScriptByteArray & ref) {
    dataLen = ref.dataLen;
    buffSize = ref.buffSize;
    offset = 0;
    if(buffSize > 0) {
        buff = new uint8_t[buffSize];
        memcpy(buff, &ref.buff[offset], dataLen);
    }
    return *this;
}
TScriptByteArray & TScriptByteArray::operator += (const TScriptByteArray & ref) {
    return push_back(ref.getData(), ref.length());
}

TScriptByteArray & TScriptByteArray::push_back (uint8_t b) {
    expand(1,TXBUFF_EXPAND_TAIL);
    buff[offset + dataLen] = b;
    dataLen ++;
    return *this;
}
TScriptByteArray & TScriptByteArray::push_front (uint8_t b) {
    expand(1,TXBUFF_EXPAND_HEAD);
    offset --;
    buff[offset] = b;
    dataLen ++;
    return *this;
}
TScriptByteArray & TScriptByteArray::push_back (const uint8_t * buff, int len) {
    if(len > 0) {
        expand(len,TXBUFF_EXPAND_TAIL);
        for(int i=0;i<len;i++) {
            this->buff[offset + dataLen + i] = buff[i];
        }
        dataLen += len;
    }
    return *this;
}
TScriptByteArray & TScriptByteArray::push_back (const TScriptByteArray & ref) {
    if(ref.dataLen > 0) {
        push_back(ref.getData(), ref.length());
    }
    return *this;
}
TScriptByteArray & TScriptByteArray::push_front (const uint8_t * buff, int len) {
    if(len > 0) {
        expand(len,TXBUFF_EXPAND_HEAD);
        offset -= len;
        for(int i=0;i<len;i++) {
            this->buff[offset + i] = buff[i];
        }
        dataLen += len;
    }
    return *this;
}
TScriptByteArray & TScriptByteArray::push_front (const TScriptByteArray & ref) {
    if(ref.dataLen > 0) {
        push_front(ref.getData(), ref.length());
    }
    return *this;
}
TScriptByteArray & TScriptByteArray::insert (int pos, const uint8_t * buff, int len) {
    if(pos < 0 || pos > dataLen) {
        throw TScriptException(u8"ByteArray::insert index out of range");
    }
    if(len > 0) {
        expand(len,TXBUFF_EXPAND_TAIL);
        int moveLen = dataLen - pos;
        for(int i=(moveLen - 1);i >= 0;i--) {
            this->buff[offset + pos + i + len] = this->buff[offset + pos + i];
        }
        for(int i=0;i<len;i++) {
            this->buff[offset + pos + i] = buff[i];
        }
        dataLen += len;
    }
    return *this;
}
TScriptByteArray & TScriptByteArray::insert (int pos, const TScriptByteArray & ref) {
    if(ref.dataLen > 0) {
        insert(pos, ref.getData(), ref.length());
    }
    return *this;
}

TScriptByteArray & TScriptByteArray::remove(int pos, int len) {
    if(pos < 0 || pos >= dataLen) {
        throw TScriptException(u8"ByteArray::remove index out of range");
    }
    if(len < 0) {
        dataLen = pos;
    } else if(len > 0) {
        int removeLen = dataLen - pos;
        if(removeLen > len) {
            removeLen = len;
        }
        if(pos == 0) {
            offset += removeLen;
            dataLen -= removeLen;
        } else if(removeLen == (dataLen - pos)) {
            dataLen = pos;
        } else {
            int tailMoveLen = dataLen - pos - removeLen;
            if(tailMoveLen > pos) {
                for(int i=(pos - 1);i>=0;i--) {
                    buff[offset + i + removeLen] = buff[offset + i];
                }
                dataLen -= removeLen;
                offset += removeLen;
            } else {
                for(int i=0;i<tailMoveLen;i++) {
                    buff[offset + pos + i] = buff[offset + pos + i + removeLen];
                }
                dataLen -= removeLen;
            }
        }
    }
    return *this;
}
TScriptByteArray & TScriptByteArray::removeAt(int pos) {
    return remove(pos, 1);
}
TScriptByteArray & TScriptByteArray::reverse() {
    if(dataLen > 0) {
        int halfLen = dataLen / 2;
        for(int i=0;i<halfLen;i++) {
            uint8_t b = buff[offset + i];
            buff[offset + i] = buff[offset + dataLen - i - 1];
            buff[offset + dataLen - i - 1] = b;
        }
    }
    return *this;
}

uint8_t TScriptByteArray::operator[] (int pos) {
    return get(pos);
}
uint8_t TScriptByteArray::get(int pos) {
    if(pos < 0 || pos >= dataLen) {
        throw TScriptException(u8"ByteArray::get index out of range");
    }
    return buff[offset + pos];
}
void TScriptByteArray::set(int pos, uint8_t b) {
    if(pos < 0 || pos >= dataLen) {
        throw TScriptException(u8"ByteArray::set index out of range");
    }
    buff[offset + pos] = b;
}
int TScriptByteArray::get(int pos, uint8_t * buff, int len) {
    if(pos < 0 || pos >= dataLen) {
        throw TScriptException(u8"ByteArray::get index out of range");
    }
    if(len == 0) {
        return 0;
    }
    int cpLen = dataLen - pos;
    if(len > 0 && cpLen > len) {
        cpLen = len;
    }
    memcpy(buff, &this->buff[offset + pos], cpLen);
    return cpLen;
}

TScriptByteArray& TScriptByteArray::expand(int expandSize, TXBUFF_EXPAND_TYPE direct) {
    if(expandSize > 0) {
        if(direct == TXBUFF_EXPAND_HEAD) {
            if(offset < expandSize) {
                int newExpandSize = (expandSize - offset + 255) & 0xFFFFFF00;
                if(dataLen == 0 && buffSize >= newExpandSize) {
                    offset = newExpandSize;
                    return *this;
                }
                buffSize += newExpandSize;
                uint8_t * nb = new uint8_t[buffSize];
                if(buff != nullptr) {
                    if(dataLen > 0) {
                        memcpy(&nb[newExpandSize],&buff[offset],dataLen);
                    }
                    delete buff;
                }
                buff = nb;
                offset = newExpandSize;
            }
        } else  if(direct == TXBUFF_EXPAND_SIZE) {
            int newExpandSize = (expandSize + 255) & 0xFFFFFF00;
            if(dataLen == 0 && buffSize >= newExpandSize) {
                offset = 0;
                return *this;
            }
            buffSize += newExpandSize;
            uint8_t * nb = new uint8_t[buffSize];
            if(buff != nullptr) {
                if(dataLen > 0) {
                    memcpy(&nb[offset], &buff[offset],dataLen);
                }
                delete buff;
            }
            buff = nb;
        } else if(direct == TXBUFF_EXPAND_TAIL) {
            if((buffSize - dataLen - offset) < expandSize) {
                int newExpandSize = (expandSize + 255) & 0xFFFFFF00;
                if(dataLen == 0 && buffSize >= newExpandSize) {
                    offset = 0;
                    return *this;
                }
                buffSize += newExpandSize;
                uint8_t * nb = new uint8_t[buffSize];
                if(buff != nullptr) {
                    if(dataLen > 0) {
                        memcpy(&nb[offset], &buff[offset],dataLen);
                    }
                    delete buff;
                }
                buff = nb;
            }
        }
    }
    return *this;
}

/***
class TScriptData {
public:
    TScriptData()
        :dataLen(0)
    {}
    ~TScriptData() {
    }

    TScriptData & clear() {
        buffList.clear();
        dataLen = 0;
        return *this;
    }
    TScriptData & push_back (uint8_t b) {
        if(buffList.size() == 0) {
            buffList.insert(buffList.begin(), std::shared_ptr<TScriptBuffer>(new TScriptBuffer()));
        }
        buffList.back()->push_back(b);
        dataLen ++;
        return *this;
    }
    TScriptData & push_front (uint8_t b) {
        if(buffList.size() == 0) {
            buffList.insert(buffList.begin(), std::shared_ptr<TScriptBuffer>(new TScriptBuffer()));
        }
        buffList[0]->push_front(b);
        dataLen ++;
        return *this;
    }
    TScriptData & push_back (uint8_t * buff, int len) {
        return insert(dataLen,buff,len);
    }
    TScriptData & push_front (uint8_t * buff, int len) {
        return insert(0,buff,len);
    }
    TScriptData & insert (int pos, uint8_t * buff, int len) {
        if(pos < 0 || pos > dataLen) {
            throw TScriptException(u8"Index out of range");
        }
        if(len > 0) {
            int buffStartPos = 0;
            for(int i=0;i<=buffList.size();i++) {
                if(i == buffList.size()) {
                    buffList.insert(buffList.begin() + i, std::shared_ptr<TScriptBuffer>(new TScriptBuffer()));
                }
                if(pos > 0 && pos >= buffList[i]->length()) {
                    pos -= buffList[i]->length();
                } else {
                    if((buffList[i]->length() + len) <= 1024) {
                        buffList[i]->insert(pos, buff, len);
                        dataLen += len;
                    } else {
                        std::shared_ptr<TScriptBuffer> nextBuff(new TScriptBuffer());
                        nextBuff->push_back(buffList[i]->getData(pos), buffList[i]->length() - pos);
                        buffList[i]->remove(pos);
                        buffList.insert(buffList.begin() + i + 1,nextBuff);

                        if(buffList[i]->length() < 1024) {
                            int deltaLen = 1024 - buffList[i]->length();
                            buffList[i]->insert(pos, buff, deltaLen);
                            dataLen += deltaLen;
                            len -= deltaLen;
                            buffStartPos += deltaLen;
                        }
                        int nextlistPos = i;
                        while(len > 0) {
                            int deltaLen = std::min(1024, len);
                            nextlistPos ++;
                            buffList.insert(buffList.begin() + nextlistPos, std::shared_ptr<TScriptBuffer>(new TScriptBuffer()));
                            buffList[nextlistPos]->insert(0,&buff[buffStartPos], deltaLen);
                            len -= deltaLen;
                            buffStartPos += deltaLen;
                            dataLen += deltaLen;
                        }
                    }
                    break;
                }
            }
        }
        return *this;
    }

    TScriptData & remove(int pos, int len = -1) {
        if(pos < 0 || pos >= dataLen) {
            throw TScriptException(u8"Index out of range");
        }
        int removeLen = dataLen - pos;
        if(len > 0 && removeLen > len) {
            removeLen = len;
        }
        for(int i=0;i<buffList.size();i++) {
            if(pos >= buffList[i]->length()) {
                pos -= buffList[i]->length();
            } else if(removeLen > 0) {
                int deltaLen = std::min(removeLen, buffList[i]->length() - pos);
                buffList[i]->remove(pos, deltaLen);
                removeLen -= deltaLen;
                dataLen -= deltaLen;
                pos = 0;
            } else {
                break;
            }
        }
        return *this;
    }
    TScriptData & removeAt(int pos) {
        return remove(pos,1);
    }

    uint8_t get(int pos) {
        if(pos < 0 || pos >= dataLen) {
            throw TScriptException(u8"Index out of range");
        }
        for(int i=0;i<buffList.size();i++) {
            if(pos >= buffList[i]->length()) {
                pos -= buffList[i]->length();
            } else {
                return buffList[i]->get(pos);
            }
        }
        throw TScriptException(u8"Data error");
    }
    int get(int pos, uint8_t * buff, int len) {
        if(pos < 0 || pos >= dataLen) {
            throw TScriptException(u8"Index out of range");
        }
        int remainLen = this->dataLen - pos;
        if(len > 0 && remainLen > len) remainLen = len;
        int readLen = 0;
        for(int i=0;i<buffList.size();i++) {
            if(pos >= buffList[i]->length()) {
                pos -= buffList[i]->length();
            } else if(remainLen > 0){
                int cpLen = std::min(remainLen, buffList[i]->length() - pos);
                buffList[i]->get(pos,&buff[readLen],cpLen);
                remainLen -= cpLen;
                readLen += cpLen;
                pos = 0;
            } else {
                break;
            }
        }
        return readLen;
    }

    int length() {
        return dataLen;
    }

    TScriptData & reverse() {
        std::vector<std::shared_ptr<TScriptBuffer> > newBuffList;
        int count = buffList.size();
        for(int i=0;i<buffList.size();i++) {
            newBuffList.push_back(buffList[count - i - 1]);
            newBuffList.back()->reverse();
        }
        buffList.swap(newBuffList);
        return *this;
    }

private:
    std::vector<std::shared_ptr<TScriptBuffer> > buffList;
    int dataLen;
};
***/

/****************************************** TScriptByteArray End *************************************************/


/******************************************* TScriptExpression Begin *********************************************/
std::string TScriptExpression::getTypeName(EXPRESSION_TYPE type) {
    switch(type) {
    case EXPRESSION_INVALID: return "invalid";
    case EXPRESSION_OP: return "operator sign";
    case EXPRESSION_OPFUNC: return "operator function";
    case EXPRESSION_TOKEN: return "token";
    case EXPRESSION_FUNC: return "function";
    case EXPRESSION_PROPERTY: return "property";
    case EXPRESSION_VARIABLE: return "variable";
    case EXPRESSION_VALUE: return "value";
    }
    return "unknown";
}
TScriptExpression::TScriptExpression(){
    type = EXPRESSION_TYPE::EXPRESSION_INVALID;
}
TScriptExpression::EXPRESSION_TYPE TScriptExpression::getType() {
    return type;
}
void TScriptExpression::asOpFunc() {
    if(type == EXPRESSION_OP || type == EXPRESSION_OPFUNC) {
        type = EXPRESSION_TYPE::EXPRESSION_OPFUNC;
    } else {
        throw TScriptException("Line " + std::to_string(getLineNo()) + ":" + getToken() + u8" is not operator sign");
    }
}
void TScriptExpression::asOp() {
    if(type == EXPRESSION_OP || type == EXPRESSION_OPFUNC) {
        type = EXPRESSION_TYPE::EXPRESSION_OP;
    } else {
        throw TScriptException("Line " + std::to_string(getLineNo()) + ":" +getToken() +  u8" is not operator sign");
    }
}
void TScriptExpression::asFunc() {
    if(type == EXPRESSION_FUNC || type == EXPRESSION_VARIABLE || type == EXPRESSION_TOKEN) {
        type = EXPRESSION_TYPE::EXPRESSION_FUNC;
    } else {
        throw TScriptException("Line " + std::to_string(getLineNo()) + ":" + getToken() + u8" is not function name");
    }
}
void TScriptExpression::asVariable() {
    if(type == EXPRESSION_FUNC || type == EXPRESSION_VARIABLE || type == EXPRESSION_TOKEN) {
        type = EXPRESSION_TYPE::EXPRESSION_VARIABLE;
    } else {
        throw TScriptException("Line " + std::to_string(getLineNo()) + ":" + getToken() + u8" is not variable name");
    }
}

TScriptValue & TScriptExpression::getValue() {
    if(type != EXPRESSION_VALUE) throw TScriptException("Line " + std::to_string(getLineNo()) + ":" + u8" is not value");
    return value;
}
TScriptExpression::TXSIGN_TYPE TScriptExpression::getOp() {
    if(type != EXPRESSION_OP && type != EXPRESSION_OPFUNC) throw TScriptException("Line " + std::to_string(getLineNo()) + ":" + getToken() + u8" is not operator sign");
    return sign;
}
std::string & TScriptExpression::getToken() {
    if(type != EXPRESSION_FUNC && type != EXPRESSION_VARIABLE && type != EXPRESSION_TOKEN && type != EXPRESSION_PROPERTY) throw TScriptException(getTypeName(type) + u8" is not token");
    return this->token;
}

TScriptExpression & TScriptExpression::operator = (const TScriptExpression & ref) {
    type = ref.type;
    sign = ref.sign;
    lineNo = ref.lineNo;
    if(type == EXPRESSION_FUNC || type == EXPRESSION_TOKEN || type == EXPRESSION_PROPERTY || type == EXPRESSION_VARIABLE) {
        token = ref.token;
        scriptTokenLocation = ref.scriptTokenLocation;
    }
    if(type == EXPRESSION_VALUE) {
        value = ref.value;
    }
    return *this;
}
TScriptExpression & TScriptExpression::operator = (const TScriptTokenLocation & ref) {
    scriptTokenLocation = ref;
    return *this;
}
TScriptTokenLocation & TScriptExpression::getScriptTokenLocation() {
    return scriptTokenLocation;
}
/******************************************* TScriptExpression End ***********************************************/


/******************************************* RegisteredFunction Begin ********************************************/

RegisteredFunction::RegisteredFunction() {
    userData = nullptr;
    useUserData = false;
    paramCount = 0;
    minParamCount = 0;
}
RegisteredFunction::RegisteredFunction(const RegisteredFunction & ref) {
    f0 = ref.f0;
    name = ref.name;
    paramCount = ref.paramCount;
    minParamCount = ref.minParamCount;
    useUserData = ref.useUserData;
    userData = ref.userData;
}
RegisteredFunction::~RegisteredFunction(){}
void RegisteredFunction::bind(const std::string & name, const user_func0 f, int minParamCount){this->name = name; paramCount = 0; f0 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_func1 f, int minParamCount){this->name = name; paramCount = 1; f1 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_func2 f, int minParamCount){this->name = name; paramCount = 2; f2 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_func3 f, int minParamCount){this->name = name; paramCount = 3; f3 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_func4 f, int minParamCount){this->name = name; paramCount = 4; f4 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_func5 f, int minParamCount){this->name = name; paramCount = 5; f5 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_func6 f, int minParamCount){this->name = name; paramCount = 6; f6 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_func7 f, int minParamCount){this->name = name; paramCount = 7; f7 = f; this->minParamCount = minParamCount;}

void RegisteredFunction::bind(const std::string & name, const user_data_func0 f, void * data, int minParamCount){this->name = name; paramCount = 0; useUserData = true; userData = data; fd0 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_data_func1 f, void * data, int minParamCount){this->name = name; paramCount = 1; useUserData = true; userData = data; fd1 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_data_func2 f, void * data, int minParamCount){this->name = name; paramCount = 2; useUserData = true; userData = data; fd2 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_data_func3 f, void * data, int minParamCount){this->name = name; paramCount = 3; useUserData = true; userData = data; fd3 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_data_func4 f, void * data, int minParamCount){this->name = name; paramCount = 4; useUserData = true; userData = data; fd4 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_data_func5 f, void * data, int minParamCount){this->name = name; paramCount = 5; useUserData = true; userData = data; fd5 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_data_func6 f, void * data, int minParamCount){this->name = name; paramCount = 6; useUserData = true; userData = data; fd6 = f; this->minParamCount = minParamCount;}
void RegisteredFunction::bind(const std::string & name, const user_data_func7 f, void * data, int minParamCount){this->name = name; paramCount = 7; useUserData = true; userData = data; fd7 = f; this->minParamCount = minParamCount;}

std::string & RegisteredFunction::getName() {
    return name;
}
TScriptValue RegisteredFunction::exec(std::vector<TScriptValue> paramList) {
    if(paramCount < paramList.size()) {
        throw TScriptException(name + u8" parameters is more than " + TStringHelper::number(paramCount));
    }

    int minCount = minParamCount;
    if(minCount == -1) {
        minCount = paramCount;
    }
    if(minCount > paramCount) {
        minCount = paramCount;
    }
    if(minCount > paramList.size()) {
        if(minCount == paramCount) {
            throw TScriptException(name + u8"parameters is less than " + TStringHelper::number(minCount));
        } else {
            throw TScriptException(name + u8"parameters is required at least " + TStringHelper::number(minCount));
        }
    }
    if(paramCount > paramList.size()) {
        //parameters required
        //if parameters is more, discard the more parameters.
        //if parameters is less, append null from tail
        while(paramCount > paramList.size()) {
            paramList.push_back(TScriptValue());
        }
    }
    TScriptValue res;
    if(!useUserData) {
        switch(paramCount) {
        case 0: res = f0(); break;
        case 1: res = f1(paramList[0]); break;
        case 2: res = f2(paramList[0],paramList[1]); break;
        case 3: res = f3(paramList[0],paramList[1],paramList[2]); break;
        case 4: res = f4(paramList[0],paramList[1],paramList[2],paramList[3]); break;
        case 5: res = f5(paramList[0],paramList[1],paramList[2],paramList[3],paramList[4]); break;
        case 6: res = f6(paramList[0],paramList[1],paramList[2],paramList[3],paramList[4],paramList[5]); break;
        case 7: res = f7(paramList[0],paramList[1],paramList[2],paramList[3],paramList[4],paramList[5],paramList[6]); break;
        }
    } else {
        switch(paramCount) {
        case 0: res = fd0(userData); break;
        case 1: res = fd1(userData,paramList[0]); break;
        case 2: res = fd2(userData,paramList[0],paramList[1]); break;
        case 3: res = fd3(userData,paramList[0],paramList[1],paramList[2]); break;
        case 4: res = fd4(userData,paramList[0],paramList[1],paramList[2],paramList[3]); break;
        case 5: res = fd5(userData,paramList[0],paramList[1],paramList[2],paramList[3],paramList[4]); break;
        case 6: res = fd6(userData,paramList[0],paramList[1],paramList[2],paramList[3],paramList[4],paramList[5]); break;
        case 7: res = fd7(userData,paramList[0],paramList[1],paramList[2],paramList[3],paramList[4],paramList[5],paramList[6]); break;
        }
    }
    return res;
}

RegisteredFunction & RegisteredFunction::operator = (const RegisteredFunction & ref) {
    switch(ref.paramCount) {
    case 0: f0=ref.f0;break;
    case 1: f1=ref.f1;break;
    case 2: f2=ref.f2;break;
    case 3: f3=ref.f3;break;
    case 4: f4=ref.f4;break;
    case 5: f5=ref.f5;break;
    case 6: f6=ref.f6;break;
    case 7: f7=ref.f7;break;
    }
    name = ref.name;
    paramCount = ref.paramCount;
    minParamCount = ref.minParamCount;
    useUserData = ref.useUserData;
    userData = ref.userData;
    return *this;
}
/******************************************* RegisteredFunction End **********************************************/





/******************************************* TScriptHelper Begin *************************************************/
TScriptHelper::TScriptHelper() {
    std::vector<std::string> sl = TStringHelper::split(std::string(signcList),";");
    TCollectHelper::append(sl,TStringHelper::split(std::string(signccList),";"));
    TCollectHelper::append(sl,TStringHelper::split(std::string(signcccList),";"));
    for(std::string & s: sl) {
        int div = TStringHelper::strfind(s,",");
        std::string sign = s.substr(0,div);
        int op = TStringHelper::toInt(s.substr(div + 1));
        sign2opMap[sign] = op;
        op2signMap[op] = sign;
        opList.push_back(sign);
    }
    signsetList = TStringHelper::split(std::string(signccsetList),";");

    sign2simpleMap["+="] = "+";
    sign2simpleMap["-="] = "-";
    sign2simpleMap["*="] = "*";
    sign2simpleMap["/="] = "/";
    sign2simpleMap["%="] = "%";
    sign2simpleMap[">>="] = ">>";
    sign2simpleMap["<<="] = "<<";
    sign2simpleMap["&&="] = "&&";
    sign2simpleMap["||="] = "||";

}

static TScriptHelper * scriptHelperInstance = NULL;
TScriptHelper * TScriptHelper::getInstance() {
    if(scriptHelperInstance == NULL) {
        scriptHelperInstance = new TScriptHelper();
    }
    return scriptHelperInstance;
}
TScriptValue & TScriptHelper::throwException(const std::string & s) {
    throw TScriptException(s);
}
bool TScriptHelper::isToken(const std::string & s) {
    int strLen = strlen(signall);
    int startIdx = 0;
    if(s == "~") {
        if(strLen == 1) {
            return false;
        }
        startIdx ++;
    }
    for(int i=startIdx;i<strLen;i++) {
        if(TStringHelper::strfind(s,signall[i]) >= 0) {
            return false;
        }
    }
    return true;
}
//op include setSign
bool TScriptHelper::isOp(const std::string & sign) {
    return TCollectHelper::contains(opList,sign);
}
bool TScriptHelper::isSetOpSign(const std::string & sign) {
    return TCollectHelper::contains(signsetList,sign);
}
bool TScriptHelper::isSignPrefix(const std::string & sign) {
    for(const std::string & s: opList) {
        if(TStringHelper::startsWith(s,sign)) {
            return true;
        }
    }
    return false;
}
bool TScriptHelper::isCompareOp(const TScriptExpression::TXSIGN_TYPE op) {
    if(op == TScriptExpression::TXSIGN_EQUAL
            || op == TScriptExpression::TXSIGN_NOTEQ
            || op == TScriptExpression::TXSIGN_GT
            || op == TScriptExpression::TXSIGN_GTEQ
            || op == TScriptExpression::TXSIGN_LT
            || op == TScriptExpression::TXSIGN_LTEQ
            ) {
        return true;
    }
    return false;
}
bool TScriptHelper::isSetOp(const TScriptExpression::TXSIGN_TYPE op) {
    if(op == TScriptExpression::TXSIGN_SET
            || op == TScriptExpression::TXSIGN_ADDSET
            || op == TScriptExpression::TXSIGN_SUBSET
            || op == TScriptExpression::TXSIGN_MULSET
            || op == TScriptExpression::TXSIGN_DIVSET
            || op == TScriptExpression::TXSIGN_MODSET
            || op == TScriptExpression::TXSIGN_LMOVSET
            || op == TScriptExpression::TXSIGN_RMOVSET
            || op == TScriptExpression::TXSIGN_ANDSET
            || op == TScriptExpression::TXSIGN_ORSET
            || op == TScriptExpression::TXSIGN_ADD1
            || op == TScriptExpression::TXSIGN_SUB1
            ) {
        return true;
    }
    return false;
}
std::string TScriptHelper::op2Sign(const TScriptExpression::TXSIGN_TYPE op) {
    if(op2signMap.find(op) != op2signMap.end()) {
        return op2signMap[op];
    }
    return "";
}
TScriptExpression::TXSIGN_TYPE TScriptHelper::sign2Op(const std::string & sign) {
    int n = sign2opMap[sign];
    return (TScriptExpression::TXSIGN_TYPE)n;
}
const std::string & TScriptHelper::simplilySign(const std::string & fromSign) {
    if(sign2simpleMap.find(fromSign) != sign2simpleMap.end()) {
        return sign2simpleMap[fromSign];
    }
    return fromSign;
}

/******************************************* TScriptHelper End ***************************************************/


/******************************************* TScriptModuleEngine Begin *******************************************/
TScriptModuleEngine * TScriptModuleEngine::getModuleEngine(TScriptGlobalEngine * scriptGlobalEngine, TScriptModule * scriptModule) {
    return scriptGlobalEngine->getScriptModuleEngine(scriptModule);
}
TScriptValue TScriptModuleEngine::getVarVal(const std::string & name) {
    std::map<std::string,TScriptValue>::iterator iter = valueMap.find(name);
    if(iter != valueMap.end()) {
        return iter->second;
    }
    throw TScriptException(name + " is undefined");
}
bool TScriptModuleEngine::setVarVal(const std::string & name, const TScriptValue & value) {
    valueMap[name] = value;
    return true;
}

TScriptValue TScriptModuleEngine::evalScript() {
    scriptStatementEngine = std::shared_ptr<TScriptStatementEngine>(new TScriptStatementEngine(this, NULL, scriptModule->scriptStatement.get()));
    scriptStatementEngine->setReturnStop(true);
    return scriptStatementEngine->evalStatement();
}
/******************************************* TScriptModuleEngine End *********************************************/

/******************************************* TScriptGlobalEngine Begin *******************************************/
TScriptGlobalEngine::TScriptGlobalEngine() {
    getVarFunc = nullptr;
    setVarFunc = nullptr;
    initDefaultFunctions();
}
TScriptGlobalEngine::~TScriptGlobalEngine() {

}
TScriptValue TScriptGlobalEngine::applyUserFunc(int calllineNo, const std::string & name, std::vector<TScriptValue> & paramList) {
    if(name == "Array") {
        return TScriptValue(paramList);
    }
    if(name == "ByteArray") {
        if(paramList.size() != 0) {
            throw TScriptException(std::string("Line " + std::to_string(calllineNo) + ":" + u8"create ByteArray need non parameters"));
        }
        return TScriptValue(std::shared_ptr<TScriptByteArray>(new TScriptByteArray()));
    }
    if(name == "Map") {
        TScriptValue mapVal;
        mapVal.setMap(TScriptMap());
        int paramLen = paramList.size();
        for(int i=0;i<paramLen;i+=2) {
            mapVal.getMap().put(paramList[i + 0].toString(),paramList[i + 1]);
        }
        return mapVal;
    }
    if(name == "printf") {
        if(paramList.size() < 1) {
            throw TScriptException(std::string("Line " + std::to_string(calllineNo) + ":" + u8"printf need parameters"));
        }
        std::string res;
        int startIdx = 0;
        int div;
        std::string & formatText = paramList[0].toString();
        int paramIndex;
        for(paramIndex=1;paramIndex < paramList.size();paramIndex++) {
            if(startIdx >= formatText.length()) {
                break;
            }
            div = TStringHelper::strfind(formatText,"%", startIdx);
            if(div >= 0) {
                bool findNext = false;
                for(int i=div;i<formatText.size();i++) {
                    if(formatText[i] == ' ' || formatText[i] == '\t' || formatText[i] == '\r' || formatText[i] == '\n') {
                        findNext = true;
                        res.append(formatText.substr(startIdx, div - startIdx));
                        std::string format = formatText.substr(div,i);
                        if(TStringHelper::endsWith(format,"lld")) {
                            res.push_back(paramList[paramIndex].toLongLong());
                        } else if(TStringHelper::endsWith(format,"d")) {
                            res.push_back(paramList[paramIndex].toInt());
                        } else if(TStringHelper::endsWith(format,"f")) {
                            res.push_back(paramList[paramIndex].toDouble());
                        } else if(TStringHelper::endsWith(format,"s")) {
                            res.append(paramList[paramIndex].toString());
                        } else if(TStringHelper::endsWith(format,"c")) {
                            res.push_back(paramList[paramIndex].toChar());
                        } else {
                            res.append(format);
                        }
                        startIdx = i;
                        break;
                    }
                }
                if(!findNext) {
                    break;
                }
            } else {
                break;
            }
        }
        if(startIdx < formatText.length()) {
            res.append(formatText.substr(startIdx));
        }
        std::cout << res << std::endl;
        return res;
    }
    if(name == "min") {
        bool hasDouble = false;
        bool hasLongLong = false;
        if(paramList.size() == 0) {
            throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
        }
        for(int i=0;i<paramList.size();i++) {
            if(paramList[i].isString()) {
                if((TStringHelper::strfind(paramList[i].getString(),'.')) > 0) {
                    hasDouble = true;
                } else if(paramList[i].getString().size() >= 9) {
                    hasLongLong = true;
                }
            } else if(paramList[i].isDouble()) {
                hasDouble = true;
            } else if(paramList[i].isLongLong()) {
                hasLongLong = true;
            } else if(paramList[i].isInt()) {

            } else {
                throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
            }
        }
        if(hasDouble) {
            double minVal = paramList[0].toDouble();
            for(int i=1;i<paramList.size();i++) {
                minVal = std::min(minVal, paramList[i].toDouble());
            }
            return minVal;
        }
        if(hasLongLong) {
            long long minVal = paramList[0].toLongLong();
            for(int i=1;i<paramList.size();i++) {
                minVal = std::min(minVal, paramList[i].toLongLong());
            }
            return minVal;
        }
        int minVal = paramList[0].toInt();
        for(int i=1;i<paramList.size();i++) {
            minVal = std::min(minVal, paramList[i].toInt());
        }
        return minVal;
    }
    if(name == "max") {
        bool hasDouble = false;
        bool hasLongLong = false;
        if(paramList.size() == 0) {
            throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
        }
        for(int i=0;i<paramList.size();i++) {
            if(paramList[i].isString()) {
                if((TStringHelper::strfind(paramList[i].getString(),'.')) > 0) {
                    hasDouble = true;
                } else if(paramList[i].getString().size() >= 9) {
                    hasLongLong = true;
                }
            } else if(paramList[i].isDouble()) {
                hasDouble = true;
            } else if(paramList[i].isLongLong()) {
                hasLongLong = true;
            } else if(paramList[i].isInt()) {

            } else {
                throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
            }
        }
        if(hasDouble) {
            double maxVal = paramList[0].toDouble();
            for(int i=1;i<paramList.size();i++) {
                maxVal = std::max(maxVal, paramList[i].toDouble());
            }
            return maxVal;
        }
        if(hasLongLong) {
            long long maxVal = paramList[0].toLongLong();
            for(int i=1;i<paramList.size();i++) {
                maxVal = std::max(maxVal, paramList[i].toLongLong());
            }
            return maxVal;
        }
        int maxVal = paramList[0].toInt();
        for(int i=1;i<paramList.size();i++) {
            maxVal = std::max(maxVal, paramList[i].toInt());
        }
        return maxVal;
    }
    if(name == "sum") {
        bool hasDouble = false;
        if(paramList.size() == 0) {
            throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
        }
        for(int i=0;i<paramList.size();i++) {
            if(paramList[i].isString()) {
                if((TStringHelper::strfind(paramList[i].getString(),'.')) > 0) {
                    hasDouble = true;
                }
            } else if(paramList[i].isDouble()) {
                hasDouble = true;
            } else if(!(paramList[i].isLongLong() || paramList[i].isInt())) {
                throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
            }
        }
        if(hasDouble) {
            double sumVal = paramList[0].toDouble();
            for(int i=1;i<paramList.size();i++) {
                sumVal  +=  paramList[i].toDouble();
            }
            return sumVal;
        }
        long long sumVal = paramList[0].toLongLong();
        for(int i=1;i<paramList.size();i++) {
            sumVal  +=  paramList[i].toLongLong();
        }
        return sumVal;
    }
    if(name == "avg") {
        if(paramList.size() == 0) {
            throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
        }
        for(int i=0;i<paramList.size();i++) {
            if(!(paramList[i].isString() || paramList[i].isInt() || paramList[i].isDouble() || paramList[i].isLongLong())) {
                throw TScriptException("Line " + std::to_string(calllineNo) + ": " + name + u8" need number parameters");
            }
        }
        double avgVal = paramList[0].toDouble();
        for(int i=1;i<paramList.size();i++) {
            avgVal += paramList[i].toDouble();
        }
        return avgVal / paramList.size();
    }
    std::string simplySign = TScriptHelper::getInstance()->simplilySign(name);
    std::map<std::string,RegisteredFunction>::iterator iter = registeredFunctionMap.find(simplySign);
    if(iter == registeredFunctionMap.end()) {
        throw TScriptException("Line " + std::to_string(calllineNo) + ": " + simplySign + u8" is undefined");
    }
    try {
        return iter->second.exec(paramList);
    }  catch (TScriptException & ee) {
        throw TScriptException("Line " + std::to_string(calllineNo) + ": " + simplySign + ee.message());
    }
}

void TScriptGlobalEngine::initDefaultFunctions()
{
    bindUserFunc("getBaseDir", [this]()->TScriptValue{
        return this->getBaseDir();
    });
    bindUserFunc("console", [](TScriptValue&ee)->TScriptValue{
        std::cout << ee.toString() << std::endl;
        return ee;
    });
    bindUserFunc("debug", [](TScriptValue&ee)->TScriptValue{
        std::cout << ee.toString() << std::endl;
        return ee;
    });
    bindUserFunc("sin", [](TScriptValue&ee)->TScriptValue{return sin(ee.toDouble());});
    bindUserFunc("cos", [](TScriptValue&ee)->TScriptValue{return cos(ee.toDouble());});
    bindUserFunc("tan", [](TScriptValue&ee)->TScriptValue{return tan(ee.toDouble());});
    bindUserFunc("asin", [](TScriptValue&ee)->TScriptValue{return asin(ee.toDouble());});
    bindUserFunc("acos", [](TScriptValue&ee)->TScriptValue{return acos(ee.toDouble());});
    bindUserFunc("atan", [](TScriptValue&ee)->TScriptValue{return atan(ee.toDouble());});

    bindUserFunc("atan2", [](TScriptValue&y,TScriptValue&x)->TScriptValue{return atan2(y.toDouble(),x.toDouble());});

    bindUserFunc("pow", [](TScriptValue&ee, TScriptValue&nn)->TScriptValue{
        int n = nn.toInt();
        if (n == 0) {
            return 1;
        } else if (n == 1) {
            return ee;
        }
        double f = ee.toDouble();
        if (n == 2) {
            return f * f;
        }
        return std::pow(f, nn.toDouble());
    });
    bindUserFunc("log", [](TScriptValue&ee)->TScriptValue{return log(ee.toDouble());});
    bindUserFunc("log2", [](TScriptValue&ee)->TScriptValue{return log2(ee.toDouble());});
    bindUserFunc("sqrt", [](TScriptValue&ee)->TScriptValue{return sqrt(ee.toDouble());});
    bindUserFunc("exp", [](TScriptValue&ee)->TScriptValue{return exp(ee.toDouble());});

    bindUserFunc("atoi", [](TScriptValue&ee)->TScriptValue{return ee.toInt();});
    bindUserFunc("atol", [](TScriptValue&ee)->TScriptValue{return ee.toLongLong();});
    bindUserFunc("atof", [](TScriptValue&ee)->TScriptValue{return ee.toDouble();});
    bindUserFunc("itoa", [](TScriptValue&ee)->TScriptValue{return ee.toString();});
    bindUserFunc("ltoa", [](TScriptValue&ee)->TScriptValue{return ee.toString();});
    bindUserFunc("ftoa", [](TScriptValue&ee)->TScriptValue{return ee.toString();});
    bindUserFunc("floor", [](TScriptValue&ee)->TScriptValue{return std::floor(ee.toDouble());});
    bindUserFunc("ceil", [](TScriptValue&ee)->TScriptValue{return std::ceil(ee.toDouble());});
    bindUserFunc("strcmp", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{return e1.toString() == e2.toString();});
    bindUserFunc("lower", [](TScriptValue&ee)->TScriptValue{return TStringHelper::toLower(ee.toString());});
    bindUserFunc("upper", [](TScriptValue&ee)->TScriptValue{return TStringHelper::toUpper(ee.toString());});
    bindUserFunc("isNull", [](TScriptValue&ee)->TScriptValue{return ee.isNull();});

    bindUserFunc("milliseconds", []()->TScriptValue{
                     std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
                     return tp.time_since_epoch().count();
                 });
    bindUserFunc("datetime", []()->TScriptValue{
                     auto now = std::chrono::system_clock::now();
                     auto t_now = std::chrono::system_clock::to_time_t(now);
                     auto tm_now = std::localtime(&t_now);
                     std::stringstream ss;
                     ss << std::put_time(tm_now, "%Y%m%d%H%M%S");
                     return ss.str();
                 });
    bindUserFunc("date", []()->TScriptValue{
                     auto now = std::chrono::system_clock::now();
                     auto t_now = std::chrono::system_clock::to_time_t(now);
                     auto tm_now = std::localtime(&t_now);
                     std::stringstream ss;
                     ss << std::put_time(tm_now, "%Y%m%d");
                     return ss.str();
                 });
    bindUserFunc("time", []()->TScriptValue{
                     auto now = std::chrono::system_clock::now();
                     auto t_now = std::chrono::system_clock::to_time_t(now);
                     auto tm_now = std::localtime(&t_now);
                     std::stringstream ss;
                     ss << std::put_time(tm_now, "%H%M%S");
                     return ss.str();
                 });

    bindUserFunc("abs", [](TScriptValue&ee)->TScriptValue{
        if(ee.isChar()) return ee.toInt();
        if(ee.isBool()) return ee.toInt();
        if(ee.isInt()) return abs(ee.toInt());
        if(ee.isLongLong()) return abs(ee.toLongLong());
        if(ee.isDouble()) return abs(ee.toDouble());
        if(ee.isString()) {
            std::string & s = ee.toString();
            if(TStringHelper::strfind(s,".") > 0) {
                return abs(ee.toDouble());
            } else {
                if(s.length() >= 9) {
                    return abs(ee.toLongLong());
                } else {
                    return abs(ee.toInt());
                }
            }
        }
        return 0;
    });
    bindUserFunc("++", [](TScriptValue&ee)->TScriptValue{return (ee.isLongLong()?(ee.toLongLong() + 1):(ee.toInt() + 1));});
    bindUserFunc("--", [](TScriptValue&ee)->TScriptValue{return (ee.isLongLong()?(ee.toLongLong() - 1):(ee.toInt() - 1));});
    bindUserFunc("+", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            std::vector<TScriptValue> rList;
            if(e1.isArray()) {
                rList.push_back(e1.getArray().get());
            } else {
                rList.push_back(e1);
            }
            if(e2.isArray()) {
                rList.push_back(e2.getArray().get());
            } else {
                rList.push_back(e2);
            }
            return rList;
        };
        if(e1.isString()) {return e1.toString() + e2.toString();}
        if(e1.isBool() && e2.isBool()) {return e1.toBool() || e2.toBool();}
        if(e1.isDouble() || e2.isDouble()) {return e1.toDouble() + e2.toDouble();}
        if(e1.isLongLong() || e2.isLongLong()) {return e1.toLongLong() + e2.toLongLong();}
        return e1.toInt() + e2.toInt();
    });
    bindUserFunc("-", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray()) {
            std::vector<TScriptValue> rList;
            rList.push_back(e1.getArray().get());
            if(e2.isArray()) {
                for(int i=0;i<e2.getArray().length();i++) {
                    TCollectHelper::remove(rList, e2.getArray()[i]);
                }
            } else {
                TCollectHelper::remove(rList, e2);
            }
            return rList;
        };
        if(e1.isString()) {
            if((e1.isString() && e1.indexOf(".") >= 0) || e2.isDouble() || (e2.isString() && e2.indexOf(".") >= 0)) {
                return e1.toDouble() - e2.toDouble();
            } else {
                return e1.toLongLong() - e2.toLongLong();
            }
        }
        if(e1.isBool() && e2.isBool()) {return e2.toBool()?false:e1.toBool();}
        if(e1.isDouble() || e2.isDouble()) {return e1.toDouble() - e2.toDouble();}
        if(e1.isLongLong() || e2.isLongLong()) {return e1.toLongLong() - e2.toLongLong();}
        return e1.toInt() - e2.toInt();
    });
    bindUserFunc("*", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            std::vector<TScriptValue> r1List;
            if(e1.isArray()) {
                r1List.push_back(e1.getArray().get());
            } else {
                r1List.push_back(e1);
            }
            std::vector<TScriptValue> r2List;
            if(e2.isArray()) {
                r2List.push_back(e2.getArray().get());
            } else {
                r2List.push_back(e2);
            }
            std::vector<TScriptValue> rList;
            for(int r1=0;r1<r1List.size();r1++) {
                for(int r2=0;r2<r1List.size();r2++) {
                    if(r1List[r1].equals(r2List[r2])) {
                        rList.push_back(r1List[r1]);
                        break;
                    }
                }
            }
            return rList;
        };
        if(e1.isString()) {
            if((e1.isString() && e1.indexOf(".") >= 0) || e2.isDouble() || (e2.isString() && e2.indexOf(".") >= 0)) {
                return e1.toDouble() * e2.toDouble();
            } else {
                return e1.toLongLong() * e2.toLongLong();
            }
        }
        if(e1.isBool() && e2.isBool()) {return e1.toBool() && e2.toBool();}
        if(e1.isDouble() || e2.isDouble()) {return e1.toDouble() * e2.toDouble();}
        if(e1.isLongLong() || e2.isLongLong()) {return e1.toLongLong() * e2.toLongLong();}
        return e1.toLongLong() * e2.toLongLong();
    });
    bindUserFunc("/", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            return 0;
        };
        if(e1.isString() || e2.isString()) {
            if((e1.isString() && e1.indexOf(".") >= 0) || e2.isDouble() || (e2.isString() && e2.indexOf(".") >= 0)) {
                double d2 = e2.toDouble();
                if(d2 == 0) {
                    throw TScriptException(u8"Divide by zero exception");
                }
                return e1.toDouble() / d2;
            } else {
                long long d2 = e2.toLongLong();
                if(d2 == 0) {
                    throw TScriptException(u8"Divide by zero exception");
                }
                return e1.toLongLong() / d2;
            }
        }
        if(e1.isDouble() || e2.isDouble()) {
            double d2 = e2.toDouble();
            if(d2 == 0) {
                throw TScriptException(u8"Divide by zero exception");
            }
            return e1.toDouble() / d2;
        }
        if(e1.isLongLong() || e2.isLongLong()) {
            long long d2 = e2.toLongLong();
            if(d2 == 0) {
                throw TScriptException(u8"Divide by zero exception");
            }
            return e1.toLongLong() / d2;
        }
        int d2 = e2.toInt();
        if(d2 == 0) {
            throw TScriptException(u8"Divide by zero exception");
        }
        return e1.toInt() / d2;
    });
    bindUserFunc("%", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        TScriptValue defe;
        defe = 0;
        if(e1.isArray() || e2.isArray()) {
            return defe;
        };
        if(e1.isString() || e2.isString()) {
            long long d2 = e2.toLongLong();
            if(d2 == 0) {
                throw TScriptException(u8"Divide by zero exception");
            }
            return e1.toLongLong() % d2;
        }
        if(e1.isDouble() || e2.isDouble()) {
            long long d2 = e2.toLongLong();
            if(d2 == 0) {
                throw TScriptException(u8"Divide by zero exception");
            }
            return e1.toLongLong() % d2;
        }
        if(e1.isLongLong() || e2.isLongLong()) {
            long long d2 = e2.toLongLong();
            if(d2 == 0) {
                throw TScriptException(u8"Divide by zero exception");
            }
            return e1.toLongLong() % d2;
        }
        int d2 = e2.toInt();
        if(d2 == 0) {
            throw TScriptException(u8"Divide by zero exception");
        }
        return e1.toInt() % d2;
    });
    bindUserFunc(">>", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        TScriptValue defe;
        defe = 0;
        if(e1.isArray() || e2.isArray()) {
            return defe;
        };
        if(e1.isString() || e2.isString()) {
            return e1.toLongLong() >> e2.toInt();
        }
        if(e1.isDouble() || e2.isDouble()) {return e1.toLongLong() >> e2.toInt();}
        if(e1.isLongLong() || e2.isLongLong()) {return e1.toLongLong() >> e2.toInt();}
        return e1.toInt() >> e2.toInt();
    });
    bindUserFunc("<<", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        TScriptValue defe;
        defe = 0;
        if(e1.isArray()) {
            e1.getArray() << e2;
            return e1;
        };
        if(e1.isByteArray()) {
            if(e2.isChar() || e2.isInt() || e2.isLongLong()) {
                e1.getByteArray().push_back(e2.toChar());
            } else if(e2.isString()) {
                std::string & s = e2.getString();
                e1.getByteArray().push_back((const uint8_t *)s.data(), s.size());
            } else {
                throw TScriptException(TScriptValue::getTypeName(e1.getType()) + " << " + TScriptValue::getTypeName(e2.getType()) + u8" is not supported");
            }
            return e1;
        }
        return e1 << e2;
    });
    //  && is called in eval
    bindUserFunc("&&", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        TScriptValue defe;
        defe = false;
        if(e1.isArray() || e2.isArray()) {
            return (e1.getArray().length() > 0) && (e2.getArray().length() > 0);
        };
        return e1.toBool() && e2.toBool();
    });
    //  || is called in eval
    bindUserFunc("||", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            return (e1.getArray().length() > 0) || (e2.getArray().length() > 0);
        };
        return e1.toBool() || e2.toBool();
    });
    bindUserFunc("!", [](TScriptValue&e1)->TScriptValue{
        if(e1.isArray()) {
            return e1.getArray().length() == 0;
        };
        return !e1.toBool();
    });
    bindUserFunc(">", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            return false;
        }
        if(e1.isString() && e2.isString()) {
            return e1.toString() > e2.toString();
        }
        if(e1.isDouble() || e2.isDouble()) {
            return e1.toDouble() > e2.toDouble();
        }
        return e1.toLongLong() > e2.toLongLong();
    });
    bindUserFunc("<", [this](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            return false;
        }
        std::vector<TScriptValue> eeList;
        eeList.push_back(e2);
        eeList.push_back(e1);
        return applyUserFunc(0, ">",eeList);
    });
    bindUserFunc(">=", [this](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            return false;
        }
        std::vector<TScriptValue> eeList;
        eeList.push_back(e1);
        eeList.push_back(e2);
        TScriptValue res = applyUserFunc(0, "<",eeList);
        return !res.toBool();
    });
    bindUserFunc("<=", [this](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        if(e1.isArray() || e2.isArray()) {
            return false;
        }
        std::vector<TScriptValue> eeList;
        eeList.push_back(e1);
        eeList.push_back(e2);
        TScriptValue res = applyUserFunc(0, ">",eeList);
        return !res.toBool();
    });
    bindUserFunc("==", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        return e1.equals(e2);
    });
    bindUserFunc("!=", [](TScriptValue&e1,TScriptValue&e2)->TScriptValue{
        return !e1.equals(e2);
    });
}
void TScriptGlobalEngine::initVarValue(std::map<std::string,TScriptValue> & varValMap) {
    for(std::map<std::string,TScriptValue>::iterator iter = varValMap.begin();iter != varValMap.end();iter ++) {
        valueMap[iter->first] = iter->second;
    }
}
void TScriptGlobalEngine::clearVar() {
    valueMap.clear();
}
void TScriptGlobalEngine::setBaseDir(const std::string & baseDir) {
    this->baseDir = TScriptFile::getAbsolutePath(baseDir);
}
TScriptValue TScriptGlobalEngine::evalScript(const std::string & script, bool isScriptFile) {
    scriptProject = std::shared_ptr<TScriptProject>(new TScriptProject());
    if(baseDir != "") {
        scriptProject->addLibPath(baseDir + "/lib");
    }

    TScriptModule * scriptModule = scriptProject->loadMainModule(script,isScriptFile);

    std::shared_ptr<TScriptModuleEngine> scriptModuleEngine(new TScriptMainModuleEngine(this,scriptModule));
    scriptModuleEngineMap[scriptModule] = scriptModuleEngine;
    TScriptValue ret = scriptModuleEngine->evalScript();
    scriptModuleEngineMap[scriptModule] = NULL;
    scriptModuleEngineMap.clear();
    return ret;
}

TScriptValue TScriptGlobalEngine::getVarVal(const std::string & name) {
    std::map<std::string,TScriptValue>::iterator iter = valueMap.find(name);
    if(iter != valueMap.end()) {
        return iter->second;
    }
    if(getVarFunc == nullptr) {
        throw TScriptException(name + " is undefined");
    }
    TScriptValue v = getVarFunc(name);
    valueMap[name] = v;
    return v;
}
bool TScriptGlobalEngine::setVarVal(const std::string & name, const TScriptValue & value) {
    valueMap[name] = value;
    if(setVarFunc != nullptr) {
        setVarFunc(name,value);
    }
    return true;
}
TScriptModuleEngine * TScriptGlobalEngine::getScriptModuleEngine(TScriptModule * scriptModule) {
    if(scriptModule == NULL) {
        return nullptr;
    }

    std::map<TScriptModule *, std::shared_ptr<TScriptModuleEngine> >::iterator iter = scriptModuleEngineMap.find(scriptModule);
    if(iter != scriptModuleEngineMap.end()) {
        return iter->second.get();
    }

    std::shared_ptr<TScriptModuleEngine> scriptModuleEngine = std::shared_ptr<TScriptModuleEngine>(new TScriptModuleEngine(this, scriptModule));
    scriptModuleEngineMap[scriptModule] = scriptModuleEngine;
    scriptModuleEngine->evalScript();
    return scriptModuleEngine.get();
}

/******************************************* TScriptGlobalEngine End *********************************************/


/******************************************* TScriptProject Begin *************************************************/
TScriptProject::TScriptProject(){
    mainScriptModule = NULL;
}
TScriptProject::~TScriptProject(){
    mainScriptModule = NULL;
    for(std::map< std::string,TScriptModule* >::iterator iter = moduleMap.begin();iter != moduleMap.end();iter ++) {
        delete iter->second;
    }
    moduleMap.clear();
}

void TScriptProject::addLibPath(const std::string & path) {
    if(path.length() == 0) {
        return;
    }
    std::string absPath = TScriptFile::getAbsolutePath(path);
    libPathList.push_back(path);
}

TScriptModule * TScriptProject::loadMainModule(const std::string & script, bool isScriptFile) {
    if(isScriptFile) {
        std::string absFile = TScriptFile::getAbsoluteFile(script);
        mainScriptModule = createModule(absFile);
    } else {
        mainScriptModule = new TScriptModule(this,"");
        moduleMap[""] = mainScriptModule;
        mainScriptModule->loadScript(script);
    }
    return mainScriptModule;
}

TScriptModule * TScriptProject::getMainModule() {
    return mainScriptModule;
}
TScriptModule * TScriptProject::getModule(const std::string & baseDir, const std::string & importFileName) {
    if(importFileName.at(0) == '/') {
        for(int i=0;i<libPathList.size();i++) {
            std::string absFile = TScriptFile::getAbsoluteFile(libPathList[i] + importFileName);
            if(TScriptFile::exists(absFile)) {
                if(moduleMap[absFile] != NULL) {
                    return moduleMap[absFile];
                }
                return createModule(absFile);
            }
        }
    } else if(baseDir != "" ) {
        std::string absFile = TScriptFile::getAbsoluteFile(baseDir + importFileName);
        if(TScriptFile::exists(absFile)) {
            if(moduleMap[absFile] != NULL) {
                return moduleMap[absFile];
            }
            return createModule(absFile);
        }
    }
    throw TScriptException(std::string("import invalid file: " + importFileName));
}

TScriptModule * TScriptProject::createModule(const std::string & file) {
    TScriptModule * scriptModule = new TScriptModule(this,file);
    moduleMap[file] = scriptModule;
    std::string script = TScriptFile::readText(file);
	scriptModule->loadScript(script);
    return scriptModule;
}
/******************************************* TScriptProject End ***************************************************/


/******************************************* TScriptModule Begin **************************************************/
void TScriptModule::loadScript(const std::string & script) {
    std::vector<LineNoString<std::string> > tokenList;
    TScriptHelper::getInstance()->expressionToTokenList(TScriptHelper::getInstance()->removeComments(script), tokenList);
    std::vector<TScriptStatmemtItem> scriptStatmemtItemList;
    TScriptHelper::getInstance()->expressionTokenToStatmentItemList(tokenList,scriptStatmemtItemList);
    TScriptTokenLocations scriptTokenLocations;
    scriptStatement = std::shared_ptr<TScriptStatement>(new TScriptStatement(this, nullptr,scriptTokenLocations,scriptStatmemtItemList));
}
TScriptModule * TScriptModule::getModule(const std::string & file) {
    return scriptProject->getModule(baseDir, file);
}

TScriptClass * TScriptTokenLocation::getScriptClass() {
    if(tokenType == 2 && scriptClass == NULL) {
        scriptClass = scriptStatement->findScriptClass(name);
    }
    return scriptClass;
}
TScriptFunction * TScriptTokenLocation::getScriptFunction() {
    if(tokenType == 1 && scriptFunction == NULL) {
        scriptFunction = scriptStatement->findScriptFunction(name);
    }
    return scriptFunction;
}
/******************************************* TScriptModule End ****************************************************/


/******************************************* TScriptStatement Begin **********************************************/
TScriptStatement::TScriptStatement(TScriptModule* scriptModule, TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, TScriptStatmemtItem & scriptStatmemtItem)
    :scriptModule(scriptModule), ownerScriptStatement(ownerScriptStatement),ownerScriptClass(NULL),statmentType(TScriptStatement::STATMENT_TYPE::SIMPLE) {
    init(scriptTokenLocations,scriptStatmemtItem);
}
TScriptStatement::TScriptStatement(TScriptModule* scriptModule, TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, std::vector<TScriptStatmemtItem> & scriptStatmemtItemList, TScriptClass * ownerScriptClass, const std::vector<std::string> & paramNameList)
    :scriptModule(scriptModule), ownerScriptStatement(ownerScriptStatement),ownerScriptClass(ownerScriptClass),statmentType(TScriptStatement::STATMENT_TYPE::SIMPLE) {
    valueNameList = paramNameList;
    for(int i=0;i<paramNameList.size();i++) {
        TScriptTokenLocation scriptTokenLocation(0,paramNameList[i]);
        scriptTokenLocation.scriptStatement = this;
        scriptTokenLocations.valueLocationMap[paramNameList[i]] = scriptTokenLocation;
    }
    init(scriptTokenLocations,scriptStatmemtItemList,ownerScriptClass);
}
TScriptStatement::TScriptStatement(TScriptModule* scriptModule, TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, std::vector<LineNoString<std::string> > & expressionTokenList)
    :scriptModule(scriptModule),ownerScriptStatement(ownerScriptStatement),ownerScriptClass(NULL),statmentType(TScriptStatement::STATMENT_TYPE::SIMPLE) {

    std::vector<TScriptStatmemtItem> scriptStatmemtItemList;
    TScriptHelper::getInstance()->expressionTokenToStatmentItemList(expressionTokenList, scriptStatmemtItemList);
    if(scriptStatmemtItemList.size() == 0) {
        statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
    } else if(scriptStatmemtItemList.size() == 1) {
        init(scriptTokenLocations,scriptStatmemtItemList[0]);
    } else {
        init(scriptTokenLocations,scriptStatmemtItemList,ownerScriptClass);
    }
}

TScriptStatement::~TScriptStatement() {
    for(TScriptStatement * statment: scriptStatementList) {
        delete statment;
    }
    scriptStatementList.clear();
}


TScriptClass * TScriptStatement::findScriptClass(const std::string & name) {
    std::map< std::string,std::shared_ptr<TScriptClass> >::iterator iter = scriptClassMap.find(name);
    if(iter != scriptClassMap.end()) {
        return iter->second.get();
    }
    if(ownerScriptStatement != nullptr) {
        return ownerScriptStatement->findScriptClass(name);
    }
    return nullptr;
}
TScriptFunction * TScriptStatement::findScriptFunction(const std::string & name) {
    std::map< std::string,std::shared_ptr<TScriptFunction> >::iterator iter = scriptFunctionMap.find(name);
    if(iter != scriptFunctionMap.end()) {
        return iter->second.get();
    }
    if(ownerScriptStatement != nullptr) {
        return ownerScriptStatement->findScriptFunction(name);
    }
    return nullptr;
}
void TScriptStatement::bindScriptFunction(const std::string & name, std::shared_ptr<TScriptFunction> scriptFunction) {
    scriptFunctionMap[name] = scriptFunction;
}
void TScriptStatement::bindScriptClass(const std::string & name, std::shared_ptr<TScriptClass> scriptClass) {
    scriptClassMap[name] = scriptClass;
}

void TScriptStatement::init(TScriptTokenLocations & scriptTokenLocations, std::vector<TScriptStatmemtItem> & scriptStatmemtItemList, TScriptClass * ownerScriptClass) {
    if(scriptStatmemtItemList.size() == 0) {
        statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
        return;
    } else {
        statmentType = TScriptStatement::STATMENT_TYPE::MULTIPLE;
        std::string name;

        for(TScriptStatmemtItem & scriptStatmemtItem: scriptStatmemtItemList) {
            std::string token = scriptStatmemtItem.getToken();
            if(token == "function") {
                name = scriptStatmemtItem[0][0];
                TScriptTokenLocation scriptTokenLocation(1,name);
                scriptTokenLocation.ownerScriptClass = ownerScriptClass;
                scriptTokenLocation.scriptStatement = this;
                scriptTokenLocations.funcLocationMap[name] = scriptTokenLocation;
            }
            if(token == "class") {
                name = scriptStatmemtItem[0][0];
                TScriptTokenLocation scriptTokenLocation(2,name);
                scriptTokenLocation.scriptStatement = this;
                scriptTokenLocations.classLocationMap[name] = scriptTokenLocation;
            }
        }

        for(TScriptStatmemtItem & scriptStatmemtItem: scriptStatmemtItemList) {
            scriptStatementList.push_back(new TScriptStatement(this->scriptModule, this,scriptTokenLocations, scriptStatmemtItem));
        }
    }
}

void TScriptStatement::init(TScriptTokenLocations & scriptTokenLocations, TScriptStatmemtItem & scriptStatmemtItem) {
    LineNoString<std::string> & token = scriptStatmemtItem.getToken();
    if(scriptStatmemtItem.count() == 0) {
        statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
        return;
    }
    if(token == "") {
        statmentType = TScriptStatement::STATMENT_TYPE::SIMPLE;
        std::vector<LineNoString<std::string> > & tokenList = scriptStatmemtItem[0];
        if(tokenList.size() == 0) {
            statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
        } else {
            if(tokenList[0] == "continue") {
                if(tokenList.size() > 1) {
                    throw TScriptException(std::string("Line " + std::to_string(getLineNo()) + ":" + u8"continue share no parameters"));
                }
                statmentType = TScriptStatement::STATMENT_TYPE::CONTINUE;
                tokenList.clear();
            } else if(tokenList[0] == "break") {
                if(tokenList.size() > 1) {
                    throw TScriptException(std::string("Line " + std::to_string(getLineNo()) + ":" + u8"break share no parameters"));
                }
                statmentType = TScriptStatement::STATMENT_TYPE::BREAK;
                tokenList.clear();
            } else if(tokenList[0] == "return") {
                TCollectHelper::pop_front(tokenList);
                if(tokenList.size() == 0) {
                    statmentType = TScriptStatement::STATMENT_TYPE::RETURNNONE;
                } else {
                    statmentType = TScriptStatement::STATMENT_TYPE::RETURNVALUE;
                }
            } else if(tokenList[0] == "let") {
                if(tokenList.size() == 1) {
                    throw TScriptException(std::string("Line " + std::to_string(getLineNo()) + ":" + u8"let shall be followed with one variable name"));
                }
                if(!TScriptHelper::getInstance()->isToken(tokenList[1])) {
                    throw TScriptException(std::string("Line " + std::to_string(getLineNo()) + ":" + u8"variable naem " + tokenList[1].get() + u8" is illegal"));
                }

                initValueName = tokenList[1];
                if(TCollectHelper::contains(ownerScriptStatement->valueNameList,initValueName)) {
                    throw TScriptException(std::string("Line " + std::to_string(getLineNo()) + ":" + initValueName + " is declared duplicated"));
                }
                ownerScriptStatement->valueNameList.push_back(initValueName);
                if(tokenList.size() == 2) {
                    statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
                    tokenList.clear();
                } else if(tokenList[2].get() != "="){
                    throw TScriptException(std::string("Line " + std::to_string(getLineNo()) + ":" + u8"wrong ussage of let, no \"=\""));
                } else {
                    TCollectHelper::pop_front(tokenList);
                }
                TScriptTokenLocation scriptTokenLocation(0, initValueName);
                scriptTokenLocation.scriptStatement = ownerScriptStatement;
                scriptTokenLocations.valueLocationMap[initValueName] = scriptTokenLocation;
            }
        }
        if(tokenList.size() > 0) {
            scriptTreeNode = TScriptTreeNode::expressionTokenList2TScriptTreeNode(tokenList);

            relocationSymbol(scriptTokenLocations,scriptTreeNode);
        }
    } else if(token == "{}" ) {
        TScriptTokenLocations newScriptTokenLocations = scriptTokenLocations;
        statmentType = TScriptStatement::STATMENT_TYPE::MULTIPLE;
        int statmentCount = scriptStatmemtItem.count();
        for(int i=0;i<statmentCount;i++) {
            scriptStatementList.push_back(new TScriptStatement(scriptModule,this,newScriptTokenLocations,scriptStatmemtItem[i]));
        }
    } else if(token == "for" ) {
        TScriptTokenLocations newScriptTokenLocations = scriptTokenLocations;
        statmentType = TScriptStatement::STATMENT_TYPE::FOR;
        int statmentCount = scriptStatmemtItem.count();
        for(int i=0;i<statmentCount;i++) {
            scriptStatementList.push_back(new TScriptStatement(scriptModule,this,newScriptTokenLocations,scriptStatmemtItem[i]));
        }
    } else if(token == "if" ){
        TScriptTokenLocations newScriptTokenLocations = scriptTokenLocations;
        statmentType = TScriptStatement::STATMENT_TYPE::IF;
        int statmentCount = scriptStatmemtItem.count();
        for(int i=0;i<statmentCount;i++) {
            scriptStatementList.push_back(new TScriptStatement(scriptModule,this,newScriptTokenLocations,scriptStatmemtItem[i]));
        }
    } else if(token == "while" ){
        TScriptTokenLocations newScriptTokenLocations = scriptTokenLocations;
        statmentType = TScriptStatement::STATMENT_TYPE::WHILE;
        int statmentCount = scriptStatmemtItem.count();
        for(int i=0;i<statmentCount;i++) {
            scriptStatementList.push_back(new TScriptStatement(scriptModule,this,newScriptTokenLocations,scriptStatmemtItem[i]));
        }
    } else if(token == "do" ){
        TScriptTokenLocations newScriptTokenLocations = scriptTokenLocations;
        statmentType = TScriptStatement::STATMENT_TYPE::DOWHILE;
        int statmentCount = scriptStatmemtItem.count();
        for(int i=0;i<statmentCount;i++) {
            scriptStatementList.push_back(new TScriptStatement(scriptModule,this,newScriptTokenLocations,scriptStatmemtItem[i]));
        }
    } else if(token == "function") {
        TScriptTokenLocations newScriptTokenLocations = scriptTokenLocations;
        statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
        std::string funcName = scriptStatmemtItem[0][0];
        if(scriptStatmemtItem.count() == 3) {
            if(scriptStatmemtItem[0][0].get().at(0) == '~') {
                if(ownerScriptStatement->ownerScriptClass == NULL) {
                    throw TScriptException(std::string("Line " + std::to_string(getLineNo()) + ":" + u8"function name " + scriptStatmemtItem[0][0].get() + " is illegal"));
                }
            }
            std::shared_ptr<TScriptFunction> scriptFunction(new TScriptFunction(ownerScriptStatement, newScriptTokenLocations, scriptStatmemtItem[0][0], scriptStatmemtItem[1], scriptStatmemtItem[2]));
            scriptFunction->ownerScriptClass = ownerScriptStatement->ownerScriptClass;
            ownerScriptStatement->scriptFunctionMap[funcName] = scriptFunction;
        } else if(scriptStatmemtItem.count() == 4) {
            std::shared_ptr<TScriptFunction> scriptFunction(new TScriptFunction(ownerScriptStatement, newScriptTokenLocations, scriptStatmemtItem[0][0], scriptStatmemtItem[1], scriptStatmemtItem[3]));
            ownerScriptStatement->scriptFunctionMap[funcName] = scriptFunction;

            scriptFunction->parentScriptTreeNode = TScriptTreeNode::expressionTokenList2TScriptTreeNode(scriptStatmemtItem[2]);
            relocationSymbol(newScriptTokenLocations,scriptFunction->parentScriptTreeNode);
        }
    } else if(token == "class") {
        statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
        LineNoString<std::string> className = scriptStatmemtItem[0][0];
        std::string parentClassName;
        if(scriptStatmemtItem[1].size() == 1) {
            parentClassName = scriptStatmemtItem[1][0];
        }
        std::vector<LineNoString<std::string> > classExpressionList = scriptStatmemtItem[2];
        std::shared_ptr<TScriptClass> scriptClass(new TScriptClass(ownerScriptStatement, scriptTokenLocations, className, parentClassName, classExpressionList));
        ownerScriptStatement->scriptClassMap[className] = scriptClass;
    } else if(token == "import") {
        statmentType = TScriptStatement::STATMENT_TYPE::EMPTY;
        std::string moduleFile = TScriptHelper::getInstance()->removeQuoteClose(TScriptHelper::getInstance()->strHandwrite2Val(scriptStatmemtItem[0][0]));
        TScriptModule * importScriptModule = scriptModule->getModule(moduleFile);
        if(importScriptModule == NULL) {
            throw TScriptException("Line " + std::to_string(getLineNo()) + ":" + u8"import file " + moduleFile + u8" is not existed");
        }
        TScriptStatement * moduleStatement = importScriptModule->getScriptStatement();
        if(moduleStatement != NULL) {
            if(moduleStatement->statmentType == TScriptStatement::STATMENT_TYPE::MULTIPLE) {
                if(moduleStatement->valueNameList.size() > 0) {
                    for(int i=0;i<moduleStatement->valueNameList.size();i++) {
                        TScriptTokenLocation scriptTokenLocation(0,moduleStatement->valueNameList[i]);
                        scriptTokenLocation.scriptModule = importScriptModule;
                        scriptTokenLocation.scriptStatement = importScriptModule->getScriptStatement();
                        scriptTokenLocations.valueLocationMap[moduleStatement->valueNameList[i]] = scriptTokenLocation;
                    }
                }
                if(moduleStatement->scriptFunctionMap.size() > 0) {
                    for(std::map<std::string,std::shared_ptr<TScriptFunction> >::iterator iter = moduleStatement->scriptFunctionMap.begin();iter != moduleStatement->scriptFunctionMap.end(); iter ++) {
                        TScriptTokenLocation scriptTokenLocation(1,iter->first);
                        scriptTokenLocation.scriptModule = importScriptModule;
                        scriptTokenLocation.scriptStatement = importScriptModule->getScriptStatement();
//                        scriptTokenLocation.scriptFunction = iter->second.get();
                        scriptTokenLocations.funcLocationMap[iter->first] = scriptTokenLocation;
                    }
                }
                if(moduleStatement->scriptClassMap.size() > 0) {
                    for(std::map<std::string,std::shared_ptr<TScriptClass> >::iterator iter = moduleStatement->scriptClassMap.begin();iter != moduleStatement->scriptClassMap.end(); iter ++) {
                        TScriptTokenLocation scriptTokenLocation(2,iter->first);
                        scriptTokenLocation.scriptModule = importScriptModule;
                        scriptTokenLocation.scriptStatement = importScriptModule->getScriptStatement();
//                        scriptTokenLocation.scriptClass = iter->second.get();
                        scriptTokenLocations.classLocationMap[iter->first] = scriptTokenLocation;
                    }
                }
            }
        }
    }
}

TScriptStatementEngine * TScriptStatementEngine::getScriptStatementEngine(TScriptStatement * scriptStatement) {
    if(scriptStatement == this->scriptStatement) {
        return this;
    }
    TScriptStatementEngine * ownerEngine = getOwnerStatementEngine();
    while(ownerEngine != NULL) {
        if(ownerEngine->scriptStatement == scriptStatement) {
            return ownerEngine;
        }
        ownerEngine = ownerEngine->getOwnerStatementEngine();
	}
    return NULL;
}

void TScriptStatementEngine::declareVarName(const std::string & name) {
    std::map<std::string,TScriptValue>::iterator iter = valueMap.find(name);
    if(iter != valueMap.end()) {
        throw TScriptException("Line " + std::to_string(scriptStatement->getLineNo()) + ":" + u8" variable " + name + u8" is declared duplicated");
    }
    valueMap[name] = TScriptValue::V_INVALID;
}

TScriptValue TScriptStatementEngine::getVarVal(const std::string & name) {
    std::map<std::string,TScriptValue>::iterator iter = valueMap.find(name);
    if(iter != valueMap.end()) {
        return iter->second;
    }
    if(getOwnerStatementEngine() != nullptr) {
        return getOwnerStatementEngine()->getVarVal(name);
    }
    return getModule()->getVarVal(name);
}
bool TScriptStatementEngine::setVarVal(const std::string & name, const TScriptValue & value) {
    std::map<std::string,TScriptValue>::iterator iter = valueMap.find(name);
    if(iter != valueMap.end()) {
        iter->second = value;
        return true;
    }
    if(getOwnerStatementEngine() != nullptr) {
        return getOwnerStatementEngine()->setVarVal(name, value);
    }
    return getModule()->setVarVal(name, value);
}
TScriptValue TScriptStatementEngine::applyUserFunc(int calllineNo, const std::string & name, std::vector<TScriptValue> & paramList) {
    return getGlobal()->applyUserFunc(calllineNo, name, paramList);
}

TScriptValue TScriptStatementEngine::evalStatementContinue(TScriptStatement * scriptStatement) {
    TScriptStatement * statement;
    TScriptStatementEngine * scriptStatementEngine = this;
    while(scriptStatementEngine != nullptr && (statement = scriptStatementEngine->getScriptStatement()) != nullptr) {
        scriptStatementEngine->ctrlFlag = TScriptStatement::STATMENT_CTRL_CONTINUE;
        if(statement->statmentType == TScriptStatement::STATMENT_TYPE::FOR || statement->statmentType == TScriptStatement::STATMENT_TYPE::WHILE || statement->statmentType == TScriptStatement::STATMENT_TYPE::DOWHILE) {
            break;
        }
        scriptStatementEngine = scriptStatementEngine->getOwnerStatementEngine();
    }
    if(scriptStatementEngine == nullptr) {
        throw TScriptException("Line " + std::to_string(scriptStatement->getLineNo()) + ":" + u8" uncontrolled continue");
    }
    return TScriptValue::V_INVALID;
}
TScriptValue TScriptStatementEngine::evalStatementBreak(TScriptStatement * scriptStatement) {
    TScriptStatement * statement;
    TScriptStatementEngine * scriptStatementEngine = this;
    while(scriptStatementEngine != nullptr && (statement = scriptStatementEngine->getScriptStatement()) != nullptr) {
        scriptStatementEngine->ctrlFlag = TScriptStatement::STATMENT_CTRL_BREAK;
        if(statement->statmentType == TScriptStatement::STATMENT_TYPE::FOR || statement->statmentType == TScriptStatement::STATMENT_TYPE::WHILE || statement->statmentType == TScriptStatement::STATMENT_TYPE::DOWHILE) {
            break;
        }
        scriptStatementEngine = scriptStatementEngine->getOwnerStatementEngine();
    }
    if(scriptStatementEngine == nullptr) {
        throw TScriptException("Line " + std::to_string(scriptStatement->getLineNo()) + ":" + u8" uncontrolled break");
    }
    return TScriptValue::V_INVALID;
}

void TScriptStatementEngine::evalStatementSetReturnMark(TScriptStatement * scriptStatement)
{
    TScriptStatementEngine * scriptStatementEngine = this;
    while(scriptStatementEngine != nullptr) {
        if(scriptStatementEngine->getScriptStatement() != nullptr) {
            scriptStatementEngine->ctrlFlag = TScriptStatement::STATMENT_CTRL_RETURN;
        }
        if(scriptStatementEngine->isReturnStop()) {
            break;
        }
        scriptStatementEngine = scriptStatementEngine->getOwnerStatementEngine();
    }
    if(scriptStatementEngine == nullptr) {
        TScriptHelper::throwException(u8"Line " + std::to_string(scriptStatement->getLineNo()) + u8": wrong useage of return");
    }
}
TScriptValue TScriptStatementEngine::evalStatementReturn(TScriptStatement * scriptStatement) {
    evalStatementSetReturnMark(scriptStatement);
    return TScriptValue::V_INVALID;
}
TScriptValue TScriptStatementEngine::evalStatementReturnValue(TScriptStatement * scriptStatement) {
    evalStatementSetReturnMark(scriptStatement);
    return eval(scriptStatement->scriptTreeNode);
}
TScriptValue TScriptStatementEngine::evalStatementComplex(TScriptStatement * scriptStatement) {
    std::auto_ptr<TScriptStatementEngine> scriptStatementEngine(new TScriptStatementEngine(scriptModuleEngine, this,scriptStatement));
    return scriptStatementEngine->evalStatement();
}
TScriptValue TScriptStatementEngine::evalStatement(TScriptStatement * scriptStatement) {
    if(!scriptStatement->initValueName.empty()) {
        declareVarName(scriptStatement->initValueName);
    }
    if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::EMPTY) {
        return TScriptValue::V_INVALID;
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::SIMPLE) {
        return eval(scriptStatement->scriptTreeNode);
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::CONTINUE) {
        return evalStatementContinue(scriptStatement);
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::BREAK) {
        return evalStatementBreak(scriptStatement);
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::RETURNNONE) {
        return evalStatementReturn(scriptStatement);
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::RETURNVALUE) {
        return evalStatementReturnValue(scriptStatement);
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::MULTIPLE
              || scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::IF
              || scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::WHILE
              || scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::DOWHILE
              || scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::FOR
              ) {
        return evalStatementComplex(scriptStatement);
    }
    return TScriptHelper::throwException(u8"Line " + std::to_string(scriptStatement->getLineNo()) + ": unknown statement type");
}

TScriptValue TScriptStatementEngine::evalStatementMultiple() {
    TScriptValue lastResult;
    for(TScriptStatement * statment: scriptStatement->scriptStatementList) {
        lastResult = evalStatement(statment);
        if(ctrlFlag != TScriptStatement::STATMENT_CTRL_NONE) {
            return lastResult;
        }
    }
    return lastResult;
}
TScriptValue TScriptStatementEngine::evalStatementFor() {
    TScriptValue lastResult;
    evalStatement(scriptStatement->scriptStatementList[0]);
    while(scriptStatement->scriptStatementList[1]->statmentType == TScriptStatement::STATMENT_TYPE::EMPTY || (lastResult = evalStatement(scriptStatement->scriptStatementList[1])).toBool()) {
        if(scriptStatement->scriptStatementList[3]->statmentType != TScriptStatement::STATMENT_TYPE::EMPTY) {
            lastResult = evalStatement(scriptStatement->scriptStatementList[3]);
            if(ctrlFlag == TScriptStatement::STATMENT_CTRL_RETURN || ctrlFlag == TScriptStatement::STATMENT_CTRL_BREAK) {
                return lastResult;
            }
        }
        evalStatement(scriptStatement->scriptStatementList[2]);
    }
    return lastResult;

}
TScriptValue TScriptStatementEngine::evalStatementIf() {
    TScriptValue lastResult;
    int statementCount = scriptStatement->scriptStatementList.size();
    int pos = 0;
    while(pos < (statementCount - 1)) {
        lastResult = evalStatement(scriptStatement->scriptStatementList[pos]);
        if(lastResult.toBool()) {
            return evalStatement(scriptStatement->scriptStatementList[pos+1]);
        } else {
            if((pos + 2) == (statementCount - 1)) {
                return evalStatement(scriptStatement->scriptStatementList[pos+2]);
            } else {
                pos += 2;
            }
        }
    }
    return lastResult;
}
TScriptValue TScriptStatementEngine::evalStatementWhile() {
    TScriptValue lastResult;
    while((lastResult = evalStatement(scriptStatement->scriptStatementList[0])).toBool()) {
        if(scriptStatement->scriptStatementList[1]->statmentType != TScriptStatement::STATMENT_TYPE::EMPTY) {
            lastResult = evalStatement(scriptStatement->scriptStatementList[1]);
            if(ctrlFlag == TScriptStatement::STATMENT_CTRL_RETURN || ctrlFlag == TScriptStatement::STATMENT_CTRL_BREAK) {
                return lastResult;
            }
        }
    }
    return lastResult;

}
TScriptValue TScriptStatementEngine::evalStatementDoWhile() {
    TScriptValue lastResult;
    do {
        if(scriptStatement->scriptStatementList[0]->statmentType != TScriptStatement::STATMENT_TYPE::EMPTY) {
            lastResult = evalStatement(scriptStatement->scriptStatementList[0]);
            if(ctrlFlag == TScriptStatement::STATMENT_CTRL_RETURN || ctrlFlag == TScriptStatement::STATMENT_CTRL_BREAK) {
                return lastResult;
            }
        }
    }while((lastResult = evalStatement(scriptStatement->scriptStatementList[1])).toBool());
    return lastResult;

}

TScriptValue TScriptStatementEngine::evalStatement() {
    if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::MULTIPLE) {
        return evalStatementMultiple();
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::IF) {
        return evalStatementIf();
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::WHILE) {
        return evalStatementWhile();
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::DOWHILE) {
        return evalStatementDoWhile();
    } else if(scriptStatement->statmentType == TScriptStatement::STATMENT_TYPE::FOR) {
        return evalStatementFor();
    } else {
        return evalStatement(scriptStatement);
    }
}

TScriptGlobalEngine * TScriptStatementEngine::getGlobal() {
    return scriptModuleEngine->getGlobal();
}
/******************************************* TScriptStatement End ************************************************/


/******************************************* TScriptFunctionEngine Begin *****************************************/
TScriptValue TScriptFunctionEngine::applyScriptFunction(TScriptStatementEngine * ownerScriptStatementEngine, TScriptFunction & scriptFunction, std::vector<TScriptValue> & paramList) {
    std::auto_ptr<TScriptFunctionEngine> scriptFunctionEngine(new TScriptFunctionEngine(ownerScriptStatementEngine, &scriptFunction,paramList));
    return scriptFunctionEngine->eval();
}
/******************************************* TScriptFunctionEngine End *******************************************/


/******************************************* TScriptClassEngine Begin ********************************************/
TScriptClassEngine::TScriptClassEngine(TScriptStatementEngine * ownerScriptStatementEngine, TScriptClass * scriptClass)
    :ownerScriptStatementEngine(ownerScriptStatementEngine),scriptClass(scriptClass),vObj(NULL)
{
}
TScriptClassEngine::~TScriptClassEngine()  {
}

void TScriptClassEngine::setObject(TScriptClassObject * scriptObject) {
    vObj = scriptObject;
}
TScriptClassObject * TScriptClassEngine::getObject() {
    return vObj;
}
bool TScriptClassEngine::hasVar(const std::string & name) {
    return TCollectHelper::contains(scriptClass->scriptStatement->valueNameList,name);
}
TScriptFunction * TScriptClassEngine::getInternalFunction(const std::string & name) {
    return scriptClass->scriptStatement->findScriptFunction(name);
}
void TScriptClassEngine::applyInitFunc(std::vector<TScriptValue> & paramList) {
    TScriptFunction * scriptFunction = scriptStatementEngine->getScriptStatement()->findScriptFunction(scriptClass->className);
    if(scriptFunction == NULL) {
        return;
    }
    TScriptFunctionEngine scriptFunctionEngine(scriptStatementEngine.get(), scriptFunction,paramList);

    if(vObj->getSuperObject() != NULL) {
        std::vector<TScriptValue> parentParamList;
        if(scriptFunction->parentScriptTreeNode.isValid()) {
            for(int i=0;i<scriptFunction->parentScriptTreeNode.getParamCount();i++) {
                parentParamList.push_back(scriptFunctionEngine.mainScriptStatementEngine->eval(scriptFunction->parentScriptTreeNode[i]));
            }
        }
        vObj->getSuperObject()->getEngine()->applyInitFunc(parentParamList);
    }
    scriptFunctionEngine.eval();
}
std::shared_ptr<TScriptObject>  TScriptClassEngine::create(TScriptStatementEngine * ownerScriptStatementEngine,TScriptClass * scriptClass, std::vector<TScriptValue> & paramList, bool isInstanceObject)
{
    std::vector<TScriptValue> defaultValueList;
    std::shared_ptr<TScriptObject> parentObject;

    TScriptClass * parentScriptClass = scriptClass->parentScriptClassLocation.getScriptClass();
    if(parentScriptClass != NULL) {
        if(parentScriptClass->ownerScriptStatement->scriptModule == scriptClass->ownerScriptStatement->scriptModule) {
            TScriptStatementEngine * parentStatementEngine = ownerScriptStatementEngine;
            while(parentStatementEngine != NULL) {
                if(parentStatementEngine->getScriptStatement() == parentScriptClass->ownerScriptStatement) {
                    parentObject = create(parentStatementEngine, parentScriptClass, defaultValueList, false);
                    break;
                }
                parentStatementEngine = parentStatementEngine->ownerScriptStatementEngine;
            }
            if(parentStatementEngine == NULL) {
                throw TScriptException(std::string(u8"Super class " + std::string(parentScriptClass->className) + u8" is undefined"));
            }
        } else {
            TScriptModuleEngine * scriptModuleEngine = ownerScriptStatementEngine->getGlobal()->getScriptModuleEngine(parentScriptClass->ownerScriptStatement->scriptModule);
            parentObject = create(scriptModuleEngine->getStatementEngine(), parentScriptClass, defaultValueList, false);
        }
    }

    std::shared_ptr<TScriptClassEngine> scriptClassEngine(new TScriptClassEngine(ownerScriptStatementEngine, scriptClass));
    std::shared_ptr<TScriptClassObject> scriptObject(new TScriptClassObject());
    scriptObject->init(scriptClass->className, parentObject);
    scriptObject->setEngine(scriptClassEngine);
    scriptClassEngine->setObject(scriptObject.get());

    scriptClassEngine->scriptStatementEngine = std::shared_ptr<TScriptStatementEngine>(new TScriptStatementEngine(ownerScriptStatementEngine->scriptModuleEngine, ownerScriptStatementEngine, scriptClass->scriptStatement.get()));
    scriptClassEngine->scriptStatementEngine->ownerScriptClassEngine = scriptClassEngine.get();
    scriptClassEngine->scriptStatementEngine->setReturnStop(true);
    scriptClassEngine->scriptStatementEngine->evalStatement();

    if(isInstanceObject) {
        scriptClassEngine->applyInitFunc(paramList);
    }

    return scriptObject;
}

/******************************************* TScriptClassEngine End **********************************************/

/******************************************* TScriptClass Begin **************************************************/
TScriptClass::TScriptClass(TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, const LineNoString<std::string> & className, const std::string & parentClassName, std::vector<LineNoString<std::string> > & expressionTokenList)
    :ownerScriptStatement(ownerScriptStatement),className(className)
{
    TScriptTokenLocations newScriptTokenLocations = scriptTokenLocations;
    if(parentClassName != "") {
        if(scriptTokenLocations.classLocationMap.find(parentClassName) != scriptTokenLocations.classLocationMap.end()) {
            parentScriptClassLocation = scriptTokenLocations.classLocationMap[parentClassName];
        }
    }

    initSymbol(newScriptTokenLocations);
    init(newScriptTokenLocations, expressionTokenList);
}
void TScriptClass::initSymbol(TScriptTokenLocations & scriptTokenLocations) {
    TScriptClass * parentScriptClass = parentScriptClassLocation.getScriptClass();
    if(parentScriptClass != NULL) {
        parentScriptClass->initSymbol(scriptTokenLocations);
        for(std::map< std::string,std::shared_ptr<TScriptFunction> >::iterator iter = parentScriptClass->scriptStatement->scriptFunctionMap.begin();iter != parentScriptClass->scriptStatement->scriptFunctionMap.end();iter ++) {
            if(parentScriptClass->className != iter->first) {
                TScriptTokenLocation scriptTokenLocation(1,iter->first);
				scriptTokenLocation.scriptModule = parentScriptClassLocation.scriptModule;
                scriptTokenLocation.ownerScriptClass = parentScriptClass;
                scriptTokenLocation.scriptStatement = parentScriptClass->scriptStatement.get();
                scriptTokenLocations.funcLocationMap[iter->first] = scriptTokenLocation;
            }
        }
    }
}
void TScriptClass::init(TScriptTokenLocations & scriptTokenLocations, std::vector<LineNoString<std::string> > & expressionTokenList) {
    std::vector<TScriptStatmemtItem> scriptStatmemtItemList;
    TScriptHelper::getInstance()->expressionTokenToStatmentItemList(expressionTokenList,scriptStatmemtItemList);
    scriptStatement = std::shared_ptr<TScriptStatement>(new TScriptStatement(ownerScriptStatement->scriptModule, ownerScriptStatement,scriptTokenLocations,scriptStatmemtItemList, this));
}

/******************************************* TScriptClass End ****************************************************/

/******************************************* TScriptFunction Begin ***********************************************/

TScriptFunction::TScriptFunction(TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, const LineNoString<std::string> & name, std::vector<LineNoString<std::string> > & paramList, const std::vector<LineNoString<std::string> > & expressionList)
    :ownerScriptStatement(ownerScriptStatement),name(name)
{
    ownerScriptClass = ownerScriptStatement->ownerScriptClass;
    this->paramList = paramList;

    init(scriptTokenLocations, expressionList);
}

TScriptFunction::~TScriptFunction() {
}

const std::string & TScriptFunction::getName() {
    return name;
}

void TScriptFunction::init(TScriptTokenLocations & scriptTokenLocations, const std::vector<LineNoString<std::string> > & expressionList) {
    std::vector<TScriptStatmemtItem> scriptStatmemtItemList;
    TScriptHelper::getInstance()->expressionTokenToStatmentItemList(expressionList,scriptStatmemtItemList);
    std::vector<std::string> paramList;
    for(int i=0;i<this->paramList.size();i++) {
        paramList.push_back(this->paramList[i]);
    }
    scriptStatement = std::shared_ptr<TScriptStatement>(new TScriptStatement(ownerScriptStatement->scriptModule, ownerScriptStatement,scriptTokenLocations, scriptStatmemtItemList, nullptr, paramList));
}

TScriptFunctionEngine::TScriptFunctionEngine(TScriptStatementEngine * ownerScriptStatementEngine, TScriptFunction * scriptFunction, std::vector<TScriptValue> & valueList)
    :ownerScriptStatementEngine(ownerScriptStatementEngine),scriptFunction(scriptFunction) {
    mainScriptStatementEngine = std::shared_ptr<TScriptStatementEngine>(new TScriptStatementEngine(ownerScriptStatementEngine->scriptModuleEngine, ownerScriptStatementEngine, scriptFunction->scriptStatement.get()));
    mainScriptStatementEngine->setReturnStop(true);
    initParamValueList(valueList);
}

void TScriptFunctionEngine::initParamValueList(std::vector<TScriptValue> & valueList) {
    for(int i=0;i<scriptFunction->paramList.size();i++) {
        if(i < valueList.size()) {
            mainScriptStatementEngine->valueMap[scriptFunction->paramList[i]] = valueList[i];
        } else {
            mainScriptStatementEngine->valueMap[scriptFunction->paramList[i]] = TScriptValue();
        }
    }
}
TScriptValue TScriptFunctionEngine::eval() {
    return mainScriptStatementEngine->evalStatement();
}

/******************************************* TScriptFunction End *************************************************/



/******************************************* TScriptHelper Begin *************************************************/
std::string TScriptHelper::strHandwrite2Val(const std::string & s) {
    bool convertOpen = false;
    std::string r;
    for(int i=0;i<s.size();i++) {
        char c = s.at(i);
        if(convertOpen) {
            convertOpen = false;
            r.push_back(convertChar(c));
            continue;
        }
        if(c == '\\') {
            convertOpen = true;
            continue;
        }
        r.push_back(c);
    }
    return r;
}

std::string TScriptHelper::strVal2Handwrite(const std::string & s) {
    std::string r;
    for(int i=0;i<s.size();i++) {
        r.append(charConvertHandwriteStr(s.at(i)));
    }
    return r;
}
std::string TScriptHelper::removeQuoteClose(const std::string & s) {
    std::string ns = s;
    ns.erase(0,1);
    ns.pop_back();
    return ns;
}
std::string TScriptHelper::char2Handwrite(const char c)
{
    return charConvertHandwriteStr(c);
}
std::string TScriptHelper::removeComments(const std::string & expression)
{
    int commentStep = 0;
    bool quoteOpen = false;
    bool quote2Open = false;
    bool convertOpen = false;
    int LFCount;
    std::wstring wexpression = TStringHelper::toWString(expression);
    std::wstring r;
    for(int i=0;i<=wexpression.length();i++) {
        if(i == wexpression.length()) {
            if(quoteOpen || quote2Open) {
                throw TScriptException(u8"Unmatched char \' or \"");
            }
            if(commentStep > 0) {
                throw TScriptException(u8"Unmatched char /");
            }
            break;
        }
        wchar_t c = wexpression[i];
        if(commentStep > 0) {
            if(commentStep == 1) {
                LFCount = 0;
                if(c == '/') {
                    r.erase(r.length() - 1);
                    commentStep = 2;
                } else if(c == '*'){
                    r.erase(r.length() - 1);
                    commentStep = 3;
                } else {
                    r.push_back(c);
                    commentStep = 0;
                }
                continue;
            } else if(commentStep == 2) {
                if(c == '\r' || c == '\n') {
                    commentStep = 0;
                    r.push_back(c);
                    continue;
                }
                continue;
            } else if(commentStep == 3) {
                if(c == '\n') {
                    LFCount ++;
                }
                if(c == '/') {
                    if(wexpression[i - 1] == '*') {
                        commentStep = 0;
                        for(int i=0;i<LFCount;i++) {
                            r.push_back('\n');
                        }
                        continue;
                    }
                }
                continue;
            }
            continue;
        }
        r.push_back(c);
        if(quoteOpen) {
            if(convertOpen) {
                convertOpen = false;
                continue;
            }
            if(c == '\\') {
                convertOpen = true;
                continue;
            }
            if(c == '\'') {
                quoteOpen = false;
                continue;
            }
            continue;
        }
        if(quote2Open) {
            if(convertOpen) {
                convertOpen = false;
                continue;
            }
            if(c == '\\') {
                convertOpen = true;
                continue;
            }
            if(c == '\"') {
                quote2Open = false;
                continue;
            }
            continue;
        }
        if(c == '\'') {
            quoteOpen = true;
            continue;
        }
        if(c == '\"') {
            quote2Open = true;
            continue;
        }
        if(c == '/') {
            commentStep = 1;
            continue;
        }
    }
    return TStringHelper::fromWString(r);
}
void TScriptHelper::expressionToTokenList(const std::string & expressions, std::vector<LineNoString<std::string> > & resultList) {
    bool convertOpen = false;
    bool quoteOpen = false;
    bool quote2Open = false;
    bool multisignOpen = false;
    bool moduleSignOpen = false;
    std::wstring r;
    std::vector<LineNoString<std::wstring>> rwList;
    std::wstring wexpression = TStringHelper::toWString(expressions);
    //divide expression into tokens and save to rList
    int LFNoIndex = 1;
    for(int i=0;i<=wexpression.size();i++) {
        if(i == wexpression.size()) {
            TStringHelper::wtrim(r);
            if(r.size() > 0) {
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
            }
            r.clear();
            break;
        }
        wchar_t c = wexpression.at(i);
        if(c == '\n') {
            LFNoIndex ++;
        }
        if(convertOpen) {
            convertOpen = false;
            r.push_back(c);
            continue;
        }
        if(quoteOpen) {
            r.push_back(c);
            if(c == '\\') {
                convertOpen = true;
                continue;
            }
            if(c == '\'') {
                quoteOpen = false;

                TStringHelper::wtrim(r);
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
                r.clear();
            }
            continue;
        }
        if(quote2Open) {
            r.push_back(c);
            if(c == '\\') {
                convertOpen = true;
                continue;
            } else if(c == '\"') {
                quote2Open = false;
                TStringHelper::wtrim(r);
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
                r.clear();
            }
            continue;
        }
        if(multisignOpen) {
            if(TStringHelper::isChar(c) && strchr(multisignList,c) != nullptr) {
                r.push_back(c);
                continue;
            } else {
                std::wstring sign;
                for(;;) {
                    if(r.size() == 0) {
                        if(sign.size() > 0) {
                            rwList.push_back(LineNoString<std::wstring>(sign,LFNoIndex));
                        }
                        break;
                    }
                    sign.push_back(r.at(0));
                    if(TScriptHelper::getInstance()->isOp(TStringHelper::fromWString(sign))) {
                        r.erase(0,1);
                    } else {
                        if(r.size() >= 2) {
                            sign.push_back(r.at(1));
                             if(TScriptHelper::getInstance()->isOp(TStringHelper::fromWString(sign))) {
                                 r.erase(0,2);
                                 continue;
                             } else {
                                 sign.pop_back();
                             }
                        }
                        if(sign.size() == 1) {
                            r.erase(0,1);
                            rwList.push_back(LineNoString<std::wstring>(sign,LFNoIndex));
                            sign.clear();
                        } else {
                            sign.pop_back();
                            rwList.push_back(LineNoString<std::wstring>(sign,LFNoIndex));
                            sign.clear();
                        }
                    }
                }
                multisignOpen = false;
                //keep the c free to be handled by next
            }
        }
        if(moduleSignOpen) {
            if(c == ':') {
                r.pop_back();
                TStringHelper::wtrim(r);
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
                r.clear();
				r.push_back(':'); r.push_back(':');
				rwList.push_back(LineNoString<std::wstring>(r, LFNoIndex));
				r.clear();
				moduleSignOpen = false;
                continue;
            } else {
				TStringHelper::wtrim(r);
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
                r.clear();
				moduleSignOpen = false;
                //shall continue to handle the char
            }
        }
        if(c == ':') {
            if(r.length() > 0) {
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
                r.clear();
            }
            r.push_back(c);
            moduleSignOpen = true;
			continue;
        }
        if(c == '\'') {
            r.push_back(c);
            quoteOpen = true;
            continue;
        }
        if(c == '\"') {
            r.push_back(c);
            quote2Open = true;
            continue;
        }
        //check the float number
        if(c == '.') {
            if(r.length() > 0) {
                char p = r.at(0);
                if(p >= '0' && p <= '9') {
                    if(TStringHelper::wstrfind(r,'.') >= 0) {
                        throw TScriptException(u8"Line " + TStringHelper::number(LFNoIndex) + u8": float format is error");
                    }
                    r.push_back(c);
                    continue;
                }
            }
        }
        if(TStringHelper::isChar(c) && strchr(multisignList,c) != nullptr) {
            TStringHelper::wtrim(r);
            if(r.length() > 0) {
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
                r.clear();
            }
            r.push_back(c);
            multisignOpen = true;
            continue;
        }
        if(TStringHelper::isChar(c) && (c == ' ' || c == '\t' || c == '\r' || c == '\n' || strchr(signall,c) != nullptr)) {
            TStringHelper::wtrim(r);
            if(r.length() > 0) {
                rwList.push_back(LineNoString<std::wstring>(r,LFNoIndex));
                r.clear();
            }
            if(!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) {
                std::wstring s;s.push_back(c);
                rwList.push_back(LineNoString<std::wstring>(s,LFNoIndex));
            }
            continue;
        }
        r.push_back(c);
    }

    std::vector<LineNoString<std::string> > rList;
    for(std::vector<LineNoString<std::wstring> >::iterator iter = rwList.begin();iter != rwList.end();iter ++) {
        rList.push_back(LineNoString<std::string>(TStringHelper::fromWString(*iter),iter->getLineNo()));
    }

    int rListSize = rList.size();
    std::string closeStack;
    for(int i=0;i<=rListSize;i++) {
        if(i == rListSize) {
            if(closeStack.size() > 0) {
                throw TScriptException((u8"Line " + TStringHelper::number(rList.back().getLineNo()) + u8": unmatched char \"{" + closeStack + "\""));
            }
            break;
        }
        LineNoString<std::string> & token = rList[i];
        if(token == "(" || token == "[" || token == "{") {
            closeStack.push_back(token.get().at(0));
            continue;
        }
        if(token == ")" || token == "]" || token == "}") {
            if(closeStack.size() == 0) {
                throw TScriptException((u8"Line " + TStringHelper::number(rList[i].getLineNo()) + u8": unmatched token \"" + token.get() + "\""));
            }
            char lastChar = closeStack.at(closeStack.size() - 1);
            char shallChar = token.get().at(0);
            if(shallChar == ')') {
                shallChar = '(';
            } else if(shallChar == ']') {
                shallChar = '[';
            } else if(shallChar == '}') {
                shallChar = '{';
            }
            if(lastChar != shallChar) {
                throw TScriptException((u8"Line " + TStringHelper::number(rList[i].getLineNo()) + u8": unmatched char \"" + TStringHelper::fromChar(lastChar) + "\""));
            }
            closeStack.erase(closeStack.size() - 1);
            continue;
        }
    }
    resultList.swap(rList);
}

/**
 * convert Tokens to statements
 * @brief TScriptHelper::expressionTokenToStatmentItemList
 * @param expressionTokenList
 * @param resultList
 */
void TScriptHelper::expressionTokenToStatmentItemList(const std::vector<LineNoString<std::string> > & expressionTokenList, std::vector<TScriptStatmemtItem> & scriptStatmemtItemList)
{
    int tokenCount = expressionTokenList.size();
    int forStep = 0;
    int ifStep = 0;
    int whileStep = 0;
    int doStep = 0;
    int multiStep = 0;
    int functionStep = 0;
    int classStep = 0;
    int importStep = 0;

    int bracketCount = 0;
    int sbracketCount = 0;
    int braceCount = 0;

    int closeCount = 0;
    std::vector<LineNoString<std::string> > statmentTokenList;
    TScriptStatmemtItem scriptStatmemtItem;

    for(int i=0;i<=tokenCount;i++) {
        if(i == tokenCount) {
            if(statmentTokenList.size() > 0) {
                scriptStatmemtItem << statmentTokenList;
                statmentTokenList.clear();

                scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                scriptStatmemtItemList.back().swap(scriptStatmemtItem);
//                scriptStatmemtItemList.push_back( scriptStatmemtItem);
//                scriptStatmemtItem.clear();
            }
            break;
        }
        const LineNoString<std::string> & token = expressionTokenList[i];
        if(bracketCount > 0) {
            statmentTokenList.push_back(token);
            if(token == ")") {
                bracketCount --;
            }
            continue;
        }
        if(sbracketCount > 0) {
            statmentTokenList.push_back(token);
            if(token == "]") {
                sbracketCount --;
            }
            continue;
        }
        if(braceCount > 0) {
            statmentTokenList.push_back(token);
            if(token == "}") {
                braceCount --;
            }
            continue;
        }
        if(forStep > 0) {
            if(forStep == 1) {
                if(token != "(") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement for is wrong");
                }
                forStep = 2;
                continue;
            } else if(forStep == 2 || forStep == 3 || forStep == 4){
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement for is wrong");
                }
                if(token == ";") {
                    if(forStep == 2 || forStep == 3) {
                        statmentTokenList.pop_back();
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();
                        forStep ++;
                        continue;
                    }
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement for is wrong");
                }
                if(token == ")") {
                    if(forStep == 4) {
                        statmentTokenList.pop_back();
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();
                        forStep = 5;
                        continue;
                    }
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement for is wrong");
                }
                continue;
            } else if(forStep == 5) {
                if(token == ";") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();
                    forStep = 0;
                    continue;
                }
                statmentTokenList.push_back(token);
                if(token == "{") {
                    forStep = 25;
                    continue;
                }
                forStep = 15;
                continue;
            } else if(forStep == 15) {
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == ")" || token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement for is wrong");
                }
                if(token == ";") {
                    statmentTokenList.pop_back();
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();
                    forStep = 0;
                    continue;
                }
                continue;
            } else if(forStep == 25) {
                statmentTokenList.push_back(token);
                if(token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "}") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();
                    forStep = 0;
                    continue;
                }
            }
            continue;
        }
        if(ifStep > 0) {
            if(ifStep == 1) {
                if(token != "(") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement if is wrong");
                }
                ifStep = 2;
                continue;
            } else if(ifStep == 2) {
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement if is wrong");
                }
                if(token == ")") {
                    statmentTokenList.pop_back();
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
                    ifStep  = 3;
                    continue;
                }
                continue;
            } else if(ifStep == 3 || ifStep == 5) {
                if(ifStep == 5) {
                    if(token == "if") {
                        ifStep = 1;
                        continue;
                    }
                }
                if(token == ";") {
                    if(((i + 1) == tokenCount) || expressionTokenList[i + 1] != "else") {
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();

                        scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                        scriptStatmemtItemList.back().swap(scriptStatmemtItem);
        //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
        //                scriptStatmemtItem.clear();
                        ifStep = 0;
                        continue;
                    } else {
                        if(ifStep == 5) {
                            throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement if-else is wrong");
                        }
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();
                        ifStep = 4;
                        continue;
                    }
                }
                statmentTokenList.push_back(token);
                if(token == "{") {
                    if(ifStep == 5) {
                        ifStep = 525;
                    } else {
                        ifStep = 325;
                    }
                    continue;
                }
                if(ifStep == 5) {
                    ifStep = 515;
                } else {
                    ifStep = 315;
                }
                continue;
            } else if(ifStep == 315 || ifStep == 515) {
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == ")" || token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement if is wrong");
                }
                if(token == ";") {
                    if(((i + 1) == tokenCount) || expressionTokenList[i + 1] != "else") {
                        statmentTokenList.pop_back();
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();

                        scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                        scriptStatmemtItemList.back().swap(scriptStatmemtItem);
        //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
        //                scriptStatmemtItem.clear();
                        ifStep = 0;
                        continue;
                    } else {
                        if(ifStep == 515) {
                            throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement if-else is wrong");
                        }
                        statmentTokenList.pop_back();
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();
                        ifStep = 4;
                        continue;
                    }
                }
                continue;
            } else if(ifStep == 325 || ifStep == 525) {
                statmentTokenList.push_back(token);
                if(token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "}") {
                    if(((i + 1) == tokenCount) || expressionTokenList[i + 1] != "else") {
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();

                        scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                        scriptStatmemtItemList.back().swap(scriptStatmemtItem);
        //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
        //                scriptStatmemtItem.clear();
                        ifStep = 0;
                        continue;
                    } else {
                        if(ifStep == 525) {
                            throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement if-else is wrong");
                        }
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();
                        ifStep = 4;
                        continue;
                    }
                }
                continue;
            } else if(ifStep == 4) {//else
                ifStep = 5;
            }
            continue;
        }
        if(whileStep > 0) {
            if(whileStep == 1) {
                if(token != "(") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement while is wrong");
                }
                whileStep = 2;
                continue;
            } else if(whileStep == 2) {
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement while is wrong");
                }
                if(token == ")") {
                    statmentTokenList.pop_back();
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
                    whileStep  = 3;
                    continue;
                }
                continue;
            } else if(whileStep == 3) {
                if(token == ";") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();
                    whileStep = 0;
                    continue;
                }
                statmentTokenList.push_back(token);
                if(token == "{") {
                    whileStep = 325;
                    continue;
                }
                whileStep = 315;
                continue;
            } else if(whileStep == 315) {
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == ")" || token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement while is wrong");
                }
                if(token == ";") {
                    statmentTokenList.pop_back();
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();
                    whileStep = 0;
                    continue;
                }
                continue;
            } else if(whileStep == 325) {
                statmentTokenList.push_back(token);
                if(token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "}") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();
                    whileStep = 0;
                    continue;
                }
            }
            continue;
        }
        if(doStep > 0) {
            if(doStep == 1) {
                if(token == ";") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
                    doStep = 4;
                    continue;
                }
                statmentTokenList.push_back(token);
                if(token == "{") {
                    doStep = 325;
                    continue;
                }
                doStep = 315;
                continue;
            } else if(doStep == 315) {
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == ")" || token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement do...while is wrong");
                }
                if(token == ";") {
                    statmentTokenList.pop_back();
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
                    doStep = 4;
                    continue;
                }
                continue;
            } else if(doStep == 325) {
                statmentTokenList.push_back(token);
                if(token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "}") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
                    doStep = 4;
                    continue;
                }
                continue;
            } if(doStep == 4) {
                if(token != "while") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement do...while is wrong");
                }
                doStep = 5;
                continue;
            } if(doStep == 5) {
                if(token != "(") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement do...while is wrong");
                }
                doStep = 6;
                continue;
            } if(doStep == 6) {
                statmentTokenList.push_back(token);
                if(token == "(" || token == "[" || token == "{") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(token == ")" || token == "]" || token == "}") {
                        closeCount --;
                    }
                    continue;
                }
                if(token == "]" || token == "}") {
                    throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement do...while is wrong");
                }
                if(token == ")") {
                    if((i + 1) < tokenCount && expressionTokenList[i + 1] != ";") {
                        throw TScriptException(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" statement do...while is wrong");
                    }
                    statmentTokenList.pop_back();
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();

                    doStep = 0;
                    continue;
                }
            }
            continue;
        }
        if(multiStep > 0) {
            statmentTokenList.push_back(token);
            if(token == "(" || token == "[" || token == "{") {
                closeCount ++;
                continue;
            }
            if(closeCount > 0) {
                if(token == ")" || token == "]" || token == "}") {
                    closeCount --;
                }
                continue;
            }
            if(token == ")" || token == "]") {
                LineNoString<std::string> tt = token;
                throw TScriptException((u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" unmatched " + tt.get()));
            }
            if(token == "}") {
                statmentTokenList.pop_back();
                scriptStatmemtItem << statmentTokenList;
                statmentTokenList.clear();

                scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                scriptStatmemtItemList.back().swap(scriptStatmemtItem);
//                scriptStatmemtItemList.push_back( scriptStatmemtItem);
//                scriptStatmemtItem.clear();
                multiStep = 0;
            }
            continue;
        }
        if(functionStep > 0) {
            if(functionStep == 1) {
                LineNoString<std::string> tt = token;
                if(tt == "~") {
                    if(statmentTokenList.size() > 0) {
                        throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" function name " + tt.get() + " is error"));
                    }
                    statmentTokenList.push_back(token);
                    continue;
                }
                if(!TScriptHelper::getInstance()->isToken(tt)) {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" function name " + tt.get() + " is error"));
                }
                if(statmentTokenList.size() == 0) {
                    statmentTokenList.push_back(token);
                } else {
                    LineNoString<std::string> ttt("~" + tt.get(), tt.getLineNo());
                    statmentTokenList.clear();
                    statmentTokenList.push_back(ttt);
                }
                scriptStatmemtItem << statmentTokenList;
                statmentTokenList.clear();
                functionStep = 2;
            } else if(functionStep == 2) {
                if(token != "(") {
                    LineNoString<std::string> tt = token;
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" function define error " + tt.get()));
                }
                functionStep = 3;
            } else if(functionStep == 3) {
                if(token != ")") {
                    statmentTokenList.push_back(token);
                } else {
                    if(statmentTokenList.size() == 0) {
                        scriptStatmemtItem << statmentTokenList;
                    } else {
                        statmentTokenList.push_back(LineNoString<std::string>(",",0));
                        std::vector<LineNoString<std::string> > paramList;
                        bool isError = false;
                        if((statmentTokenList.size() % 2) == 0) {
                            for(int i=0;i<statmentTokenList.size();i+=2) {
                                if(TScriptHelper::getInstance()->isToken(statmentTokenList[i]) && statmentTokenList[i + 1] == ",") {
                                    paramList.push_back(statmentTokenList[i]);
                                } else {
                                    isError = true;
                                }
                            }
                        } else {
                            isError = true;
                        }
                        if(isError) {
                            throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" function define error " + scriptStatmemtItem[0][0].get()));
                        }
                        scriptStatmemtItem << paramList;
                    }
                    statmentTokenList.clear();
                    functionStep = 14;
                }
                continue;
            } else if(functionStep == 14) {
                if(token == ":") {
                    functionStep = 24;
                    continue;
                }
                if(token != "{") {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" function define error " + scriptStatmemtItem[0][0].get()));
                }
                statmentTokenList.push_back(token);
                closeCount = 1;
                functionStep = 5;
                continue;
            } else if(functionStep == 24) {
                statmentTokenList.push_back(token);
                functionStep = 25;
                continue;
            } else if(functionStep == 25) {
                if(token != "(") {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" function define error " + scriptStatmemtItem[0][0].get()));
                }
                statmentTokenList.push_back(token);
                closeCount = 1;
                functionStep = 26;
                continue;
            } else if(functionStep == 26) {
                statmentTokenList.push_back(token);
                if(token == "(") {
                    closeCount ++;
                    continue;
                }
                if(token == ")") {
                    closeCount --;
                }
                if(closeCount == 0) {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
                    functionStep = 4;
                }
                continue;
            } else if(functionStep == 4) {
                if(token != "{") {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" function define error " + scriptStatmemtItem[0][0].get()));
                }
                statmentTokenList.push_back(token);
                closeCount = 1;
                functionStep = 5;
                continue;
            } else if(functionStep == 5) {
                statmentTokenList.push_back(token);
                if(token == "{") {
                    closeCount ++;
                    continue;
                }
                if(token == "}") {
                    closeCount --;
                    if(closeCount == 0) {
                        scriptStatmemtItem << statmentTokenList;
                        statmentTokenList.clear();

                        scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                        scriptStatmemtItemList.back().swap(scriptStatmemtItem);
        //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
        //                scriptStatmemtItem.clear();
                        functionStep = 0;
                    }
                    continue;
                }
            }
            continue;
        }
        if(classStep > 0) {
            LineNoString<std::string> tt = token;
            if(classStep == 1) {
                if(!TScriptHelper::getInstance()->isToken(tt)) {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" class name error " + tt.get()));
                }
                statmentTokenList.push_back(token);
                scriptStatmemtItem << statmentTokenList;
                statmentTokenList.clear();
                classStep = 2;
            } else if(classStep == 2) {
                if(token == ":") {
                    classStep = 3;
                } else if(token == "{") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
					closeCount = 1;
                    classStep = 5;
                } else {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" class declare error " + tt.get()));
                }
            } else if(classStep == 3) {
                if(!TScriptHelper::getInstance()->isToken(tt)) {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" super class name error " + tt.get()));
                }
                statmentTokenList.push_back(token);
                classStep = 4;
                continue;
            } else if(classStep == 4) {
                if(token == ",") {
                    classStep = 3;
                } else if(token == "{") {
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();
					closeCount = 1;
                    classStep = 5;
                } else {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8" super class name error " + tt.get()));
                }
            } else if(classStep == 5) {
                statmentTokenList.push_back(token);
                if(token == "{") {
                    closeCount ++;
                    continue;
                }
				if (token == "}") {
					closeCount--;
				}
                if(closeCount == 0) {
                    statmentTokenList.pop_back();
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
    //                scriptStatmemtItemList.push_back( scriptStatmemtItem);
    //                scriptStatmemtItem.clear();
                    classStep = 6;
                }
            } else if(classStep == 6) {
                if(token != ";") {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8"class define shall be followed by ';'"));
                }
                classStep = 0;
            }
            continue;
        }
        //import "/path/filename";
        if(importStep > 0) {
            LineNoString<std::string> tt = token;
            if(importStep == 1) {
                if(!(tt.get().at(0) == '\"' && tt.get().at(tt.get().size() - 1) == '\"')) {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8"import module " + tt.get() + u8" error ,file shall be file path closed by double quotes"));
                }
                if((i + 1) == tokenCount || expressionTokenList[i + 1] == ";") {
                    statmentTokenList.push_back(token);
                    scriptStatmemtItem << statmentTokenList;
                    statmentTokenList.clear();

                    scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                    scriptStatmemtItemList.back().swap(scriptStatmemtItem);
                    if((i + 1) == tokenCount) {
                        importStep = 0;
                    } else {
                        importStep = 2;
                    }
                } else {
                    throw TScriptException(std::string(u8"Line " + TStringHelper::number(token.getLineNo()) + ": " + u8"import module shall be ended by ';'"));
                }
            } else if(importStep == 2) {
                importStep = 0;
            }
            continue;
        }
        if(token == "for") {
            forStep = 1;
            scriptStatmemtItem.setToken(token);
            continue;
        }
        if(token == "if") {
            ifStep = 1;
            scriptStatmemtItem.setToken(token);
            continue;
        }
        if(token == "while") {
            whileStep = 1;
            scriptStatmemtItem.setToken(token);
            continue;
        }
        if(token == "do") {
            doStep = 1;
            scriptStatmemtItem.setToken(token);
            continue;
        }
        if(token == "function") {
            functionStep = 1;
            scriptStatmemtItem.setToken(token);
            continue;
        }
        if(token == "class") {
            classStep = 1;
            scriptStatmemtItem.setToken(token);
            continue;
        }

        if(token == "import") {
            importStep = 1;
            scriptStatmemtItem.setToken(token);
            continue;
        }

        if(token == "{") {
            if(statmentTokenList.size() == 0) {
                multiStep = 1;
                scriptStatmemtItem.setToken(LineNoString<std::string>("{}",0));
                continue;
            } else {
                //Map Object
//                throw TScriptException(u8"Map is not supported yet");
            }
        }
        statmentTokenList.push_back(token);
        if(token == "(") {
            bracketCount ++;
            continue;
        }
        if(token == "[") {
            sbracketCount ++;
            continue;
        }
        if(token == "{") {
            braceCount ++;
            continue;
        }
        if(token == ";") {
            if(statmentTokenList.size() > 0) {
                statmentTokenList.pop_back();
                scriptStatmemtItem << statmentTokenList;
                statmentTokenList.clear();

                scriptStatmemtItemList.push_back(TScriptStatmemtItem());
                scriptStatmemtItemList.back().swap(scriptStatmemtItem);
//                scriptStatmemtItemList.push_back( scriptStatmemtItem);
//                scriptStatmemtItem.clear();
                continue;
            }
        }
    }
}
/******************************************* TScriptHelper End ***************************************************/


/******************************************* TScriptTreeNode Begin ***********************************************/
//=(var,res)
TScriptTreeNode & TScriptTreeNode::op2TScriptTreeNode(TScriptTreeNode & treeNodeOp, const TScriptTreeNode & treeNodeLeft, const TScriptTreeNode & treeNodeRight) {
    treeNodeOp.get().asOpFunc();
    treeNodeOp << treeNodeLeft;
    treeNodeOp << treeNodeRight;
    return treeNodeOp;
}
//singel operator: !(this),++(this),--(this),(+this)
TScriptTreeNode & TScriptTreeNode::op2TScriptTreeNode(TScriptTreeNode & treeNodeOp, const TScriptTreeNode & treeNodeValue) {
    treeNodeOp.get().asOpFunc();
    treeNodeOp << treeNodeValue;
    return treeNodeOp;
}

//convert to function .(this,func,...)
TScriptTreeNode TScriptTreeNode::internalFunc2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & internalFuncExpressionList) {
    TScriptTreeNode funcTreeNode;
    funcTreeNode.get().setFunc(LineNoString<std::string>(".",0));

    if(treeNode.get().isToken()) {
        treeNode.get().asVariable();
    }
    funcTreeNode << treeNode;

    //expression shall need at lest three parameters:
    //first: function name
    //seonds: (
    //last: )
    if(internalFuncExpressionList.size() < 3) {
        throw TScriptException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" function " + treeNode.get().getToken() + " error usage");
    }
    funcTreeNode << TScriptExpression().setToken(internalFuncExpressionList[0]);

    std::vector<LineNoString<std::string> > paramExpressionList;
    for(int i=2;i<(internalFuncExpressionList.size() - 1);i++) {
        paramExpressionList.push_back(internalFuncExpressionList[i]);
    }

    std::vector< std::vector<LineNoString<std::string> > > resultll = splitExpressionList(paramExpressionList,",");
    for(const std::vector<LineNoString<std::string> > & sl : resultll) {
        funcTreeNode << expressionTokenList2TScriptTreeNode(sl);
    }

    return funcTreeNode;
}
//convert to properties: .(this,prop)
TScriptTreeNode TScriptTreeNode::internalProperty2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & internalFuncExpressionList) {
    TScriptTreeNode propTreeNode;
    propTreeNode.get().setProperty(LineNoString<std::string>(".",0));

    if(treeNode.get().isToken()) {
        treeNode.get().asVariable();
    }
    propTreeNode << treeNode;

    if(internalFuncExpressionList.size() != 1) {
        throw TScriptException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " +u8"property " + treeNode.get().getToken() + " error usage");
    }
    propTreeNode << TScriptExpression().setToken(LineNoString<std::string>(internalFuncExpressionList[0]));
    return propTreeNode;
}
//convert function: ?(condExpression,trueExpression,falseExpression)
TScriptTreeNode TScriptTreeNode::if2TScriptTreeNode(TScriptTreeNode & treeNodeCondition, const std::vector<LineNoString<std::string> > & trueExpressionList, const std::vector<LineNoString<std::string> > & falseExpressionList) {
    TScriptTreeNode funcTreeNode;
    funcTreeNode.get().setFunc(LineNoString<std::string>("?",0));

    if(treeNodeCondition.get().isToken()) {
        treeNodeCondition.get().asVariable();
    }
    funcTreeNode << treeNodeCondition;
    funcTreeNode << expressionTokenList2TScriptTreeNode(trueExpressionList);
    funcTreeNode << expressionTokenList2TScriptTreeNode(falseExpressionList);

    return funcTreeNode;
}

//convert to function: [](this,index)
TScriptTreeNode TScriptTreeNode::arrayElement2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & arrayIndexExpressionList) {
    TScriptTreeNode funcTreeNode;
    funcTreeNode.get().setFunc(LineNoString<std::string>("[]",0));

    funcTreeNode << treeNode;
    funcTreeNode << expressionTokenList2TScriptTreeNode(arrayIndexExpressionList);
    return funcTreeNode;
}

//conver to function: [a,b,c]=>array(a,b,c)
TScriptTreeNode TScriptTreeNode::array2TScriptTreeNode(const std::vector<LineNoString<std::string> > & arrayItemExpressionList) {
    TScriptTreeNode funcTreeNode;
    funcTreeNode.get().setFunc(LineNoString<std::string>("Array",0));

    std::vector< std::vector<LineNoString<std::string> > > resultll = splitExpressionList(arrayItemExpressionList,",");
    for(const std::vector<LineNoString<std::string> > & sl : resultll) {
        funcTreeNode << expressionTokenList2TScriptTreeNode(sl);
    }
    return funcTreeNode;
}
TScriptTreeNode TScriptTreeNode::map2TScriptTreeNode(const std::vector<LineNoString<std::string> > & arrayItemExpressionList) {
    TScriptTreeNode funcTreeNode;
    funcTreeNode.get().setFunc(LineNoString<std::string>("Map",0));

    std::vector< std::vector<LineNoString<std::string> > > resultll = splitExpressionList(arrayItemExpressionList,",");
    for(std::vector<LineNoString<std::string> > & sl : resultll) {
        if(sl.size() < 3) {
            throw TScriptException(u8"Line " + TStringHelper::number(sl[0].getLineNo()) + ": " + u8"Map expression error");
        }
        if(sl[1] != ":") {
            throw TScriptException(u8"Line " + TStringHelper::number(sl[0].getLineNo()) + ": " + u8"Map expression error");
        }
        if(sl[0].get()[0] == '\"') {
            if(sl[0].get().back() != '\"') {
                throw TScriptException(u8"Line " + TStringHelper::number(sl[0].getLineNo()) + ": " + u8"Map::" + sl[0].get() + u8" expression error");
            }
            sl[0].get().pop_back();sl[0].get().erase(0,1);
            sl[0].get() = TScriptHelper::getInstance()->strHandwrite2Val(sl[0]);
        }
        TScriptTreeNode token;
        funcTreeNode << TScriptExpression().setValue(sl[0].get());
        sl.erase(sl.begin());
        sl.erase(sl.begin());
        funcTreeNode << expressionTokenList2TScriptTreeNode(sl);
    }
    return funcTreeNode;
}

std::vector< std::vector<LineNoString<std::string> > > TScriptTreeNode::splitExpressionList(const std::vector<LineNoString<std::string> > & paramExpressionList, const std::string & div)
{
    std::vector<std::vector<LineNoString<std::string> > > resultll;
    if(paramExpressionList.size() > 0) {
        int bracketCount = 0;
        int sbracketCount = 0;
        std::vector<LineNoString<std::string> > subExpressionList;
        for(int i=0;i<=paramExpressionList.size();i++) {
            if(i == paramExpressionList.size()) {
                if(bracketCount > 0) {
                    throw TScriptException(u8"Line " + TStringHelper::number(paramExpressionList[0].getLineNo()) + ": " + u8"unmatched char \"(\"");
                }
                if(sbracketCount > 0) {
                    throw TScriptException(u8"unmatched char \"[\"");
                }
                if(subExpressionList.size() > 0) {
                    resultll.push_back(subExpressionList);
                    subExpressionList.clear();
                }
                break;
            }
            LineNoString<std::string>  s = paramExpressionList[i];
            if(bracketCount > 0) {
                subExpressionList.push_back(s);
                if(s == "(") {
                    bracketCount ++;
                    continue;
                }
                if(s == ")") {
                    bracketCount --;
                    if(bracketCount < 0) {
                        throw TScriptException(u8"Line " + TStringHelper::number(paramExpressionList[0].getLineNo()) + ": " +u8"unmatched \")\"");
                    }
                }
                continue;
            }
            if(sbracketCount > 0) {
                subExpressionList.push_back(s);
                if(s == "[") {
                    sbracketCount ++;
                    continue;
                }
                if(s == "]") {
                    sbracketCount --;
                    if(sbracketCount < 0) {
                        throw TScriptException(u8"Line " + TStringHelper::number(paramExpressionList[0].getLineNo()) + ": " +u8"unmatched \"]\"");
                    }
                }
                continue;
            }
            if(s == "(") {
                subExpressionList.push_back(s);
                bracketCount ++;
                continue;
            }
            if(s == "[") {
                subExpressionList.push_back(s);
                sbracketCount ++;
                continue;
            }
            if(s == div) {
                if(div == ",") {
                    if(subExpressionList.size() == 0) {
                        throw TScriptException(u8"Line " + TStringHelper::number(paramExpressionList[0].getLineNo()) + ": " +u8"wrong parameters list");
                    }
                }
                if(div == ";") {
                    if(subExpressionList.size() == 0) {
                    }
                }
                if(subExpressionList.size() > 0) {
                    resultll.push_back(subExpressionList);
                    subExpressionList.clear();
                }
                continue;
            }
            subExpressionList.push_back(s);
        }
    }
    return resultll;
}
//convert function: xxx(...)
TScriptTreeNode TScriptTreeNode::function2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & paramExpressionList) {
    if(treeNode.get().isToken()) {
        treeNode.get().asFunc();
    }

    std::vector<std::vector<LineNoString<std::string> > > resultll = splitExpressionList(paramExpressionList,",");
    for(const std::vector<LineNoString<std::string> > & sl : resultll) {
        treeNode << expressionTokenList2TScriptTreeNode(sl);
    }
    return treeNode;
}
//TScriptTreeNode TScriptTreeNode::function2TScriptTreeNode(const LineNoString<std::string> & name, const std::vector<LineNoString<std::string> > & paramExpressionList)
//{
//    TScriptTreeNode treeNode;
//    TScriptExpression func;
//    func.setFunc(name);

//    treeNode = func;
//    std::vector< std::vector<LineNoString<std::string> > > resultll = splitExpressionList(paramExpressionList,",");
//    for(const std::vector<LineNoString<std::string> > & sl : resultll) {
//        treeNode << expressionTokenList2TScriptTreeNode(sl);
//    }
//    return treeNode;
//}

/**
 * convert expressionList to TScriptTreeNodeList
 */
TScriptTreeNode TScriptTreeNode::expressionTokenList2TScriptTreeNode(const std::vector<LineNoString<std::string> > & expressionTokenList) {
    int bracketCount = 0;
    int sbracketCount = 0;
    int braceCount = 0;
    bool dotOpen = false;
    int dotBracketCount = 0;
    bool ifOpen = false;
    int ifBracketCount = 0;
    int ifsBracketCount = 0;
    int ifIndex = 0;
    int arrayorfuncCount = 0;
    int arraystep = 0;
    int closeCount = 0;
    int mapstep = 0;
    std::vector<LineNoString<std::string> > collectVarList;
    std::vector<LineNoString<std::string> > expressStack;
    std::vector<LineNoString<std::string> > subExpressionList;
    std::vector<LineNoString<std::string> > if1List;
    std::vector<LineNoString<std::string> > if2List;
    std::vector<TScriptTreeNode> treeNodeList;

    int expressionLength = expressionTokenList.size();
    for(int i=0;i<expressionLength;i++) {
        const LineNoString<std::string> & s = expressionTokenList[i];
        if(bracketCount > 0) {
            if(s == "(") {
                subExpressionList.push_back(s);
                bracketCount ++;
                continue;
            }
            if(s == ")") {
                bracketCount --;
                if(bracketCount == 0) {
                    if(!treeNodeList.empty() && treeNodeList.back().get().isToken()) {
                        TScriptTreeNode etn(treeNodeList.back());
                        treeNodeList.pop_back();
                        //pop function name, recover to origin List
                        treeNodeList.push_back(function2TScriptTreeNode(etn,subExpressionList));
                    } else {
                        //remove "()"
                        treeNodeList.push_back(expressionTokenList2TScriptTreeNode(subExpressionList));
                    }
                    subExpressionList.clear();
                    continue;
                }
            }
            subExpressionList.push_back(s);
            continue;
        }
        if(sbracketCount > 0) {
            if(s == "[") {
                subExpressionList.push_back(s);
                sbracketCount ++;
                continue;
            }
            if(s == "]") {
                sbracketCount --;
                if(sbracketCount == 0) {
                    //can be VAR(OR TOKEN),VAL,FUNC(OR TOKEN),K[]，but not OP
                    //eg.
                    //"abc"[0];str.toList(",")[0];str.toList(",",";")[0][0]
                    //
                    if(!treeNodeList.empty() && !treeNodeList.back().get().isOp()) {
                        TScriptTreeNode etn(treeNodeList.back());
                        treeNodeList.pop_back();

                        treeNodeList.push_back(arrayElement2TScriptTreeNode(etn,subExpressionList));
                    } else {
                        //not support VAR = []
                        throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8" expression error of []");
                    }
                    subExpressionList.clear();
                    continue;
                }
            }
            subExpressionList.push_back(s);
            continue;
        }
        //dot shall not following operation sign，dot shall be followed by one token，if the token is followed by '(', then the token is function name, otherwise, it is property name
        if(dotOpen) {
            subExpressionList.push_back(s);
            if(subExpressionList.size() == 1) {
                if((i + 1) == expressionLength || expressionTokenList[i + 1] != "(") {
                    if(!TScriptHelper::getInstance()->isToken(subExpressionList[0])) {
                        throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8" expression error of " + subExpressionList[0].get());
                    }
                    TScriptTreeNode etn(treeNodeList.back());
                    treeNodeList.pop_back();

                    treeNodeList.push_back(internalProperty2TScriptTreeNode(etn,subExpressionList));
                    subExpressionList.clear();
                    dotOpen = false;
                    continue;
                }
            }
            if(s == "(") {
                dotBracketCount ++;
                continue;
            }
            if(s == ")") {
                dotBracketCount --;
                if(dotBracketCount == 0) {
                    if(!treeNodeList.empty() && !treeNodeList.back().get().isOp()) {
                        TScriptTreeNode etn(treeNodeList.back());
                        treeNodeList.pop_back();

                        treeNodeList.push_back(internalFunc2TScriptTreeNode(etn,subExpressionList));
                    } else {
                        throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8" expresson error of dot");
                    }
                    subExpressionList.clear();
                    dotOpen = false;
                    continue;
                }
            }
            continue;
        }
        if(ifOpen) {
            if(ifIndex == 1) {
                if(ifBracketCount > 0) {
                    if1List.push_back(LineNoString<std::string> (s));
                    if(s == "(") {
                        ifBracketCount ++;
                        continue;
                    }
                    if(s == ")") {
                        ifBracketCount --;
                        continue;
                    }
                    continue;
                }
                if(ifsBracketCount > 0) {
                    if1List.push_back(LineNoString<std::string> (s));
                    if(s == "[") {
                        ifsBracketCount ++;
                        continue;
                    }
                    if(s == "]") {
                        ifsBracketCount --;
                        continue;
                    }
                    continue;
                }
                if(s == "(") {
                    if1List.push_back(LineNoString<std::string> (s));
                    ifBracketCount ++;
                    continue;
                }
                if(s == "[") {
                    if1List.push_back(LineNoString<std::string> (s));
                    ifsBracketCount ++;
                    continue;
                }
                if(s == ":") {
                    ifIndex = 2;
                } else {
                    if1List.push_back(LineNoString<std::string> (s));
                }
                continue;
            }
            if(ifIndex == 2) {
                if(ifBracketCount > 0) {
                    if2List.push_back(LineNoString<std::string> (s));
                    if(s == "(") {
                        ifBracketCount ++;
                        continue;
                    }
                    if(s == ")") {
                        ifBracketCount --;
                        continue;
                    }
                    if(i == (expressionLength - 1)) {
                        if(ifBracketCount > 0) {
                            throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8" unmatched char \"(\"");
                        }
                        TScriptTreeNode etn(treeNodeList.back());
                        treeNodeList.pop_back();
                        treeNodeList.push_back(if2TScriptTreeNode(etn,if1List, if2List));
                        if1List.clear();
                        if2List.clear();
                        ifOpen = false;
                    }
                    continue;
                }
                if(ifsBracketCount > 0) {
                    if2List.push_back(s);
                    if(s == "[") {
                        ifsBracketCount ++;
                        continue;
                    }
                    if(s == "]") {
                        ifsBracketCount --;
                        continue;
                    }
                    if(i == (expressionLength - 1)) {
                        if(ifsBracketCount > 0) {
                            throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8"unmatched char \"[\"");
                        }
                        TScriptTreeNode etn(treeNodeList.back());
                        treeNodeList.pop_back();
                        treeNodeList.push_back(if2TScriptTreeNode(etn,if1List, if2List));
                        if1List.clear();
                        if2List.clear();
                        ifOpen = false;
                    }
                    continue;
                }
                if(s == "(") {
                    if2List.push_back(s);
                    ifBracketCount ++;
                    continue;
                }
                if(s == "[") {
                    if2List.push_back(s);
                    ifsBracketCount ++;
                    continue;
                }
                if(s == ".") {
                    if2List.push_back(s);
                    if(i == (expressionLength - 1)) {
                        throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8" expression has error");
                    }
                    continue;
                }
                char c = LineNoString<std::string> (s).get().at(0);
                if(strchr(signall,c) == nullptr) {
                    if2List.push_back(s);
                    if(i == (expressionLength - 1)) {
                        TScriptTreeNode etn(treeNodeList.back());
                        treeNodeList.pop_back();
                        treeNodeList.push_back(if2TScriptTreeNode(etn,if1List, if2List));
                        if1List.clear();
                        if2List.clear();
                        ifOpen = false;
                    }
                    continue;
                }
                TScriptTreeNode etn(treeNodeList.back());
                treeNodeList.pop_back();
                treeNodeList.push_back(if2TScriptTreeNode(etn,if1List, if2List));
                if1List.clear();
                if2List.clear();
                ifOpen = false;
                //shall not to continue
            }
        }
        if(arrayorfuncCount > 0) {
            if(arraystep == 1) {
                subExpressionList.push_back(s);
                if(s == "[") {
                    closeCount ++;
                    continue;
                }
                if(closeCount > 0) {
                    if(s == "]") {
                        closeCount --;
                    }
                    continue;
                }
                if(s == "]") {
                    subExpressionList.pop_back();
                    if((i + 1) == expressionLength) {
                        treeNodeList.push_back(array2TScriptTreeNode(subExpressionList));
                        subExpressionList.clear();
                        arraystep = 0;
                        arrayorfuncCount = 0;
                        continue;
                    } else {
                        throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8"[] expression has error");
                    }
                }
                continue;
            }
            continue;
        }
        if(mapstep > 0) {
            if(mapstep == 1) {
                subExpressionList.push_back(s);
                if(s == "{") {
                    braceCount ++;
                }
                if(braceCount > 0) {
                    if(s == "}") {
                        braceCount --;
                        continue;
                    }
                }
                if(s == "}") {
                    subExpressionList.pop_back();
                    treeNodeList.push_back(map2TScriptTreeNode(subExpressionList));
                    subExpressionList.clear();
                    mapstep = 0;
                    continue;
                }
                continue;
            }
        }
        if(s == "(") {
            bracketCount ++;
            continue;
        }
        if(s == "[") {
            bool isSBracketStart = false;
            if(i == 0) {
                isSBracketStart = true;
            } else if(expressionTokenList[i - 1] == "," || expressionTokenList[i - 1] == ";"|| expressionTokenList[i - 1] == "}"){
                isSBracketStart = true;
            } else if(TScriptHelper::getInstance()->isOp(LineNoString<std::string> (expressionTokenList[i - 1]))){
                isSBracketStart = true;
            }
            if(isSBracketStart) {
                arrayorfuncCount ++;
                arraystep = 1;
            } else {
                sbracketCount ++;
            }
            continue;
        }
        if(s == "{") {
            mapstep = 1;
            continue;
        }
        if(s == ".") {
            dotOpen = true;
            continue;
        }
        if(s == "?") {
            if(treeNodeList.empty() || treeNodeList.back().get().isOp()) {
                throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8"? expression has error");
            }
            ifOpen = true;
            ifIndex = 1;
            ifBracketCount = 0;
            continue;
        }
        if(s == ")") {
            throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8"unmatched char \")\"");
        }
        if(s == "]") {
            throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8"unmatched char \"]\"");
        }

        char c = LineNoString<std::string> (s).get().at(0);
        TScriptExpression ee;
        if(c >= '0' && c <= '9') {
            if(TStringHelper::strfind(LineNoString<std::string> (s).get(),".") > 0) {
                ee.setValue(TStringHelper::toDouble(LineNoString<std::string> (s).get()));
            } else if(LineNoString<std::string> (s).get().size() > 2 && LineNoString<std::string> (s).get().at(0) == '0' && (LineNoString<std::string> (s).get().at(1) == 'x' || LineNoString<std::string> (s).get().at(1) == 'X')) {
                std::string sub = LineNoString<std::string> (s).get().substr(2);
                if(sub.size() <= 9) {
                    ee.setValue(std::stoi(LineNoString<std::string> (s).get().substr(2),nullptr, 16));
                } else {
                    ee.setValue(std::stoll(LineNoString<std::string> (s).get().substr(2),nullptr, 16));
                }
            } else if(LineNoString<std::string> (s).get().length() >= 9) {
                ee.setValue(TStringHelper::toLongLong(LineNoString<std::string> (s).get()));
            } else {
                ee.setValue(TStringHelper::toInt(LineNoString<std::string> (s).get()));
            }
        } else if(c == '\'') {
            std::string t = TScriptHelper::getInstance()->strHandwrite2Val(LineNoString<std::string> (s).get().substr(1,LineNoString<std::string> (s).get().length() - 2));
            if(t.length() != 1) {
                throw TScriptException(u8"Line " + TStringHelper::number(s.getLineNo()) + ": " + u8"expression has error");
            }
            ee.setValue(t.at(0));
        } else if(c == '\"') {
            ee.setValue(TScriptHelper::getInstance()->strHandwrite2Val(LineNoString<std::string> (s).get().substr(1,LineNoString<std::string> (s).get().length() - 2)));
        } else if(TStringHelper::toLower(LineNoString<std::string> (s).get()) == "true") {
            ee.setValue(true);
        } else if(TStringHelper::toLower(LineNoString<std::string> (s).get()) == "false") {
            ee.setValue(false);
        } else if(TStringHelper::toLower(LineNoString<std::string> (s).get()) == "null") {
            ee.setValue(TScriptValue().setNull());
        } else if(TScriptHelper::getInstance()->isOp(LineNoString<std::string> (s).get())) {
            TScriptExpression::TXSIGN_TYPE op = TScriptHelper::getInstance()->sign2Op(LineNoString<std::string> (s).get());
            ee.setOp(op);
        } else {
            ee.setToken(s);
        }
        treeNodeList.push_back(ee);
    }

    //expression priority，*/% > single sign > compare sign > logic sign(&& ||)
    //1.handle*/%
    //2.handle single sign
    //3.handle low priority sign
    std::vector<TScriptTreeNode> treeNodeResultList;
    bool opOpen = false;
    for(int i=0;i<treeNodeList.size();i++) {
        treeNodeResultList.push_back(treeNodeList[i]);
        int lowerListLength = treeNodeResultList.size();
        if(opOpen) {
            opOpen = false;
            if(lowerListLength < 3) {
                throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
            }
            TScriptTreeNode res(op2TScriptTreeNode(treeNodeResultList[lowerListLength - 2],treeNodeResultList[lowerListLength - 3],treeNodeResultList[lowerListLength - 1]));
            treeNodeResultList.pop_back();
            treeNodeResultList.pop_back();
            treeNodeResultList.pop_back();
            treeNodeResultList.push_back(res);
            continue;
        }
        if(treeNodeList[i].get().isOp()) {
            TScriptExpression::TXSIGN_TYPE op = treeNodeList[i].get().getOp();
            if(op == TScriptExpression::TXSIGN_MUL || op == TScriptExpression::TXSIGN_DIV || op == TScriptExpression::TXSIGN_MOD) {
                opOpen = true;
                if(i == (treeNodeList.size() - 1)) {
                    throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
                }
                if(treeNodeResultList[lowerListLength - 2].get().isOp()) {
                    throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
                }
            }
        }
    }
    //single sign，! + -，if tow signs is +-，"()" is need，or the first is operator sign
    //++ and -- priority is lower
    opOpen = false;
    treeNodeList.clear();
    treeNodeList.swap(treeNodeResultList);

    for(int i=0;i<treeNodeList.size();i++) {
        treeNodeResultList.push_back(treeNodeList[i]);
        int lowerListLength = treeNodeResultList.size();
        if(opOpen) {
            opOpen = false;
            if(lowerListLength < 2) {
                throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"single sign expression error");
            }
            if(treeNodeResultList[lowerListLength - 1].get().isOp()) {
                throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"single sign expression error");
            }
            TScriptTreeNode res(op2TScriptTreeNode(treeNodeResultList[lowerListLength - 2],treeNodeResultList[lowerListLength - 1]));
            treeNodeResultList.pop_back();
            treeNodeResultList.pop_back();
            treeNodeResultList.push_back(res);
            continue;
        }
        if(treeNodeList[i].get().isOp()) {
            TScriptExpression::TXSIGN_TYPE op = treeNodeList[i].get().getOp();
            if(op == TScriptExpression::TXSIGN_NOT) {
                opOpen = true;
                if(i == (treeNodeList.size() - 1)) {
                    throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"single sign expression error");
                }
                if(lowerListLength >= 2) {
                    if(!treeNodeResultList[lowerListLength - 2].get().isOp()) {
                        throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"single sign expression error");
                    }
                    if((i + 1) == treeNodeList.size()) {
                        throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"single sign expression error");
                    }
                }
            }
            // ++ --
            if(op == TScriptExpression::TXSIGN_ADD1 || op == TScriptExpression::TXSIGN_SUB1) {
                if(lowerListLength < 2) {
                    throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"single sign expression error");
                }
                TScriptTreeNode res(op2TScriptTreeNode(treeNodeResultList[lowerListLength - 1],treeNodeResultList[lowerListLength - 2]));
                treeNodeResultList.pop_back();
                treeNodeResultList.pop_back();
                treeNodeResultList.push_back(res);
                continue;

            }
            //+-
            if(op == TScriptExpression::TXSIGN_ADD || op == TScriptExpression::TXSIGN_SUB) {
                if(lowerListLength == 1) {
                    opOpen = true;
                } else {
                    if(treeNodeResultList[lowerListLength - 2].get().isSetOp()) {
                        opOpen = true;
                    } else if(treeNodeResultList[lowerListLength - 2].get().isCompareOp()) {
                        opOpen = true;
                    }
                }
            }
        }
    }

    //compare sign
    opOpen = false;
    treeNodeList.clear();
    treeNodeList.swap(treeNodeResultList);

    for(int i=0;i<treeNodeList.size();i++) {
        treeNodeResultList.push_back(treeNodeList[i]);
        int lowerListLength = treeNodeResultList.size();
        if(opOpen) {
            opOpen = false;
            if(lowerListLength < 3) {
                throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
            }
            TScriptTreeNode res(op2TScriptTreeNode(treeNodeResultList[lowerListLength - 2],treeNodeResultList[lowerListLength - 3],treeNodeResultList[lowerListLength - 1]));
            treeNodeResultList.pop_back();
            treeNodeResultList.pop_back();
            treeNodeResultList.pop_back();
            treeNodeResultList.push_back(res);
            continue;
        }
        if(treeNodeList[i].get().isOp()) {
            TScriptExpression::TXSIGN_TYPE op = treeNodeList[i].get().getOp();
            if(TScriptHelper::getInstance()->isCompareOp(op)) {
                opOpen = true;
                if(i == (treeNodeList.size() - 1)) {
                    throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
                }
                if(treeNodeResultList[lowerListLength - 2].get().isOp()) {
                    throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
                }
            }
        }
    }

    //low priority
    opOpen = false;
    treeNodeList.clear();
    treeNodeList.swap(treeNodeResultList);

    for(int i=0;i<treeNodeList.size();i++) {
        treeNodeResultList.push_back(treeNodeList[i]);
        int lowerListLength = treeNodeResultList.size();
        if(opOpen) {
            opOpen = false;
            if(lowerListLength < 3) {
                throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
            }
            TScriptTreeNode res(op2TScriptTreeNode(treeNodeResultList[lowerListLength - 2],treeNodeResultList[lowerListLength - 3],treeNodeResultList[lowerListLength - 1]));
            treeNodeResultList.pop_back();
            treeNodeResultList.pop_back();
            treeNodeResultList.pop_back();
            treeNodeResultList.push_back(res);
            continue;
        }
        if(treeNodeList[i].get().isOp()) {
            TScriptExpression::TXSIGN_TYPE op = treeNodeList[i].get().getOp();
            if(!TScriptHelper::getInstance()->isSetOp(op)) {
                if(lowerListLength == 1) {
                    throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"double sign expression error");
                }
                if(op == TScriptExpression::TXSIGN_TYPE::TXSIGN_ADD1 || op == TScriptExpression::TXSIGN_TYPE::TXSIGN_SUB1) {
                    if(i != (treeNodeList.size() - 1)) {
                        //++ or --
                        throw TScriptException(u8"Line " + TStringHelper::number(treeNodeList[i].getLineNo()) + ": " + u8"++ or -- shall follow one variable name");
                    }
                    TScriptTreeNode res(op2TScriptTreeNode(treeNodeResultList[lowerListLength - 1],treeNodeResultList[lowerListLength - 2]));
                    treeNodeResultList.pop_back();
                    treeNodeResultList.pop_back();
                    treeNodeResultList.push_back(res);
                    continue;
                } else {
                    opOpen = true;
                }
            }
        }
    }
    if(treeNodeResultList.size() == 3) {
        int lowerListLength = treeNodeResultList.size();
        if(!treeNodeResultList[1].get().isOp()) {
            throw TScriptException(u8"Line " + TStringHelper::number(treeNodeResultList[1].getLineNo()) + ": " + u8" expression error");
        }
        TScriptTreeNode res(op2TScriptTreeNode(treeNodeResultList[lowerListLength - 2],treeNodeResultList[lowerListLength - 3],treeNodeResultList[lowerListLength - 1]));
        treeNodeResultList.pop_back();
        treeNodeResultList.pop_back();
        treeNodeResultList.pop_back();
        treeNodeResultList.push_back(res);
    }
    if(treeNodeResultList.size() != 1) {
        throw TScriptException(u8"Line " + TStringHelper::number(treeNodeResultList[1].getLineNo()) + ": " + u8" expression error");
    }
    return treeNodeResultList[0];
}
/******************************************* TScriptTreeNode End *************************************************/


/******************************************* TScriptInterface Begin **********************************************/
TScriptModuleEngine * TScriptStatementEngine::getScriptModuleEngine(TScriptModule * scriptModule) {
    return getGlobal()->getScriptModuleEngine(scriptModule);
}
bool TScriptStatementEngine::setValue(TScriptTreeNode & treeNode, const TScriptValue & v) {
    if(treeNode.get().isToken() || treeNode.get().isVariable()) {
        return setVarVal(treeNode, v);
    } else if(treeNode.get().isFunc()) {
        if(treeNode.get().getToken() == "[]") {
            TScriptValue val = eval(treeNode[0]);
            if(val.isArray()) {
                val.getArray()[eval(treeNode[1]).toInt()] = v;
            } else if(val.isByteArray()) {
                val.getByteArray().set(eval(treeNode[1]).toInt(),v.toChar());
            } else if(val.isMap()) {
                val.getMap().put(eval(treeNode[1]).toString(), v);
            } else {
                TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8"unsupport [] expression");
            }
        } else {
            TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8"unsupport " + treeNode.get().getToken() + u8" expression");
        }
    } else if(treeNode.get().isProperty()) {
        TScriptValue val = eval(treeNode[0]);
        if(val.isObject()) {
            return val.getObject()->set(treeNode[1].get().getToken(), v);
        }
        TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode[0].getLineNo()) + ": " + u8" unsupported property");
    } else {
        TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8"unsupport expression");
    }
    return true;
}
TScriptValue TScriptStatementEngine::getVarVal(TScriptTreeNode & treeNode) {
    if(treeNode.get().isToken() || treeNode.get().isVariable()) {
        std::string & name = treeNode.get().getToken();
        if(name == "") {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" express has error");
        }
        if(treeNode.get().getScriptTokenLocation().scriptModule != NULL) {
            TScriptModuleEngine * scriptModuleEngine = getScriptModuleEngine(treeNode.get().getScriptTokenLocation().scriptModule);
            return scriptModuleEngine->getStatementEngine()->getVarVal(name);
        }
        return getVarVal(name);
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" express has error");
    }
}
bool TScriptStatementEngine::setVarVal(TScriptTreeNode & treeNode, const TScriptValue & value) {
    if(treeNode.get().isToken() || treeNode.get().isVariable()) {
        std::string & name = treeNode.get().getToken();
        if(name == "") {
            TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" express has error");
        }
        if(treeNode.get().getScriptTokenLocation().scriptModule != NULL) {
            TScriptModuleEngine * scriptModuleEngine = getScriptModuleEngine(treeNode.get().getScriptTokenLocation().scriptModule);
            return scriptModuleEngine->getStatementEngine()->setVarVal(treeNode.get().getToken(), value);
        }
        return setVarVal(treeNode.get().getToken(), value);
    } else {
        TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" express has error");
    }
    return false;
}
TScriptValue TScriptStatementEngine::evalConditionValue(TScriptTreeNode &treeNode) {
    TScriptValue cond= eval(treeNode[0]);
    if(cond.toBool()) {
        return eval(treeNode[1]);
    } else {
        return eval(treeNode[2]);
    }
}
TScriptValue TScriptStatementEngine::evalArrayValue(TScriptTreeNode &treeNode) {
    TScriptValue val = eval(treeNode[0]);
    TScriptValue idx = eval(treeNode[1]);
    int pos;
    if(val.isString()) {
        std::string & s = val.toString();
        pos = idx.toInt();
        if(pos >= s.length()) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" string index out of range");
        }
        return s.at(pos);
    } else if(val.isArray()) {
        pos = idx.toInt();
        if(pos >= val.getArray().length()) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" array index out of range");
        }
        return val.getArray()[pos];
    } else if(val.isByteArray()) {
        pos = idx.toInt();
        return val.getByteArray()[pos];
    } else if(val.isMap()) {
        return val.getMap()[idx.toString()];
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" unsupport [] expression");
    }
}

TScriptValue TScriptStatementEngine::evalInternalFuncString(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    std::string & s = val.toString();
    if(token == "toInt") {
        return TStringHelper::toInt(s);
    } else if(token == "toLong") {
        return TStringHelper::toLongLong(s);
    } else if(token == "toDouble") {
        return TStringHelper::toDouble(s);
    } else if(token == "length" || token == "size") {
        return (int)s.length();
    } else if(token == "charAt") {
        if(treeNode.getParamCount() < 3) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        return s.at(eval(treeNode[2]).toInt());
    } else if(token == "indexOf") {
        if(treeNode.getParamCount() < 3) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        if(treeNode.getParamCount() == 3) {
            return TStringHelper::strfind(s,eval(treeNode[2]).toString());
        }
        if(treeNode.getParamCount() == 4) {
            return TStringHelper::strfind(s,eval(treeNode[2]).toString(),eval(treeNode[3]).toInt());
        }
        return TStringHelper::strfind(s,eval(treeNode[2]).toString(),eval(treeNode[3]).toInt(),eval(treeNode[4]).toInt());
    } else if(token == "lastIndexOf") {
        if(treeNode.getParamCount() < 3) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " +  token  + u8" less parameters");
        }
        if(treeNode.getParamCount() == 3) {
            return (int)s.rfind(eval(treeNode[2]).toString());
        }
        return (int)s.rfind(eval(treeNode[2]).toString(),eval(treeNode[3]).toInt());
    } else if(token == "trim") {
        return TStringHelper::trim(s);
    } else if(token == "substring") {
        if(treeNode.getParamCount() < 3) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " +  token  + u8" less parameters");
        }
        if(treeNode.getParamCount() == 3) {
            return s.substr(eval(treeNode[2]).toInt());
        } else {
            int startIndex = eval(treeNode[2]).toInt();
            return s.substr(startIndex,eval(treeNode[3]).toInt() - startIndex);
        }
    } else if(token == "erase") {
        if(treeNode.getParamCount() < 3) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        int pos = eval(treeNode[2]).toInt();
        int count = s.length() - pos;
        if(treeNode.getParamCount() >= 4) {
            count = eval(treeNode[3]).toInt();
        }
        s.erase(pos,count);
        if(treeNode[0].get().isToken() || treeNode[0].get().isVariable() || treeNode[0].get().isProperty()) {
            setValue(treeNode[0],s);
        }
        return s;
    } else if(token == "empty") {
        return s.length() == 0;
    } else if(token == "append" || token == "push_back") {
        if(treeNode.getParamCount() < 3) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        s.append(eval(treeNode[2]).toString());
        if(treeNode[0].get().isToken() || treeNode[0].get().isVariable() || treeNode[0].get().isProperty()) {
            setValue(treeNode[0],TScriptValue(s));
        }
        return s;
    } else if(token == "push_front") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        s.insert(0,eval(treeNode[2]).toString());
        if(treeNode[0].get().isToken() || treeNode[0].get().isVariable() || treeNode[0].get().isProperty()) {
            setValue(treeNode[0],s);
        }
        return s;
    } else if(token == "truncate") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        int pos = eval(treeNode[2]).toInt();
        s.erase(pos,s.length() - pos);
        if(treeNode[0].get().isToken() || treeNode[0].get().isVariable() || treeNode[0].get().isProperty()) {
            setValue(treeNode[0],s);
        }
        return s;
    } else if(token == "chop") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        int count = eval(treeNode[2]).toInt();
        s.erase(s.length() - count, count);
        if(treeNode[0].get().isToken() || treeNode[0].get().isVariable() || treeNode[0].get().isProperty()) {
            setValue(treeNode[0],TScriptValue(s));
        }
        return s;
    } else if(token == "replace") {
        if(treeNode.getParamCount() < 4) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        TStringHelper::replace(s,eval(treeNode[2]).toString() , eval(treeNode[3]).toString());
        if(treeNode[0].get().isToken() || treeNode[0].get().isVariable() || treeNode[0].get().isProperty()) {
            setValue(treeNode[0],TScriptValue(s));
        }
        return s;
    } else if(token == "split") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        std::vector<std::string> sList = TStringHelper::split(s, eval(treeNode[2]).toString());
        std::vector<TScriptValue> array;
        for(std::string & ss: sList) {
            array.push_back(ss);
        }
        return array;
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " +  token + u8" unsupported function name");
    }

}
TScriptValue TScriptStatementEngine::evalInternalFuncArray(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    if(token == "length" || token == "size") {
        return val.getArray().length();
    } else if(token == "at") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        return val.getArray()[eval(treeNode[2]).toInt()];
    } else if(token == "indexOf") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        TScriptValue ee = eval(treeNode[2]);
        return val.getArray().indexOf(ee);
    } else if(token == "resize") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        val.getArray().resize(eval(treeNode[2]).toInt());
        return val;
    } else if(token == "reverse") {
        int count = val.getArray().length();
        std::vector<TScriptValue> array;
        for(int i=0;i<count;i++) {
            array.push_back(val.getArray()[count - 1 -i]);
        }
        val.setArray(array);
        if(treeNode[0].get().isVariable() || treeNode[0].get().isToken()) {
            std::string varname = treeNode[0].get().getToken();
            setVarVal(varname, val);
        }
        return val;
    } else if(token == "clear") {
        val.clear();
        if (treeNode[0].get().isVariable() || treeNode[0].get().isToken()) {
            std::string varname = treeNode[0].get().getToken();
            setVarVal(varname, val);
        }
        return val;
    } else if(token == "remove") {
        if(treeNode.getParamCount() < 3) {

            throw TScriptException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        int length = val.getArray().length();
        int startIndex = eval(treeNode[2]).toInt();
        int endIndex = length;
        if(treeNode.getParamCount() >= 4) {
            endIndex = eval(treeNode[3]).toInt();
            if(endIndex > length) {
                endIndex = length;
            }
        }
        std::vector<TScriptValue> newVal;
        int eleIdx = 0;
        while(eleIdx < length) {
            if(eleIdx < startIndex || eleIdx >= endIndex) {
                newVal.push_back(val.getArray()[eleIdx]);
                eleIdx ++;
            } else {
                eleIdx = endIndex;
            }
        }
        val.setArray(newVal);
        if(treeNode[0].get().isVariable() || treeNode[0].get().isToken()) {
            std::string varname = treeNode[0].get().getToken();
            setVarVal(varname, val);
        }
        return val;
    } else if(token == "join") {
        std::vector<std::string> ll;
        int count = val.getArray().length();
        for(int i=0;i<count;i++) {
            ll.push_back(val.getArray()[i].toString());
        }
        if(treeNode.getParamCount() < 3) {
            return TStringHelper::join(ll,"");
        } else {
            return TStringHelper::join(ll,eval(treeNode[2]).toString());
        }
    } else if(token == "push_front") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        val.getArray().push_front(eval(treeNode[2]));
        if(treeNode[0].get().isVariable() || treeNode[0].get().isToken()) {
            std::string varname = treeNode[0].get().getToken();
            setVarVal(varname, val);
        }
        return val;
    } else if(token == "push_back" || token == "append") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        val.getArray().push_back(eval(treeNode[2]));
        if(treeNode[0].get().isVariable() || treeNode[0].get().isToken()) {
            std::string varname = treeNode[0].get().getToken();
            setVarVal(varname, val);
        }
        return val;
    } else if(token == "pop_front") {
        val.getArray().pop_front();
        if(treeNode[0].get().isVariable() || treeNode[0].get().isToken()) {
            std::string varname = treeNode[0].get().getToken();
            setVarVal(varname, val);
        }
        return val;
    } else if(token == "pop_back") {
        val.getArray().pop_back();
        if(treeNode[0].get().isVariable() || treeNode[0].get().isToken()) {
            std::string varname = treeNode[0].get().getToken();
            setVarVal(varname, val);
        }
        return val;
    } else if(token == "empty") {
        return val.getArray().length() == 0;
    } else if(token == "contains") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        TScriptValue ee = eval(treeNode[2]);
        return val.getArray().indexOf(ee) >= 0;
    } else if(token == "slice") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        int length = val.getArray().length();
        int startIndex = eval(treeNode[2]).toInt();
        int endIndex = length;
        if(treeNode.getParamCount() >= 4) {
            endIndex = eval(treeNode[3]).toInt();
            if(endIndex > length) {
                endIndex = length;
            }
        }
        return val.getArray().slice(startIndex, endIndex - startIndex);
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + "Array::" + token + u8" unsupported");
    }
}
TScriptValue TScriptStatementEngine::evalInternalFuncByteArray(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    if(token == "length" || token == "size") {
        return val.getByteArray().length();
    } else if(token == "at") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        return val.getByteArray()[eval(treeNode[2]).toInt()];
    } else if(token == "charAt") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        return (char)val.getByteArray()[eval(treeNode[2]).toInt()];
    } else if(token == "reverse") {
        val.getByteArray().reverse();
        return val;
    } else if(token == "clear") {
        return val.clear();
    } else if(token == "remove") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        int pos = eval(treeNode[2]).toInt();
        if(treeNode.getParamCount() >= 4) {
            int length = eval(treeNode[3]).toInt();
            val.getByteArray().remove(pos, length);
        } else {
            val.getByteArray().remove(pos);
        }
        return val;
    } else if(token == "removeAt") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        int pos = eval(treeNode[2]).toInt();
        val.getByteArray().removeAt(pos);
        return val;
    } else if(token == "push_front") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        TScriptValue param1 = eval(treeNode[2]);
        if(param1.isChar() || param1.isInt() || param1.isLongLong()) {
            val.getByteArray().push_front(param1.toChar());
        } else if(param1.isString()) {
            std::string & s = param1.getString();
            val.getByteArray().push_front((const uint8_t*)s.data(), s.size());
        } else if(param1.isByteArray()) {
            val.getByteArray().push_front(param1.getByteArray());
        } else {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" wrong parameter type");
        }
        return val;
    } else if(token == "push_back") {
        if(treeNode.getParamCount() < 3) {

            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        TScriptValue param1 = eval(treeNode[2]);
        if(param1.isChar() || param1.isInt() || param1.isLongLong()) {
            val.getByteArray().push_back(param1.toChar());
        } else if(param1.isString()) {
            std::string & s = param1.getString();
            val.getByteArray().push_back((const uint8_t*)s.data(), s.size());
        } else if(param1.isByteArray()) {
            val.getByteArray().push_back(param1.getByteArray());
        } else {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" wrong parameter type");
        }
        return val;
    } else if(token == "insert") {
        if(treeNode.getParamCount() < 4) {

            throw TScriptException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" less parameters");
        }
        TScriptValue pos1 = eval(treeNode[2]);
        if(!(pos1.isInt() || pos1.isLongLong() || pos1.isString())) {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" wrong parameter type");
        }
        int pos = pos1.toInt();
        TScriptValue param1 = eval(treeNode[3]);
        if(param1.isChar() || param1.isInt() || param1.isLongLong()) {
            uint8_t c = param1.toChar();
            val.getByteArray().insert(pos, &c, 1);
        } else if(param1.isString()) {
            std::string & s = param1.getString();
            val.getByteArray().insert(pos, (const uint8_t*)s.data(), s.size());
        } else if(param1.isByteArray()) {
            val.getByteArray().insert(pos, param1.getByteArray());
        } else {
            return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token  + u8" wrong parameter type");
        }
        return val;
    } else if(token == "empty") {
        return val.getByteArray().empty();
    } else if(token == "toString") {
        return std::string((const char *)val.getByteArray().getData(), val.getByteArray().length());
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + "Buffer::" + token + u8" is unsupported");
    }

}
TScriptValue TScriptStatementEngine::evalInternalFuncMap(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    if(token == "empty") {
        return val.getMap().length() == 0;
    } else if(token == "length") {
        return val.getMap().length();
    } else if(token == "clear") {
        val.getMap().clear();
        return val;
    } else if(token == "contains") {
        return val.getMap().contains(eval(treeNode[2]).toString());
    } else if(token == "keys") {
        std::vector<std::string> keys = val.getMap().keys();
        TScriptArray array;
        for(int i=0;i<keys.size();i++) {
            array << keys[i];
        }
        return array;
    } else if(token == "get") {
        return val.getMap()[eval(treeNode[2]).toString()];
    } else if(token == "put") {
        return val.getMap().put(eval(treeNode[2]).toString(),eval(treeNode[3]));
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + "Map::" + token + u8" is unsupported");
    }

}
TScriptValue TScriptStatementEngine::evalInternalFuncObject(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();

    std::vector<TScriptValue> paramList;
    int paramCount = treeNode.getParamCount();
    for(int i=2;i<paramCount;i++) {
        paramList.push_back(eval(treeNode[i]));
    }
    return val.getObject()->invoke(token,paramList);
}
TScriptValue TScriptStatementEngine::evalInternalFuncChar(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    if(token == "toInt") {
        return val.toInt();
    } else if(token == "toString") {
        return val.toString();
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token + u8" is unsupported");
    }
}
TScriptValue TScriptStatementEngine::evalInternalFuncInt(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    if(token == "toChar") {
        return val.toChar();
    } else if(token == "toLong") {
        return val.toLongLong();
    } else if(token == "toString") {
        return val.toString();
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token + u8" is unsupported");
    }

}
TScriptValue TScriptStatementEngine::evalInternalFuncLongLong(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    if(token == "toChar") {
        return val.toChar();
    } else if(token == "toInt") {
        return val.toInt();
    } else if(token == "toString") {
        return val.toString();
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token + u8" is unsupported");
    }
}
TScriptValue TScriptStatementEngine::evalInternalFuncDouble(TScriptValue & val, TScriptTreeNode & treeNode) {
    std::string token = treeNode[1].get().getToken();
    if(token == "toInt") {
        return val.toInt();
    } else if(token == "toLong") {
        return val.toLongLong();
    } else if(token == "toString") {
        return val.toString();
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + token + u8" is unsupported");
    }

}
TScriptValue TScriptStatementEngine::evalInternalFunc(TScriptTreeNode &treeNode) {
    TScriptValue val = eval(treeNode[0]);
    if(val.isString()) {
        return evalInternalFuncString(val, treeNode);
    } else if(val.isChar()) {
        return evalInternalFuncChar(val, treeNode);
    } else if(val.isInt()) {
        return evalInternalFuncInt(val, treeNode);
    } else if(val.isLongLong()) {
        return evalInternalFuncLongLong(val, treeNode);
    } else if(val.isDouble()) {
        return evalInternalFuncDouble(val, treeNode);
    } else if(val.isArray()) {
        return evalInternalFuncArray(val, treeNode);
    } else if(val.isByteArray()) {
        return evalInternalFuncByteArray(val, treeNode);
    } else if(val.isMap()) {
        return evalInternalFuncMap(val, treeNode);
    } else if(val.isObject()) {
        return evalInternalFuncObject(val, treeNode);
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + treeNode[1].get().getToken() + u8" is unsupported");
    }
}

TScriptValue TScriptStatementEngine::evalOpFuncSet(TScriptTreeNode & treeNode) {
    TScriptValue newVal = eval(treeNode[1]);
    setValue(treeNode[0],newVal);
    //setVarVal(token, newVal);
    return newVal;
}
TScriptValue TScriptStatementEngine::evalOpFuncAndSet(TScriptTreeNode & treeNode) {
    if(!eval(treeNode[0]).toBool()) {
        //no need to set
        return false;
    } else {
        if(eval(treeNode[1]).toBool()) {
            //no need to set
            return true;
        } else {
            TScriptValue newVal(false);
            setValue(treeNode[0],newVal);
            //setVarVal(token, newVal);
            return newVal;
        }
    }
}
TScriptValue TScriptStatementEngine::evalOpFuncOrSet(TScriptTreeNode & treeNode) {
    if(eval(treeNode[0]).toBool()) {
        //no need to set
        return true;
    } else {
        if(eval(treeNode[1]).toBool()) {
            TScriptValue newVal(true);
            setValue(treeNode[0],newVal);
            //setVarVal(token, newVal);
            return newVal;
        } else {
            //no need to set
            return false;
        }
    }

}
TScriptValue TScriptStatementEngine::evalOpFuncSignSet(TScriptTreeNode & treeNode) {
    std::auto_ptr<std::vector<TScriptValue> > paramList(new std::vector<TScriptValue>());
    int paramCount = treeNode.getParamCount();
    for(int i=0;i<paramCount;i++) {
        paramList->push_back(eval(treeNode[i]));
    }

    TScriptValue newVal = applyUserFunc(treeNode.getLineNo(), TScriptHelper::getInstance()->op2Sign(treeNode.get().getOp()), *paramList);
    setValue(treeNode[0],newVal);
    //setVarVal(token, newVal);
    return newVal;

}

TScriptValue TScriptStatementEngine::evalOpFuncAnd(TScriptTreeNode & treeNode) {
    if(!eval(treeNode[0]).toBool()) {
        return false;
    } else {
        return eval(treeNode[1]).toBool();
    }
}
TScriptValue TScriptStatementEngine::evalOpFuncOr(TScriptTreeNode & treeNode) {
    if(eval(treeNode[0]).toBool()) {
        return true;
    } else {
        return eval(treeNode[1]).toBool();
    }
}
TScriptValue TScriptStatementEngine::evalOpFuncSign(TScriptTreeNode & treeNode) {
    std::auto_ptr<std::vector<TScriptValue> > paramList(new std::vector<TScriptValue>());
    if(treeNode.getParamCount() == 1) {
        if(treeNode.get().getOp() == TScriptExpression::TXSIGN_ADD) {
            return eval(treeNode[0]);
        } else if(treeNode.get().getOp() == TScriptExpression::TXSIGN_SUB) {
            paramList->push_back(0);
            paramList->push_back(eval(treeNode[0]));
        }
    } else {
        for(int i=0;i<treeNode.getParamCount();i++) {
            paramList->push_back(eval(treeNode[i]));
        }
    }
    return applyUserFunc(treeNode.getLineNo(), TScriptHelper::getInstance()->op2Sign(treeNode.get().getOp()), *paramList);
}

TScriptValue TScriptStatementEngine::evalOpFunc(TScriptTreeNode & treeNode) {

    if(treeNode.get().isSetOp()) {
        //first parameter shall be variable name
        if(treeNode.getParamCount() == 0) {
            return TScriptHelper::throwException(u8"Line " + std::to_string(treeNode.getLineNo()) + TScriptHelper::getInstance()->op2Sign(treeNode.get().getOp()).c_str() + u8" is error");
        }
        if(treeNode[0].get().isToken() || treeNode[0].get().isVariable() || treeNode[0].get().isProperty() || treeNode[0].get().isFunc()) {
            // = is different to others
            if(treeNode.get().getOp() == TScriptExpression::TXSIGN_SET) {
                return evalOpFuncSet(treeNode);
            } else if(treeNode.get().getOp() == TScriptExpression::TXSIGN_ANDSET) {
                return evalOpFuncAndSet(treeNode);
            } else if(treeNode.get().getOp() == TScriptExpression::TXSIGN_ORSET) {
                return evalOpFuncOrSet(treeNode);
            } else {
                return evalOpFuncSignSet(treeNode);
            }
        } else {
            return TScriptHelper::throwException(u8"Line " + std::to_string(treeNode.getLineNo()) + u8": expression has error");
        }
    } else {
        //spectial handle
        //eg.   ((sa.length() > 5) && (sa.at(5) == 'c'))
        if(treeNode.get().getOp() == TScriptExpression::TXSIGN_AND) {
            return evalOpFuncAnd(treeNode);
        } else if(treeNode.get().getOp() == TScriptExpression::TXSIGN_OR) {
            return evalOpFuncOr(treeNode);
        } else {
            return evalOpFuncSign(treeNode);
        }
    }
}
TScriptValue TScriptStatementEngine::evalPropertyFunc(TScriptTreeNode & treeNode) {
    if(treeNode.get().getToken() != ".") {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" property expression has error");
    }
    TScriptValue val = eval(treeNode[0]);
    if(val.isObject()) {
        return val.getObject()->get(treeNode[1].get().getToken());
    } else {
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" property " + treeNode[1].get().getToken() + u8" is undefined");
    }
}
TScriptValue TScriptStatementEngine::evalScriptFuncCreateClass(TScriptTreeNode & treeNode) {
    std::auto_ptr<std::vector<TScriptValue> > paramList(new std::vector<TScriptValue>());
    for(int i=0;i<treeNode.getParamCount();i++) {
        paramList->push_back(eval(treeNode[i]));
    }
    if(treeNode.get().getScriptTokenLocation().scriptModule == nullptr) {
        TScriptStatementEngine * scriptStatementEngine = getScriptStatementEngine(treeNode.get().getScriptTokenLocation().scriptStatement);
        if(scriptStatementEngine != NULL) {
            return TScriptClassEngine::create(scriptStatementEngine, treeNode.get().getScriptTokenLocation().getScriptClass(), *paramList);
        }
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8" class " + treeNode.get().getScriptTokenLocation().getScriptClass()->getName() + u8" out of reference");
    } else {
        return TScriptClassEngine::create(getScriptModuleEngine(treeNode.get().getScriptTokenLocation().scriptModule)->getStatementEngine(), treeNode.get().getScriptTokenLocation().getScriptClass(), *paramList);
    }
}
TScriptValue TScriptStatementEngine::evalScriptFuncClassFunction(TScriptTreeNode & treeNode) {
    std::auto_ptr<std::vector<TScriptValue> > paramList(new std::vector<TScriptValue>());
    for(int i=0;i<treeNode.getParamCount();i++) {
        paramList->push_back(eval(treeNode[i]));
    }
    TScriptStatementEngine * checkClassStatementEngine = this;
    while(checkClassStatementEngine != NULL) {
        if(checkClassStatementEngine != NULL && checkClassStatementEngine->ownerScriptClassEngine != NULL) {
            return checkClassStatementEngine->ownerScriptClassEngine->getObject()->invoke(treeNode.get().getScriptTokenLocation().getScriptFunction()->getName(),*paramList.get());
        }
        checkClassStatementEngine = checkClassStatementEngine->getOwnerStatementEngine();
    }
    return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + treeNode.get().getScriptTokenLocation().getScriptFunction()->getOwnerScriptClass()->getName() + u8"." + treeNode.get().getScriptTokenLocation().getScriptFunction()->getName() + " is undefined");
}
TScriptValue TScriptStatementEngine::evalScriptFuncNormalFunction(TScriptTreeNode & treeNode) {
    std::auto_ptr<std::vector<TScriptValue> > paramList(new std::vector<TScriptValue>());
    for(int i=0;i<treeNode.getParamCount();i++) {
        paramList->push_back(eval(treeNode[i]));
    }
    if(treeNode.get().getScriptTokenLocation().scriptModule == nullptr) {
        TScriptStatementEngine * scriptStatementEngine = getScriptStatementEngine(treeNode.get().getScriptTokenLocation().scriptStatement);
        if(scriptStatementEngine != NULL) {
            return TScriptFunctionEngine::applyScriptFunction(scriptStatementEngine, *treeNode.get().getScriptTokenLocation().getScriptFunction(), *paramList);
        }
        return TScriptHelper::throwException(u8"Line " + TStringHelper::number(treeNode.getLineNo()) + ": " + u8"function " + treeNode.get().getScriptTokenLocation().getScriptFunction()->getName() + u8" out of reference");
        //return TScriptFunctionEngine::applyScriptFunction(this, *scriptFunction, *paramList);
    } else {
        TScriptModuleEngine * scriptModuleEngine = getScriptModuleEngine(treeNode.get().getScriptTokenLocation().scriptModule);
        return TScriptFunctionEngine::applyScriptFunction(scriptModuleEngine->getStatementEngine(), *treeNode.get().getScriptTokenLocation().getScriptFunction(), *paramList);
    }
}
TScriptValue TScriptStatementEngine::evalScriptFunc4(TScriptTreeNode & treeNode) {
    std::auto_ptr<std::vector<TScriptValue> > paramList(new std::vector<TScriptValue>());
    for(int i=0;i<treeNode.getParamCount();i++) {
        paramList->push_back(eval(treeNode[i]));
    }
    return applyUserFunc(treeNode.getLineNo(), treeNode.get().getToken(), *paramList);
}

TScriptValue TScriptStatementEngine::evalScriptFunc(TScriptTreeNode & treeNode)
{
    if(treeNode.get().getScriptTokenLocation().getScriptClass() != nullptr) {
        return evalScriptFuncCreateClass(treeNode);
    } else if(treeNode.get().getScriptTokenLocation().getScriptFunction() != nullptr) {
        if(treeNode.get().getScriptTokenLocation().getScriptFunction()->getOwnerScriptClass() != NULL) {
            return evalScriptFuncClassFunction(treeNode);
        } else {
            return evalScriptFuncNormalFunction(treeNode);
        }
    }
    return evalScriptFunc4(treeNode);
}
TScriptValue TScriptStatementEngine::eval(TScriptTreeNode & treeNode) {
    if(treeNode.get().isValue()) {
        return treeNode.get().getValue();
    } else if(treeNode.get().isToken() || treeNode.get().isVariable()) {//default is variable
        return getVarVal(treeNode);
    } else if(treeNode.get().isFunc()) {
        if(treeNode.get().isConditionToken()) {
            return evalConditionValue(treeNode);
        } else if(treeNode.get().isArrayToken()) {
            return evalArrayValue(treeNode);
        } else if(treeNode.get().isInternalFuncToken()) {
            return evalInternalFunc(treeNode);
        } else {
            return evalScriptFunc(treeNode);
        }
    } else if(treeNode.get().isOpFunc()) {
        return evalOpFunc(treeNode);
    } else if(treeNode.get().isProperty()) {
        return evalPropertyFunc(treeNode);
    } else {
        return TScriptHelper::throwException(u8"Line " + std::to_string(treeNode.getLineNo()) + u8": unknow how to calculate");
    }
}
/******************************************* TScriptInterface End ************************************************/

/******************************************* TScriptEngine Begin *************************************************/
TScriptEngine::TScriptEngine() {
    scriptEngine = new TScriptGlobalEngine();
}
TScriptEngine::~TScriptEngine() {
    delete scriptEngine;
}
void TScriptEngine::setBaseDir(const std::string & baseDir) {
    scriptEngine->setBaseDir(baseDir);
}
void TScriptEngine::initVarValue(std::map<std::string,TScriptValue> & varValMap) {
    if(!varValMap.empty()) {
        scriptEngine->initVarValue(varValMap);
    }
}
void TScriptEngine::clearVar() {
    scriptEngine->clearVar();
}

TScriptValue TScriptEngine::evalScript(const std::string & script, bool isScriptFile) {
    return scriptEngine->evalScript(script, isScriptFile);
}
void TScriptEngine::bindGetVarFunc(get_var_func f) {
    scriptEngine->bindGetVarFunc(f);
}
void TScriptEngine::bindSetVarFunc(set_var_func f) {
    scriptEngine->bindSetVarFunc(f);
}

void TScriptEngine::bindUserFunc(const std::string & name, const user_func0 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}
void TScriptEngine::bindUserFunc(const std::string & name, const user_func1 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}
void TScriptEngine::bindUserFunc(const std::string & name, const user_func2 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}
void TScriptEngine::bindUserFunc(const std::string & name, const user_func3 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}
void TScriptEngine::bindUserFunc(const std::string & name, const user_func4 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}
void TScriptEngine::bindUserFunc(const std::string & name, const user_func5 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}
void TScriptEngine::bindUserFunc(const std::string & name, const user_func6 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}
void TScriptEngine::bindUserFunc(const std::string & name, const user_func7 f, int minParamCount) {
    scriptEngine->bindUserFunc(name, f, minParamCount);
}

void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func0 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}
void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func1 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}
void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func2 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}
void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func3 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}
void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func4 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}
void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func5 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}
void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func6 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}
void TScriptEngine::bindUserDataFunc(const std::string & name, const user_data_func7 f, void*data, int minParamCount) {
    scriptEngine->bindUserDataFunc(name, f, data, minParamCount);
}

std::vector<std::string> TScriptEngine::getVarList() {
    return scriptEngine->getVarList();
}
std::map<std::string,TScriptValue> & TScriptEngine::getVarMap() {
    return scriptEngine->getVarMap();
}

/******************************************* TScriptEngine End ***************************************************/

/****************************************** Implementation End ***************************************************/

}
