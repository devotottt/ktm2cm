#include "digitalparamwidget.h"
#include "ui_digitalparamwidget.h"

DigitalParamWidget::DigitalParamWidget(QWidget *parent, QString bitName, QString bitShortName, bool normalBit, bool isEmergencyExist, int bitPosition) :
    QWidget(parent),
    ui(new Ui::DigitalParamWidget)
{
    ui->setupUi(this);
    bitName_ = bitName;
    bitShortName_ = bitShortName;
    normalBit_ = normalBit;
    bitPosition_ = bitPosition;
    isEmergencyExist_ = isEmergencyExist;
    ui->shortName->setText(bitShortName_);
    ui->name->setText(bitName_);
    ui->value->setText(QString("0"));
}

DigitalParamWidget::~DigitalParamWidget()
{
    delete ui;
}
void DigitalParamWidget::setValue(unsigned byteValue){

    value_ = ((byteValue >> bitPosition_) & 0x01) > 0;
    if (value_ == normalBit_ || !isEmergencyExist_)
        ui->value->setStyleSheet("QLabel { background-color : transparent; color : black; }");
    else
        ui->value->setStyleSheet("QLabel { background-color : #c54242; color : white; }");
    ui->value->setText(QString("%1").arg((int)value_));
}
