#ifndef XTHREAD_H
#define XTHREAD_H

#include <QObject>
#include <QThread>
#include <functional>
#include <memory>

namespace XThread {

typedef std::function<void()> XThreadRunning;

class XThread : public QThread
{
    Q_OBJECT
public:
    explicit XThread(XThreadRunning threadRunning, QObject *parent = nullptr);

protected:
    void run() override;
private:
    XThreadRunning threadRunning;
};

}


#endif // XTHREAD_H
