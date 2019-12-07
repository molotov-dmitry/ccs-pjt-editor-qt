#ifndef DIALOGFILEOPTIONS_H
#define DIALOGFILEOPTIONS_H

#include <QDialog>
#include <QMap>

#include "parser/configsettings.h"
#include "parser/fileoptions.h"

namespace Ui {
class DialogFileOptions;
}

class DialogFileOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFileOptions(const ConfigSettings& config, const FileOptions& fileOptions, QWidget *parent = nullptr);

    ~DialogFileOptions();

public slots:

    void resetValue();
    void checkValues();

private:

    enum Flags
    {
        FLAG_INCLUDE_PATHS      = 0x0001,
        FLAG_DEFINES            = 0x0002,
        FLAG_UNDEFINES          = 0x0004,

        FLAG_DEBUG              = 0x0008,
        FLAG_DATA_ACCESS_MODEL  = 0x0010,
        FLAG_MH                 = 0x0020,
        FLAG_MI                 = 0x0040,
        FLAG_MV                 = 0x0080,
        FLAG_OPTIMIZATION_LEVEL = 0x0100,
        FLAG_OPTIMIZATION_TYPE  = 0x0200,
        FLAG_OTHER_OPTIONS      = 0x0400,

        FLAG_ALL                = 0xFFFF
    };

    Ui::DialogFileOptions *ui;

    QMap<QObject*, int> mFlagButtons;

    QString mDefaultIncludePaths;
    QString mDefaultDefines;
    QString mDefaultUndefines;

    bool mDefaultEnabledDebug;
    bool mDefaultEnabledDataAccessModel;
    bool mDefaultEnabledMh;
    bool mDefaultEnabledMi;
    bool mDefaultEnabledMv;
    bool mDefaultEnabledOptimizationLevel;
    bool mDefaultEnabledOptimizationType;
    bool mDefaultEnabledOtherOptions;

    QString mDefaultDataAccessModel;
    int mDefaultMh;
    int mDefaultMi;
    QString mDefaultMv;
    int mDefaultOptimizationLevel;
    int mDefaultOptimizationType;
    QString mDefaultOtherOptions;

    void resetValue(int flags);
};

#endif // DIALOGFILEOPTIONS_H
