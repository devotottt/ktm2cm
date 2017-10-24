#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class ParamWidget;
}

class ParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParamWidget(QWidget *parent, QString shortName,
                         double lLimit, double hLimit,
                         double lwLimit, double hwLimit,
                         int precision);
    ~ParamWidget();

    void setValue(const unsigned unitValue);
    static int roundParameter (double x);

private:
    Ui::ParamWidget *ui;
    QString shortName_;
    double lLimit_;
    double hLimit_;
    double lwLimit_;
    double hwLimit_;
    double value_;
    int precision_;
};

#endif // PARAMWIDGET_H
