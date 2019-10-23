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

    void openProject(const QString& path);

private slots:

    void on_action_open_triggered();
    void on_action_new_triggered();
    void on_action_save_triggered();
    void on_action_save_as_triggered();

    void on_boxProjectType_currentIndexChanged(int index);

    void on_boxProjectType_activated(int index);

    void on_editCpuFamily_activated(const QString &cpuFamily);

    void on_boxProjects_currentIndexChanged(int index);

    void on_boxConfigurations_currentIndexChanged(int index);

    void on_buttonConfigurationAdd_clicked();

    void on_buttonConfigurationRename_clicked();

    void on_buttonConfigurationCopy_clicked();

    void on_buttonConfigurationRemove_clicked();

    void on_buttonAddSource_clicked();

    void on_buttonRemoveSource_clicked();

    void on_editCompilerIncludePaths_listUpdated();
    void on_editCompilerDefines_listUpdated();
    void on_editCompilerUndefines_listUpdated();

    void updateOtherCompilerOptions();
    void updateLinkerOptions();
    void updateArchiverOptions();

    void on_widgetPreBuildSteps_updated();
    void on_widgetPostBuildSteps_updated();

    void on_buttonMenuExtra_clicked();

private:
    Ui::MainWindow *ui;

    enum SettingsTabs
    {
        TAB_BUILD_STEPS,
        TAB_COMPILER,
        TAB_LINKER,
        TAB_ARCHIVER
    };

    QTextCodec*               mProjectCodec;

    QVector <QString>         mProjectPaths;
    QVector <ProjectSettings> mProjects;

    ProjectSettings*          mCurrentProject;
    ConfigSettings*           mCurrentConfig;

    void clearProject();
    void reloadProject();
    void clearProjectSettings();
    void reloadProjectSettings();

    void updateToolsTabs();

    void reloadSources();
};

#endif // MAINWINDOW_H
