#pragma once

#include <QtWidgets/QMainWindow>
#include <QCloseEvent>
#include "ui_ScriptEngineApp.h"
#include "XThread.h"
#include "expressionengineclient.h"

class ScriptEngineApp : public QMainWindow
{
    Q_OBJECT

public:
    ScriptEngineApp(QWidget *parent = Q_NULLPTR);
    ~ScriptEngineApp();

    void debug(const QString & message);

protected:
    void closeEvent(QCloseEvent *event) override;

Q_SIGNALS:
    void externalCMD(int cmd, const QString & msg);

    void funcExec(FFunc func);

private slots:
    void on_pushButtonExecute_clicked();

    void on_pushButtonClear_clicked();

    void on_pushButtonClose_clicked();

    void on_pushButtonToJSON_clicked();

    void on_externalCMD(int cmd, const QString & msg);

    void on_FuncExec(FFunc func);
    void on_pushButtonSetProjectPath_clicked();

private:
    Ui::ScriptEngineAppClass ui;

    std::shared_ptr<XThread::XThread> xThread;
};
