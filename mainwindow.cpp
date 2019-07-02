#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>

#include "parser/projectreader.h"
#include "parser/utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->treePreBuildSteps->header()->setStretchLastSection(false);
    ui->treePreBuildSteps->header()->resizeSections(QHeaderView::ResizeToContents);

    mProjectCodec = QTextCodec::codecForName("Windows-1251");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_open_triggered()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                QString::fromUtf8("Открыть  проект"),
                                                QString(),
                                                QString("*.pjt"));

    if (path.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(path);

    ProjectReader reader(path.toStdString().c_str());

    if (not reader.read())
    {
        QMessageBox::critical(this,
                              QString::fromUtf8("Открыть  проект"),
                              QString::fromUtf8(reader.lastError().c_str()));

        return;
    }

    mProjects.append(reader.projectSettings());
    mProjectPaths.append(path);

    ui->boxProjects->addItem(fileInfo.fileName());
}

void MainWindow::on_action_new_triggered()
{
    mProjects.append(ProjectSettings());
    mProjectPaths.append(QString());

    ui->boxProjects->addItem("New project");
}

void MainWindow::on_action_save_triggered()
{

}

void MainWindow::on_action_save_as_triggered()
{

}

void MainWindow::on_boxProjectType_currentIndexChanged(int index)
{
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

void MainWindow::on_boxProjects_currentIndexChanged(int index)
{
    if (index < 0)
    {
        ui->tabProjectSettings->setEnabled(false);
        ui->boxConfigurations->clear();
        clearProjectSettings();
    }
    else
    {
        ui->tabProjectSettings->setEnabled(true);
        reloadProject();
        reloadProjectSettings();
    }
}

void MainWindow::on_boxConfigurations_currentIndexChanged(int index)
{
    if (index < 0)
    {
        ui->tabProjectSettings->setEnabled(false);
        clearProjectSettings();
    }
    else
    {
        ui->tabProjectSettings->setEnabled(true);
        reloadProjectSettings();
    }
}

void MainWindow::clearProject()
{
    ui->boxConfigurations->clear();
}

void MainWindow::reloadProject()
{
    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    int currentIndex = ui->boxProjects->currentIndex();

    clearProject();

    for (const std::string& config : mProjects[currentIndex].configs())
    {
        ui->boxConfigurations->addItem(mProjectCodec->toUnicode(config.c_str()));
    }

    if (mProjects[currentIndex].c_sources().size() > 0)
    {
        QTreeWidgetItem* root = new QTreeWidgetItem();

        root->setText(0, QString::fromUtf8("Sources"));

        for(const std::string& source : mProjects[currentIndex].c_sources())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem();

            item->setText(0, mProjectCodec->toUnicode(source.c_str()));

            root->addChild(item);
        }

        ui->treeSources->addTopLevelItem(root);
    }

    if (mProjects[currentIndex].c_libraries().size() > 0)
    {
        QTreeWidgetItem* root = new QTreeWidgetItem();

        root->setText(0, QString::fromUtf8("Libraries"));

        for(const std::string& file : mProjects[currentIndex].c_libraries())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem();

            item->setText(0, mProjectCodec->toUnicode(file.c_str()));

            root->addChild(item);
        }

        ui->treeSources->addTopLevelItem(root);
    }

    if (mProjects[currentIndex].c_commands().size() > 0)
    {
        QTreeWidgetItem* root = new QTreeWidgetItem();

        root->setText(0, QString::fromUtf8("Command"));

        for(const std::string& file : mProjects[currentIndex].c_commands())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem();

            item->setText(0, mProjectCodec->toUnicode(file.c_str()));

            root->addChild(item);
        }

        ui->treeSources->addTopLevelItem(root);
    }
}

void MainWindow::clearProjectSettings()
{
    //// General settings ======================================================

    ui->boxProjectType->setCurrentIndex(ProjectSettings::PROJECT_UNKNOWN);
    ui->editCpuFamily->clear();

    ui->treePreBuildSteps->clear();
    ui->treePostBuildSteps->clear();

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

bool isConfig(const std::string& line, const char* option, std::string& value)
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

bool isIntConfig(const std::string& line, const char* option, int& value)
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
    clearProjectSettings();

    if (ui->boxProjects->currentIndex() < 0)
    {
        return;
    }

    if (ui->boxConfigurations->currentIndex() < 0)
    {
        return;
    }

    const ProjectSettings& settings = mProjects.at(ui->boxProjects->currentIndex());
    const std::string config = ui->boxConfigurations->currentText().toStdString();
    ConfigSettings configSettings = settings.configSettings(config.c_str());

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

//    ui->editCpuFamily->setCurrentText(mProjectCodec->toUnicode(settings.cpuFamily.c_str()))

    for (const std::string& preBuildStep : configSettings.preBuildSteps())
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();

        item->setText(0, mProjectCodec->toUnicode(preBuildStep.c_str()));

        ui->treePreBuildSteps->addTopLevelItem(item);
    }

    for (const std::string& postBuildStep : configSettings.postBuildSteps())
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();

        item->setText(0, mProjectCodec->toUnicode(postBuildStep.c_str()));

        ui->treePostBuildSteps->addTopLevelItem(item);
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

//    {
//        QStringList undefinesList;

//        for (const std::string& undefine : configSettings.())
//        {
//            undefinesList.append(mProjectCodec->toUnicode(undefine.c_str()));
//        }

//        ui->editCompilerIncludePaths->setText(undefinesList.join(';'));
//    }

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
}
