#include "expressionengineclient.h"
#include "tscriptqapi.h"
#include <QMessageBox>

using namespace TScript;

class TScriptNativeQMessageObject: public TScriptNativeObject
{
public:
    TScriptNativeQMessageObject(TScriptEngineClient * scriptEngineClient, FFuncExec funcExec):scriptEngineClient(scriptEngineClient),funcExec(funcExec) {
    }
    ~TScriptNativeQMessageObject() override {
    }
    virtual std::string getObjectName() override {
        return "QMessageBoxNativeObject";
    }

    virtual TScriptValue nativeMethod(const std::string & name, std::vector<TScriptValue> & valueList) override {
        if(name == "information") {
            if(funcExec != NULL) {
                funcExec(1, [this,&valueList]() {
                    QMessageBox::information(nullptr, QString::fromStdString(valueList[0].toString()), QString::fromStdString(valueList[1].toString()));
                    scriptEngineClient->output("information finish");
                });
            }
        }
        if(name == "question") {
            if(funcExec != NULL) {
                funcExec(1, [this,&valueList]() {
                    bool res = (QMessageBox::question(nullptr, QString::fromStdString(valueList[0].toString()), QString::fromStdString(valueList[1].toString())) == QMessageBox::Yes);
                    if(res) {
                        scriptEngineClient->output("Yes");
                    } else {
                        scriptEngineClient->output("No");
                    }
                });
            }

        }
        throw TScriptException(getObjectName() + u8"." + name + " is unsupported");
    }
private:
    TScriptEngineClient * scriptEngineClient;
    FFuncExec funcExec;
};
#include "XThread.h"


TScriptEngineClient::TScriptEngineClient(FOutput foutput):foutput(foutput)
{
    scriptEngine.bindUserFunc(u8"sleep", [](TScript::TScriptValue & value)->TScript::TScriptValue{
        QThread::sleep(value.toLongLong());
        return TScript::TScriptValue();
    });
    scriptEngine.bindUserFunc(u8"msleep", [](TScript::TScriptValue & value)->TScript::TScriptValue{
        QThread::msleep(value.toLongLong());
        return TScript::TScriptValue();
    });
    scriptEngine.bindUserFunc(u8"output", [this](TScript::TScriptValue & value)->TScript::TScriptValue{
        output(value.toString());
        return TScript::TScriptValue();
    });
    scriptEngine.bindUserFunc(u8"debug", [this](TScript::TScriptValue & value)->TScript::TScriptValue{
        output(value.toString());
        return TScript::TScriptValue();
    });
    scriptEngine.bindUserFunc(u8"externalCMD", [this](TScript::TScriptValue & cmd, TScript::TScriptValue & value)->TScript::TScriptValue{
        TScript::TScriptValue v;
        if(this->fExternalCMD != nullptr) {
            fExternalCMD(cmd.toInt(), value.toString());
        }
        return v;
    });
    scriptEngine.bindUserFunc(u8"QMessageBox", [this]()->TScript::TScriptValue {
        TScript::TScriptValue v(std::shared_ptr<TScriptNativeObject>(new TScriptNativeQMessageObject(this, [this](int cmd, FFunc func) {
			if (fFuncExec != NULL) {
				fFuncExec(cmd, func);
			}
        })));
		return v;
	});
    scriptEngine.bindUserFunc(u8"CreateNativeThread", [](TScript::TScriptValue & threadObject)->TScript::TScriptValue{
        return std::shared_ptr<TScriptNativeObject>(new TScriptNativeThreadObject(threadObject.getNativeObject()));
    });
    scriptEngine.bindUserFunc(u8"CreateNativeMutex", []()->TScript::TScriptValue{
        return std::shared_ptr<TScriptNativeObject>(new TScriptNativeMutexObject());
    });
}

void TScriptEngineClient::eval(const std::string & scriptText, const std::string & jsonValues) {
    scriptEngine.clearVar();
    try {
        scriptEngine.evalScript(scriptText);
    }
    catch(TScript::TScriptException& e) {
        output(e.message());
    }
}

void TScriptEngineClient::output(const std::string & msg) {
    if(foutput != nullptr) {
        foutput(msg);
    }
}

void TScriptEngineClient::setExternalCMD(FExternalCMD f) {
    fExternalCMD = f;
}
void TScriptEngineClient::setFuncExec(FFuncExec f) {
    fFuncExec = f;
}

void TScriptEngineClient::setProjectPath(const std::string & projectPath) {
    scriptEngine.setBaseDir(projectPath);
}
