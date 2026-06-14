#ifndef EXPRESSIONENGINECLIENT_H
#define EXPRESSIONENGINECLIENT_H

#include "tscriptengine.h"

#include <functional>

typedef std::function<void(const std::string & msg)>    FOutput;
typedef std::function<void(int cmd, const std::string & message)> FExternalCMD;
typedef std::function<void()> FFunc;
typedef std::function<void(int cmd, FFunc func)> FFuncExec;


class TScriptEngineClient
{
public:
    TScriptEngineClient(FOutput foutput);

    void setProjectPath(const std::string & projectPath);
    void setFuncExec(FFuncExec f);
    void setExternalCMD(FExternalCMD f);
    void eval(const std::string & scriptText, const std::string & jsonValues = "");
    void output(const std::string & msg);
private:
    TScript::TScriptEngine scriptEngine;
    FOutput foutput;
    FExternalCMD fExternalCMD = NULL;
    FFuncExec fFuncExec = NULL;
};

#endif // EXPRESSIONENGINECLIENT_H
