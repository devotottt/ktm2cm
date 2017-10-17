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
                         int lLimit, int hLimit,
                         int lwLimit, int hwLimit);
    ~ParamWidget();

    void setValue(const unsigned unitValue);
    static int roundParameter (double x);

private:
    Ui::ParamWidget *ui;
    QString shortName_;
    int lLimit_;
    int hLimit_;
    int lwLimit_;
    int hwLimit_;
    int value_;
};

#endif // PARAMWIDGET_H
