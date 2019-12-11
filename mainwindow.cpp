#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QFontDatabase>
#include <QMenu>
#include <QMimeDatabase>
#include <QFileIconProvider>
#include <QShortcut>

#include "parser/projectreader.h"
#include "parser/export/projectexportccs3.h"
#include "parser/utils.h"

#include "dialogconfigurationrename.h"
#include "dialoglinkordereditor.h"
#include "dialogfileoptions.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCurrentProject(nullptr),
    mCurrentConfig(nullptr),
    mLoadingProject(false)
{
    ui->setupUi(this);

    mProjectCodec = QTextCodec::codecForName("Windows-1251");

    ui->editCompilerOtherOptions->setSeparator(' ');
    ui->editLinkerOtherOptions->setSeparator(' ');
    ui->editArchiverOtherOptions->setSeparator(' ');

    on_boxProjects_currentIndexChanged(ui->boxProjects->currentIndex());
    on_boxConfigurations_currentIndexChanged(ui->boxConfigurations->currentIndex());

    connect(ui->buttonProjectNew,  SIGNAL(clicked()), ui->action_new,  SLOT(trigger()));
    connect(ui->buttonProjectOpen, SIGNAL(clicked()), ui->action_open, SLOT(trigger()));
    connect(ui->buttonProjectSave, SIGNAL(clicked()), ui->action_save, SLOT(trigger()));

    connect(ui->editCompilerOtherOptions, SIGNAL(listUpdated()), this, SLOT(updateOtherCompilerOptions()));

    connect(ui->checkFullSymbolicDebug, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkDataAccessModel, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkMh, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkMi, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkMv, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkCompilerOptimizationLevel, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkCompilerOptimizationType, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));

    connect(ui->editCompilerDataAccessModel, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->editMh, SIGNAL(valueChanged(int)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->editMi, SIGNAL(valueChanged(int)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->editMv, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->boxCompilerOptimizationLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->editCompilerOptimizationType, SIGNAL(valueChanged(int)), this, SLOT(updateOtherCompilerOptions()));

    connect(ui->editLinkerOtherOptions, SIGNAL(listUpdated()), this, SLOT(updateLinkerOptions()));

    connect(ui->checkLinkerAbsoluteExecutable, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerRomAutoinitModel, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerUnspecifiedSectionsWarning, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerRereadLibraries, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerOutputFile, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerMapFile, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));

    connect(ui->editLinkerOutputFile, SIGNAL(textChanged(QString)), this, SLOT(updateLinkerOptions()));
    connect(ui->editLinkerMapFile, SIGNAL(textChanged(QString)), this, SLOT(updateLinkerOptions()));

    connect(ui->editArchiverOtherOptions, SIGNAL(listUpdated()), this, SLOT(updateArchiverOptions()));

    //// Shortcuts (because unused actions cannot be called using hotkeys) =====

    QShortcut* newShortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(newShortcut, SIGNAL(activated()), ui->action_new, SLOT(trigger()));

    QShortcut* openShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);
    connect(openShortcut, SIGNAL(activated()), ui->action_open, SLOT(trigger()));

    QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(saveShortcut, SIGNAL(activated()), ui->action_save, SLOT(trigger()));

    QShortcut* saveAllShortcut = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
    connect(saveAllShortcut, SIGNAL(activated()), ui->action_save_all, SLOT(trigger()));

    QShortcut* exitShortcut = new QShortcut(QKeySequence("Ctrl+Q"), this);
    connect(exitShortcut, SIGNAL(activated()), ui->action_exit, SLOT(trigger()));

    updateWindowTitle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openProject(const QString& path)
{
    QFileInfo fileInfo(path);

    ProjectReader reader(path.toStdString().c_str());

    if (not reader.read())
    {
        QMessageBox::critical(this,
                              QString::fromUtf8("Открытие проекта"),
                              QString::fromUtf8(reader.lastError().c_str()));

        return;
    }

    mProjects.append(reader.projectSettings());
    mSavedProjects.append(reader.projectSettings());
    mProjectPaths.append(path);

    ui->boxProjects->addItem(fileInfo.fileName());

    ui->boxProjects->setCurrentIndex(ui->boxProjects->count() - 1);

    checkProjectChanged();
}

void MainWindow::on_action_open_triggered()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                QString::fromUtf8("Открыть проект"),
                                                QString(),
                                                QString("*.pjt"));

    if (path.isEmpty())
    {
        return;
    }

    openProject(path);
}

void MainWindow::on_action_new_triggered()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                QString::fromUtf8("Создать проект"),
                                                QString(),
                                                QString("*.pjt"));

    if (path.isEmpty())
    {
        return;
    }

    if (not path.endsWith(".pjt", Qt::CaseInsensitive))
    {
        path.append(".pjt");
    }

    QFileInfo fileInfo(path);

    ProjectSettings newProject;

    {
        ProjectExportCcs3 writer;

        if (not writer.write(newProject, path.toStdString().c_str()))
        {
            QMessageBox::critical(this,
                                  QString::fromUtf8("Создание проекта"),
                                  QString::fromUtf8(writer.lastError().c_str()));

            return;
        }
    }

    mProjects.append(newProject);
    mSavedProjects.append(newProject);
    mProjectPaths.append(path);

    ui->boxProjects->addItem(fileInfo.fileName());
    ui->boxProjects->setCurrentIndex(ui->boxProjects->count() - 1);

    checkProjectChanged();
}

void MainWindow::on_action_save_triggered()
{
    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    int currentIndex = ui->boxProjects->currentIndex();

    {
        QString path = mProjectPaths[currentIndex];

        ProjectExportCcs3 writer;

        if (not writer.write(mProjects[currentIndex], path.toStdString().c_str()))
        {
            QMessageBox::critical(this,
                                  QString::fromUtf8("Сохранение проекта"),
                                  QString::fromUtf8(writer.lastError().c_str()));

            return;
        }

        mSavedProjects[currentIndex] = mProjects[currentIndex];

        checkProjectChanged();
    }
}

void MainWindow::on_action_save_as_triggered()
{
    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    int currentIndex = ui->boxProjects->currentIndex();

    QString path = QFileDialog::getSaveFileName(this,
                                                QString::fromUtf8("Сохранить проект"),
                                                QString(),
                                                QString("*.pjt"));

    if (path.isEmpty())
    {
        return;
    }

    if (not path.endsWith(".pjt", Qt::CaseInsensitive))
    {
        path.append(".pjt");
    }

    QFileInfo fileInfo(path);

    ProjectExportCcs3 writer;

    if (not writer.write(mProjects[currentIndex], path.toStdString().c_str()))
    {
        QMessageBox::critical(this,
                              QString::fromUtf8("Сохранение проекта"),
                              QString::fromUtf8(writer.lastError().c_str()));

        return;
    }

    mProjectPaths[currentIndex] = path;
    ui->boxProjects->setItemText(currentIndex, fileInfo.fileName());

    mSavedProjects[currentIndex] = mProjects[currentIndex];
    checkProjectChanged();
}

void MainWindow::on_action_save_all_triggered()
{
    mAllProjectsSaved = true;

    for (int i = 0; i < ui->boxProjects->count(); ++i)
    {
        if (mProjects[i] == mSavedProjects[i])
        {
            continue;
        }

        QString path = mProjectPaths[i];

        ProjectExportCcs3 writer;

        if (not writer.write(mProjects[i], path.toStdString().c_str()))
        {
            QMessageBox::critical(this,
                                  QString::fromUtf8("Сохранение проекта"),
                                  QString::fromUtf8(writer.lastError().c_str()));

            mAllProjectsSaved = false;

            continue;
        }

        mSavedProjects[i] = mProjects[i];

        if (i == ui->boxProjects->currentIndex())
        {
            checkProjectChanged();
        }
        else
        {
            ui->boxProjects->setItemIcon(i, QIcon(":/icons/status-saved.png"));
        }
    }
}

void MainWindow::on_boxProjectType_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    updateToolsTabs();
}

void MainWindow::on_boxProjectType_activated(int index)
{
    if (mCurrentProject == nullptr)
    {
        return;
    }

    switch (index)
    {
    case ProjectSettings::PROJECT_EXECUTABLE:

        mCurrentProject->removeTool("Archiver");
        mCurrentProject->addTool("Compiler");
        mCurrentProject->addTool("Linker");

        mCurrentProject->setProjectType(ProjectSettings::Type::EXECUTABLE);

        break;

    case ProjectSettings::PROJECT_LIBRARY:

        mCurrentProject->removeTool("Linker");
        mCurrentProject->addTool("Compiler");
        mCurrentProject->addTool("Archiver");

        mCurrentProject->setProjectType(ProjectSettings::Type::EXECUTABLE);

        break;

    case ProjectSettings::PROJECT_UNKNOWN:
    default:

        break;
    }

    updateToolsTabs();
    checkProjectChanged();
}

void MainWindow::on_editCpuFamily_currentTextChanged(const QString &cpuFamily)
{
    if (mLoadingProject)
    {
        return;
    }

    int currentIndex = ui->boxProjects->currentIndex();

    if (currentIndex >= 0)
    {
        mProjects[currentIndex].setCpuFamily(cpuFamily.toStdString().c_str());
        checkProjectChanged();
    }
}

void MainWindow::on_boxProjects_currentIndexChanged(int index)
{
    if (index < 0)
    {
        ui->boxConfigurations->setEnabled(false);
        ui->boxConfigurations->clear();

        ui->buttonConfigurationAdd->setEnabled(false);

        ui->buttonProjectSave->setEnabled(false);
        ui->action_save->setEnabled(false);
        ui->action_save_as->setEnabled(false);
        ui->action_save_all->setEnabled(false);

        clearProjectSettings();
    }
    else
    {
        ui->boxConfigurations->setEnabled(true);

        ui->buttonConfigurationAdd->setEnabled(true);

        ui->buttonProjectSave->setEnabled(true);
        ui->action_save->setEnabled(true);
        ui->action_save_as->setEnabled(true);
        ui->action_save_all->setEnabled(true);

        mCurrentProject = &mProjects[index];

        reloadProject();
        reloadProjectSettings();
        checkProjectChanged();
    }

    updateSources();
    updateToolsTabs();
    updateWindowTitle();
}

void MainWindow::on_boxConfigurations_currentIndexChanged(int index)
{
    if (index < 0)
    {
        ui->buttonConfigurationCopy->setEnabled(false);
        ui->buttonConfigurationRename->setEnabled(false);
        ui->buttonConfigurationRemove->setEnabled(false);

        clearProjectSettings();
    }
    else
    {
        ui->buttonConfigurationCopy->setEnabled(true);
        ui->buttonConfigurationRename->setEnabled(true);
        ui->buttonConfigurationRemove->setEnabled(true);
        reloadProjectSettings();
    }

    updateSources();
    updateToolsTabs();
    updateWindowTitle();
}

void MainWindow::on_buttonConfigurationAdd_clicked()
{
    int projectIndex = ui->boxProjects->currentIndex();

    if (projectIndex < 0)
    {
        return;
    }

    DialogConfigurationRename dialog;

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string configName = dialog.name().toStdString();

        ProjectSettings& project = mProjects[projectIndex];

        project.addConfig(configName.c_str());

        ui->boxConfigurations->addItem(dialog.name());

        checkProjectChanged();
    }
}

void MainWindow::on_buttonConfigurationRename_clicked()
{
    int projectIndex = ui->boxProjects->currentIndex();
    int configurationIndex = ui->boxConfigurations->currentIndex();

    if (projectIndex < 0 || configurationIndex < 0)
    {
        return;
    }

    DialogConfigurationRename dialog;

    dialog.setName(ui->boxConfigurations->currentText());

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string oldName = ui->boxConfigurations->currentText().toStdString();
        std::string configName = dialog.name().toStdString();

        ProjectSettings& project = mProjects[projectIndex];

        project.renameConfig(oldName, configName);

        ui->boxConfigurations->setItemText(configurationIndex, dialog.name());

        checkProjectChanged();
    }
}

void MainWindow::on_buttonConfigurationCopy_clicked()
{
    int projectIndex = ui->boxProjects->currentIndex();
    int configurationIndex = ui->boxConfigurations->currentIndex();

    if (projectIndex < 0 || configurationIndex < 0)
    {
        return;
    }

    DialogConfigurationRename dialog;

    dialog.setName(ui->boxConfigurations->currentText() + " Copy");

    if (dialog.exec() == QDialog::Accepted)
    {
        std::string oldName = ui->boxConfigurations->currentText().toStdString();
        std::string configName = dialog.name().toStdString();

        ProjectSettings& project = mProjects[projectIndex];

        project.copyConfig(oldName, configName);

        ui->boxConfigurations->addItem(dialog.name());

        checkProjectChanged();
    }
}

void MainWindow::on_buttonConfigurationRemove_clicked()
{
    int projectIndex = ui->boxProjects->currentIndex();
    int configurationIndex = ui->boxConfigurations->currentIndex();

    if (projectIndex < 0 || configurationIndex < 0)
    {
        return;
    }

    std::string configName = ui->boxConfigurations->currentText().toStdString();

    ProjectSettings& project = mProjects[projectIndex];

    project.removeConfig(configName.c_str());

    ui->boxConfigurations->removeItem(configurationIndex);

    checkProjectChanged();
}

void MainWindow::on_buttonAddSource_clicked()
{
    int currentIndex = ui->boxProjects->currentIndex();

    if (currentIndex < 0)
    {
        return;
    }

    if (mProjectPaths[currentIndex].isEmpty())
    {
        QMessageBox::warning(this,
                             QString::fromUtf8("Добавление файлов"),
                             QString::fromUtf8("Необходимо сохранить проект прежде чем добавлять файлы"));
        return;
    }

    ProjectSettings& project = mProjects[currentIndex];

    QStringList path = QFileDialog::getOpenFileNames(this,
                                                     QString::fromUtf8("Добавить файлы"),
                                                     QString(),
                                                     QString("*"));

    if (path.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(mProjectPaths[currentIndex]);
    QDir projectDir = fileInfo.absoluteDir();

    foreach (const QString& source, path)
    {
        project.addSource(projectDir.relativeFilePath(source).toStdString().c_str());
    }

    reloadSources();
    checkProjectChanged();
}

void MainWindow::on_buttonRemoveSource_clicked()
{
    int currentIndex = ui->boxProjects->currentIndex();

    if (currentIndex < 0)
    {
        return;
    }

    ProjectSettings& project = mProjects[currentIndex];

    if (ui->treeSources->selectedItems().isEmpty())
    {
        return;
    }

    foreach (const QTreeWidgetItem* item, ui->treeSources->selectedItems())
    {
        if (item->childCount() > 0)
        {
            continue;
        }

        project.removeSource(item->text(0).toStdString().c_str());
    }

    reloadSources();
    checkProjectChanged();
}

void MainWindow::on_buttonReplaceSource_clicked()
{
    if (mCurrentProject == nullptr)
    {
        return;
    }

    QDir projectDir;
    {
        int currentIndex = ui->boxProjects->currentIndex();
        if (currentIndex < 0)
        {
            return;
        }

        QFileInfo fileInfo(mProjectPaths[currentIndex]);
        projectDir = fileInfo.absoluteDir();
    }

    if (ui->treeSources->selectedItems().isEmpty())
    {
        return;
    }

    foreach (const QTreeWidgetItem* item, ui->treeSources->selectedItems())
    {
        if (item->childCount() > 0)
        {
            continue;
        }

        std::string oldsource = item->text(0).toStdString();

        QString path = QFileDialog::getOpenFileName(this,
                                                    QString::fromUtf8("Заменить файл"),
                                                    QString(),
                                                    QString("*"));

        if (path.isEmpty())
        {
            continue;
        }

        std::string newsource = projectDir.relativeFilePath(path).toStdString();

        for (std::string config : mCurrentProject->configs())
        {
            FileOptions options = mCurrentProject->configSettings(config).fileOptions(oldsource);

            if (not options.isDefault())
            {
                mCurrentProject->config(config).file(newsource) = options;
            }
        }

        mCurrentProject->addSource(newsource.c_str());
        mCurrentProject->removeSource(oldsource.c_str());
    }

    reloadSources();
    checkProjectChanged();
}

void MainWindow::on_editCompilerIncludePaths_listUpdated()
{
    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearIncludePaths();

    foreach (const QString& option, ui->editCompilerIncludePaths->items())
    {
        mCurrentConfig->addIncludePath(option.toStdString());
    }

    checkProjectChanged();
}

void MainWindow::on_editCompilerDefines_listUpdated()
{
    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearDefines();

    foreach (const QString& option, ui->editCompilerDefines->items())
    {
        mCurrentConfig->addDefine(option.toStdString());
    }

    checkProjectChanged();
}

void MainWindow::on_editCompilerUndefines_listUpdated()
{
    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearUndefines();

    foreach (const QString& option, ui->editCompilerUndefines->items())
    {
        mCurrentConfig->addUndefine(option.toStdString());
    }

    checkProjectChanged();
}

void MainWindow::updateOtherCompilerOptions()
{
    if (mLoadingProject)
    {
        return;
    }

    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearCompilerOptions();

    if (ui->checkFullSymbolicDebug->isChecked())
    {
        mCurrentConfig->addOtherCompilerOption("-g");
    }

    if (ui->checkCompilerOptimizationLevel->isChecked())
    {
        int value = ui->boxCompilerOptimizationLevel->currentIndex();

        mCurrentConfig->addOtherCompilerOption(string_format("-o%d", value));
    }

    if (ui->checkMh->isChecked())
    {
        int value = ui->editMh->value();

        mCurrentConfig->addOtherCompilerOption(string_format("-mh%d", value));
    }

    if (ui->checkMi->isChecked())
    {
        int value = ui->editMi->value();

        mCurrentConfig->addOtherCompilerOption(string_format("-mi%d", value));
    }

    if (ui->checkCompilerOptimizationType->isChecked())
    {
        int value = ui->editCompilerOptimizationType->value();

        mCurrentConfig->addOtherCompilerOption(string_format("-ms%d", value));
    }

    if (ui->checkMv->isChecked())
    {
        std::string value = ui->editMv->currentText().toStdString();

        mCurrentConfig->addOtherCompilerOption(string_format("-mv%s", value.c_str()));
    }

    if (ui->checkDataAccessModel->isChecked())
    {
        std::string value = ui->editCompilerDataAccessModel->currentText().toStdString();

        mCurrentConfig->addOtherCompilerOption(string_format("--mem_model:data=%s", value.c_str()));
    }

    foreach (const QString& option, ui->editCompilerOtherOptions->items())
    {
        mCurrentConfig->addOtherCompilerOption(option.toStdString());
    }

    checkProjectChanged();
}

void MainWindow::updateLinkerOptions()
{
    if (mLoadingProject)
    {
        return;
    }

    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearLinkerOptions();

    if (ui->checkLinkerAbsoluteExecutable->isChecked())
    {
        mCurrentConfig->addLinkerOption("-a");
    }

    if (ui->checkLinkerRomAutoinitModel->isChecked())
    {
        mCurrentConfig->addLinkerOption("-c");
    }

    if (ui->checkLinkerMapFile->isChecked())
    {
        std::string value = ui->editLinkerMapFile->text().toStdString();
        mCurrentConfig->addLinkerOption("-m", value, true);
    }

    if (ui->checkLinkerOutputFile->isChecked())
    {
        std::string value = ui->editLinkerOutputFile->text().toStdString();
        mCurrentConfig->addLinkerOption("-o", value, true);
    }

    if (ui->checkLinkerUnspecifiedSectionsWarning->isChecked())
    {
        mCurrentConfig->addLinkerOption("-w");
    }

    if (ui->checkLinkerRereadLibraries->isChecked())
    {
        mCurrentConfig->addLinkerOption("-x");
    }

    foreach (const QString& option, ui->editLinkerOtherOptions->items())
    {
        mCurrentConfig->addLinkerOption(option.toStdString().c_str());
    }

    checkProjectChanged();
}

void MainWindow::updateArchiverOptions()
{
    if (mLoadingProject)
    {
        return;
    }

    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearArchiverOptions();

    foreach (const QString& option, ui->editArchiverOtherOptions->items())
    {
        mCurrentConfig->addArchiverOption(option.toStdString().c_str());
    }

    checkProjectChanged();
}

void MainWindow::on_widgetPreBuildSteps_updated()
{
    if (mLoadingProject)
    {
        return;
    }

    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->preBuildStepsRef() = ui->widgetPreBuildSteps->buildStepList();

    checkProjectChanged();
}

void MainWindow::on_widgetPostBuildSteps_updated()
{
    if (mLoadingProject)
    {
        return;
    }

    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->postBuildStepsRef() = ui->widgetPostBuildSteps->buildStepList();

    checkProjectChanged();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QList<int> unsavedProjects;

    for (int i = 0; i < ui->boxProjects->count(); ++i)
    {
        if (mProjects[i] != mSavedProjects[i])
        {
            unsavedProjects.append(i);
        }
    }

    if (not unsavedProjects.isEmpty())
    {
        QString title = "Following projects have unsaved changes:\n\n";

        foreach (int i, unsavedProjects)
        {
            title.append(ui->boxProjects->itemText(i));
            title.append("\n");
        }

        title.append("\n");
        title.append("Save them before quit?");

        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "Quit", title,
                                      QMessageBox::Yes |
                                      QMessageBox::No |
                                      QMessageBox::Cancel);

        if (reply == QMessageBox::Cancel)
        {
            event->ignore();
            return;
        }
        else if (reply != QMessageBox::No)
        {
            ui->action_save_all->trigger();

            if (not mAllProjectsSaved)
            {
                event->ignore();
                return;
            }
        }
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::clearProject()
{
    mCurrentProject = nullptr;

    //// Clear configurations combobox =========================================

    ui->boxConfigurations->clear();

    //// General settings ======================================================

    ui->boxProjectType->setCurrentIndex(ProjectSettings::PROJECT_UNKNOWN);
    ui->editCpuFamily->clearEditText();
}

void MainWindow::reloadProject()
{
    mCurrentProject = nullptr;

    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    mLoadingProject = true;

    clearProject();

    int currentIndex = ui->boxProjects->currentIndex();

    mCurrentProject = &mProjects[ui->boxProjects->currentIndex()];

    const ProjectSettings& settings = *mCurrentProject;

    //// Configurations ========================================================

    for (const std::string& config : mProjects[currentIndex].configs())
    {
        ui->boxConfigurations->addItem(mProjectCodec->toUnicode(config.c_str()));
    }

    //// Files =================================================================

    reloadSources();

    //// General settings ======================================================

    switch (settings.projectType())
    {
    case ProjectSettings::Type::UNKNOWN:
        ui->boxProjectType->setCurrentIndex(ProjectSettings::PROJECT_UNKNOWN);
        break;

    case ProjectSettings::Type::EXECUTABLE:
        ui->boxProjectType->setCurrentIndex(ProjectSettings::PROJECT_EXECUTABLE);
        break;

    case ProjectSettings::Type::LIBRARY:
        ui->boxProjectType->setCurrentIndex(ProjectSettings::PROJECT_LIBRARY);
        break;
    }

    ui->editCpuFamily->setCurrentText(mProjectCodec->toUnicode(settings.cpuFamily().c_str()));

    mLoadingProject = false;
}

void MainWindow::clearProjectSettings()
{
    mCurrentConfig = nullptr;

    //// Build steps ===========================================================

    ui->widgetPreBuildSteps->clear();
    ui->widgetPostBuildSteps->clear();

    //// Compiler ==============================================================

    ui->editCompilerIncludePaths->clear();
    ui->editCompilerDefines->clear();
    ui->editCompilerUndefines->clear();

    ui->checkFullSymbolicDebug->setChecked(false);
    ui->checkDataAccessModel->setChecked(false);
    ui->checkMh->setChecked(false);
    ui->checkMi->setChecked(false);
    ui->checkMv->setChecked(false);
    ui->checkCompilerOptimizationLevel->setChecked(false);
    ui->checkCompilerOptimizationType->setChecked(false);

    ui->editMh->clear();
    ui->editMi->clear();
    ui->editMv->clearEditText();

    ui->boxCompilerOptimizationLevel->setCurrentIndex(0);
    ui->editCompilerOptimizationType->setValue(0);

    ui->editCompilerOtherOptions->clear();

    //// Linker ================================================================

    ui->checkLinkerAbsoluteExecutable->setChecked(false);
    ui->checkLinkerRomAutoinitModel->setChecked(false);
    ui->checkLinkerRereadLibraries->setChecked(false);
    ui->checkLinkerUnspecifiedSectionsWarning->setChecked(false);

    ui->checkLinkerOutputFile->setChecked(false);
    ui->checkLinkerMapFile->setChecked(false);

    ui->editLinkerOutputFile->clear();
    ui->editLinkerMapFile->clear();

    ui->editLinkerOtherOptions->clear();

    //// Archiver ==============================================================

    ui->editArchiverOtherOptions->clear();
}

static bool isConfig(const std::string& line, const std::string& option, std::string& value)
{
    if (starts_with(line, option, true))
    {
        std::string::size_type optionLen = option.length();
        if ((line.length() > optionLen) && (line.at(optionLen) == '='))
        {
            value = line.data() + optionLen + 1;
        }
        else
        {
            value = line.data() + optionLen;
        }
    }
    else
    {
        return false;
    }

    remove_quotes(value);

    return true;
}

static bool isIntConfig(const std::string& line, const char* option, int& value)
{
    std::string valueStr;

    if (not isConfig(line, option, valueStr))
    {
        return false;
    }

    return (sscanf(valueStr.c_str(), "%d", &value) == 1);
}

void MainWindow::reloadProjectSettings()
{
    mCurrentConfig = nullptr;

    int currentTab = ui->tabProjectSettings->currentIndex();

    clearProjectSettings();

    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    if (ui->boxConfigurations->currentIndex() < 0)
    {
        return;
    }

    ProjectSettings& settings = mProjects[ui->boxProjects->currentIndex()];
    const std::string config = ui->boxConfigurations->currentText().toStdString();
    ConfigSettings configSettings = settings.configSettings(config.c_str());

    mCurrentConfig = &settings.config(config.c_str());

    //// Update project configuration options ==================================

    mLoadingProject = true;

    //// Build steps -----------------------------------------------------------

    ui->widgetPreBuildSteps->setBuildSteps(configSettings.preBuildStepsRef());

    ui->widgetPostBuildSteps->setBuildSteps(configSettings.postBuildStepsRef());

    //// Compiler --------------------------------------------------------------

    {
        QStringList includePathList;

        for (const std::string& includePath : configSettings.includePaths())
        {
            includePathList.append(mProjectCodec->toUnicode(includePath.c_str()));
        }

        ui->editCompilerIncludePaths->setText(includePathList.join(';'));
    }

    {
        QStringList defineList;

        for (const std::string& define : configSettings.defines())
        {
            defineList.append(mProjectCodec->toUnicode(define.c_str()));
        }

        ui->editCompilerDefines->setText(defineList.join(';'));
    }

    {
        QStringList undefinesList;

        for (const std::string& undefine : configSettings.undefines())
        {
            undefinesList.append(mProjectCodec->toUnicode(undefine.c_str()));
        }

        ui->editCompilerUndefines->setText(undefinesList.join(';'));
    }

    {
        QStringList otherOptions;

        for (const std::string& option : configSettings.compilerOptions())
        {
            std::string value;
            int         intValue;

            if (option == "-g")
            {
                ui->checkFullSymbolicDebug->setChecked(true);
            }
            else if (isConfig(option, "--mem_model:data", value))
            {
                ui->checkDataAccessModel->setChecked(true);
                ui->editCompilerDataAccessModel->setCurrentText(mProjectCodec->toUnicode(value.c_str()));
            }
            else if (isIntConfig(option, "-mh", intValue))
            {
                ui->checkMh->setChecked(true);
                ui->editMh->setValue(intValue);
            }
            else if (isIntConfig(option, "-mi", intValue))
            {
                ui->checkMi->setChecked(true);
                ui->editMi->setValue(intValue);
            }
            else if (isConfig(option, "-mv", value))
            {
                ui->checkMv->setChecked(true);
                ui->editMv->setEditText(mProjectCodec->toUnicode(value.c_str()));
            }
            else if (isIntConfig(option, "-o", intValue) &&
                     intValue >= 0 &&
                     (intValue <= ui->boxCompilerOptimizationLevel->count()))
            {
                ui->checkCompilerOptimizationLevel->setChecked(true);
                ui->boxCompilerOptimizationLevel->setCurrentIndex(intValue);
            }
            else if (isIntConfig(option, "-ms", intValue) &&
                     (intValue >= ui->editCompilerOptimizationType->minimum()) &&
                     (intValue <= ui->editCompilerOptimizationType->maximum()))
            {
                ui->checkCompilerOptimizationType->setChecked(true);
                ui->editCompilerOptimizationType->setValue(intValue);
            }
            else
            {
                otherOptions.append(mProjectCodec->toUnicode(option.c_str()));
            }
        }

        ui->editCompilerOtherOptions->setText(otherOptions.join(' '));
    }

    //// Linker ----------------------------------------------------------------

    {
        QStringList otherOptions;

        for (const std::string& option : configSettings.linkerOptions())
        {
            std::string value;

            if (option == "-a")
            {
                ui->checkLinkerAbsoluteExecutable->setChecked(true);
            }
            else if (option == "-c")
            {
                ui->checkLinkerRomAutoinitModel->setChecked(true);
            }
            else if (option == "-w")
            {
                ui->checkLinkerUnspecifiedSectionsWarning->setChecked(true);
            }
            else if (option == "-x")
            {
                ui->checkLinkerRereadLibraries->setChecked(true);
            }
            else if (isConfig(option, "-o", value))
            {
                QString valueUnicode = mProjectCodec->toUnicode(value.c_str());
                ui->checkLinkerOutputFile->setChecked(true);
                ui->editLinkerOutputFile->setText(valueUnicode);
            }
            else if (isConfig(option, "-m", value))
            {
                QString valueUnicode = mProjectCodec->toUnicode(value.c_str());
                ui->checkLinkerMapFile->setChecked(true);
                ui->editLinkerMapFile->setText(valueUnicode);
            }
            else
            {
                otherOptions.append(mProjectCodec->toUnicode(option.c_str()));
            }
        }

        ui->editLinkerOtherOptions->setText(otherOptions.join(' '));
    }

    //// Archiver --------------------------------------------------------------

    {
        QStringList otherOptions;

        for (const std::string& option : configSettings.archiverOptions())
        {
            {
                otherOptions.append(mProjectCodec->toUnicode(option.c_str()));
            }
        }

        ui->editArchiverOtherOptions->setText(otherOptions.join(' '));
    }

    //// -----------------------------------------------------------------------

    mLoadingProject = false;

    //// =======================================================================

    ui->tabProjectSettings->setCurrentIndex(currentTab);
}

void MainWindow::updateToolsTabs()
{
    ui->tabProject->setEnabled(mCurrentProject != nullptr);

    if (mCurrentProject == nullptr || mCurrentConfig == nullptr)
    {
        ui->tabProjectSettings->setEnabled(false);
    }
    else
    {
        ui->tabProjectSettings->setEnabled(true);
    }

    if (mCurrentProject != nullptr)
    {
        uint32_t flags = mCurrentProject->toolFlags();

        bool haveCompiler = (flags & ProjectSettings::TOOL_COMPILER) != 0;
        bool haveLinker   = (flags & ProjectSettings::TOOL_LINKER) != 0;
        bool haveArchiver = (flags & ProjectSettings::TOOL_ARCHIVER) != 0;

        ui->tabProjectSettings->setTabEnabled(TAB_BUILD_STEPS, true);
        ui->tabProjectSettings->setTabEnabled(TAB_COMPILER,    haveCompiler);
        ui->tabProjectSettings->setTabEnabled(TAB_LINKER,      haveLinker);
        ui->tabProjectSettings->setTabEnabled(TAB_ARCHIVER,    haveArchiver);
    }
}

void MainWindow::updateWindowTitle()
{
    QString title;

    int currentIndex = ui->boxProjects->currentIndex();
    if (currentIndex >= 0)
    {
        if (ui->boxConfigurations->currentIndex() >= 0)
        {
            title = ui->boxConfigurations->currentText() + " @ ";
        }

        title.append(ui->boxProjects->currentText());

        title.append(" - ");
    }

    title.append("CCS project editor");

    this->setWindowTitle(title);
}

void MainWindow::updateSource(QTreeWidgetItem* item, const FileOptions* options)
{
    if (item == nullptr)
    {
        return;
    }

    if (options != nullptr)
    {
        QFont font = ui->treeSources->font();

        if (options != nullptr)
        {
            if (not options->isDefault(false, false))
            {
                font.setBold(true);
            }

            if (options->isExcludedFromBuild())
            {
                font.setStrikeOut(true);
            }
        }

        item->setFont(0, font);
    }
}

void MainWindow::updateSources()
{
    for (int i = 0; i < ui->treeSources->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* root = ui->treeSources->topLevelItem(i);

        for (int j = 0; j < root->childCount(); ++j)
        {
            QTreeWidgetItem* item = root->child(j);

            std::string source = item->text(0).toStdString();

            if (mCurrentConfig != nullptr)
            {
                FileOptions options = mCurrentConfig->fileOptions(source);
                updateSource(item, &options);
            }
            else
            {
                updateSource(item, nullptr);
            }
        }
    }
}

void MainWindow::reloadSources()
{
    QFileIconProvider iconProvider;
    QMimeDatabase     mimeDatabase;

    if (mCurrentProject == nullptr)
    {
        return;
    }

    QList< QPair<QString, stringset> > sources;

    sources << qMakePair(QString("Sources"),   mCurrentProject->sources());
    sources << qMakePair(QString("Libraries"), mCurrentProject->libraries());
    sources << qMakePair(QString("Commands"),  mCurrentProject->commands());

    ui->treeSources->clear();

    foreach (const auto& src, sources)
    {
        if (not src.second.empty())
        {
            QTreeWidgetItem* root = new QTreeWidgetItem();

            root->setText(0, src.first);
            root->setIcon(0, iconProvider.icon(QFileIconProvider::Folder));

            for(const std::string& source : src.second)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem();

                QString name = mProjectCodec->toUnicode(source.c_str());
                QMimeType type = mimeDatabase.mimeTypeForFile(name);

                item->setText(0, name);
                item->setIcon(0, QIcon::fromTheme(type.iconName()));

                root->addChild(item);
            }

            ui->treeSources->addTopLevelItem(root);

            ui->treeSources->expandItem(root);
        }
    }

    updateSources();

}

void MainWindow::on_buttonMenuExtra_clicked()
{
    QMenu menu;

    menu.addAction(ui->action_save_as);
    menu.addAction(ui->action_save_all);

    menu.exec(QCursor::pos());
}

void MainWindow::on_buttonLinkerEditLinkOrder_clicked()
{
    if (mCurrentProject == nullptr || mCurrentConfig == nullptr)
    {
        return;
    }

    DialogLinkOrderEditor dialog;

    for (const std::string& src : mCurrentProject->files())
    {
        dialog.addUnordered(QString::fromStdString(src));
    }

    for (const std::string& src : mCurrentProject->files())
    {   
        int order = mCurrentConfig->fileOptions(src).linkOrder();
        if (order >= 0)
        {
            dialog.addOrdered(QString::fromStdString(src), order - 1);
        }
    }

    if (dialog.exec() == QDialog::Accepted)
    {
        mCurrentConfig->clearFileLinkOrder();

        QStringList newLinkOrder = dialog.order();

        for (int i = 0; i < newLinkOrder.count(); ++i)
        {
            QString str = newLinkOrder.at(i);

            mCurrentConfig->file(str.toStdString()).setLinkOrder(i + 1);
        }

        updateSources();
        checkProjectChanged();
    }
}

void MainWindow::on_buttonLinkerOutputFileExtra_clicked()
{
    if (mCurrentProject == nullptr || mCurrentConfig == nullptr)
    {
        return;
    }

    QString configName = ui->boxConfigurations->currentText();
    QString fileName   = ui->boxProjects->currentText().replace(QRegExp(".pjt$"), "");

    QMenu menu;

    menu.addAction("./" + configName + "/" + fileName + ".out");

    QAction* result = menu.exec(QCursor::pos());

    if (result != nullptr)
    {
        ui->editLinkerOutputFile->setText(result->text());
    }
}

void MainWindow::on_buttonLinkerMapFileExtra_clicked()
{
    if (mCurrentProject == nullptr || mCurrentConfig == nullptr)
    {
        return;
    }

    QString configName = ui->boxConfigurations->currentText();
    QString fileName   = ui->boxProjects->currentText().replace(QRegExp(".pjt$"), "");

    QMenu menu;

    menu.addAction("./" + configName + "/" + fileName + ".map");

    QAction* result = menu.exec(QCursor::pos());

    if (result != nullptr)
    {
        ui->editLinkerMapFile->setText(result->text());
    }
}

void MainWindow::checkProjectChanged()
{
    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    int currentIndex = ui->boxProjects->currentIndex();

    bool projectSaved = mProjects[currentIndex] == mSavedProjects[currentIndex];

    QIcon statusIcon;

    if (projectSaved)
    {
        statusIcon = QIcon(":/icons/status-saved.png");
    }
    else
    {
        statusIcon = QIcon(":/icons/status-changed.png");
    }

    ui->boxProjects->setItemIcon(currentIndex, statusIcon);
}

void MainWindow::on_treeSources_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if ((item == nullptr) ||
        (item->parent() == nullptr) ||
        (mCurrentProject == nullptr) ||
        (mCurrentConfig == nullptr))
    {
        return;
    }

    std::string filePath = item->text(0).toStdString();

    FileOptions& fileOptions = mCurrentConfig->file(filePath);

    DialogFileOptions dialog(*mCurrentConfig,
                             fileOptions,
                             this);

    if (dialog.exec() == QDialog::Accepted)
    {
        fileOptions = dialog.getFileOptions();

        updateSource(item, &fileOptions);

        checkProjectChanged();
    }
}
