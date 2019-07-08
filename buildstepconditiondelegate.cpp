﻿#include "buildstepconditiondelegate.h"

#include <QComboBox>

#include "buildstepslist.h"

BuildStepConditionDelegate::BuildStepConditionDelegate(QObject *parent) : QItemDelegate(parent)
{

}

QWidget *BuildStepConditionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox* editor = new QComboBox(parent);

    for (int i = 0; i < BUILD_CONDITION_COUNT; ++i)
    {
        editor->addItem(buildStepConditionString(i));
    }

    setEditorData(editor, index);

    connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndex(int)));

    return editor;
}

void BuildStepConditionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = dynamic_cast<QComboBox*>(editor);

    int value = index.data(Qt::UserRole + 1).toInt();

    comboBox->setCurrentIndex(value);
}

void BuildStepConditionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = dynamic_cast<QComboBox*>(editor);

    model->setData(index, comboBox->currentIndex(), Qt::UserRole + 1);
    model->setData(index, comboBox->currentText(), Qt::DisplayRole);

}

QString BuildStepConditionDelegate::buildStepConditionString(int condition)
{
    switch (condition)
    {
    case BUILD_IF_ANY_FILE_BUILDS:
        return QString::fromUtf8("If any file builds");

    case BUILD_ALWAYS:
        return QString::fromUtf8("Always");

    default:
        return QString();
    }
}

void BuildStepConditionDelegate::changeIndex(int)
{
    QObject* s = sender();

    if (s->isWidgetType())
    {
        QWidget* w = dynamic_cast<QWidget*>(s);

        emit commitData(w);
    }

}
