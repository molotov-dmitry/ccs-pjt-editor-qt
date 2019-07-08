#ifndef BUILDSTEPCONDITIONDELEGATE_H
#define BUILDSTEPCONDITIONDELEGATE_H

#include <QItemDelegate>

class BuildStepConditionDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit BuildStepConditionDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    static QString buildStepConditionString(int condition);

private slots:

    void changeIndex(int);
};

#endif // BUILDSTEPCONDITIONDELEGATE_H
