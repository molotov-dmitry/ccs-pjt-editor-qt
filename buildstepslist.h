#ifndef BUILDSTEPSLIST_H
#define BUILDSTEPSLIST_H

#include <QWidget>

#include <QList>
#include <QPair>


#include "parser/buildsteplist.h"

class QTreeWidgetItem;
class QTextCodec;

namespace Ui {
class BuildStepsList;
}

class BuildStepConditionDelegate;

class BuildStepsList : public QWidget
{
    Q_OBJECT

public:
    explicit BuildStepsList(QWidget *parent = nullptr);
    ~BuildStepsList();

    bool isFile();
    void setIsFile(bool isFile);

    void clear();
    void addBuildStep(const QString& command, int condition);
    void addBuildStep(const BuildStep& command);
    void addBuildSteps(const BuildStepList& list);

    void setBuildSteps(const BuildStepList& list);

    BuildStepList buildStepList() const;

    QList< QPair<QString, int> > buildSteps() const;


signals:

    void updated();

private:
    Ui::BuildStepsList *ui;

    BuildStepConditionDelegate* mDelegate;
    QTextCodec* mCodec;

    static QIcon commandIcon(const QString& command);

private slots:

    void updateCommand(QTreeWidgetItem *item, int column);

    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();

    void on_buttonMoveUp_clicked();
    void on_buttonMoveDown_clicked();
    void on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void updateList();
};

#endif // BUILDSTEPSLIST_H
