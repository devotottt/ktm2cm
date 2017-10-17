#include "displaypanel.h"
#include "ui_displaypanel.h"

DisplayPanel::DisplayPanel(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DisplayPanel)
{
    ui->setupUi(this);
    masterThread_ = nullptr;
    isConnected_ = false;
    for (int i = 1; i < 5; ++i){
        paramWidgetVector_.push_back(new ParamWidget(this, QString("SM%1").arg(i), -1000, 1000, -200, 200));
        ui->verticalLayout_2->addWidget(paramWidgetVector_.constLast());
    }
    paramWidgetVector_.push_back(new ParamWidget(this, QString("SM5"), -1000, 1000, -300, 300));
    ui->verticalLayout_3->addWidget(paramWidgetVector_.constLast());
    for (int i = 1; i < 5; ++i){
        paramWidgetVector_.push_back(new ParamWidget(this, QString("VP%1").arg(i), -1000, 1000, -200, 200));
        ui->verticalLayout_4->addWidget(paramWidgetVector_.constLast());
    }
    paramWidgetVector_.push_back(new ParamWidget(this, QString("VP5"), -1000, 1000, -300, 300));
    ui->verticalLayout_6->addWidget(paramWidgetVector_.constLast());
    paramWidgetVector_.push_back(new ParamWidget(this, QString("VR"), 0, 30000, 0, 10000));
    ui->verticalLayout_7->addWidget(paramWidgetVector_.constLast());
}

DisplayPanel::~DisplayPanel()
{
    if (isConnected_){
        disconnectFromPort();
        while(masterThread_->isRunning())
            ;
        deleteThread();
    }
    for (int i = 0; i < paramWidgetVector_.length(); ++i)
        delete paramWidgetVector_.at(i);
    delete ui;
}
bool DisplayPanel::connectToPort(const PortSettings& portSettings, QString& errStr){
    serialPort_ = new QSerialPort();
    serialPort_->setPortName(portSettings.portName);
    serialPort_->setBaudRate((QSerialPort::BaudRate)portSettings.baudrate);
    serialPort_->setDataBits((QSerialPort::DataBits)portSettings.dataBits);
    serialPort_->setParity((QSerialPort::Parity)portSettings.parity);
    serialPort_->setStopBits((QSerialPort::StopBits)portSettings.stopBits);
    deviceAddress_ = portSettings.deviceAddress;
    isConnected_ = serialPort_->open(QIODevice::ReadWrite);
    if (isConnected_){
        masterThread_ = new MastersThread(serialPort_, &mutex_, &deviceAddress_);
        masterThread_->moveToThread(masterThread_);
        serialPort_->moveToThread(masterThread_);
        //connect(exThread_, SIGNAL(started()), masterThread_, SLOT(proccess()));
        connect(masterThread_, SIGNAL(response(QVector<int>)), this, SLOT(showResponse(QVector<int>)));
        connect(masterThread_, SIGNAL(error(QString)), this, SLOT(processError(QString)));
        connect(masterThread_, SIGNAL(timeout(QString)), this, SLOT(processTimeout(QString)));
        connect(masterThread_, SIGNAL(finished()), this, SLOT(deleteThread()));
        masterThread_->start();
    }
    else
        errStr = "(Com порт "+portSettings.portName + ") - " + getStringPortError(serialPort_->error());
    this->setEnabled(isConnected_);
    return isConnected_;
}
void DisplayPanel::disconnectFromPort(){
    masterThread_->stop();
}
void DisplayPanel::deleteThread(){
    if(masterThread_ != nullptr)
        delete masterThread_;
    this->setEnabled(false);
    ui->statusLabel->setText(QString("Обмен данными не запущен"));
    isConnected_ = false;
    if(serialPort_ != nullptr)
        delete serialPort_;
}
QString DisplayPanel::getStringPortError(QSerialPort::SerialPortError error){
    QString outStringError;
    switch (error){
        case QSerialPort::NoError :             outStringError = QString("Ошибок нет"); break;
        case QSerialPort::DeviceNotFoundError : outStringError = QString("Порт не найден"); break;
        case QSerialPort::PermissionError :     outStringError = QString("Нет прав на использование порта"); break;
        case QSerialPort::OpenError :           outStringError = QString("Порт занят"); break;
        case QSerialPort::NotOpenError :        outStringError = QString("Необходимо открыть порт"); break;
        case QSerialPort::ParityError :         outStringError = QString("Ошибка чётности"); break;
        case QSerialPort::FramingError :        outStringError = QString("Ошибка фрейма"); break;
        case QSerialPort::BreakConditionError : outStringError = QString("BreakConditionError"); break;
        case QSerialPort::WriteError :          outStringError = QString("Ошибка записи в порт"); break;
        case QSerialPort::ReadError :           outStringError = QString("Ошибка чтения из порта"); break;
        case QSerialPort::ResourceError :       outStringError = QString("Ошибка ресурса порта."); break;
        case QSerialPort::UnsupportedOperationError : outStringError = QString("Неподдерживаемая системой операция"); break;
        case QSerialPort::TimeoutError :        outStringError = QString("Таймаут"); break;
        case QSerialPort::UnknownError :
        default :                               outStringError = QString("Неизвестная ошибка"); break;
    }
    return outStringError;
}
void DisplayPanel::setDeviceAddress(int deviceAddress){
    deviceAddress_ = deviceAddress;
}
void DisplayPanel::showResponse(QVector<int> responseVector){
    ui->statusLabel->setText(QString("Идёт обмен данными..."));
    for (int i = 0; i < responseVector.length() && i < paramWidgetVector_.length(); ++i)
        paramWidgetVector_.at(i)->setValue(responseVector.at(i));

}
void DisplayPanel::processError(const QString &s){
    ui->statusLabel->setText(s);
}
void DisplayPanel::processTimeout(const QString &s){
    ui->statusLabel->setText(s);
}
