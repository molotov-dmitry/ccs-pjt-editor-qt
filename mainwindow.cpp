#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QFontDatabase>

#include "parser/projectreader.h"
#include "parser/export/projectexportccs3.h"
#include "parser/utils.h"

#include "dialogconfigurationrename.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCurrentProject(nullptr),
    mCurrentConfig(nullptr)
{
    ui->setupUi(this);

    mProjectCodec = QTextCodec::codecForName("Windows-1251");

    ui->editCompilerOtherOptions->setSeparator(' ');
    ui->editLinkerOtherOptions->setSeparator(' ');
    ui->editArchiverOtherOptions->setSeparator(' ');

    on_boxProjects_currentIndexChanged(ui->boxProjects->currentIndex());
    on_boxConfigurations_currentIndexChanged(ui->boxConfigurations->currentIndex());

    connect(ui->editCompilerOtherOptions, SIGNAL(listUpdated()), this, SLOT(updateOtherCompilerOptions()));

    connect(ui->checkFullSymbolicDebug, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkDataAccessModel, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkMh, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkMi, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->checkMv, SIGNAL(toggled(bool)), this, SLOT(updateOtherCompilerOptions()));

    connect(ui->editCompilerDataAccessModel, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->editMh, SIGNAL(valueChanged(int)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->editMi, SIGNAL(valueChanged(int)), this, SLOT(updateOtherCompilerOptions()));
    connect(ui->editMv, SIGNAL(currentTextChanged(QString)), this, SLOT(updateOtherCompilerOptions()));

    connect(ui->editLinkerOtherOptions, SIGNAL(listUpdated()), this, SLOT(updateLinkerOptions()));

    connect(ui->checkLinkerAbsoluteExecutable, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerRomAutoinitModel, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerUnspecifiedSectionsWarning, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));
    connect(ui->checkLinkerRereadLibraries, SIGNAL(toggled(bool)), this, SLOT(updateLinkerOptions()));

    connect(ui->editArchiverOtherOptions, SIGNAL(listUpdated()), this, SLOT(updateArchiverOptions()));
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
    mProjectPaths.append(path);

    ui->boxProjects->addItem(fileInfo.fileName());

    ui->boxProjects->setCurrentIndex(ui->boxProjects->count() - 1);
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
    mProjects.append(ProjectSettings());
    mProjectPaths.append(QString());

    ui->boxProjects->addItem("New project");
}

void MainWindow::on_action_save_triggered()
{
    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    int currentIndex = ui->boxProjects->currentIndex();

    if (mProjectPaths[currentIndex].isEmpty())
    {
        on_action_save_as_triggered();
    }
    else
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
}

void MainWindow::on_boxProjectType_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    updateToolsTabs();
}

void MainWindow::on_boxProjectType_activated(int index)
{
    int currentIndex = ui->boxProjects->currentIndex();

    ProjectSettings::Type projectType = ProjectSettings::Type::UNKNOWN;

    switch (index)
    {
    case ProjectSettings::PROJECT_EXECUTABLE:

        projectType = ProjectSettings::Type::EXECUTABLE;

        break;

    case ProjectSettings::PROJECT_LIBRARY:
        projectType = ProjectSettings::Type::LIBRARY;

        break;

    case ProjectSettings::PROJECT_UNKNOWN:
    default:

        projectType = ProjectSettings::Type::UNKNOWN;

        break;
    }

    if (currentIndex >= 0)
    {
        mProjects[currentIndex].setProjectType(projectType);
    }
}

void MainWindow::on_editCpuFamily_activated(const QString &cpuFamily)
{
    int currentIndex = ui->boxProjects->currentIndex();

    if (currentIndex >= 0)
    {
        mProjects[currentIndex].setCpuFamily(cpuFamily.toStdString().c_str());
    }
}

void MainWindow::on_boxProjects_currentIndexChanged(int index)
{
    if (index < 0)
    {
        ui->tabProjectSettings->setEnabled(false);
        ui->boxConfigurations->clear();

        ui->action_save->setEnabled(false);
        ui->action_save_as->setEnabled(false);

        clearProjectSettings();
    }
    else
    {
        ui->tabProjectSettings->setEnabled(true);

        ui->action_save->setEnabled(true);
        ui->action_save_as->setEnabled(true);

        mCurrentProject = &mProjects[index];

        reloadProject();
        reloadProjectSettings();
    }
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

    updateToolsTabs();
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
}

void MainWindow::updateOtherCompilerOptions()
{
    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearCompilerOptions();

    if (ui->checkFullSymbolicDebug->isChecked())
    {
        mCurrentConfig->addOtherCompilerOption("-g");
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
}

void MainWindow::updateLinkerOptions()
{
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
}

void MainWindow::updateArchiverOptions()
{
    if (mCurrentConfig == nullptr)
    {
        return;
    }

    mCurrentConfig->clearArchiverOptions();

    foreach (const QString& option, ui->editArchiverOtherOptions->items())
    {
        mCurrentConfig->addArchiverOption(option.toStdString().c_str());
    }
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

    ui->editMh->clear();
    ui->editMi->clear();
    ui->editMv->clearEditText();

    ui->editCompilerOtherOptions->clear();

    //// Linker ================================================================

    ui->checkLinkerAbsoluteExecutable->setChecked(false);
    ui->checkLinkerRomAutoinitModel->setChecked(false);
    ui->checkLinkerRereadLibraries->setChecked(false);
    ui->checkLinkerUnspecifiedSectionsWarning->setChecked(false);

    ui->editLinkerOtherOptions->clear();

    //// Archiver ==============================================================

    ui->editArchiverOtherOptions->clear();
}

static bool isConfig(const std::string& line, const char* option, std::string& value)
{
    if (starts_with(line, option, true))
    {
        if (line.at(strlen(option)) == '=')
        {
            value = line.data() + strlen(option) + 1;
        }
        else
        {
            value = line.data() + strlen(option);
        }
    }
    else
    {
        return false;
    }

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

    mCurrentConfig = &settings.configSettingsRef(config.c_str());

    //// Build steps ===========================================================

    for (const std::string& preBuildStep : configSettings.preBuildSteps())
    {
        ui->widgetPreBuildSteps->addBuildStep(mProjectCodec->toUnicode(preBuildStep.c_str()),
                                              BUILD_IF_ANY_FILE_BUILDS);
    }

    for (const std::string& postBuildStep : configSettings.postBuildSteps())
    {
        ui->widgetPostBuildSteps->addBuildStep(mProjectCodec->toUnicode(postBuildStep.c_str()),
                                               BUILD_IF_ANY_FILE_BUILDS);
    }

    //// Compiler ==============================================================

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
            else
            {
                otherOptions.append(mProjectCodec->toUnicode(option.c_str()));
            }
        }

        ui->editCompilerOtherOptions->setText(otherOptions.join(' '));
    }

    //// Linker ================================================================

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
            else
            {
                otherOptions.append(mProjectCodec->toUnicode(option.c_str()));
            }
        }

        ui->editLinkerOtherOptions->setText(otherOptions.join(' '));
    }

    //// Archiver ==============================================================

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

    //// =======================================================================

    ui->tabProjectSettings->setCurrentIndex(currentTab);
}

void MainWindow::updateToolsTabs()
{
    int index = ui->boxProjectType->currentIndex();

    if (ui->boxProjects->currentIndex() < 0)
    {
        index = -1;
    }

    if (ui->boxConfigurations->currentIndex() < 0)
    {
        index = -1;
    }

    switch (index)
    {
    case ProjectSettings::PROJECT_EXECUTABLE:
        ui->tabProjectSettings->setTabEnabled(TAB_BUILD_STEPS, true);
        ui->tabProjectSettings->setTabEnabled(TAB_COMPILER, true);
        ui->tabProjectSettings->setTabEnabled(TAB_LINKER, true);
        ui->tabProjectSettings->setTabEnabled(TAB_ARCHIVER, false);

        break;

    case ProjectSettings::PROJECT_LIBRARY:
        ui->tabProjectSettings->setTabEnabled(TAB_BUILD_STEPS, true);
        ui->tabProjectSettings->setTabEnabled(TAB_COMPILER, true);
        ui->tabProjectSettings->setTabEnabled(TAB_LINKER, false);
        ui->tabProjectSettings->setTabEnabled(TAB_ARCHIVER, true);

        break;

    case ProjectSettings::PROJECT_UNKNOWN:
    default:
        ui->tabProjectSettings->setTabEnabled(TAB_BUILD_STEPS, false);
        ui->tabProjectSettings->setTabEnabled(TAB_COMPILER, false);
        ui->tabProjectSettings->setTabEnabled(TAB_LINKER, false);
        ui->tabProjectSettings->setTabEnabled(TAB_ARCHIVER, false);

        break;
    }
}

void MainWindow::reloadSources()
{
    int projectIndex = ui->boxProjects->currentIndex();

    if (projectIndex < 0)
    {
        return;
    }

    ProjectSettings& project = mProjects[projectIndex];

    QList< QPair<std::string, stringset> > sources;

    sources << qMakePair(std::string("Sources"),   project.sources());
    sources << qMakePair(std::string("Libraries"), project.libraries());
    sources << qMakePair(std::string("Commands"),  project.commands());

    ui->treeSources->clear();

    foreach (const auto& src, sources)
    {
        if (not src.second.empty())
        {
            QTreeWidgetItem* root = new QTreeWidgetItem();

            root->setText(0, QString::fromUtf8(src.first.c_str()));

            for(const std::string& source : src.second)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem();

                item->setText(0, mProjectCodec->toUnicode(source.c_str()));

                root->addChild(item);
            }

            ui->treeSources->addTopLevelItem(root);
        }
    }

}




