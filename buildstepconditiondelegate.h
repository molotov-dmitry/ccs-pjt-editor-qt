#ifndef BUILDSTEPCONDITIONDELEGATE_H
#define BUILDSTEPCONDITIONDELEGATE_H

#include <QItemDelegate>

class BuildStepConditionDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit BuildStepConditionDelegate(QObject *parent = nullptr);

    bool isIsFileCondition() const;
    void setIsFileCondition(bool fileCondition);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    static QString buildStepConditionString(int condition, bool file);

private slots:

    void changeIndex(int);

private:

    bool mIsFile;
};

#endif // BUILDSTEPCONDITIONDELEGATE_H
