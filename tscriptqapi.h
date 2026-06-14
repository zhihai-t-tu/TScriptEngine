#ifndef TSCRIPTQAPI_H
#define TSCRIPTQAPI_H

#include "tscriptengine.h"
#include "XThread.h"
#include <QMutex>

using namespace TScript;

class TScriptNativeThreadObject : public TScriptNativeObject {
public:
    TScriptNativeThreadObject(const std::shared_ptr<TScriptNativeObject> & scriptThreadObject) : scriptThreadObject(scriptThreadObject) {
        thread = std::shared_ptr<XThread::XThread>(new XThread::XThread([this]() {
            std::vector<TScriptValue> valueList;
            this->scriptThreadObject->nativeMethod("run",valueList);
        }));
    }
    virtual ~TScriptNativeThreadObject(){}
    virtual std::string getObjectName() override {
        return "ThreadNativeObject";
    }
    virtual bool set(const std::string & name, const TScriptValue & value) override {
        if(name == "priority") {
            QThread::Priority p = QThread::Priority::InheritPriority;
            int priority = value.toInt();
            if(priority == 0) p = QThread::Priority::IdlePriority;
            if(priority == 1) p = QThread::Priority::LowestPriority;
            if(priority == 2) p = QThread::Priority::LowPriority;
            if(priority == 3) p = QThread::Priority::NormalPriority;
            if(priority == 4) p = QThread::Priority::HighPriority;
            if(priority == 5) p = QThread::Priority::HighestPriority;
            if(priority == 6) p = QThread::Priority::TimeCriticalPriority;
            if(priority == 7) p = QThread::Priority::InheritPriority;
            thread->setPriority(p);
            return true;
        }
        if(name == "stackSize") {
            thread->setStackSize(value.toInt());
            return true;
        }
        throw TScriptException(getObjectName() + u8"." + name + " is unsupported");
    }
    virtual TScriptValue get(const std::string & name) override {
        if(name == "priority") {
            return thread->priority();
        }
        if(name == "stackSize") {
            return (int)thread->stackSize();
        }
        if(name == "isRunning") {
            return thread->isRunning();
        }
        if(name == "isFinished") {
            return thread->isFinished();
        }
        if(name == "isInterruptionRequested") {
            return thread->isInterruptionRequested();
        }
        throw TScriptException(getObjectName() + u8"." + name + " is unsupported");
    }

    virtual TScriptValue nativeMethod(const std::string & name, std::vector<TScriptValue> & valueList) override {
        if(name == "start") {
            thread->start();
            return true;
        } else if(name == "wait") {
            unsigned long ltime = ULONG_MAX;
            if(valueList.size() == 1) {
                ltime = valueList[0].toLongLong();
            }
            return thread->wait(ltime);
        } else if(name == "terminate") {
            thread->terminate();
            return true;
        }
        throw TScriptException(getObjectName() + u8"." + name + " is unsupported");
    }
private:
    std::shared_ptr<TScriptNativeObject> scriptThreadObject;
    std::shared_ptr<XThread::XThread> thread;
};



class TScriptNativeMutexObject : public TScriptNativeObject {
public:
    TScriptNativeMutexObject(){}
    virtual ~TScriptNativeMutexObject(){
        //mutex.unlock();
    }
    virtual std::string getObjectName() override {
        return "MutexNativeObject";
    }
    virtual bool set(const std::string & name, const TScriptValue & value) override {
        throw TScriptException(getObjectName() + u8"." + name + " is unsupported");
    }
    virtual TScriptValue get(const std::string & name) override {
        if(name == "isRecursive") {
            return mutex.isRecursive();
        }
        throw TScriptException(getObjectName() + u8"." + name + " is unsupported");
    }
    virtual TScriptValue nativeMethod(const std::string & name, std::vector<TScriptValue> & valueList) override {
        if(name == "lock") {
            mutex.lock();
        } else if(name == "unlock") {
            mutex.unlock();
        } else if(name == "tryLock") {
            if(valueList.size() == 1) {
                return mutex.tryLock(valueList[0].toInt());
            } else {
                return mutex.tryLock();
            }
        } else {
            throw TScriptException(getObjectName() + u8"." + name + " is unsupported");
        }
        return true;
    }
private:
    QMutex mutex;
};


#endif // TSCRIPTQAPI_H
