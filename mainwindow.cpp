#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<QVector<int> >("QVector<int>");

    settings = new Settings(this);
    displayPanel = new DisplayPanel(this);
    getMainWindowSettings();
    createMenusAndActions();

}

MainWindow::~MainWindow()
{
    saveMainWindowSettings();
    delete ui;
    delete displayPanel;
    delete settings;
}
void MainWindow::getMainWindowSettings(){
    QSettings config_file(QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat);
    config_file.beginGroup("mainwindow");

    this->resize(config_file.value("size", QSize(1280, 1024)).toSize());
    if (config_file.value("maximized",true).toBool())
        this->showMaximized();
    else
        this->move(config_file.value("position", QPoint(0, 0)).toPoint());
    config_file.endGroup();
    this->addDockWidget(Qt::TopDockWidgetArea, displayPanel);
}
void MainWindow::saveMainWindowSettings(){
    QSettings config_file(QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat);
    config_file.beginGroup("mainwindow");
    config_file.setValue("size", this->size());
    config_file.setValue("maximized", this->isMaximized());
    config_file.setValue("position", this->pos());
    config_file.endGroup();
}
void MainWindow::createMenusAndActions(){
    connectMenu = ui->menuBar->addMenu(QIcon(), QString("Соединение"));
    connectAction = connectMenu->addAction(QString("Подключить"));
    disconnectAction = connectMenu->addAction(QString("Отключить"));
    showSettingsDialog = ui->menuBar->addAction(QString("Настройки"));
    disconnectAction->setEnabled(false);
    connectToolBarBtn = ui->mainToolBar->addAction(QIcon(QCoreApplication::applicationDirPath()+QString("/img/ico_start.png")), QString());
    disconnectToolBarBtn = ui->mainToolBar->addAction(QIcon(QCoreApplication::applicationDirPath()+QString("/img/ico_stop.png")), QString());
    disconnectToolBarBtn->setEnabled(false);
    ui->mainToolBar->addSeparator();
    showSettingsDialogToolBarBtn = ui->mainToolBar->addAction(QIcon(QCoreApplication::applicationDirPath()+QString("/img/ico_settings.png")), QString());

    connect (connectAction, SIGNAL(triggered(bool)), this, SLOT(connectToPort()));
    connect (disconnectAction, SIGNAL(triggered(bool)), this, SLOT(disconnectFromPort()));
    connect (connectToolBarBtn, SIGNAL(triggered(bool)), this, SLOT(connectToPort()));
    connect (disconnectToolBarBtn, SIGNAL(triggered(bool)), this, SLOT(disconnectFromPort()));
    connect (showSettingsDialog, SIGNAL(triggered(bool)), this, SLOT(showSettingsDlg()));
    connect (showSettingsDialogToolBarBtn, SIGNAL(triggered(bool)), this, SLOT(showSettingsDlg()));
}
void MainWindow::connectToPort(){
    QString connectionErr = "";
    disconnectAction->setEnabled(false);
    connectAction->setEnabled(false);
    connectToolBarBtn->setEnabled(false);
    disconnectToolBarBtn->setEnabled(false);
    if (displayPanel->connectToPort(settings->getPortSettings(), connectionErr)){
        disconnectAction->setEnabled(true);
        connectAction->setEnabled(false);
        connectToolBarBtn->setEnabled(false);
        disconnectToolBarBtn->setEnabled(true);
    }
    else{
        disconnectAction->setEnabled(false);
        connectAction->setEnabled(true);
        connectToolBarBtn->setEnabled(true);
        disconnectToolBarBtn->setEnabled(false);
        QMessageBox connectionErrorDialog(QMessageBox::Critical, QString("Ошибка подключения"),
                                          QString("Ошибка: ")+connectionErr, QMessageBox::Close, this);
        connectionErrorDialog.exec();
    }
}
void MainWindow::disconnectFromPort(){
    displayPanel->disconnectFromPort();
    disconnectAction->setEnabled(false);
    connectAction->setEnabled(true);
    connectToolBarBtn->setEnabled(true);
    disconnectToolBarBtn->setEnabled(false);
}
void MainWindow::showSettingsDlg(){
    settings->show();

    if (settings->result() == 1)
        displayPanel->setDeviceAddress(settings->getPortSettings().deviceAddress);
}
