#include "dialoglisteditor.h"
#include "ui_dialoglisteditor.h"

#include <QFontDatabase>

DialogListEditor::DialogListEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogListEditor)
{
    ui->setupUi(this);

    ui->list->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    on_list_currentItemChanged(ui->list->currentItem(), nullptr);
    on_list_itemSelectionChanged();

    foreach (QAbstractButton* button, ui->buttonBox->buttons())
    {
        if (button != nullptr)
        {
            button->setMinimumHeight(32);
        }
    }
}

DialogListEditor::~DialogListEditor()
{
    delete ui;
}

void DialogListEditor::setItemList(const QString &list, QChar sep)
{
    ui->list->clear();

    if (list.isEmpty())
    {
        return;
    }

    foreach (const QString& string, list.split(sep))
    {
        QListWidgetItem* item = new  QListWidgetItem(string);

        item->setFlags(item->flags() | Qt::ItemIsEditable);

        ui->list->addItem(item);
    }
}

QString DialogListEditor::itemList(QChar sep) const
{
    QStringList list;

    for (int i = 0; i < ui->list->count(); ++i)
    {
        QListWidgetItem* item = ui->list->item(i);

        list.append(item->text());
    }

    return list.join(sep);
}

void DialogListEditor::on_buttonAdd_clicked()
{
    QListWidgetItem* item = new  QListWidgetItem();

    item->setFlags(item->flags() | Qt::ItemIsEditable);

    ui->list->addItem(item);

    foreach (QListWidgetItem* item, ui->list->selectedItems())
    {
        item->setSelected(false);
    }

    item->setSelected(true);

    ui->list->setCurrentItem(item);

    ui->list->editItem(item);
}

void DialogListEditor::on_buttonRemove_clicked()
{
    foreach (QListWidgetItem* item, ui->list->selectedItems())
    {
        delete item;
    }
}

void DialogListEditor::on_buttonMoveUp_clicked()
{
    int index = ui->list->currentRow();
    if (index < 0)
    {
        return;
    }

    QListWidgetItem* item = ui->list->takeItem(index);

    ui->list->insertItem(index - 1, item);

    ui->list->setCurrentItem(item);
}

void DialogListEditor::on_buttonMoveDown_clicked()
{
    int index = ui->list->currentRow();
    if (index < 0)
    {
        return;
    }

    QListWidgetItem* item = ui->list->takeItem(index);

    ui->list->insertItem(index + 1, item);

    ui->list->setCurrentItem(item);
}

void DialogListEditor::on_list_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    int index = ui->list->currentRow();

    ui->buttonMoveUp->setEnabled(index > 0);
    ui->buttonMoveDown->setEnabled(index > -1 && index < ui->list->count() - 1);
}

void DialogListEditor::on_list_itemSelectionChanged()
{
    ui->buttonRemove->setEnabled(not ui->list->selectedItems().isEmpty());
}
