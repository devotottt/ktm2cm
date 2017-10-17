#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

#include <QVector>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

/*template <typename T1>
struct PortSetItem{
    QString name;
    T1      value;
    PortSetItem(){}
    PortSetItem(QString name_in, T1 value_in){
        name = name_in;
        value = value_in;
    }
};*/
struct PortSettings{
    QString portName;
    int baudrate;
    int parity;
    int stopBits;
    int dataBits;
    int deviceAddress;
};

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    PortSettings getPortSettings();

public slots:
    void onBtnClickOk();
    void show();

private:
    Ui::Settings *ui;

    void initializeSettingsLists();
    void getSettingsFromConfig(PortSettings& portSettings);
    void setSettingsToFrom();
};

#endif // SETTINGS_H
