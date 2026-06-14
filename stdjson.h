#ifndef STDJSON_H
#define STDJSON_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace stdjson {

class stdjson_value;
class stdjson_array
{
public:
    stdjson_array();
    stdjson_array(const stdjson_array & v);
    stdjson_array(const std::shared_ptr<std::vector<stdjson_value > > & v);
    stdjson_value & operator [](int i);
    stdjson_value & back();
    stdjson_array & operator << (const stdjson_value & v);
    stdjson_array & operator = (const stdjson_array & v);
    stdjson_array & insert(int pos, const stdjson_value & v);
    stdjson_array & remove(int pos);
    stdjson_array & clear();

    int length() const;
    int size() const;

    bool empty();

    std::string toJSON();
private:
    std::shared_ptr<std::vector<stdjson_value > > jv_array;

    friend stdjson_value;
};

class stdjson_object
{
public:
    stdjson_object();
    stdjson_object(const stdjson_object & v);
    stdjson_object(const std::shared_ptr<std::map<std::string, stdjson_value > > & v);
    stdjson_value & operator [](const std::string & key);
    stdjson_object & operator = (const stdjson_object & v);
    std::vector<std::string> keys() const;
    stdjson_object & remove(const std::string & key);
    stdjson_object & clear();
    bool empty();

    std::string toJSON();
private:
    std::shared_ptr<std::map<std::string, stdjson_value > > jv_object;

    friend stdjson_value;
};


class stdjson_value
{
public:
    enum json_type {
        jt_null = 0,
        jt_bool = 1,
        jt_int = 2,
        jt_longlong = 3,
        jt_double = 4,
        jt_string = 5,
        jt_array = 6,
        jt_object = 7
    };
public:
    stdjson_value();
    stdjson_value(bool v);
    stdjson_value(int v);
    stdjson_value(long long v);
    stdjson_value(double v);
    stdjson_value(const std::string & v);
    stdjson_value(const std::vector<stdjson_value> & v);
    stdjson_value(const std::map<std::string, stdjson_value> & v);
    stdjson_value(const stdjson_value & v);

    json_type getType();
    bool isNull();
    bool isBool();
    bool isInt();
    bool isLongLong();
    bool isDouble();
    bool isString();
    bool isArray();
    bool isObject();
    bool toBool();
    int toInt();
    long long toLong();
    double toDouble();
    std::string toString();
    stdjson_array toArray();
    stdjson_object toObject();
    stdjson_value & operator =(bool v);
    stdjson_value & operator =(int v);
    stdjson_value & operator =(long long v);
    stdjson_value & operator =(double v);
    stdjson_value & operator =(const std::string & v);
    stdjson_value & operator =(const stdjson_array & v);
    stdjson_value & operator =(const stdjson_object & v);
    stdjson_value & operator =(const stdjson_value & v);
    bool equals (const stdjson_value & v);
    bool operator == (const stdjson_value & v);
    bool operator != (const stdjson_value & v);

    std::string toJSON();
    static stdjson_value fromJSON(const std::string & json, int offset = 0, int end = -1);
private:
    json_type type;
    union {
        bool jv_bool;
        int jv_int;
        long long jv_longlong;
        double jv_double;
    };
    std::string jv_string;
    std::shared_ptr<std::vector<stdjson_value > > jv_array;
    std::shared_ptr<std::map<std::string, stdjson_value > > jv_object;
};

}

#endif // STDJSON_H
