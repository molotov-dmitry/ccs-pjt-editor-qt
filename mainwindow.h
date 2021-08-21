#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QVector>

#include "parser/projectsettings.h"

namespace Ui {
class MainWindow;
}

class QTextCodec;
class QTreeWidgetItem;

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
    void on_action_save_all_triggered();

    void on_boxProjectType_currentIndexChanged(int index);

    void on_boxProjectType_activated(int index);

    void on_editCpuFamily_currentTextChanged(const QString &cpuFamily);

    void on_boxProjects_currentIndexChanged(int index);

    void on_boxConfigurations_currentIndexChanged(int index);

    void on_buttonConfigurationAdd_clicked();
    void on_buttonConfigurationRename_clicked();
    void on_buttonConfigurationCopy_clicked();
    void on_buttonConfigurationRemove_clicked();

    void on_buttonAddSource_clicked();
    void on_buttonRemoveSource_clicked();
    void on_buttonReplaceSource_clicked();

    void on_editCompilerIncludePaths_listUpdated();
    void on_editCompilerDefines_listUpdated();
    void on_editCompilerUndefines_listUpdated();

    void on_editLinkerLibraryPaths_listUpdated();
    void on_editLinkerLibraries_listUpdated();

    void updateOutputFile();
    void updateMapFile();

    void updateOtherCompilerOptions();
    void updateOtherLinkerOptions();
    void updateOtherArchiverOptions();

    void on_widgetPreBuildSteps_updated();
    void on_widgetPostBuildSteps_updated();

    void on_buttonMenuExtra_clicked();

    void on_buttonLinkerEditLinkOrder_clicked();
    void on_buttonLinkerOutputFileExtra_clicked();
    void on_buttonLinkerMapFileExtra_clicked();

    void checkProjectChanged();

    void on_treeSources_itemDoubleClicked(QTreeWidgetItem *item, int column);

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
    QVector <ProjectSettings> mSavedProjects;

    ProjectSettings*          mCurrentProject;
    ConfigSettings*           mCurrentConfig;

    bool                      mLoadingProject;
    bool                      mAllProjectsSaved;

    void closeEvent(QCloseEvent* event);

    void clearProject();
    void reloadProject();
    void clearProjectSettings();
    void reloadProjectSettings();

    void updateToolsTabs();
    void updateWindowTitle();

    void updateSource(QTreeWidgetItem* item, const FileOptions* options);
    void updateSources();
    void reloadSources();
};

#endif // MAINWINDOW_H
