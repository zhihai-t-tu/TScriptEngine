#include "XThread.h"

namespace XThread {

XThread::XThread(XThreadRunning threadRunning, QObject *parent) : QThread(parent),threadRunning(threadRunning)
{

}

void XThread::run() {
    if(threadRunning != nullptr) {
        threadRunning();
    }
}

}
