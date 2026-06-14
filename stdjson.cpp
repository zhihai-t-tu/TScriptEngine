#include "stdjson.h"

#include <math.h>
#include <algorithm>
#include <exception>

namespace stdjson {

static std::string handwrite2string(const std::string &s, int offset = 0, int end = -1);
static int strfind(const std::string & s, const std::string & sub, int offset = 0, int end = -1);


stdjson_array::stdjson_array() {
}
stdjson_array::stdjson_array(const stdjson_array & v) {
    jv_array = v.jv_array;
}
stdjson_array::stdjson_array(const std::shared_ptr<std::vector<stdjson_value > > & v) {
    jv_array = v;
}
stdjson_value & stdjson_array::operator [](int i) {
    if(i < 0 || i >= size()) {
        throw std::out_of_range("out of array length");
    }
    return (*jv_array)[i];
}
stdjson_value & stdjson_array::back() {
    if(size() == 0) {
        throw std::out_of_range("out of array length");
    }
    return jv_array->back();
}
stdjson_array & stdjson_array::operator << (const stdjson_value & v) {
    if(jv_array == nullptr) {
        jv_array = std::shared_ptr<std::vector<stdjson_value > >(new std::vector<stdjson_value >());
    }
    jv_array->push_back(v);
    return *this;
}
stdjson_array & stdjson_array::operator = (const stdjson_array & v) {
    jv_array = v.jv_array;
    return *this;
}
stdjson_array & stdjson_array::insert(int pos, const stdjson_value & v) {
    if(pos < (-size()) || pos > size()) {
        throw std::out_of_range("out of array length");
    }
    if(jv_array == nullptr) {
        jv_array = std::shared_ptr<std::vector<stdjson_value > >(new std::vector<stdjson_value >());
    }

    if(pos < 0) pos += size();
    jv_array->insert(jv_array->begin() + pos, v);
    return *this;
}
stdjson_array & stdjson_array::remove(int pos) {
    if(pos <= (-size()) || pos >= size()) {
        throw std::out_of_range("out of array length");
    }
    if(pos < 0) pos += size();
    jv_array->erase(jv_array->begin() + pos);
    return *this;
}
stdjson_array & stdjson_array::clear() {
    if(jv_array != nullptr) {
        jv_array->clear();
    }
    return *this;
}

int stdjson_array::length() const {
    return size();
}
int stdjson_array::size() const {
    if(jv_array == nullptr) {
        return 0;
    }
    return jv_array->size();
}

bool stdjson_array::empty() {
    return (jv_array == nullptr || jv_array->size() == 0);
}

std::string stdjson_array::toJSON() {
    std::string r = "[";
    for(int i=0;i<jv_array->size();i++) {
        if(i > 0) {
            r.append(",");
        }
        r.append((*jv_array)[i].toJSON());
    }
    r.append("]");
    return r;
}

stdjson_object::stdjson_object() {
}
stdjson_object::stdjson_object(const stdjson_object & v) {
    jv_object = v.jv_object;
}
stdjson_object::stdjson_object(const std::shared_ptr<std::map<std::string, stdjson_value > > & v) {
    jv_object = v;
}
stdjson_value & stdjson_object::operator [](const std::string & key) {
    if(jv_object == nullptr) jv_object = std::shared_ptr<std::map<std::string, stdjson_value > >(new std::map<std::string, stdjson_value >());
    return (*jv_object)[key];
}
stdjson_object & stdjson_object::operator = (const stdjson_object & v) {
    jv_object = v.jv_object;
    return *this;
}
std::vector<std::string> stdjson_object::keys() const {
    std::vector<std::string> ks;
    if(jv_object != nullptr) {
        for(std::map<std::string, stdjson_value >::iterator iter = jv_object->begin(); iter != jv_object->end();iter ++) {
            ks.push_back(iter->first);
        }
    }
    return ks;
}
stdjson_object & stdjson_object::remove(const std::string & key) {
    if(jv_object != nullptr) {
        std::map<std::string, stdjson_value >::iterator iter = jv_object->find(key);
        if(iter != jv_object->end()) {
            jv_object->erase(iter);
        }
    }
    return *this;
}
stdjson_object & stdjson_object::clear() {
    if(jv_object != nullptr) {
        jv_object->clear();
    }
    return *this;
}
bool stdjson_object::empty() {
    return (jv_object == nullptr || jv_object->empty());
}
std::string stdjson_object::toJSON() {
    std::string r = "{";
    int keyCount = 0;
    for(std::map<std::string, stdjson_value >::iterator iter = jv_object->begin(); iter != jv_object->end(); iter ++) {
        keyCount ++;
        if(keyCount > 1) {
            r.append(",");
        }
        r.append("\"" + iter->first + "\": " + iter->second.toJSON());
    }
    r.append("}");
    return r;
}

stdjson_value::stdjson_value():type(json_type::jt_null){}
stdjson_value::stdjson_value(bool v):type(json_type::jt_bool),jv_bool(v){}
stdjson_value::stdjson_value(int v):type(json_type::jt_int),jv_int(v){}
stdjson_value::stdjson_value(long long v):type(json_type::jt_longlong),jv_longlong(v){}
stdjson_value::stdjson_value(double v):type(json_type::jt_double),jv_double(v){}
stdjson_value::stdjson_value(const std::string & v):type(json_type::jt_string),jv_string(v){}
stdjson_value::stdjson_value(const std::vector<stdjson_value> & v):type(json_type::jt_array)
{
    jv_array = std::shared_ptr<std::vector<stdjson_value > >(new std::vector<stdjson_value>(v) );
}
stdjson_value::stdjson_value(const std::map<std::string, stdjson_value> & v):type(json_type::jt_object){
    jv_object = std::shared_ptr<std::map<std::string, stdjson_value> >(new std::map<std::string, stdjson_value>(v) );
}
stdjson_value::stdjson_value(const stdjson_value & v) {
    type = v.type;
    if(isBool()) jv_bool = v.jv_bool;
    if(isInt()) jv_int = v.jv_int;
    if(isLongLong()) jv_longlong = v.jv_longlong;
    if(isDouble()) jv_double = v.jv_double;
    if(isString()) jv_string = v.jv_string;
    if(isArray()) jv_array = v.jv_array;
    if(isObject()) jv_object = v.jv_object;
}

stdjson_value::json_type stdjson_value::getType() {
    return type;
}
bool stdjson_value::isNull() {
    return type == json_type::jt_null;
}
bool stdjson_value::isBool() {
    return type == json_type::jt_bool;
}
bool stdjson_value::isInt() {
    return type == json_type::jt_int;
}
bool stdjson_value::isLongLong() {
    return type == json_type::jt_longlong;
}
bool stdjson_value::isDouble() {
    return type == json_type::jt_double;
}
bool stdjson_value::isString() {
    return type == json_type::jt_string;
}
bool stdjson_value::isArray() {
    return type == json_type::jt_array;
}
bool stdjson_value::isObject() {
    return type == json_type::jt_object;
}
bool stdjson_value::toBool() {
    if(isBool()) return jv_bool;
    return false;
}
int stdjson_value::toInt() {
    if(isInt()) return jv_int;
    if(isLongLong()) return jv_longlong;
    if(isDouble()) return (int)jv_double;
    if(isString()) return atoi(jv_string.c_str());
    return 0;
}
long long stdjson_value::toLong() {
    if(isInt()) return jv_int;
    if(isLongLong()) return jv_longlong;
    if(isDouble()) return (long long)jv_double;
    if(isString()) return atoll(jv_string.c_str());
    return 0;
}
double stdjson_value::toDouble() {
    if(isDouble()) return jv_double;
    if(isInt()) return jv_int;
    if(isLongLong()) return jv_longlong;
    if(isString()) return atof(jv_string.c_str());
    return 0;
}
std::string stdjson_value::toString() {
    if(isString()) return jv_string;
    //if(isDouble()) return std::to_string((long double)jv_double);
    if(isDouble()) return std::to_string(jv_double);
    if(isInt()) return std::to_string(jv_int);
    if(isLongLong()) return std::to_string(jv_longlong);
    if(isBool()) return (jv_bool)?"true":"false";
    return "";
}
stdjson_array stdjson_value::toArray() {
    if(isArray()) return stdjson_array(jv_array);
    return stdjson_array();
}
stdjson_object stdjson_value::toObject() {
    stdjson_object jvo;
    if(isObject()) return jvo = jv_object;
    return jvo;
}

stdjson_value & stdjson_value::operator =(bool v) {
    type = jt_bool;
    jv_bool = v;
    return *this;
}
stdjson_value & stdjson_value::operator =(int v) {
    type = jt_int;
    jv_int = v;
    return *this;
}
stdjson_value & stdjson_value::operator =(long long v) {
    type = jt_longlong;
    jv_longlong = v;
    return *this;
}
stdjson_value & stdjson_value::operator =(double v) {
    type = jt_double;
    jv_double = v;
    return *this;
}
stdjson_value & stdjson_value::operator =(const std::string & v) {
    type = jt_string;
    jv_string = v;
    return *this;
}
stdjson_value & stdjson_value::operator =(const stdjson_array & v) {
    type = jt_array;
    jv_array = v.jv_array;
    return *this;
}
stdjson_value & stdjson_value::operator =(const stdjson_object & v) {
    type = jt_object;
    jv_object = v.jv_object;
    return *this;
}
stdjson_value & stdjson_value::operator =(const stdjson_value & v) {
    type = v.type;
    if(isBool()) jv_bool = v.jv_bool;
    if(isInt()) jv_int = v.jv_int;
    if(isLongLong()) jv_longlong = v.jv_longlong;
    if(isDouble()) jv_double = v.jv_double;
    if(isString()) jv_string = v.jv_string;
    if(isArray()) jv_array = v.jv_array;
    if(isObject()) jv_object = v.jv_object;
    return *this;
}
bool stdjson_value::equals (const stdjson_value & v) {
    if(type != v.type) return false;
    if(isBool()) return jv_bool == v.jv_bool;
    if(isInt()) return jv_int == v.jv_int;
    if(isLongLong()) return jv_longlong == v.jv_longlong;
    if(isDouble()) return jv_double == v.jv_double;
    if(isString()) return jv_string == v.jv_string;
    if(isArray()) return jv_array == v.jv_array;
    if(isObject()) return jv_object == v.jv_object;
    return true;
}
bool stdjson_value::operator == (const stdjson_value & v) {
    return equals(v);
}
bool stdjson_value::operator != (const stdjson_value & v) {
    return !equals(v);
}
std::string stdjson_value::toJSON() {
    if(isBool() || isInt() || isLongLong() || isDouble()) return toString();
    if(isNull()) return "null";
    if(isString()) {
        if(strfind(jv_string,"\"") < 0) {
            return "\"" + jv_string + "\"";
        } else {
            std::string s;
            size_t p = 0;
            size_t start = 0;
            for(;;) {
                p = strfind(jv_string,"\\", start);
                if(p < 0) {
                    s.append(jv_string.substr(start));
                    break;
                } else {
                    s.append(jv_string.substr(start, p));
                    start = p + 1;
                    s.append("\\\\");
                }
            }
            std::string r;
            start = 0;
            for(;;) {
                p = strfind(s,"\"", start);
                if(p == std::string::npos) {
                    r.append(s.substr(start));
                    break;
                } else {
                    r.append(s.substr(start, p));
                    start = p + 1;
                    r.append("\\\"");
                }
            }
            return r;
        }
    }
    if(isArray()) {
        return toArray().toJSON();
    }
    if(isObject()) {
        return toObject().toJSON();
    }
    return "";
}
stdjson_value stdjson_value::fromJSON(const std::string & json, int offset, int end) {
    if(end == -1 || end > json.size()) {
        end = json.size();
    }
    if(end <= offset) {
        return stdjson_value();
    }
    for(int i=offset;i<end;i++) {
        char fc = json[i];
        if(fc == ' ' || fc == '\t' || fc == '\r' || fc == '\n') {
            offset ++;
        } else {
            break;
        }
    }
    for(int i=(end - 1);i>=offset;i--) {
        char fc = json[i];
        if(fc == ' ' || fc == '\t' || fc == '\r' || fc == '\n') {
            end --;
        } else {
            break;
        }
    }
    if(end <= offset) {
        return stdjson_value();
    }
    char fc = json[offset];
    if(fc == '{') {
        std::map<std::string, stdjson_value > jv_object;
        bool quote2Open = false;
        bool convertOpen = false;
        int braceCount = 0;
        int sbracketCount = 0;
        std::string key;
        int mode = 0;//05:key,1:':',2:value
        int step = 0;
        std::string r;
        for(int i = offset + 1;i < end; i++) {
            char c = json[i];
            if(mode == 0 || mode == 5) {
                if(step == 0) {
                    if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        continue;
                    }
                    if(c == '\"') {
                        r.push_back(c);
                        step = 1;
                        continue;
                    }
                    if(c == '}') {
                        if(mode == 5) {
                            return stdjson_value();
                        }
                        return stdjson_value(jv_object);
                    }
                    return stdjson_value();
                }
                if(step == 1) {
                    r.push_back(c);
                    if(convertOpen) {
                        convertOpen = false;
                        continue;
                    }
                    if(c == '\\') {
                        convertOpen = true;
                        continue;
                    }
                    if(c == '\"') {
                        key = handwrite2string(r);
                        r.clear();
                        mode = 1;
                        step = 0;
                        continue;
                    }
                    continue;
                }
            }
            if(mode == 1) {
                if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                    continue;
                }
                if(c != ':') {
                    return stdjson_value();
                }
                mode = 2;
                step = 0;
                continue;
            }
            if(mode == 2) {
                if(step == 0) {
                    if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        continue;
                    }
                    if(c == '\"') {
                        r.push_back(c);
                        step = 1;
                        continue;
                    } else if(c == '[') {
                        r.push_back(c);
                        sbracketCount ++;
                        step = 3;
                    } else if(c == '{') {
                        r.push_back(c);
                        braceCount ++;
                        step = 3;
                    } else {
                        r.push_back(c);
                        step = 2;
                    }
                    continue;
                }
                if(step == 1) {
                    r.push_back(c);
                    if(convertOpen) {
                        convertOpen = false;
                        continue;
                    }
                    if(c == '\\') {
                        convertOpen = true;
                        continue;
                    }
                    if(c == '\"') {
                        jv_object[key] = handwrite2string(r);
                        key.clear();r.clear();
                        step = 5;
                        continue;
                    }
                    continue;
                }
                if(step == 5) {
                    if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        continue;
                    }
                    if(c == ',') {
                        mode = 5;
                        step = 0;
                        continue;
                    }
                    if(c == '}') {
                        if((i + 1) == end) {
                            return stdjson_value(jv_object);
                        }
                    }
                    return stdjson_value();
                }
                if(step == 2) {
                    if(c == ',' || c == '}') {
                        if(c == '}' && (i + 1) != end) {
                            return stdjson_value();
                        }
                        jv_object[key] = fromJSON(r);
                        if(c == '}' && (i + 1) == end) {
                            return stdjson_value(jv_object);
                        }
                        key.clear();r.clear();
                        mode = 5;
                        step = 0;
                    } else if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        jv_object[key] = fromJSON(r);
                        key.clear();r.clear();
                        step = 5;
                    } else {
                        r.push_back(c);
                    }
                    continue;
                }
                if(step == 3) {
                    if(sbracketCount == 0 && braceCount == 0) {
                        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                            continue;
                        }
                        if(c == ',' || c == '}') {
                            if(c == '}' && (i + 1) != end) {
                                return stdjson_value();
                            }
                            jv_object[key] = fromJSON(r);
                            if(c == '}' && (i + 1) == end) {
                                return stdjson_value(jv_object);
                            }
                            key.clear();r.clear();
                            mode = 5;
                            step = 0;
                            continue;
                        } else {
                            return stdjson_value();
                        }
                    }
                    if(quote2Open) {
                        r.push_back(c);
                        if(convertOpen) {
                            convertOpen = false;
                            continue;
                        }
                        if(c == '\\') {
                            convertOpen = true;
                        } else if(c == '\"') {
                            quote2Open = false;
                        }
                        continue;
                    }
                    r.push_back(c);
                    if(c == '\"') {
                        quote2Open = true;
                        continue;
                    }
                    if(c == '{') {
                        braceCount ++;
                        continue;
                    }
                    if(c == '}') {
                        braceCount --;
                        if(braceCount < 0) {
                            return stdjson_value();
                        }
                        continue;
                    }
                    if(c == '[') {
                        sbracketCount ++;
                        continue;
                    }
                    if(c == ']') {
                        sbracketCount --;
                        if(sbracketCount < 0) {
                            return stdjson_value();
                        }
                        continue;
                    }
                    continue;
                }
            }
        }
        return stdjson_value();
    } else if(fc == '[') {
        std::vector<stdjson_value> jv_array;
        bool quote2Open = false;
        bool convertOpen = false;
        int braceCount = 0;
        int sbracketCount = 0;
        int mode = 0;
        int step = 0;
        std::string r;
        for(int i = offset + 1;i < end; i++) {
            char c = json[i];
            if(mode == 0 || mode == 5) {
                if(step == 0) {
                    if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        continue;
                    }
                    if(c == ']') {
                        if(mode == 5) {
                            return stdjson_value();
                        }
                        return stdjson_value(jv_array);
                    }
                    if(c == '\"') {
                        r.push_back(c);
                        step = 1;
                        continue;
                    }
                    if(c == '[') {
                        r.push_back(c);
                        sbracketCount ++;
                        step = 3;
                        continue;
                    }
                    if(c == '{') {
                        r.push_back(c);
                        braceCount ++;
                        step = 3;
                        continue;
                    }
                    r.push_back(c);
                    step = 2;
                    continue;
                }
                if(step == 1) {
                    r.push_back(c);
                    if(convertOpen) {
                        convertOpen = false;
                        continue;
                    }
                    if(c == '\\') {
                        convertOpen = true;
                        continue;
                    }
                    if(c == '\"') {
                        jv_array.push_back(handwrite2string(r));
                        r.clear();
                        step = 5;
                        continue;
                    }
                    continue;
                }
                if(step == 5) {
                    if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        continue;
                    }
                    if(c == ',') {
                        mode = 5;
                        step = 0;
                        continue;
                    }
                    if(c == ']') {
                        if((i + 1) == end) {
                            return stdjson_value(jv_array);
                        }
                    }
                    return stdjson_value();
                }
                if(step == 2) {
                    if(c == ',' || c == ']') {
                        if(c == ']' && (i + 1) != end) {
                            return stdjson_value();
                        }
                        jv_array.push_back(fromJSON(r));
                        r.clear();
                        if(c == ']' && (i + 1) == end) {
                            return stdjson_value(jv_array);
                        }
                        mode = 5;
                        step = 0;
                    } else if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                        jv_array.push_back(fromJSON(r));
                        r.clear();
                        step = 5;
                    } else {
                        r.push_back(c);
                    }
                    continue;
                }
                if(step == 3) {
                    if(sbracketCount == 0 && braceCount == 0) {
                        if(c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                            continue;
                        }
                        if(c == ',' || c == ']') {
                            if(c == ']' && (i + 1) != end) {
                                return stdjson_value();
                            }
                            jv_array.push_back(fromJSON(r));
                            r.clear();
                            if(c == ']' && (i + 1) == end) {
                                return stdjson_value(jv_array);
                            }
                            mode = 5;
                            step = 0;
                            continue;
                        } else {
                            return stdjson_value();
                        }
                    }
                    if(quote2Open) {
                        r.push_back(c);
                        if(convertOpen) {
                            convertOpen = false;
                            continue;
                        }
                        if(c == '\\') {
                            convertOpen = true;
                        }
                        if(c == '\"') {
                            quote2Open = false;
                        }
                        continue;
                    }
                    r.push_back(c);
                    if(c == '\"') {
                        quote2Open = true;
                        continue;
                    }
                    if(c == '{') {
                        braceCount ++;
                        continue;
                    }
                    if(c == '}') {
                        braceCount --;
                        if(braceCount < 0) {
                            return stdjson_value();
                        }
                        continue;
                    }
                    if(c == '[') {
                        sbracketCount ++;
                        continue;
                    }
                    if(c == ']') {
                        sbracketCount --;
                        if(sbracketCount < 0) {
                            return stdjson_value();
                        }
                        continue;
                    }
                    continue;
                }
            }
        }
        return stdjson_value();
    } else if(fc == '"') {
        return stdjson_value(handwrite2string(json,offset, end));
    } else if(fc >= '0' && fc <= '9') {
        size_t kk = strfind(json,".", offset + 1, end - 1);
        if(kk != std::string::npos) {
            return stdjson_value(atof(json.substr(offset, end - offset).c_str()));
        } else {
            if((end - offset) > 2 && (json[offset + 1] == 'x' || json[offset + 1] == 'X')) {
                return stdjson_value(std::stoll(json.substr(offset, end - offset), nullptr, 16));
            } else {
                return stdjson_value(std::stoll(json.substr(offset, end - offset)));
            }
        }
    } else {
        std::string s = json.substr(offset, end - offset);
        if(s == "true") {
            return stdjson_value(true);
        }
        if(s == "false") {
            return stdjson_value(false);
        }
        return stdjson_value();
    }
}
int strfind(const std::string & s, const std::string & sub, int offset, int end) {
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
std::string handwrite2string(const std::string &s, int offset, int end) {
    if(end == -1 || end > s.size()) end = s.size();
    if(s.find("\\", offset + 1, end - (offset + 2)) != std::string::npos) {
        bool convertOpen = false;
        std::string r;
        for(int i = offset + 1;i < (end - 1); i++) {
            char c = s[i];
            if(convertOpen) {
                if(c == 't') {
                    r.append("\t");
                } else if(c == 'r') {
                    r.append("\r");
                } else if(c == 'n') {
                    r.append("\n");
                } else {
                    r.push_back(c);
                }
                convertOpen = false;
                continue;
            }
            if(c == '\\') {
                convertOpen = true;
            } else {
                r.push_back(c);
            }
        }
        return r;
    } else {
        return s.substr(offset + 1, end - (offset + 2));
    }
}


}
