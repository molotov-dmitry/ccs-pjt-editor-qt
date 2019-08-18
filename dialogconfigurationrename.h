#ifndef DIALOGCONFIGURATIONRENAME_H
#define DIALOGCONFIGURATIONRENAME_H

#include <QDialog>

namespace Ui {
class DialogConfigurationRename;
}

class DialogConfigurationRename : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfigurationRename(QWidget *parent = nullptr);
    ~DialogConfigurationRename();

    void setName(const QString& name);
    QString name() const;

private:
    Ui::DialogConfigurationRename *ui;
};

#endif // DIALOGCONFIGURATIONRENAME_H
