#include "paramwidget.h"
#include "ui_paramwidget.h"
#include <math.h>

ParamWidget::ParamWidget(QWidget *parent, QString shortName,
                         int lLimit, int hLimit,
                         int lwLimit, int hwLimit) :
    QWidget(parent),
    ui(new Ui::ParamWidget)
{
    ui->setupUi(this);
    shortName_ = shortName;
    lLimit_ = lLimit;
    hLimit_ = hLimit;
    lwLimit_ = lwLimit;
    hwLimit_ = hwLimit;
    value_ = 0;

    ui->shortName->setText(shortName_);
    ui->value->setText(QString("0"));
}

ParamWidget::~ParamWidget()
{
    delete ui;
}
void ParamWidget::setValue(const unsigned unitValue){
    value_ = lLimit_ + round(((double)unitValue*(hLimit_ - lLimit_))/65535);
    if (value_ <= hwLimit_ && value_ >= lwLimit_)
        ui->value->setStyleSheet("QLabel { background-color : transparent; color : black; }");
    else
        ui->value->setStyleSheet("QLabel { background-color : #c54242; color : white; }");
    ui->value->setText(QString("%1").arg(value_));

}

int ParamWidget::roundParameter (double x) {
    return (int)((x - floor(x)) >= 0.5) ? ceil(x) : floor(x);
}
