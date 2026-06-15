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



#ifndef TSCRIPTENGINEIMPL_H
#define TSCRIPTENGINEIMPL_H
#include "tscriptengine.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>


namespace TScript {

/****************************************** Declaration Begin ****************************************************/
class TScriptStatement;
class TScriptStatmemtItem;
class TScriptFunction;
class TScriptClass;
class TScriptModule;
class TScriptProject;
class TScriptTreeNode;

class TScriptGlobalEngine;
class TScriptStatementEngine;
class TScriptFunctionEngine;
class TScriptClassEngine;
class TScriptModuleEngine;
/****************************************** Declaration End ******************************************************/



/****************************************** Definiation Begin ****************************************************/


/****************************************** Class LineToken Begin ************************************************/
template<typename  T>
class   LineNoString
{
public:
    LineNoString():lineNo(0) {}
    LineNoString(const T & s, int lineNo):s(s),lineNo(lineNo) {}
    LineNoString(const LineNoString<T> & ref) {
        s = ref.s;
        lineNo = ref.lineNo;
    }
    LineNoString<T> & operator = (const LineNoString<T> & ref) {
        s = ref.s;
        lineNo = ref.lineNo;
        return *this;
    }
    inline bool operator == (const T & ref) const {
        return s == ref;
    }
    inline bool operator != (const T & ref) const {
        return s != ref;
    }
    inline bool operator == (const LineNoString<T> & ref) const {
        return s == ref.s;
    }
    inline bool operator != (const LineNoString<T> & ref) const {
        return s != ref.s;
    }
    inline operator const T&() {
        return s;
    }
    inline T & get() {
        return s;
    }
    int getLineNo() const {
        return lineNo;
    }
    LineNoString<T> & swap(LineNoString<T> & ref) {
        s.swap(ref.s);
        int no = lineNo;
        lineNo = ref.lineNo;
        ref.lineNo = no;
        return *this;
    }
private:
    T s;
    int lineNo;
};

/****************************************** Class LineToken End **************************************************/

/****************************************** Class RegisteredFunction Begin ***************************************/
class TScriptFile {
public:
    TScriptFile();
    ~TScriptFile();

    static bool exists(const std::string & filepath);
    static size_t length(const std::string & filepath);
    static std::string readText(const std::string & filepath);
    static TScriptByteArray readData(const std::string & filepath, long long pos = 0, long long len = -1);
    static bool writeText(const std::string & filepath, const std::string & text, bool isAppend = false);
    static bool writeData(const std::string & filepath, const TScriptByteArray & byteArray, bool isAppend = false);

    static void mkdirs(const std::string & path);
    static bool isAbsolutePath(const std::string & path);
    static std::string getAbsoluteFile(const std::string & file);
    static std::string getAbsolutePath(const std::string & path);
    static std::string getParentPath(const std::string & file);
    static std::string getSimpleFileName(const std::string & file);
private:
    std::fstream file;
};


class RegisteredFunction {
public:
    RegisteredFunction();
    RegisteredFunction(const RegisteredFunction & ref);
    ~RegisteredFunction();
    void bind(const std::string & name, const user_func0 f, int minParamCount = -1);
    void bind(const std::string & name, const user_func1 f, int minParamCount = -1);
    void bind(const std::string & name, const user_func2 f, int minParamCount = -1);
    void bind(const std::string & name, const user_func3 f, int minParamCount = -1);
    void bind(const std::string & name, const user_func4 f, int minParamCount = -1);
    void bind(const std::string & name, const user_func5 f, int minParamCount = -1);
    void bind(const std::string & name, const user_func6 f, int minParamCount = -1);
    void bind(const std::string & name, const user_func7 f, int minParamCount = -1);

    void bind(const std::string & name, const user_data_func0 f, void * data, int minParamCount = -1);
    void bind(const std::string & name, const user_data_func1 f, void * data, int minParamCount = -1);
    void bind(const std::string & name, const user_data_func2 f, void * data, int minParamCount = -1);
    void bind(const std::string & name, const user_data_func3 f, void * data, int minParamCount = -1);
    void bind(const std::string & name, const user_data_func4 f, void * data, int minParamCount = -1);
    void bind(const std::string & name, const user_data_func5 f, void * data, int minParamCount = -1);
    void bind(const std::string & name, const user_data_func6 f, void * data, int minParamCount = -1);
    void bind(const std::string & name, const user_data_func7 f, void * data, int minParamCount = -1);

    std::string & getName();
    TScriptValue exec(std::vector<TScriptValue> paramList);

    RegisteredFunction & operator = (const RegisteredFunction & ref);
private:
    user_func0  f0;
    user_func1  f1;
    user_func2  f2;
    user_func3  f3;
    user_func4  f4;
    user_func5  f5;
    user_func6  f6;
    user_func7  f7;
    user_data_func0  fd0;
    user_data_func1  fd1;
    user_data_func2  fd2;
    user_data_func3  fd3;
    user_data_func4  fd4;
    user_data_func5  fd5;
    user_data_func6  fd6;
    user_data_func7  fd7;

    std::string name;
    int paramCount;
    int minParamCount;
    bool useUserData;
    void * userData;
};
/****************************************** Class RegisteredFunction End *****************************************/

class TScriptClassEngine;
class TScriptClassObject : public TScriptObject {
public:
    TScriptClassObject();
    ~TScriptClassObject();
    void init(const std::string & className, std::shared_ptr<TScriptObject> & parentObject);

    bool set(const std::string & name, const TScriptValue & value);
    TScriptValue get(const std::string & name);
    std::string getObjectName();

    TScriptValue invoke(const std::string & method, std::vector<TScriptValue> & paramList);

    TScriptClassObject & operator = ( const TScriptClassObject & ref);

    TScriptClassEngine * getEngine();
    void setEngine(std::shared_ptr<TScriptClassEngine> engine);

    TScriptClassObject * getInstanceObject() {
        return (TScriptClassObject*)instanceObject;
    }
    TScriptClassObject * getSuperObject() {
        return (TScriptClassObject*)superObject.get();
    }
private:
    std::shared_ptr<TScriptClassEngine> scriptClassEngine;
    std::string className;
    std::shared_ptr<TScriptObject> superObject;
    bool hasToString;
    std::string sVal;
    TScriptObject * instanceObject;

    friend TScriptClassEngine;
};

/****************************************** Class TScriptExpression Begin ****************************************/
class TScriptTokenLocation {
public:
    TScriptTokenLocation():tokenType(-1){}
    TScriptTokenLocation(int tokenType, const std::string & name):tokenType(tokenType),name(name) {}
    TScriptTokenLocation(const TScriptTokenLocation & ref) {
        tokenType = ref.tokenType;
        name = ref.name;
        scriptModule = ref.scriptModule;
        scriptStatement = ref.scriptStatement;
    }

    TScriptTokenLocation & operator = (const TScriptTokenLocation & ref) {
        tokenType = ref.tokenType;
        name = ref.name;
        scriptModule = ref.scriptModule;
        scriptStatement = ref.scriptStatement;
        return *this;
    }
    int tokenType;
    std::string name;

    TScriptFunction * getScriptFunction();
    TScriptClass * getScriptClass();

    TScriptClass * ownerScriptClass = NULL;
    TScriptModule * scriptModule = NULL;
    TScriptStatement * scriptStatement = NULL;
private:
    TScriptFunction * scriptFunction = NULL;
    TScriptClass * scriptClass = NULL;
};

class TScriptExpression {
public:
    enum EXPRESSION_TYPE {
        EXPRESSION_INVALID = 0, //invalid
        EXPRESSION_OP = 1,      //operator
        EXPRESSION_OPFUNC = 2,      //operator
        EXPRESSION_TOKEN = 3,   //token, FUNC or VAL
        EXPRESSION_FUNC = 4,    //function
        EXPRESSION_PROPERTY = 5,     //property
        EXPRESSION_VARIABLE = 6,     //variable
        EXPRESSION_VALUE = 7,     //value
    };

    static std::string getTypeName(EXPRESSION_TYPE);

    enum TXSIGN_TYPE {
        TXSIGN_LMOV = 201,      //  <<
        TXSIGN_RMOV = 202,      //  >>
        TXSIGN_ADDSET = 203,    //  +=
        TXSIGN_SUBSET = 204,    //  -=
        TXSIGN_MULSET = 205,    //  *=
        TXSIGN_DIVSET = 206,    //  /=
        TXSIGN_MODSET = 207,    //  %=
        TXSIGN_EQUAL = 208,     //  ==
        TXSIGN_AND = 209,       // &&
        TXSIGN_OR = 210,        // ||
        TXSIGN_NOTEQ = 211,    // !=
        TXSIGN_GTEQ = 212,      //  >=
        TXSIGN_LTEQ = 213,      //  <=
        TXSIGN_ADD1 = 214,      //  ++
        TXSIGN_SUB1 = 215,      //  --

        TXSIGN_LMOVSET = 301,   //  <<=
        TXSIGN_RMOVSET = 302,   //  >>=
        TXSIGN_ANDSET = 303,    // &&=
        TXSIGN_ORSET = 304,     // ||=

        TXSIGN_ADD = 101,       // +
        TXSIGN_SUB = 102,       // -
        TXSIGN_MUL = 103,       // *
        TXSIGN_DIV = 104,       // /
        TXSIGN_MOD = 105,       //  %
        TXSIGN_SET = 106,       //  =
        TXSIGN_NOT = 107,       //  !
        TXSIGN_GT = 108,        //  >
        TXSIGN_LT = 109,        //  <
    };

public:
    TScriptExpression();

    EXPRESSION_TYPE getType();
    void asOpFunc();
    void asOp();
    void asFunc();
    void asVariable();
    TScriptExpression & setProperty(const LineNoString<std::string> & token) {this->token = LineNoString<std::string>(token); lineNo = token.getLineNo(); type = EXPRESSION_PROPERTY; return *this;}
    TScriptExpression & setFunc(const LineNoString<std::string> & token) {this->token = LineNoString<std::string>(token); lineNo = token.getLineNo(); type = EXPRESSION_FUNC; return *this;}
    TScriptExpression & setVar(const LineNoString<std::string> & token) {this->token = LineNoString<std::string>(token); lineNo = token.getLineNo(); type = EXPRESSION_VARIABLE; return *this;}
    TScriptExpression & setToken(const LineNoString<std::string> & token) {this->token = LineNoString<std::string>(token); lineNo = token.getLineNo(); type = EXPRESSION_TOKEN; return *this;}
    TScriptExpression & setOp(const TXSIGN_TYPE op) {sign = op; type = EXPRESSION_OP; return *this;}
    TScriptExpression & setOpFunc(const TXSIGN_TYPE op) {sign = op; type = EXPRESSION_OPFUNC; return *this;}
    TScriptExpression & setValue(const TScriptValue & v) {value = v; type = EXPRESSION_VALUE; return *this;}

    bool isInvalid() {
        return (type == EXPRESSION_INVALID);
    }
    bool isOp() {
        return (type == EXPRESSION_OP);
    }
    bool isOpFunc() {
        return (type == EXPRESSION_OPFUNC);
    }
    bool isValue() {
        return (type == EXPRESSION_VALUE);
    }
    bool isFunc() {
        return (type == EXPRESSION_FUNC);
    }
    bool isProperty() {
        return (type == EXPRESSION_PROPERTY);
    }
    bool isToken() {
        return (type == EXPRESSION_TOKEN);
    }
    bool isVariable() {
        return (type == EXPRESSION_VARIABLE);
    }
    bool isSetOp() {
        if(type == EXPRESSION_OP || type == EXPRESSION_OPFUNC) {
            return (sign == TXSIGN_ADD1 || sign == TXSIGN_SUB1
                    || sign == TXSIGN_ADDSET || sign == TXSIGN_SUBSET || sign == TXSIGN_MULSET || sign == TXSIGN_DIVSET || sign == TXSIGN_MODSET
                    || sign == TXSIGN_ANDSET || sign == TXSIGN_ORSET
                    || sign == TXSIGN_LMOVSET || sign == TXSIGN_RMOVSET
                    || sign == TXSIGN_SET);
        }
        return false;
    }
    bool isCompareOp() {
        if(type == EXPRESSION_OP || type == EXPRESSION_OPFUNC) {
            return (sign == TXSIGN_GT || sign == TXSIGN_GTEQ
                    || sign == TXSIGN_LT || sign == TXSIGN_LTEQ || sign == TXSIGN_EQUAL || sign == TXSIGN_NOTEQ);
        }
        return false;
    }


    TScriptValue & getValue();
    TXSIGN_TYPE getOp();
    std::string & getToken();
    bool isConditionToken() {
        return token == "?";
    }
    bool isArrayToken() {
        return token == "[]";
    }
    bool isInternalFuncToken() {
        return token == ".";
    }

    TScriptExpression & operator = (const TScriptExpression & ref);

    TScriptExpression & operator = (const TScriptTokenLocation & ref);

    TScriptTokenLocation & getScriptTokenLocation();

    int getLineNo() {
        return lineNo;
    }
private:
    TScriptValue value;
    EXPRESSION_TYPE type;
    TXSIGN_TYPE sign;
    std::string token;
    int lineNo;
    TScriptTokenLocation scriptTokenLocation;
};
/****************************************** Class TScriptExpression End ******************************************/



/****************************************** Class TScriptTreeNode Begin ******************************************/
class TScriptTreeNode {
public:
    TScriptTreeNode(){
        expression = std::shared_ptr<TScriptExpression>(new TScriptExpression());
        paramList = std::shared_ptr< std::vector<TScriptTreeNode> >(new std::vector<TScriptTreeNode>());
    }
    TScriptTreeNode(const TScriptExpression & e){
        expression = std::shared_ptr<TScriptExpression>(new TScriptExpression(e));
        paramList = std::shared_ptr< std::vector<TScriptTreeNode> >(new std::vector<TScriptTreeNode>());
    }
    TScriptTreeNode(const TScriptTreeNode & ee) {
        expression = ee.expression;
        paramList = ee.paramList;
    }
    ~TScriptTreeNode(){}
    bool isValid() {
        return expression != NULL && expression->getType() != TScriptExpression::EXPRESSION_INVALID;
    }

    TScriptTreeNode &  addParam(TScriptExpression & ee) {
        paramList->push_back(ee);
        return *this;
    }
    TScriptTreeNode & addParam(TScriptTreeNode & ee) {
        paramList->push_back(ee);
        return *this;
    }
    inline int getParamCount() {
        return paramList->size();
    }
    inline TScriptExpression & get() {
        return *expression;
    }
    int getLineNo() {
        int lineNo = 0;
        if(expression != nullptr) {
            lineNo = expression->getLineNo();
        }
        if(lineNo == 0 && paramList != nullptr) {
            if(paramList->size() > 0) {
                for(int i=0;i<paramList->size();i++) {
                    lineNo = (*paramList)[i].getLineNo();
                    if(lineNo != 0) {
                        return lineNo;
                    }
                }
            }
        }
        return lineNo;
    }
    TScriptTreeNode & set(TScriptExpression & e) {
        *expression = e;
        return *this;
    }
    TScriptTreeNode & operator [](int i) {
        if(i < 0 || i >= paramList->size()) {
            throw TScriptException("Line " + std::to_string(getLineNo()) + ":" + u8"从数组中获取元素索引越界");
        }
        return (*paramList)[i];
    }
    TScriptTreeNode & operator << (TScriptTreeNode ee) {
        addParam(ee);
        return *this;
    }
    TScriptTreeNode & operator << (TScriptExpression ee) {
        addParam(ee);
        return *this;
    }
    TScriptTreeNode & operator = (TScriptExpression & e) {
        *expression = e;
        paramList->clear();
        return *this;
    }
    TScriptTreeNode & operator = (const TScriptTreeNode & ee) {
        expression = ee.expression;
        paramList = ee.paramList;
        return *this;
    }
    static TScriptTreeNode & op2TScriptTreeNode(TScriptTreeNode & treeNodeOp, const TScriptTreeNode & treeNodeLeft, const TScriptTreeNode & treeNodeRight);
    static TScriptTreeNode & op2TScriptTreeNode(TScriptTreeNode & treeNodeOp, const TScriptTreeNode & treeNodeValue);
    static TScriptTreeNode internalFunc2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & arrayIndexExpressionList);
    static TScriptTreeNode internalProperty2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & arrayIndexExpressionList);
    static TScriptTreeNode if2TScriptTreeNode(TScriptTreeNode & treeNodeCondition, const std::vector<LineNoString<std::string> > & trueExpressionList, const std::vector<LineNoString<std::string> > & falseExpressionList);
    static TScriptTreeNode arrayElement2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & arrayIndexExpressionList);
    static TScriptTreeNode array2TScriptTreeNode(const std::vector<LineNoString<std::string> > & arrayItemExpressionList);
    static TScriptTreeNode map2TScriptTreeNode(const std::vector<LineNoString<std::string> > & arrayItemExpressionList);
    static TScriptTreeNode function2TScriptTreeNode(TScriptTreeNode & treeNode, const std::vector<LineNoString<std::string> > & paramExpressionList);
    static TScriptTreeNode expressionTokenList2TScriptTreeNode(const std::vector<LineNoString<std::string> > & expressionTokenList);
    static std::vector< std::vector<LineNoString<std::string> > > splitExpressionList(const std::vector<LineNoString<std::string> > & paramExpressionList, const std::string & div);
private:
    std::shared_ptr<TScriptExpression> expression;
    std::shared_ptr< std::vector<TScriptTreeNode> > paramList;
};

/****************************************** Class TScriptTreeNode End ********************************************/


/****************************************** Class  TScriptStatmemtItem Begin *************************************/
class TScriptStatmemtItem {
public:
    TScriptStatmemtItem() {}

    void setToken(const LineNoString<std::string> & token) {
        this->token = token;
    }
    TScriptStatmemtItem & operator << (const std::vector<LineNoString<std::string> > & tokenList) {
        expressionTokenList.push_back(tokenList);
        return *this;
    }
    TScriptStatmemtItem & operator = (const TScriptStatmemtItem & ref) {
        token = ref.token;
        expressionTokenList = ref.expressionTokenList;
        return *this;
    }
    TScriptStatmemtItem &  swap(TScriptStatmemtItem & ref) {
        token.swap(ref.token);
        expressionTokenList.swap(ref.expressionTokenList);
        return *this;
    }
    std::vector<LineNoString<std::string> > & operator [] (int index) {
        return expressionTokenList[index];
    }
    LineNoString<std::string> & getToken() {
        return token;
    }
    int count() {
        return expressionTokenList.size();
    }
private:
    LineNoString<std::string> token;
    std::vector< std::vector<LineNoString<std::string> > > expressionTokenList;
};
/****************************************** Class  TScriptStatmemtItem End ***************************************/


/****************************************** Class  TScriptHelper Begin *******************************************/
class TScriptHelper {
public:
    TScriptHelper();

    static TScriptHelper * getInstance();
    bool isToken(const std::string & s);
    //op包含setSign
    bool isOp(const std::string & sign);
    bool isSetOpSign(const std::string & sign);
    bool isSignPrefix(const std::string & sign);
    bool isCompareOp(const TScriptExpression::TXSIGN_TYPE op);
    bool isSetOp(const TScriptExpression::TXSIGN_TYPE op);
    std::string op2Sign(const TScriptExpression::TXSIGN_TYPE op);
    TScriptExpression::TXSIGN_TYPE sign2Op(const std::string & sign);
    const std::string & simplilySign(const std::string & fromSign);

    std::string removeComments(const std::string & expression);
    void expressionToTokenList(const std::string & expression, std::vector<LineNoString<std::string> > & resultList);
    void expressionTokenToStatmentItemList(const std::vector<LineNoString<std::string> > & expressionTokenList, std::vector<TScriptStatmemtItem> & scriptStatmemtItemList);
    std::string strHandwrite2Val(const std::string & s);
    std::string strVal2Handwrite(const std::string & s);
    std::string char2Handwrite(const char c);
    std::string removeQuoteClose(const std::string & s);

    static TScriptValue & throwException(const std::string & s);
private:
    std::vector<std::string> opList;
    std::map<std::string,int> sign2opMap;
    std::map<int,std::string> op2signMap;
    std::vector<std::string> signsetList;
    std::map<std::string,std::string> sign2simpleMap;
};
/****************************************** Class  TScriptHelper End *********************************************/


/****************************************** Class  TScriptInterface Begin ****************************************/
class TScriptTokenLocations {
public:
    TScriptTokenLocations(){}

    TScriptTokenLocations(const TScriptTokenLocations & ref) {
        valueLocationMap = ref.valueLocationMap;
        funcLocationMap = ref.funcLocationMap;
        classLocationMap = ref.classLocationMap;
    }

    bool isValue(const std::string & name) {
        return (valueLocationMap.find(name) != valueLocationMap.end());
    }
    bool isFunc(const std::string & name) {
        return (valueLocationMap.find(name) != funcLocationMap.end());
    }
    bool isClass(const std::string & name) {
        return (valueLocationMap.find(name) != classLocationMap.end());
    }

    std::map<std::string, TScriptTokenLocation > valueLocationMap;
    std::map<std::string, TScriptTokenLocation > funcLocationMap;
    std::map<std::string, TScriptTokenLocation > classLocationMap;
};


/****************************************** Class  TScriptEngineGlobal Begin *************************************/
class TScriptGlobalEngine
{
public:
    TScriptGlobalEngine();
    ~TScriptGlobalEngine();

    void setBaseDir(const std::string & baseDir);
    std::string & getBaseDir() {
        return baseDir;
    }
    void initVarValue(std::map<std::string,TScriptValue> & varValMap);
    void clearVar();

    TScriptValue evalScript(const std::string & script, bool isScriptFile = false);

    void bindGetVarFunc(get_var_func f){getVarFunc = f;}
    void bindSetVarFunc(set_var_func f){setVarFunc = f;}

    void bindUserFunc(const std::string & name, const user_func0 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserFunc(const std::string & name, const user_func1 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserFunc(const std::string & name, const user_func2 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserFunc(const std::string & name, const user_func3 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserFunc(const std::string & name, const user_func4 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserFunc(const std::string & name, const user_func5 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserFunc(const std::string & name, const user_func6 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserFunc(const std::string & name, const user_func7 f, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,minParamCount);registeredFunctionMap[name] = r;}

    void bindUserDataFunc(const std::string & name, const user_data_func0 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserDataFunc(const std::string & name, const user_data_func1 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserDataFunc(const std::string & name, const user_data_func2 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserDataFunc(const std::string & name, const user_data_func3 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserDataFunc(const std::string & name, const user_data_func4 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserDataFunc(const std::string & name, const user_data_func5 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserDataFunc(const std::string & name, const user_data_func6 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}
    void bindUserDataFunc(const std::string & name, const user_data_func7 f, void*data, int minParamCount = -1){RegisteredFunction r;r.bind(name,f,data,minParamCount);registeredFunctionMap[name] = r;}

    std::vector<std::string> getVarList() {
        std::vector<std::string> rList;
        for(std::map<std::string,TScriptValue>::iterator item = valueMap.begin();item != valueMap.end(); item++) {
            rList.push_back(item->first);
        }
        return rList;
    }
    std::map<std::string,TScriptValue> & getVarMap() {
        return valueMap;
    }

    TScriptValue getVarVal(const std::string & name);
    bool setVarVal(const std::string & name, const TScriptValue & value);

    TScriptValue applyUserFunc(int calllineNo, const std::string & name, std::vector<TScriptValue> & paramList);

    TScriptModuleEngine * getScriptModuleEngine(TScriptModule * scriptModule);

private:
    std::map<std::string,RegisteredFunction> registeredFunctionMap;
    get_var_func getVarFunc;
    set_var_func setVarFunc;

    std::string baseDir;

    std::map<std::string,TScriptValue> valueMap;

    std::map<TScriptModule *, std::shared_ptr<TScriptModuleEngine> > scriptModuleEngineMap;

    std::shared_ptr<TScriptProject> scriptProject;
    void initDefaultFunctions();
};

/****************************************** Class  TScriptEngineGlobal End ***************************************/


/****************************************** Class  TScriptStatement Begin ****************************************/

class TScriptStatement
{
public:
    TScriptStatement(TScriptModule* scriptModule, TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, TScriptStatmemtItem & scriptStatmemtItem);
    TScriptStatement(TScriptModule* scriptModule, TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, std::vector<TScriptStatmemtItem> & scriptStatmemtItemList, TScriptClass * ownerScriptClass = NULL, const std::vector<std::string> & paramNameList= std::vector<std::string>());
    TScriptStatement(TScriptModule* scriptModule, TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, std::vector<LineNoString<std::string> > & expressionTokenList);

    ~TScriptStatement();
    enum STATMENT_TYPE {
        EMPTY = 100,
        SIMPLE = 0,
        MULTIPLE = 1,
        IF = 2,
        FOR = 3,
        WHILE = 4,
        DOWHILE = 5,
        CONTINUE = 6,
        BREAK = 7,
        RETURNNONE = 8,
        RETURNVALUE = 9,
    };
    enum STATMENT_CTRL_TYPE {
        STATMENT_CTRL_NONE = 0,
        STATMENT_CTRL_RETURN = 1,
        STATMENT_CTRL_CONTINUE = 2,
        STATMENT_CTRL_BREAK = 3
    };

    void bindScriptFunction(const std::string & name, std::shared_ptr<TScriptFunction> scriptFunction);
    void bindScriptClass(const std::string & name, std::shared_ptr<TScriptClass> scriptClass);

    TScriptClass * findScriptClass(const std::string & name);
    TScriptFunction * findScriptFunction(const std::string & name);

    int getLineNo() {
        return scriptTreeNode.getLineNo();
    }
    TScriptClass * getOwnerScriptClass() {
        return ownerScriptClass;
    }
private:
    TScriptModule * scriptModule;
    TScriptStatement * ownerScriptStatement;
    TScriptClass * ownerScriptClass;

    std::string initValueName;
    std::vector<std::string> valueNameList;
    STATMENT_TYPE   statmentType;
    TScriptTreeNode scriptTreeNode;
    std::vector<TScriptStatement*> scriptStatementList;
    std::map< std::string,std::shared_ptr<TScriptFunction> > scriptFunctionMap;
    std::map< std::string,std::shared_ptr<TScriptClass> > scriptClassMap;
    std::map< std::string, std::shared_ptr<TScriptModule>> scriptModuleMap;

    void init(TScriptTokenLocations & scriptTokenLocations, std::vector<TScriptStatmemtItem> & scriptStatmemtItemList, TScriptClass * ownerScriptClass = NULL);
    void init(TScriptTokenLocations & scriptTokenLocations, TScriptStatmemtItem & scriptStatmemtItem);

    friend TScriptClass;
    friend TScriptFunction;
    friend TScriptStatementEngine;
    friend TScriptClassEngine;
};

class TScriptStatementEngine
{
public:
    TScriptStatementEngine(TScriptModuleEngine * scriptModuleEngine, TScriptStatementEngine * ownerScriptStatementEngine, TScriptStatement * scriptStatement)
        :scriptModuleEngine(scriptModuleEngine), ownerScriptStatementEngine(ownerScriptStatementEngine),scriptStatement(scriptStatement) {
    }
    ~TScriptStatementEngine(){}

    TScriptStatement * getScriptStatement() {
        return scriptStatement;
    }

    TScriptValue evalStatement();

    void declareVarName(const std::string & name);

    TScriptStatementEngine * getScriptStatementEngine(TScriptStatement * scriptStatement);

    TScriptValue applyUserFunc(int calllineNo, const std::string & name, std::vector<TScriptValue> & paramList);

    TScriptValue eval(TScriptTreeNode & treeNode);
    bool setValue(TScriptTreeNode & treeNode, const TScriptValue & v);

    TScriptValue getVarVal(const std::string & name);
    bool setVarVal(const std::string & name, const TScriptValue & value);
    TScriptValue getVarVal(TScriptTreeNode & treeNode);
    bool setVarVal(TScriptTreeNode & treeNode, const TScriptValue & value);

    virtual TScriptModuleEngine * getScriptModuleEngine(TScriptModule * scriptModule);

    TScriptGlobalEngine * getGlobal();
    TScriptModuleEngine * getModule() { return scriptModuleEngine;}
    TScriptStatementEngine * getOwnerStatementEngine() { return ownerScriptStatementEngine;}

    void setReturnStop(bool isStop) { returnStop = isStop; }
    bool isReturnStop() { return returnStop; }


private:
    TScriptModuleEngine * scriptModuleEngine;
    TScriptStatementEngine * ownerScriptStatementEngine;
    TScriptStatement * scriptStatement;
    TScriptClassEngine * ownerScriptClassEngine = NULL;
    std::map<std::string,TScriptValue> valueMap;
    TScriptStatement::STATMENT_CTRL_TYPE ctrlFlag = TScriptStatement::STATMENT_CTRL_NONE;

    bool returnStop = false;

    TScriptValue evalStatement(TScriptStatement * scriptStatement);
    TScriptValue evalStatementContinue(TScriptStatement * scriptStatement);
    TScriptValue evalStatementBreak(TScriptStatement * scriptStatement);
    TScriptValue evalStatementReturn(TScriptStatement * scriptStatement);
    TScriptValue evalStatementReturnValue(TScriptStatement * scriptStatement);
    TScriptValue evalStatementComplex(TScriptStatement * scriptStatement);

    TScriptValue evalStatementMultiple();
    TScriptValue evalStatementFor();
    TScriptValue evalStatementIf();
    TScriptValue evalStatementWhile();
    TScriptValue evalStatementDoWhile();

    void evalStatementSetReturnMark(TScriptStatement * scriptStatement);

    TScriptValue evalConditionValue(TScriptTreeNode & treeNode);
    TScriptValue evalArrayValue(TScriptTreeNode & treeNode);
    TScriptValue evalInternalFunc(TScriptTreeNode & treeNode);
    TScriptValue evalOpFunc(TScriptTreeNode & treeNode);
    TScriptValue evalPropertyFunc(TScriptTreeNode & treeNode);
    TScriptValue evalScriptFunc(TScriptTreeNode & treeNode);
    TScriptValue evalScriptFuncCreateClass(TScriptTreeNode & treeNode);
    TScriptValue evalScriptFuncClassFunction(TScriptTreeNode & treeNode);
    TScriptValue evalScriptFuncNormalFunction(TScriptTreeNode & treeNode);
    TScriptValue evalScriptFunc4(TScriptTreeNode & treeNode);

    TScriptValue evalOpFuncSet(TScriptTreeNode & treeNode);
    TScriptValue evalOpFuncAndSet(TScriptTreeNode & treeNode);
    TScriptValue evalOpFuncOrSet(TScriptTreeNode & treeNode);
    TScriptValue evalOpFuncSignSet(TScriptTreeNode & treeNode);

    TScriptValue evalOpFuncAnd(TScriptTreeNode & treeNode);
    TScriptValue evalOpFuncOr(TScriptTreeNode & treeNode);
    TScriptValue evalOpFuncSign(TScriptTreeNode & treeNode);

    TScriptValue evalInternalFuncString(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncArray(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncByteArray(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncMap(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncObject(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncChar(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncInt(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncLongLong(TScriptValue & val, TScriptTreeNode & treeNode);
    TScriptValue evalInternalFuncDouble(TScriptValue & val, TScriptTreeNode & treeNode);

    friend TScriptClass;
    friend TScriptFunctionEngine;
    friend TScriptClassEngine;
};


/****************************************** Class  TScriptStatement End ******************************************/


/****************************************** Class TScriptFunction Begin ******************************************/
class TScriptFunction {
public:
    TScriptFunction(TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, const LineNoString<std::string> & name, std::vector<LineNoString<std::string> > & paramList, const std::vector<LineNoString<std::string> > & expressionList);
    ~TScriptFunction();

    const std::string & getName();
    int getLineNo() {
        return name.getLineNo();
    }
    TScriptClass * getOwnerScriptClass() {
        return ownerScriptClass;
    }
private:
    TScriptStatement * ownerScriptStatement;
    TScriptClass * ownerScriptClass;
    LineNoString<std::string> name;
    std::vector<LineNoString<std::string> > paramList;
    std::shared_ptr<TScriptStatement> scriptStatement;
    TScriptTreeNode parentScriptTreeNode;

    void init(TScriptTokenLocations & scriptTokenLocations, const std::vector<LineNoString<std::string> > & expressionList);

    friend TScriptFunctionEngine;
    friend TScriptClassEngine;
    friend TScriptStatement;
};

class TScriptFunctionEngine
{
public:
    TScriptFunctionEngine(TScriptStatementEngine * ownerScriptStatementEngine, TScriptFunction * scriptFunction, std::vector<TScriptValue> & valueList);
    ~TScriptFunctionEngine() {}

    TScriptValue eval();

    TScriptFunction * getScriptFunction() {
        return scriptFunction;
    }

    void initVarVal(const std::string & name, const TScriptValue & value) {
        mainScriptStatementEngine->setVarVal(name, value);
    }
    void initParamValueList(std::vector<TScriptValue> & valueList);

    static TScriptValue applyScriptFunction(TScriptStatementEngine * ownerScriptStatementEngine, TScriptFunction & scriptFunction, std::vector<TScriptValue> & paramList);

private:
    TScriptStatementEngine * ownerScriptStatementEngine;
    TScriptFunction * scriptFunction;

    std::shared_ptr<TScriptStatementEngine> mainScriptStatementEngine;

    friend TScriptClassEngine;
};

/****************************************** Class TScriptFunction End ********************************************/

/****************************************** Class  TScriptClass Begin ********************************************/

class TScriptClass
{
public:
    TScriptClass(TScriptStatement * ownerScriptStatement, TScriptTokenLocations & scriptTokenLocations, const LineNoString<std::string> & className, const std::string & parentClassName, std::vector<LineNoString<std::string> > & expressionTokenList);

    const std::string & getName() { return className; }
    int getLineNo() {
        return className.getLineNo();
    }
private:
    TScriptStatement * ownerScriptStatement;
    LineNoString<std::string> className;
    TScriptTokenLocation parentScriptClassLocation;

    std::shared_ptr<TScriptStatement> scriptStatement;

    void initSymbol(TScriptTokenLocations & scriptTokenLocations);
    void init(TScriptTokenLocations & scriptTokenLocations, std::vector<LineNoString<std::string> > & expressionTokenList);

    friend TScriptClassEngine;
};
/****************************************** Class  TScriptStatement End ******************************************/


/****************************************** Class  TScriptObjectEngine Begin *************************************/
class TScriptClassEngine
{
public:
    TScriptClassEngine(TScriptStatementEngine * ownerScriptStatementEngine, TScriptClass * scriptClass);
    ~TScriptClassEngine();

    void setObject(TScriptClassObject * scriptObject);
    TScriptClassObject * getObject();

    static std::shared_ptr<TScriptObject> create(TScriptStatementEngine * ownerScriptStatementEngine, TScriptClass * scriptClass, std::vector<TScriptValue> & paramList, bool isInstanceObject = true);

    void applyInitFunc(std::vector<TScriptValue> & paramList);

    bool hasVar(const std::string & name);
    TScriptFunction * getInternalFunction(const std::string & name);
    TScriptStatementEngine * getStatementEngine() {
        return scriptStatementEngine.get();
    }
private:
    TScriptStatementEngine * ownerScriptStatementEngine;
    TScriptClass * scriptClass;
    TScriptClassObject * vObj;
    std::shared_ptr<TScriptStatementEngine> scriptStatementEngine;

    friend TScriptClass;
};
/****************************************** Class  TScriptObjectEngine Begin *************************************/


/****************************************** Class  TScriptProject Begin ******************************************/
class TScriptProject {
public:
    TScriptProject();
    ~TScriptProject();

    void addLibPath(const std::string & path);
    std::vector<std::string> & getLibPath() {
        return libPathList;
    }

    TScriptModule * loadMainModule(const std::string & script, bool isScriptFile = false);
    TScriptModule * getMainModule();
    TScriptModule * getModule(const std::string & baseDir, const std::string & importFileName);
private:
    TScriptModule * createModule(const std::string & file);

    TScriptModule * mainScriptModule;//not in moduleMap
    std::map<std::string,TScriptModule *> moduleMap;
    std::vector<std::string> libPathList;
};
/****************************************** Class  TScriptProject End ********************************************/


/****************************************** Class  TScriptModule Begin *******************************************/
class TScriptModule {
public:
    TScriptModule(TScriptProject * scriptProject, const std::string & absoluteFile): scriptProject(scriptProject) {
        baseDir = TScriptFile::getParentPath(absoluteFile);
        moduleAbsFileName = TScriptFile::getSimpleFileName(absoluteFile);
    }
    ~TScriptModule(){
    }

    void loadScript(const std::string & script);
    TScriptModule * getModule(const std::string & file);
    TScriptStatement * getScriptStatement() {
        return scriptStatement.get();
    }
    TScriptProject * getProject() {
        return scriptProject;
    }
private:
    TScriptProject * scriptProject;
    std::string baseDir;
    std::string moduleAbsFileName;
    std::shared_ptr<TScriptStatement> scriptStatement;

    friend TScriptModuleEngine;
};


class TScriptModuleEngine {
public:
    TScriptModuleEngine(TScriptGlobalEngine * scriptGlobalEngine, TScriptModule * scriptModule)
        :scriptGlobalEngine(scriptGlobalEngine),scriptModule(scriptModule)
    {
    }
    virtual ~TScriptModuleEngine() {}

    virtual TScriptValue getVarVal(const std::string & name);
    virtual bool setVarVal(const std::string & name, const TScriptValue & value);

    TScriptGlobalEngine * getGlobal() {
        return scriptGlobalEngine;
    }
    static TScriptModuleEngine * getModuleEngine(TScriptGlobalEngine * scriptGlobalEngine, TScriptModule * scriptModule);

    TScriptValue applyUserFunc(int calllineNo, const std::string & name, std::vector<TScriptValue> & paramList) {
        return getGlobal()->applyUserFunc(calllineNo, name, paramList);
    }

    TScriptStatementEngine * getStatementEngine() {
        return scriptStatementEngine.get();
    }
    TScriptValue evalScript();

protected:
    TScriptGlobalEngine * scriptGlobalEngine;
    TScriptModule * scriptModule;
    std::shared_ptr<TScriptStatementEngine> scriptStatementEngine;
    std::map<std::string,TScriptValue> valueMap;
};

class TScriptMainModuleEngine : public TScriptModuleEngine {
public:
    TScriptMainModuleEngine(TScriptGlobalEngine * scriptGlobalEngine, TScriptModule * scriptModule)
        :TScriptModuleEngine(scriptGlobalEngine, scriptModule)
    {
    }
    ~TScriptMainModuleEngine() override {}

    TScriptValue getVarVal(const std::string & name) override {
        return getGlobal()->getVarVal(name);
    }
    bool setVarVal(const std::string & name, const TScriptValue & value)  override {
        return getGlobal()->setVarVal(name,value);
    }
};
/****************************************** Class  TScriptModule End *********************************************/

/****************************************** Definiation End ******************************************************/


}



#endif // TSCRIPTENGINEIMPL_H
