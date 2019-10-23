﻿#ifndef BUILDSTEPSLIST_H
#define BUILDSTEPSLIST_H

#include <QWidget>

#include <QList>
#include <QPair>

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
    void addBuildStep(const QString& command, int condition);

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

    void on_buttonMoveUp_clicked();
    void on_buttonMoveDown_clicked();
    void on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
};

#endif // BUILDSTEPSLIST_H
