#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "settings.h"
#include "displaypanel.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void connectToPort();
    void disconnectFromPort();
    void showSettingsDlg();

private:
    Ui::MainWindow *ui;
    Settings *settings;
    DisplayPanel * displayPanel;
    QMenu *connectMenu;
    QAction *connectAction;
    QAction *disconnectAction;
    QAction *showSettingsDialog;
    QAction *connectToolBarBtn;
    QAction *disconnectToolBarBtn;
    QAction *showSettingsDialogToolBarBtn;

    void getMainWindowSettings();
    void saveMainWindowSettings();
    void createMenusAndActions();

};

#endif // MAINWINDOW_H
