#include "dialoglinkordereditor.h"
#include "ui_dialoglinkordereditor.h"

DialogLinkOrderEditor::DialogLinkOrderEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLinkOrderEditor)
{
    ui->setupUi(this);

    foreach (QAbstractButton* button, ui->buttonBox->buttons())
    {
        if (button != nullptr)
        {
            button->setMinimumHeight(32);
        }
    }

    on_listUnordered_currentItemChanged(ui->listUnordered->currentItem(), nullptr);
    on_listOrdered_currentItemChanged(ui->listOrdered->currentItem(), nullptr);

    connect(ui->listOrdered->model(),
            SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            this,
            SLOT(updateList()));
}

DialogLinkOrderEditor::~DialogLinkOrderEditor()
{
    delete ui;
}

void DialogLinkOrderEditor::addUnordered(const QString& file)
{
    ui->listUnordered->addItem(file);
}

void DialogLinkOrderEditor::addOrdered(const QString& file, int order)
{
    int linkOrder = mOrderList.count() - 1;

    for (int i = 0; i < mOrderList.count(); ++i)
    {
        if (order < mOrderList[i])
        {
            linkOrder = i;
            break;
        }
    }

    mOrderList.insert(linkOrder, order);

    ui->listOrdered->insertItem(linkOrder, file);

    foreach (QListWidgetItem* item, ui->listUnordered->findItems(file, Qt::MatchExactly))
    {
        delete item;
    }
}

QStringList DialogLinkOrderEditor::order() const
{
    QStringList result;

    for(int i = 0; i < ui->listOrdered->count(); ++i)
    {
        QString str = ui->listOrdered->item(i)->text();
        result.append(str);
    }

    return result;
}

void DialogLinkOrderEditor::on_buttonAdd_clicked()
{
    moveCurrentItem(ui->listUnordered, ui->listOrdered);
}

void DialogLinkOrderEditor::on_buttonRemove_clicked()
{
    moveCurrentItem(ui->listOrdered, ui->listUnordered);
}

void DialogLinkOrderEditor::on_buttonMoveUp_clicked()
{
    int index = ui->listOrdered->currentRow();
    if (index < 0)
    {
        return;
    }

    QListWidgetItem* item = ui->listOrdered->takeItem(index);

    ui->listOrdered->insertItem(index - 1, item);

    ui->listOrdered->setCurrentItem(item);
}

void DialogLinkOrderEditor::on_buttonMoveDown_clicked()
{
    int index = ui->listOrdered->currentRow();
    if (index < 0)
    {
        return;
    }

    QListWidgetItem* item = ui->listOrdered->takeItem(index);

    ui->listOrdered->insertItem(index + 1, item);

    ui->listOrdered->setCurrentItem(item);
}

void DialogLinkOrderEditor::on_listUnordered_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (current != nullptr)
    {
        ui->listOrdered->setCurrentItem(nullptr);
    }

    ui->buttonAdd->setEnabled(current != nullptr);
}

void DialogLinkOrderEditor::on_listOrdered_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (current != nullptr)
    {
        ui->listUnordered->setCurrentItem(nullptr);
    }

    ui->buttonRemove->setEnabled(current != nullptr);

    int index = ui->listOrdered->currentRow();

    ui->buttonMoveUp->setEnabled(index > 0);
    ui->buttonMoveDown->setEnabled(index > -1 && index < ui->listOrdered->count() - 1);
}

void DialogLinkOrderEditor::on_listOrdered_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);

    on_buttonRemove_clicked();
}

void DialogLinkOrderEditor::on_listUnordered_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);

    on_buttonAdd_clicked();
}

void DialogLinkOrderEditor::moveCurrentItem(QListWidget* from, QListWidget* to)
{
    int row = from->currentRow();
    if (row < 0)
    {
        return;
    }

    QListWidgetItem* item = from->takeItem(row);
    if (item != nullptr)
    {
        to->addItem(item);

        if (from->count() > 0)
        {
            if (row < from->count())
            {
                from->setCurrentRow(row);
            }
            else
            {
                from->setCurrentRow(row - 1);
            }
        }
        else
        {
            to->setCurrentItem(item);
        }
    }
}

void DialogLinkOrderEditor::updateList()
{
    on_listOrdered_currentItemChanged(ui->listOrdered->currentItem(), nullptr);
}
