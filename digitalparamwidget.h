#ifndef DIGITALPARAMWIDGET_H
#define DIGITALPARAMWIDGET_H

#include <QWidget>

namespace Ui {
class DigitalParamWidget;
}

class DigitalParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DigitalParamWidget(QWidget *parent, QString bitName, QString bitShortName, bool normalBit, bool isEmergencyExist, int bitPosition);
    ~DigitalParamWidget();

    void setValue(unsigned byteValue);

private:
    Ui::DigitalParamWidget *ui;
    bool value_;
    bool normalBit_;
    bool isEmergencyExist_;
    int bitPosition_;
    QString bitName_;
    QString bitShortName_;
};

#endif // DIGITALPARAMWIDGET_H
