#ifndef DISPLAYPANEL_H
#define DISPLAYPANEL_H

#include <QDockWidget>
#include <QSerialPort>
#include <QMutex>

#include "settings.h"
#include "mastersthread.h"
#include "paramwidget.h"
#include "digitalparamwidget.h"

namespace Ui {
class DisplayPanel;
}

class DisplayPanel : public QDockWidget
{
    Q_OBJECT

public:
    explicit DisplayPanel(QWidget *parent = 0);
    ~DisplayPanel();

    bool connectToPort(const PortSettings& portSetting, QString& errStrs);
    void disconnectFromPort();
    static QString getStringPortError(QSerialPort::SerialPortError);
    void setDeviceAddress(int deviceAddress);

public slots:
    void showResponse(QVector<int> responseVector);
    void processError(const QString &s);
    void processTimeout(const QString &s);
    void deleteThread();

private:
    Ui::DisplayPanel *ui;
    int deviceAddress_;
    QSerialPort *serialPort_;
    MastersThread *masterThread_;
    bool isConnected_;
    QMutex mutex_;
    QVector<ParamWidget*> paramWidgetVector_;
    QVector<DigitalParamWidget*> b71WidgetVector_;
    QVector<DigitalParamWidget*> b72WidgetVector_;
    QVector<DigitalParamWidget*> b75WidgetVector_;
};

#endif // DISPLAYPANEL_H
