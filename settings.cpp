#include "settings.h"
#include "ui_settings.h"



Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    initializeSettingsLists();
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onBtnClickOk()));
}

Settings::~Settings()
{
    delete ui;
}
void Settings::show(){
    setSettingsToFrom();
    this->exec();
}
void Settings::initializeSettingsLists(){
    ui->portBaudrateSelect->addItem(QString("1200"), QSerialPort::Baud1200);
    ui->portBaudrateSelect->addItem(QString("2400"), QSerialPort::Baud2400);
    ui->portBaudrateSelect->addItem(QString("4800"), QSerialPort::Baud4800);
    ui->portBaudrateSelect->addItem(QString("9600"), QSerialPort::Baud9600);
    ui->portBaudrateSelect->addItem(QString("19200"), QSerialPort::Baud19200);
    ui->portBaudrateSelect->addItem(QString("38400"), QSerialPort::Baud38400);
    ui->portBaudrateSelect->addItem(QString("57600"), QSerialPort::Baud57600);
    ui->portBaudrateSelect->addItem(QString("115200"), QSerialPort::Baud115200);

    ui->portDataBitsSelect->addItem(QString("5"), QSerialPort::Data5);
    ui->portDataBitsSelect->addItem(QString("6"), QSerialPort::Data6);
    ui->portDataBitsSelect->addItem(QString("7"), QSerialPort::Data7);
    ui->portDataBitsSelect->addItem(QString("8"), QSerialPort::Data8);

    ui->portParitySelect->addItem(QString("Отсутсвует"), QSerialPort::NoParity);
    ui->portParitySelect->addItem(QString("Even"), QSerialPort::EvenParity);
    ui->portParitySelect->addItem(QString("Odd"), QSerialPort::OddParity);
    ui->portParitySelect->addItem(QString("Space"), QSerialPort::SpaceParity);
    ui->portParitySelect->addItem(QString("Mark"), QSerialPort::MarkParity);

    ui->portStopBitsSelect->addItem(QString("1"), QSerialPort::OneStop);
    ui->portStopBitsSelect->addItem(QString("1.5"), QSerialPort::OneAndHalfStop);
    ui->portStopBitsSelect->addItem(QString("2"), QSerialPort::TwoStop);

}
void Settings::getSettingsFromConfig(PortSettings& portSettings){
    QSettings config_file(QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat);
    config_file.beginGroup("port");
    portSettings.portName =   config_file.value(QString("number"), 1).toString();
    portSettings.baudrate = config_file.value(QString("baudrate"), (int)QSerialPort::Baud9600).toInt();
    portSettings.dataBits = config_file.value(QString("databits"), (int)QSerialPort::Data8).toInt();
    portSettings.parity =   config_file.value(QString("parity"), (int)QSerialPort::NoParity).toInt();
    portSettings.stopBits = config_file.value(QString("stopbits"), (int)QSerialPort::OneStop).toInt();
    portSettings.deviceAddress = config_file.value(QString("address"), 1).toInt();
    config_file.endGroup();
}
void Settings::setSettingsToFrom(){
    int selectIndex;
    PortSettings portSettings;
    ui->portNumberSelect->clear();
    getSettingsFromConfig(portSettings);
    QList<QSerialPortInfo> comPortList = QSerialPortInfo::availablePorts();
    for (int i = 0; i < comPortList.size(); ++i){
        ui->portNumberSelect->addItem(comPortList[i].portName(), comPortList[i].portName());
    };
    selectIndex = ui->portNumberSelect->findData(portSettings.portName);
    if (selectIndex < 0){
        ui->portNumberSelect->addItem(QString("%1(disable)").arg(portSettings.portName), portSettings.portName);
        selectIndex = ui->portNumberSelect->count()-1;
    }
    ui->portNumberSelect->setCurrentIndex(selectIndex);
    selectIndex = ui->portBaudrateSelect->findData(portSettings.baudrate);
    ui->portBaudrateSelect->setCurrentIndex(selectIndex < 0 ? 0: selectIndex);
    selectIndex = ui->portDataBitsSelect->findData(portSettings.dataBits);
    ui->portDataBitsSelect->setCurrentIndex(selectIndex < 0 ? 0: selectIndex);
    selectIndex = ui->portParitySelect->findData(portSettings.parity);
    ui->portParitySelect->setCurrentIndex(selectIndex < 0 ? 0: selectIndex);
    selectIndex = ui->portStopBitsSelect->findData(portSettings.stopBits);
    ui->portStopBitsSelect->setCurrentIndex(selectIndex < 0 ? 0: selectIndex);
    ui->deviceAddressInput->setValue(portSettings.deviceAddress);
}
void Settings::onBtnClickOk(){
    QSettings config_file (QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat);
    config_file.beginGroup("port");
    config_file.setValue (QString("number"), ui->portNumberSelect->currentData().toString());
    config_file.setValue (QString("baudrate"), ui->portBaudrateSelect->currentData().toInt());
    config_file.setValue (QString("databits"), ui->portDataBitsSelect->currentData().toInt());
    config_file.setValue(QString("parity"), ui->portParitySelect->currentData().toInt());
    config_file.setValue(QString("stopbits"), ui->portStopBitsSelect->currentData().toInt());
    config_file.setValue(QString("address"), ui->deviceAddressInput->value());
    config_file.endGroup();
}
PortSettings Settings::getPortSettings(){
    PortSettings portSettings;
    getSettingsFromConfig(portSettings);

    return portSettings;
}
