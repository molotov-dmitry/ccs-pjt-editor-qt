#include "dialogfileoptions.h"
#include "ui_dialogfileoptions.h"

#include <QTextCodec>

#include "parser/utils.h"

static bool isConfig(const std::string& line, const std::string& option, QString& value)
{
    std::string stdValue;

    if (starts_with(line, option, true))
    {
        std::string::size_type optionLen = option.length();
        if ((line.length() > optionLen) && (line.at(optionLen) == '='))
        {
            stdValue = line.data() + optionLen + 1;
        }
        else
        {
            stdValue = line.data() + optionLen;
        }
    }
    else
    {
        return false;
    }

    remove_quotes(stdValue);

    value = QString::fromStdString(stdValue);

    return true;
}

static bool isIntConfig(const std::string& line, const char* option, int& value)
{
    QString valueStr;

    if (not isConfig(line, option, valueStr))
    {
        return false;
    }

    bool ok;

    value = valueStr.toInt(&ok);

    return ok;
}

DialogFileOptions::DialogFileOptions(const ConfigSettings& config, const FileOptions& fileOptions, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DialogFileOptions),
    mDefaultEnabledDebug(false),
    mDefaultEnabledDataAccessModel(false),
    mDefaultEnabledMh(false),
    mDefaultEnabledMi(false),
    mDefaultEnabledMv(false),
    mDefaultEnabledOptimizationLevel(false),
    mDefaultEnabledOptimizationType(false),
    mDefaultEnabledOtherOptions(false),
    mDefaultMh(0),
    mDefaultMi(0),
    mDefaultOptimizationLevel(0),
    mDefaultOptimizationType(0),
    mLinkOrder(fileOptions.linkOrder())
{
    ui->setupUi(this);

    ui->widgetPreBuildSteps->setIsFile(true);
    ui->widgetPostBuildSteps->setIsFile(true);

    ui->editOtherOptions->setSeparator(' ');

    mFlagButtons.insert(ui->buttonResetIncludePaths, FLAG_INCLUDE_PATHS);
    mFlagButtons.insert(ui->buttonResetDefines, FLAG_DEFINES);
    mFlagButtons.insert(ui->buttonResetUndefines, FLAG_UNDEFINES);
    mFlagButtons.insert(ui->buttonResetFullSymbolicDebug, FLAG_DEBUG);
    mFlagButtons.insert(ui->buttonResetDataAccessModel, FLAG_DATA_ACCESS_MODEL);
    mFlagButtons.insert(ui->buttonResetMh, FLAG_MH);
    mFlagButtons.insert(ui->buttonResetMi, FLAG_MI);
    mFlagButtons.insert(ui->buttonResetMv, FLAG_MV);
    mFlagButtons.insert(ui->buttonResetOptimizationLevel, FLAG_OPTIMIZATION_LEVEL);
    mFlagButtons.insert(ui->buttonResetOptimizationType, FLAG_OPTIMIZATION_TYPE);
    mFlagButtons.insert(ui->buttonResetOtherOptions, FLAG_OTHER_OPTIONS);
    mFlagButtons.insert(ui->buttonResetAll, FLAG_ALL);

    //// Set exclude from build flag ===========================================

    ui->checkExcludeFromBuild->setChecked(fileOptions.isExcludedFromBuild());

    //// Set build condition ===================================================

    ui->boxBuildCondition->setCurrentIndex(fileOptions.buildCondition());

    //// Set default values ====================================================

    //// Include paths ---------------------------------------------------------

    QStringList includePathList;

    for (std::string includePath : config.includePaths())
    {
        includePathList.append(QString::fromStdString(includePath));
    }

    mDefaultIncludePaths = includePathList.join(ui->editIncludePaths->separator());

    //// Defines ---------------------------------------------------------------

    QStringList defineList;

    for (std::string define : config.defines())
    {
        defineList.append(QString::fromStdString(define));
    }

    mDefaultDefines = defineList.join(ui->editDefines->separator());

    //// Undefines -------------------------------------------------------------

    QStringList undefineList;

    for (std::string undefine : config.undefines())
    {
        undefineList.append(QString::fromStdString(undefine));
    }

    mDefaultUndefines = undefineList.join(ui->editUndefines->separator());

    //// Other options ---------------------------------------------------------

    QStringList otherOptions;

    for (const std::string& option : config.otherCompilerOptions())
    {
        QString value;
        int     intValue;

        if (option == "-g")
        {
            mDefaultEnabledDebug = true;
        }
        else if (isConfig(option, "--mem_model:data", value))
        {
            mDefaultEnabledDataAccessModel = true;
            mDefaultDataAccessModel = value;
        }
        else if (isIntConfig(option, "-mh", intValue))
        {
            mDefaultEnabledMh = true;
            mDefaultMh = intValue;
        }
        else if (isIntConfig(option, "-mi", intValue))
        {
            mDefaultEnabledMi = true;
            mDefaultMi = intValue;
        }
        else if (isConfig(option, "-mv", value))
        {
            mDefaultEnabledMv = true;
            mDefaultMv = value;
        }
        else if (isIntConfig(option, "-o", intValue) &&
                 intValue >= 0 &&
                 (intValue <= ui->editOptimizationLevel->count()))
        {
            mDefaultEnabledOptimizationLevel = true;
            mDefaultOptimizationLevel = intValue;
        }
        else if (isIntConfig(option, "-ms", intValue) &&
                 (intValue >= ui->editOptimizationType->minimum()) &&
                 (intValue <= ui->editOptimizationType->maximum()))
        {
            mDefaultEnabledOptimizationType = true;
            mDefaultOptimizationType = intValue;
        }
        else
        {
            otherOptions.append(QString::fromStdString(option));
        }
    }

    mDefaultOtherOptions = otherOptions.join(ui->editOtherOptions->separator());

    //// Set all values to default =============================================

    resetValue(FLAG_ALL);

    //// Remove options ========================================================

    for (const std::string& option : fileOptions.optionsRemoved())
    {
        QString value;
        int    intValue;

        if (option == "-g")
        {
            ui->checkFullSymbolicDebug->setChecked(false);
        }
        else if (isConfig(option, "-i", value))
        {
            ui->editIncludePaths->removeItem(value);
        }
        else if (isConfig(option, "-d", value))
        {
            ui->editDefines->removeItem(value);
        }
        else if (isConfig(option, "-u", value))
        {
            ui->editUndefines->removeItem(value);
        }
        else if (isConfig(option, "--mem_model:data", value))
        {
            if (value == ui->editDataAccessModel->currentText())
            {
                ui->checkDataAccessModel->setChecked(false);
                ui->editDataAccessModel->clearEditText();
            }
        }
        else if (isIntConfig(option, "-mh", intValue))
        {
            if (intValue == ui->editMh->value())
            {
                ui->checkMh->setChecked(false);
                ui->editMh->setValue(0);
            }
        }
        else if (isIntConfig(option, "-mi", intValue))
        {
            if (intValue == ui->editMi->value())
            {
                ui->checkMi->setChecked(false);
                ui->editMi->setValue(0);
            }
        }
        else if (isConfig(option, "-mv", value))
        {
            if (value == ui->editMv->currentText())
            {
                ui->checkMv->setChecked(false);
                ui->editMv->clearEditText();
            }
        }
        else if (isIntConfig(option, "-o", intValue))
        {
            if (intValue == ui->editOptimizationLevel->currentIndex())
            {
                ui->checkOptimizationLevel->setChecked(false);
                ui->editOptimizationLevel->setCurrentIndex(0);
            }
        }
        else if (isIntConfig(option, "-ms", intValue))
        {
            if (intValue == ui->editOptimizationType->value())
            {
                ui->checkOptimizationType->setChecked(false);
                ui->editOptimizationType->setValue(0);
            }
        }
        else
        {
            value = QString::fromStdString(option);
            ui->editOtherOptions->removeItem(value);
        }
    }

    //// Add options ===========================================================

    for (const std::string& option : fileOptions.optionsAdded())
    {
        QString value;
        int     intValue;

        if (option == "-g")
        {
            ui->checkFullSymbolicDebug->setChecked(true);
        }
        else if (isConfig(option, "-i", value))
        {
            ui->editIncludePaths->addItem(value);
        }
        else if (isConfig(option, "-d", value))
        {
            ui->editDefines->addItem(value);
        }
        else if (isConfig(option, "-u", value))
        {
            ui->editUndefines->addItem(value);
        }
        else if (isConfig(option, "--mem_model:data", value))
        {
            ui->checkDataAccessModel->setChecked(true);
            ui->editDataAccessModel->setCurrentText(value);
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
            ui->editMv->setCurrentText(value);
        }
        else if (isIntConfig(option, "-o", intValue))
        {
            ui->checkOptimizationLevel->setChecked(true);
            ui->editOptimizationLevel->setCurrentIndex(intValue);
        }
        else if (isIntConfig(option, "-ms", intValue))
        {
            ui->checkOptimizationType->setChecked(true);
            ui->editOptimizationType->setValue(intValue);
        }
        else
        {
            value = QString::fromStdString(option);
            ui->editOtherOptions->addItem(value);
        }
    }

    //// Add pre build steps ===================================================

    ui->widgetPreBuildSteps->setBuildSteps(fileOptions.preBuildSteps());

    //// Add post build steps ==================================================

    ui->widgetPostBuildSteps->setBuildSteps(fileOptions.postBuildSteps());

}

DialogFileOptions::~DialogFileOptions()
{
    delete ui;
}

static void getDifference(const ListEditor* newValuesList,
                          const QString& defaultValues,
                          FileOptions& fileOptions,
                          const QString& prefix = QString(),
                          const QString& suffix = QString())
{
    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");

    QChar sep = newValuesList->separator();

    QStringList nl = newValuesList->items();
    QStringList ol = defaultValues.split(sep);

    foreach (const QString& o, ol)
    {
        if (not nl.contains(o))
        {
            QString rv = prefix + o + suffix;
            fileOptions.addOptionRemoved(codec->fromUnicode(rv).toStdString());
        }
    }

    foreach (const QString& n, nl)
    {
        if (not ol.contains(n))
        {
            QString av = prefix + n + suffix;
            fileOptions.addOptionAdded(codec->fromUnicode(av).toStdString());
        }
    }
}

static void getDifference(QString option,
                          bool wasChecked,
                          bool isChecked,
                          QString oldValue,
                          QString newValue,
                          FileOptions& fileOptions)
{
    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");

    if ((wasChecked != isChecked) || (isChecked && (oldValue != newValue)))
    {
        if (wasChecked)
        {
            QString rv = option + oldValue;
            fileOptions.addOptionRemoved(codec->fromUnicode(rv).toStdString());
        }

        if (isChecked)
        {
            QString av = option + newValue;
            fileOptions.addOptionAdded(codec->fromUnicode(av).toStdString());
        }
    }

}

static void getDifference(QString option,
                          bool wasChecked,
                          bool isChecked,
                          int oldValue,
                          int newValue,
                          FileOptions& fileOptions)
{
    getDifference(option, wasChecked, isChecked,
                  QString::number(oldValue), QString::number(newValue),
                  fileOptions);
}

FileOptions DialogFileOptions::getFileOptions()
{
    FileOptions result;

    //// Set general options ===================================================

    result.setExcludeFromBuild(ui->checkExcludeFromBuild->isChecked());

    if (mLinkOrder >= 0)
    {
        result.setLinkOrder((uint)mLinkOrder);
    }

    result.preBuildSteps() = ui->widgetPreBuildSteps->buildStepList();
    result.postBuildSteps() = ui->widgetPostBuildSteps->buildStepList();

    //// Set compiler options ==================================================

    bool changed = false;

    //// Include paths ---------------------------------------------------------

    changed = (ui->editIncludePaths->text() != mDefaultIncludePaths);
    if (changed)
    {
        getDifference(ui->editIncludePaths,
                      mDefaultIncludePaths,
                      result,
                      "-i\"", "\"");
    }

    //// Defines ---------------------------------------------------------------

    changed = (ui->editDefines->text() != mDefaultDefines);
    if (changed)
    {
        getDifference(ui->editDefines,
                      mDefaultDefines,
                      result,
                      "-d\"", "\"");
    }

    //// Undefines -------------------------------------------------------------

    changed = (ui->editUndefines->text() != mDefaultUndefines);
    if (changed)
    {
        getDifference(ui->editUndefines, mDefaultUndefines,
                      result,
                      "-u\"", "\"");
    }

    //// Full symbolic debug ---------------------------------------------------

    getDifference("-g",
                  mDefaultEnabledDebug,
                  ui->checkFullSymbolicDebug->isChecked(),
                  QString(),
                  QString(),
                  result);

    //// Data access model -----------------------------------------------------

    getDifference("--mem_model:data=",
                  mDefaultEnabledDataAccessModel,
                  ui->checkDataAccessModel->isChecked(),
                  mDefaultDataAccessModel,
                  ui->editDataAccessModel->currentText(),
                  result);

    //// Mh --------------------------------------------------------------------

    getDifference("-mh",
                  mDefaultEnabledMh,
                  ui->checkMh->isChecked(),
                  mDefaultMh,
                  ui->editMh->value(),
                  result);

    //// Mi --------------------------------------------------------------------

    getDifference("-mi",
                  mDefaultEnabledMi,
                  ui->checkMi->isChecked(),
                  mDefaultMi,
                  ui->editMi->value(),
                  result);

    //// Mv --------------------------------------------------------------------

    getDifference("-mv",
                  mDefaultEnabledMv,
                  ui->checkMv->isChecked(),
                  mDefaultMv,
                  ui->editMv->currentText(),
                  result);

    //// Optimization level ----------------------------------------------------

    getDifference("-o",
                  mDefaultEnabledOptimizationLevel,
                  ui->checkOptimizationLevel->isChecked(),
                  mDefaultOptimizationLevel,
                  ui->editOptimizationLevel->currentIndex(),
                  result);

    //// Optimization type -----------------------------------------------------

    getDifference("-ms",
                  mDefaultEnabledOptimizationType,
                  ui->checkOptimizationType->isChecked(),
                  mDefaultOptimizationType,
                  ui->editOptimizationType->value(),
                  result);

    //// Other options ---------------------------------------------------------

    changed = (ui->editOtherOptions->text() != mDefaultOtherOptions);
    if (changed)
    {
        getDifference(ui->editOtherOptions, mDefaultOtherOptions, result);
    }

    //// =======================================================================

    return result;
}

void DialogFileOptions::resetValue()
{
    int flags = mFlagButtons.value(sender(), 0);

    resetValue(flags);
}

void DialogFileOptions::checkValues()
{
    bool enabled = false;

    enabled = (ui->editIncludePaths->text() != mDefaultIncludePaths);
    ui->buttonResetIncludePaths->setEnabled(enabled);

    enabled = (ui->editDefines->text() != mDefaultDefines);
    ui->buttonResetDefines->setEnabled(enabled);

    enabled = (ui->editUndefines->text() != mDefaultUndefines);
    ui->buttonResetUndefines->setEnabled(enabled);

    enabled = (ui->checkFullSymbolicDebug->isChecked() != mDefaultEnabledDebug);
    ui->buttonResetFullSymbolicDebug->setEnabled(enabled);

    enabled = (ui->checkDataAccessModel->isChecked() != mDefaultEnabledDataAccessModel) ||
              (ui->editDataAccessModel->currentText() != mDefaultDataAccessModel);
    ui->buttonResetDataAccessModel->setEnabled(enabled);

    enabled = (ui->checkMh->isChecked() != mDefaultEnabledMh) ||
              (ui->editMh->value() != mDefaultMh);
    ui->buttonResetMh->setEnabled(enabled);

    enabled = (ui->checkMi->isChecked() != mDefaultEnabledMi) ||
              (ui->editMi->value() != mDefaultMi);
    ui->buttonResetMi->setEnabled(enabled);

    enabled = (ui->checkMv->isChecked() != mDefaultEnabledMv) ||
              (ui->editMv->currentText() != mDefaultMv);
    ui->buttonResetMv->setEnabled(enabled);

    enabled = (ui->checkOptimizationLevel->isChecked() != mDefaultEnabledOptimizationLevel) ||
              (ui->editOptimizationLevel->currentIndex() != mDefaultOptimizationLevel);
    ui->buttonResetOptimizationLevel->setEnabled(enabled);

    enabled = (ui->checkOptimizationType->isChecked() != mDefaultEnabledOptimizationType) ||
              (ui->editOptimizationType->value() != mDefaultOptimizationType);
    ui->buttonResetOptimizationType->setEnabled(enabled);

    enabled = (ui->editOtherOptions->text() != mDefaultOtherOptions);
    ui->buttonResetOtherOptions->setEnabled(enabled);
}

void DialogFileOptions::resetValue(int flags)
{
    if ((flags & FLAG_INCLUDE_PATHS) != 0)
    {
        ui->editIncludePaths->setText(mDefaultIncludePaths);
    }

    if ((flags & FLAG_DEFINES) != 0)
    {
        ui->editDefines->setText(mDefaultDefines);
    }

    if ((flags & FLAG_UNDEFINES) != 0)
    {
        ui->editUndefines->setText(mDefaultUndefines);
    }

    if ((flags & FLAG_DEBUG) != 0)
    {
        ui->checkFullSymbolicDebug->setChecked(mDefaultEnabledDebug);
    }

    if ((flags & FLAG_DATA_ACCESS_MODEL) != 0)
    {
        ui->checkDataAccessModel->setChecked(mDefaultEnabledDataAccessModel);
        ui->editDataAccessModel->setEditText(mDefaultDataAccessModel);
    }

    if ((flags & FLAG_MH) != 0)
    {
        ui->checkMh->setChecked(mDefaultEnabledMh);
        ui->editMh->setValue(mDefaultMh);
    }

    if ((flags & FLAG_MI) != 0)
    {
        ui->checkMi->setChecked(mDefaultEnabledMi);
        ui->editMi->setValue(mDefaultMi);
    }

    if ((flags & FLAG_MV) != 0)
    {
        ui->checkMv->setChecked(mDefaultEnabledMv);
        ui->editMv->setEditText(mDefaultMv);
    }

    if ((flags & FLAG_OPTIMIZATION_LEVEL) != 0)
    {
        ui->checkOptimizationLevel->setChecked(mDefaultEnabledOptimizationLevel);
        ui->editOptimizationLevel->setCurrentIndex(mDefaultOptimizationLevel);
    }

    if ((flags & FLAG_OPTIMIZATION_TYPE) != 0)
    {
        ui->checkOptimizationType->setChecked(mDefaultEnabledOptimizationType);
        ui->editOptimizationType->setValue(mDefaultOptimizationType);
    }

    if ((flags & FLAG_OTHER_OPTIONS) != 0)
    {
        ui->editOtherOptions->setText(mDefaultOtherOptions);
    }
}
