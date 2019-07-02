#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>

#include "parser/projectsettings.h"

namespace Ui {
class MainWindow;
}

class QTextCodec;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_open_triggered();

    void on_action_new_triggered();

    void on_action_save_triggered();

    void on_action_save_as_triggered();

    void on_boxProjectType_currentIndexChanged(int index);

    void on_boxProjects_currentIndexChanged(int index);

    void on_boxConfigurations_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    enum SettingsTabs
    {
        TAB_GENERAL,
        TAB_BUILD_STEPS,
        TAB_COMPILER,
        TAB_LINKER,
        TAB_ARCHIVER
    };

    QTextCodec*               mProjectCodec;

    QVector <QString>         mProjectPaths;
    QVector <ProjectSettings> mProjects;

    void clearProject();
    void reloadProject();
    void clearProjectSettings();
    void reloadProjectSettings();
};

#endif // MAINWINDOW_H
