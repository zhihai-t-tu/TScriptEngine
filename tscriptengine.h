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



#ifndef TSCRIPT_ENGINE_H
#define TSCRIPT_ENGINE_H

#include <string>
#include <iostream>
#include <stdexcept>
#include <list>
#include <map>
#include <vector>
#include <locale>
#include <codecvt>
#include <functional>
#include <memory>
#include <algorithm>

namespace TScript {


class TScriptException : public std::exception {
public:
    TScriptException(const std::string& msg);

    const std::string & message();
    TScriptException & operator = (const TScriptException & ref);
private:
    std::string error;
};

class TStringHelper {
public:
    static std::string fromWString(std::wstring w);
    static std::wstring toWString(const std::string s);
    static std::string toLower(const std::string & s);
    static std::string toUpper(const std::string & s);
    static int toInt(const std::string & s);
    static long long toLongLong(const std::string & s);
    static double toDouble(const std::string & s);
    static std::string number(bool b);
    static std::string number(int v, int radix = 10);
    static std::string number(long long v, int radix = 10);
    static std::string number(double v);
    static bool startsWith(const std::string & s, const std::string & sub);
    static bool startsWith(const std::string & s, const char * sub);
    static bool endsWith(const std::string & s, const std::string & sub);
    static bool endsWith(const std::string & s, const char * sub);
    static int wstrfind(const std::wstring & s, char c, int offset = 0, int end = -1);
    static int wstrfind(const std::wstring & s, const std::wstring & sub, int offset = 0, int end = -1);
    static int strfind(const std::string & s, char c, int offset = 0, int end = -1);
    static int strfind(const std::string & s, const std::string & sub, int offset = 0, int end = -1);
    static int strlastfind(const std::string & s, char c, int offset = -1);
    static int strlastfind(const std::string & s, const std::string & sub, int offset = -1);
    static std::string & trimLeft(std::string & s);
    static std::string & trimRight(std::string & s);
    static std::string & trim(std::string & s);
    static std::wstring & wtrimLeft(std::wstring & s);
    static std::wstring & wtrimRight(std::wstring & s);
    static std::wstring & wtrim(std::wstring & s);
    static bool isLatinString(std::wstring & s);
    static bool isChar(wchar_t c);
    static std::string fromChar(char c);
    static std::string & replace(std::string & s, std::string & sub, std::string & newstr);
    static std::string & replace(std::string & s, const char * sub, const char * newstr);
    static std::vector<std::string> split(const std::string & s, const std::string & sub);
    static std::vector<std::string> split(const std::string & s, const char * sub);
    static std::string join(const std::vector<std::string> & vector, const std::string div = "");
};


class TCollectHelper {
public:
    template<typename T>
    static std::vector<T> & append(std::vector<T> & vector, const std::vector<T> & other) {
        for(const std::string & s: other) {
            vector.push_back(s);
        }
        return vector;
    }
    template<typename T>
    static bool contains(const std::vector<T> & vector, const T & sub) {
        for(const T & s: vector) {
            if(s == sub) {
                return true;
            }
        }
        return false;
    }
    template<typename T, typename V>
    static bool contains(const std::map<T,V> & m, const T & sub) {
        return (m.find(sub) != m.end());
    }
    template<typename T>
    static std::vector<T> slice(const std::vector<T> & vector, int startPos, int count) {
        std::vector<T> resultVector;
        int len = vector.size();
        int realCount = (count < 0)?(len - startPos):std::min(count, len - startPos);
        for(int i=0;i<realCount;i++) {
            resultVector.push_back(vector[startPos + i]);
        }
        return resultVector;
    }
    template<typename T>
    static std::vector<T> & removeAt(std::vector<T> & vector, int pos) {
        int currIdx = 0;
        for(auto item = vector.begin();item != vector.end();item ++) {
            if(currIdx == pos) {
                vector.erase(item);
                break;
            }
            currIdx ++;
        }
        return vector;
    }
    template<typename T>
    static std::vector<T> & remove(std::vector<T> & vector, T & t) {
        std::vector<std::string> ss;
        for(auto item = vector.begin();item != vector.end();item ++) {
            if((*item) == t) {
                vector.erase(item);
                break;
            }
        }
        return vector;
    }
    template<typename T>
    static std::vector<T> & pop_front(std::vector<T> & vector) {
        return removeAt(vector,0);
    }
};

class TScriptValue;
class TScriptArray {
public:
    TScriptArray();
    TScriptArray(const TScriptArray & ref);
    TScriptArray(const std::vector<TScriptValue> & ref);
    TScriptArray(const std::list<TScriptValue> & ref);

    TScriptArray & resize(int n);
    int length() const;

    TScriptArray & clear();

    int indexOf(TScriptValue & ee);

    const std::vector<TScriptValue> & get();
    void toList(std::list<TScriptValue> & list);
    TScriptValue & at(int i);
    TScriptValue & back();
    TScriptValue & front() ;

    TScriptArray & push_back(const TScriptValue & ee);
    TScriptArray & push_front(const TScriptValue & ee);
    TScriptArray & pop_back();
    TScriptArray & pop_front();
    TScriptArray & setArray(const std::vector<TScriptValue> & eeList);
    TScriptArray & setArray(const std::list<TScriptValue> & eeList);


    TScriptArray & append(const TScriptValue & vv);
    TScriptArray & appendAll(const TScriptArray & ref);
    TScriptArray & appendAll(const std::vector<TScriptValue> & eeList);
    TScriptArray & appendAll(const std::list<TScriptValue> & eeList);

    TScriptArray & removeAt(int i) ;
    TScriptArray & remove(int pos, int n = -1) ;
    TScriptArray & remove(TScriptValue & ee) ;

    TScriptValue & operator [] (int i);

    TScriptArray & operator << (const TScriptValue & ee);
    TScriptArray & operator = (const TScriptArray & ref);
    TScriptArray & operator = (const std::vector<TScriptValue> & ref);
    TScriptArray & operator = (const std::list<TScriptValue> & ref);
    bool equals(const TScriptArray & ref) const;
    bool operator == (const TScriptArray & ref) const;
    bool operator != (const TScriptArray & ref) const;
    int compare(const TScriptArray & ref) const;
    bool operator < (const TScriptArray & ref) const;
    bool operator > (const TScriptArray & ref) const;

    TScriptArray slice(int pos, int n = -1);
    TScriptArray & swap(TScriptArray & ref);

    TScriptArray & sort(bool asc = true);

    std::string & toString();
private:
    std::vector<TScriptValue> valueArray;
    bool hasToString;
    std::string sVal;
};

class TScriptMap {
public:
    TScriptMap();
    TScriptMap(const TScriptMap & ref);

    TScriptMap & putAll(const TScriptMap & ref);
    TScriptMap & put(const std::string & key, const TScriptValue & value);
    TScriptValue & get(const std::string & key);
    bool contains(const std::string & key) const;

    int length() const;
    void clear();
    std::vector<std::string> keys() const;

    TScriptValue & operator[] (const std::string & key);
    TScriptMap & operator = (const TScriptMap & ref);
    bool operator == (const TScriptMap & ref) const;
    bool operator != (const TScriptMap & ref) const;
    int compare(const TScriptMap & ref) const;
    bool operator < (const TScriptMap & ref) const;
    bool operator > (const TScriptMap & ref) const;
    bool equals(const TScriptMap & ref) const;

    TScriptMap & swap(TScriptMap & ref);

    std::string & toString();
private:
    std::map<std::string,TScriptValue> valueMap;
    bool hasToString;
    std::string sVal;
};


class TScriptObject {
public:
    virtual ~TScriptObject(){}

    virtual std::string getObjectName();

    virtual bool set(const std::string & name, const TScriptValue & value);
    virtual TScriptValue get(const std::string & name);

    virtual TScriptValue invoke(const std::string & name, std::vector<TScriptValue> & valueList) = 0;
};



class TScriptByteArray {
public:
    TScriptByteArray();
    TScriptByteArray(const TScriptByteArray & ref);
    ~TScriptByteArray();

    enum TXBUFF_EXPAND_TYPE {
        TXBUFF_EXPAND_HEAD = -1,
        TXBUFF_EXPAND_SIZE = 0,
        TXBUFF_EXPAND_TAIL = 1,
    };

    uint8_t * getData(int pos = 0) const;
    bool empty() const;
    int length() const;
    TScriptByteArray & clear();
    TScriptByteArray & operator = (const TScriptByteArray & ref);
    TScriptByteArray & operator += (const TScriptByteArray & ref);

    TScriptByteArray & push_back (uint8_t b);
    TScriptByteArray & push_front (uint8_t b);
    TScriptByteArray & push_back (const uint8_t * buff, int len);
    TScriptByteArray & push_front (const uint8_t * buff, int len);
    TScriptByteArray & push_back (const TScriptByteArray & ref);
    TScriptByteArray & push_front (const TScriptByteArray & ref);
    TScriptByteArray & insert (int pos, const uint8_t * buff, int len);
    TScriptByteArray & insert (int pos, const TScriptByteArray & ref);

    TScriptByteArray & remove(int pos, int len = -1);
    TScriptByteArray & removeAt(int pos);
    TScriptByteArray & reverse();

    uint8_t operator [] (int pos);
    uint8_t get(int pos);
    void set(int pos, uint8_t b);
    int get(int pos, uint8_t * buff, int len);

    TScriptByteArray& expand(int expandSize, TXBUFF_EXPAND_TYPE direct = TXBUFF_EXPAND_SIZE);
private:
    uint8_t * buff;
    int buffSize;
    int offset;
    int dataLen;
};


class TScriptValue {
public:
    TScriptValue();
    TScriptValue(int v);
    TScriptValue(bool v);
    TScriptValue(char v);
    TScriptValue(long long v);
    TScriptValue(double v);
    TScriptValue(const std::string & v);
    TScriptValue(const TScriptValue & v);
    TScriptValue(const std::vector<TScriptValue> & eeList);
    TScriptValue(const TScriptArray & vv);
    TScriptValue(const TScriptMap & vv);
    TScriptValue(const std::shared_ptr<TScriptByteArray> & vv);
    TScriptValue(const std::shared_ptr<TScriptObject> & vv);

    enum TXVALUE_TYPE {
        TXVALUE_INVALID = 0,
        TXVALUE_NULL = 1,   //null
        TXVALUE_BOOL = 2,    //true or false
        TXVALUE_INT = 3,     //-2^31 ... 2^31-1
        TXVALUE_LONGLONG = 4,//-2^63 ... 2^63-1
        TXVALUE_DOUBLE = 5,  //double
        TXVALUE_CHAR = 6,    //char
        TXVALUE_STRING = 7,  //string
        TXVALUE_ARRAY = 8,   //[]
        TXVALUE_MAP = 9,   //{}
        TXVALUE_OBJECT = 10,   //class instance
        TXVALUE_BYTEARRAY = 11,//bytearray
    };

    static std::string getTypeName(TXVALUE_TYPE type);
    TXVALUE_TYPE getType();

    bool isInvalid() const;
    bool isNull() const;
    bool isBool() const;
    bool isInt() const;
    bool isLongLong() const;
    bool isDouble() const;
    bool isNumber() const;
    bool isChar() const;
    bool isString() const;
    bool isArray() const;
    bool isMap() const;
    bool isObject() const;
    bool isByteArray() const;

    bool toBool() const;
    int toInt() const;
    long long toLongLong() const;
    double toDouble() const;
    std::string & toString();
    const std::string & getConstString() const;
    char toChar() const;
    std::string & getString();
    TScriptArray & getArray();
    TScriptMap & getMap();
    TScriptByteArray & getByteArray();
    std::shared_ptr<TScriptObject> getObject();

    int indexOf(const std::string & s, int start = 0) const;
    int indexOf(const char c, int start = 0) const;
    int lastIndexOf(const std::string & s, int start = -1) const;
    int lastIndexOf(const char c, int start = -1) const;

    TScriptValue & append(const std::string & s);
    TScriptValue & append(const char c);
    TScriptValue & push_front(const std::string & s);
    TScriptValue & push_front(const char c);
    TScriptValue & push_back(const std::string & s);
    TScriptValue & push_back(const char c);
    TScriptValue & pop_back();
    TScriptValue & pop_front();
    TScriptValue & erase(int pos = 0, int count = -1);
    char operator [](int pos) const;

    TScriptValue & setNull();
    TScriptValue & setChar(char c);
    TScriptValue & setBool(bool v);
    TScriptValue & setInt(int v);
    TScriptValue & setLongLong(long long v);
    TScriptValue & setDouble(double v);
    TScriptValue & setString(const std::string & v);
    TScriptValue & setArray(const std::vector<TScriptValue> & eeList);
    TScriptValue & setArray(const TScriptArray & v);
    TScriptValue & setMap(const TScriptMap & v);
    TScriptValue & setByteArray(const std::shared_ptr<TScriptByteArray> & v);
    TScriptValue & setObject(const std::shared_ptr<TScriptObject> & v);

    TScriptValue & clear();
    bool equals(const TScriptValue & ee) const;
    bool operator == (const TScriptValue & ref) const;
    bool operator != (const TScriptValue & ref) const;
    int compare(const TScriptValue & v) const;
    bool operator < (const TScriptValue & v) const;
    bool operator > (const TScriptValue & v) const;

    TScriptValue & operator << (const TScriptValue & v);
    TScriptValue & operator << (char v);
    TScriptValue & operator << (const std::string & v);

    TScriptValue & operator = (bool v);
    TScriptValue & operator = (int v);
    TScriptValue & operator = (long long v);
    TScriptValue & operator = (double v);
    TScriptValue & operator = (char v);
    TScriptValue & operator = (const std::string & v);
    TScriptValue & operator = (const TScriptValue & v);
    TScriptValue & operator = (const TScriptArray & v);
    TScriptValue & operator = (const TScriptMap & v);
    TScriptValue & operator = (const std::shared_ptr<TScriptByteArray>& v);
    TScriptValue & operator = (const std::shared_ptr<TScriptObject>& v);

    static TScriptValue fromString(const std::string & s);

    static const TScriptValue V_INVALID;
private:
    TXVALUE_TYPE vtype;

    union {
        char cVal;
        int iVal;
        long long lVal;
        double dVal;
    };
    bool hasToString;
    std::string sVal;
    std::shared_ptr<TScriptArray> vArray;
    std::shared_ptr<TScriptMap> vMap;
    std::shared_ptr<TScriptObject> vObj;
    std::shared_ptr<TScriptByteArray> vByteArray;
};

typedef std::function<TScriptValue()> user_func0;
typedef std::function<TScriptValue(TScriptValue&)> user_func1;
typedef std::function<TScriptValue(TScriptValue&,TScriptValue&)> user_func2;
typedef std::function<TScriptValue(TScriptValue&,TScriptValue&,TScriptValue&)> user_func3;
typedef std::function<TScriptValue(TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_func4;
typedef std::function<TScriptValue(TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_func5;
typedef std::function<TScriptValue(TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_func6;
typedef std::function<TScriptValue(TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_func7;

typedef std::function<TScriptValue(void *)> user_data_func0;
typedef std::function<TScriptValue(void *,TScriptValue&)> user_data_func1;
typedef std::function<TScriptValue(void *,TScriptValue&,TScriptValue&)> user_data_func2;
typedef std::function<TScriptValue(void *,TScriptValue&,TScriptValue&,TScriptValue&)> user_data_func3;
typedef std::function<TScriptValue(void *,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_data_func4;
typedef std::function<TScriptValue(void *,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_data_func5;
typedef std::function<TScriptValue(void *,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_data_func6;
typedef std::function<TScriptValue(void *,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&,TScriptValue&)> user_data_func7;

typedef std::function<TScriptValue(const std::string & name)> get_var_func;
typedef std::function<void(const std::string & name, const TScriptValue & ee)> set_var_func;

class TScriptGlobalEngine;
class TScriptEngine
{
public:
    TScriptEngine();
    ~TScriptEngine();

    void setBaseDir(const std::string & baseDir);


    void initVarValue(std::map<std::string,TScriptValue> & varValMap);
    void clearVar();

    TScriptValue evalScript(const std::string & script, bool isScriptFile = false);

    void bindGetVarFunc(get_var_func f);
    void bindSetVarFunc(set_var_func f);

    void bindUserFunc(const std::string & name, const user_func0 f, int minParamCount = -1);
    void bindUserFunc(const std::string & name, const user_func1 f, int minParamCount = -1);
    void bindUserFunc(const std::string & name, const user_func2 f, int minParamCount = -1);
    void bindUserFunc(const std::string & name, const user_func3 f, int minParamCount = -1);
    void bindUserFunc(const std::string & name, const user_func4 f, int minParamCount = -1);
    void bindUserFunc(const std::string & name, const user_func5 f, int minParamCount = -1);
    void bindUserFunc(const std::string & name, const user_func6 f, int minParamCount = -1);
    void bindUserFunc(const std::string & name, const user_func7 f, int minParamCount = -1);

    void bindUserDataFunc(const std::string & name, const user_data_func0 f, void*data, int minParamCount = -1);
    void bindUserDataFunc(const std::string & name, const user_data_func1 f, void*data, int minParamCount = -1);
    void bindUserDataFunc(const std::string & name, const user_data_func2 f, void*data, int minParamCount = -1);
    void bindUserDataFunc(const std::string & name, const user_data_func3 f, void*data, int minParamCount = -1);
    void bindUserDataFunc(const std::string & name, const user_data_func4 f, void*data, int minParamCount = -1);
    void bindUserDataFunc(const std::string & name, const user_data_func5 f, void*data, int minParamCount = -1);
    void bindUserDataFunc(const std::string & name, const user_data_func6 f, void*data, int minParamCount = -1);
    void bindUserDataFunc(const std::string & name, const user_data_func7 f, void*data, int minParamCount = -1);

    std::vector<std::string> getVarList();
    std::map<std::string,TScriptValue> & getVarMap();
private:
    TScriptGlobalEngine * scriptEngine;
};

}


#endif // TSCRIPT_ENGINE_H
