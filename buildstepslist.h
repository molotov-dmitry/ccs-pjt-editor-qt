#ifndef BUILDSTEPSLIST_H
#define BUILDSTEPSLIST_H

#include <QWidget>

#include <QList>
#include <QPair>

enum BuildCondition
{
    BUILD_IF_ANY_FILE_BUILDS,
    BUILD_ALWAYS,

    BUILD_CONDITION_COUNT
};

class QTreeWidgetItem;

namespace Ui {
class BuildStepsList;
}

class BuildStepsList : public QWidget
{
    Q_OBJECT

public:
    explicit BuildStepsList(QWidget *parent = nullptr);
    ~BuildStepsList();

    void clear();
    void addBuildStep(const QString& command, BuildCondition condition);

    QList< QPair<QString, int> > buildSteps() const;


signals:

    void updated();

private:
    Ui::BuildStepsList *ui;

    static QIcon commandIcon(const QString& command);

private slots:

    void updateCommand(QTreeWidgetItem *item, int column);

    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();

};

#endif // BUILDSTEPSLIST_H
