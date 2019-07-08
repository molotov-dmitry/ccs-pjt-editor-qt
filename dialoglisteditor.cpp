#include "dialoglisteditor.h"
#include "ui_dialoglisteditor.h"

DialogListEditor::DialogListEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogListEditor)
{
    ui->setupUi(this);
}

DialogListEditor::~DialogListEditor()
{
    delete ui;
}

void DialogListEditor::setItemList(const QString &list, QChar sep)
{
    ui->list->clear();

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

    ui->list->editItem(item);
}

void DialogListEditor::on_buttonRemove_clicked()
{
    foreach (QListWidgetItem* item, ui->list->selectedItems())
    {
        delete item;
    }
}
