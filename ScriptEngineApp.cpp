#include "ScriptEngineApp.h"

#include "expressionengineclient.h"
#include <QDateTime>
#include <QMessageBox>
#include <QFileDialog>

ScriptEngineApp::ScriptEngineApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.comboBoxEngine->clear();
    ui.comboBoxEngine->addItem(u8"TScriptEngine");

    ui.progressBar->setValue(0);
    connect(this, &ScriptEngineApp::externalCMD, this, &ScriptEngineApp::on_externalCMD);
    connect(this, &ScriptEngineApp::funcExec, this, &ScriptEngineApp::on_FuncExec, Qt::BlockingQueuedConnection);
}
ScriptEngineApp::~ScriptEngineApp() {

}
void ScriptEngineApp::closeEvent(QCloseEvent *event) {
    if(xThread != nullptr) {
        if(xThread->isRunning()) {
            QMessageBox::information(nullptr, u8"提示", u8"脚本正在执行，请等待");
            event->ignore();
            return;
        }
    }
    event->accept();
}

void ScriptEngineApp::on_FuncExec(FFunc func) {
    func();
}
void ScriptEngineApp::on_externalCMD(int cmd, const QString & msg) {
    if(cmd == 2) {
        QStringList sl = msg.split(";");
        QMap<QString,QString> sMap;
        for(QString & s: sl) {
            QStringList vl = s.split("=");
            if(vl.size() == 2) {
                sMap[vl[0]] = vl[1];
            }
        }
        int value = sMap["value"].toInt();
        QString maxValue = sMap["maxValue"];
        if(maxValue.indexOf(".") > 0) {
            maxValue = maxValue.mid(0, maxValue.indexOf("."));
        }
        ui.progressBar->setMaximum(maxValue.toInt());
        ui.progressBar->setValue(value);
    } else {
        debug(msg);
    }
}
void ScriptEngineApp::on_pushButtonExecute_clicked()
{
    if(xThread != nullptr) {
        if(xThread->isRunning()) {
            QMessageBox::information(nullptr, u8"提示", u8"脚本正在执行");
            return;
        }
    }
    xThread = std::shared_ptr<XThread::XThread>(new XThread::XThread([this](){
        QString scriptText = ui.textEditScript->toPlainText();

        long long msStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
        TScriptEngineClient client([this](const std::string & msg){
            emit externalCMD(0, QString::fromStdString(msg));
        });
        client.setExternalCMD([this](int cmd, const std::string & msg){
            if(cmd == 2) {
                emit externalCMD(2, QString::fromStdString(msg));
            } else {
                emit externalCMD(0, QString::fromStdString(msg));
            }
        });
        client.setFuncExec([this](int cmd, FFunc func){
            if(cmd == 1) {
                emit funcExec(func);
            }
        });
        client.setProjectPath(this->ui.lineEditProjectPath->text().toStdString());
        client.eval(scriptText.toStdString());
        long long msStop = QDateTime::currentDateTime().toMSecsSinceEpoch();
        emit externalCMD(0, "Total ms:" + QString::number(msStop - msStart));
    }));
    xThread->start();
}


void ScriptEngineApp::on_pushButtonClear_clicked()
{
    ui.plainTextEditOutput->clear();
}


void ScriptEngineApp::on_pushButtonClose_clicked()
{
    if(xThread != nullptr) {
        if(xThread->isRunning()) {
            QMessageBox::information(nullptr, u8"提示", u8"脚本正在执行，请等待");
            return;
        }
    }
    close();
}

void ScriptEngineApp::debug(const QString & message)
{
    ui.plainTextEditOutput->appendPlainText(message);
}

#include "stdjson.h"
void ScriptEngineApp::on_pushButtonToJSON_clicked()
{
    stdjson::stdjson_array stdarray;
    stdarray << (int)1;
    stdarray << (int)2;
    stdarray.insert(-1,3);
    QString text = ui.textEditScript->toPlainText();
    stdjson::stdjson_value jvalue = stdjson::stdjson_value::fromJSON(text.toStdString());
    ui.textEditScript->setText(QString::fromStdString(stdarray.toJSON()));
}



void ScriptEngineApp::on_pushButtonSetProjectPath_clicked()
{
    QString projectPath = QFileDialog::getExistingDirectory(NULL,u8"选择工程目录",".");
    if(projectPath != "") {
        ui.lineEditProjectPath->setText(projectPath);
    }
}

