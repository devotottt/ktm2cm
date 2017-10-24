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
        paramWidgetVector_.push_back(new ParamWidget(this, QString("IM1%1").arg(i), 0, 31.25, 0, 15, 2));
        ui->verticalLayout_2->addWidget(paramWidgetVector_.constLast());
    }
    paramWidgetVector_.push_back(new ParamWidget(this, QString("IM15"), 0, 62.5, 0, 15, 2));
    ui->verticalLayout_9->addWidget(paramWidgetVector_.constLast());
    for (int i = 1; i < 5; ++i){
        paramWidgetVector_.push_back(new ParamWidget(this, QString("IM2%1").arg(i), 0, 31.25, 0, 15, 2));
        ui->verticalLayout_3->addWidget(paramWidgetVector_.constLast());
    }
    paramWidgetVector_.push_back(new ParamWidget(this, QString("IM25"), 0, 62.5, 0, 15, 2));
    ui->verticalLayout_10->addWidget(paramWidgetVector_.constLast());
    for (int i = 1; i < 5; ++i){
        paramWidgetVector_.push_back(new ParamWidget(this, QString("MPR%1").arg(i), 0, 1, 0, 0.20, 2));
        ui->verticalLayout_12->addWidget(paramWidgetVector_.constLast());
    }
    paramWidgetVector_.push_back(new ParamWidget(this, QString("MPR5"), 0, 1, 0, 0.25, 2));
    ui->verticalLayout_13->addWidget(paramWidgetVector_.constLast());

    for (int i = 1; i < 6; ++i){
        paramWidgetVector_.push_back(new ParamWidget(this, QString("AMPR%1").arg(i), 0, 5, 0, 2, 2));
        paramWidgetVector_.constLast()->hide();
    }
    for (int i = 1; i < 6; ++i){
        paramWidgetVector_.push_back(new ParamWidget(this, QString("WMPR%1").arg(i), 0, 5, 0, 2, 2));
        paramWidgetVector_.constLast()->hide();
    }

    for (int i = 1; i < 6; ++i){
        paramWidgetVector_.push_back(new ParamWidget(this, QString("TR%1").arg(i), -273, 227, 5, 85, 0));
        ui->verticalLayout_14->addWidget(paramWidgetVector_.constLast());
    }
    paramWidgetVector_.push_back(new ParamWidget(this, QString("UPS"), 0, 500, 0, 420, 0));
    ui->verticalLayout_15->addWidget(paramWidgetVector_.constLast());
    paramWidgetVector_.push_back(new ParamWidget(this, QString("IPS"), 0, 12.5, 0, 10, 2));
    ui->verticalLayout_16->addWidget(paramWidgetVector_.constLast());
    paramWidgetVector_.push_back(new ParamWidget(this, QString("IPR"), 0, 6.25, 0, 2, 2));
    ui->verticalLayout_17->addWidget(paramWidgetVector_.constLast());
    paramWidgetVector_.push_back(new ParamWidget(this, QString("FZ"), 0, 50, 0, 50, 2));
    ui->verticalLayout_18->addWidget(paramWidgetVector_.constLast());


    b71WidgetVector_.push_back(new DigitalParamWidget(this, QString("Отключение магнитного подвеса"), QString("MPO"), false, false, 5));
    b71WidgetVector_.push_back(new DigitalParamWidget(this, QString("Включение магнитного подвеса"), QString("MPV"), false, false, 4));
    b71WidgetVector_.push_back(new DigitalParamWidget(this, QString("Включение канала 5 (осевой)"), QString("MPV5"), false, false, 3));
    b71WidgetVector_.push_back(new DigitalParamWidget(this, QString("Включение каналов 3 и 4"), QString("MPV3"), false, false, 2));
    b71WidgetVector_.push_back(new DigitalParamWidget(this, QString("Включение каналов 1 и 2"), QString("MPV1"), false, false, 1));

    b72WidgetVector_.push_back(new DigitalParamWidget(this, QString("Аварийно-высокое виброперем. ротора"), QString("MPA"), true, true, 6));
    b72WidgetVector_.push_back(new DigitalParamWidget(this, QString("Повышенное виброперем. ротора "), QString("MPP"), true, true, 5));
    b72WidgetVector_.push_back(new DigitalParamWidget(this, QString("Канал 1"), QString("PP1"), true, true, 4));
    b72WidgetVector_.push_back(new DigitalParamWidget(this, QString("Канал 2"), QString("PP2"), true, true, 3));
    b72WidgetVector_.push_back(new DigitalParamWidget(this, QString("Канал 3"), QString("PP3"), true, true, 2));
    b72WidgetVector_.push_back(new DigitalParamWidget(this, QString("Канал 4"), QString("PP4"), true, true, 1));
    b72WidgetVector_.push_back(new DigitalParamWidget(this, QString("Канал 5"), QString("PP5"), true, true, 0));

    b75WidgetVector_.push_back(new DigitalParamWidget(this, QString("Наличие резервного питания"), QString("PRS"), true, true, 3));
    b75WidgetVector_.push_back(new DigitalParamWidget(this, QString("Включение основного питания"), QString("S38"), true, false, 2));
    b75WidgetVector_.push_back(new DigitalParamWidget(this, QString("Включение резервного питания"), QString("S22"), true, false, 1));
    for (int i = 0; i < b71WidgetVector_.length(); ++i)
        ui->verticalLayout_5->addWidget(b71WidgetVector_.at(i));
    for (int i = 0; i < b72WidgetVector_.length(); ++i)
        ui->verticalLayout_8->addWidget(b72WidgetVector_.at(i));
    for (int i = 0; i < b75WidgetVector_.length(); ++i)
        ui->verticalLayout_6->addWidget(b75WidgetVector_.at(i));
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
    ui->statusLabel->setStyleSheet("QLabel { background-color : transparent; color : black; }");
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
    mutex_.lock();
    deviceAddress_ = deviceAddress;
    mutex_.unlock();
}
void DisplayPanel::showResponse(QVector<int> responseVector){
    ui->statusLabel->setText(QString("Идёт обмен данными..."));
    for (int i = 0; i < responseVector.length() && i < paramWidgetVector_.length(); ++i)
        paramWidgetVector_.at(i)->setValue(responseVector.at(i));

    if (responseVector.length() < 37)
        return;
    for (int i = 0; i < b71WidgetVector_.length(); ++i)
        b71WidgetVector_.at(i)->setValue((responseVector.at(34) >> 8) & 0xff);
    for (int i = 0; i < b72WidgetVector_.length(); ++i)
        b72WidgetVector_.at(i)->setValue((responseVector.at(34)) & 0xff);
    for (int i = 0; i < b75WidgetVector_.length(); ++i)
        b75WidgetVector_.at(i)->setValue((responseVector.at(36) >> 8) & 0xff);

    ui->statusLabel->setStyleSheet("QLabel { background-color : transparent; color : black; }");
}
void DisplayPanel::processError(const QString &s){
    ui->statusLabel->setText(s);
    ui->statusLabel->setStyleSheet("QLabel { background-color : #c54242; color : white; }");
}
void DisplayPanel::processTimeout(const QString &s){
    ui->statusLabel->setText(s);
    ui->statusLabel->setStyleSheet("QLabel { background-color : #c54242; color : white; }");
}
