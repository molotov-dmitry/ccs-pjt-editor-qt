#include "buildstepslist.h"
#include "ui_buildstepslist.h"

#include <QFontDatabase>
#include <QComboBox>
#include <QTextCodec>

#include "buildstepconditiondelegate.h"

#include "parser/buildstep.h"

BuildStepsList::BuildStepsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BuildStepsList),
    mDelegate(nullptr)
{
    ui->setupUi(this);

    ui->tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    mDelegate = new BuildStepConditionDelegate(this);

    ui->tree->setItemDelegateForColumn(0, mDelegate);

    connect(ui->tree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(updateCommand(QTreeWidgetItem *, int)));

    connect(ui->tree->model(),
            SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)),
            this,
            SLOT(updateList()));

    updateList();


}

BuildStepsList::~BuildStepsList()
{
    delete ui;
}

bool BuildStepsList::isFile()
{
    return mDelegate->isIsFileCondition();
}

void BuildStepsList::setIsFile(bool isFile)
{
    mDelegate->setIsFileCondition(isFile);
}

void BuildStepsList::clear()
{
    ui->tree->clear();
}

void BuildStepsList::addBuildStep(const QString &command, int condition)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setFlags((item->flags() & ~Qt::ItemIsDropEnabled) | Qt::ItemIsEditable);
    item->setFont(1, QFontDatabase::systemFont(QFontDatabase::FixedFont));

    item->setIcon(1, commandIcon(command));

    item->setText(1, command);
    item->setData(0, Qt::UserRole + 1, condition);
    item->setData(0, Qt::DisplayRole, BuildStepConditionDelegate::buildStepConditionString(condition, isFile()));

    ui->tree->addTopLevelItem(item);
}

void BuildStepsList::addBuildStep(const BuildStep& command)
{
    QTextCodec* codec = QTextCodec::codecForName("Windows-1251");

    addBuildStep(codec->toUnicode(command.command().c_str()),
                 command.condition());
}

QList<QPair<QString, int> > BuildStepsList::buildSteps() const
{
    QList<QPair<QString, int> > result;

    for (int i = 0; i < ui->tree->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = ui->tree->topLevelItem(i);
        QPair<QString, int> buildStep(item->text(1), item->data(0, Qt::UserRole + 1).toInt());
        result.append(buildStep);
    }

    return result;
}

QIcon BuildStepsList::commandIcon(const QString &command)
{
    QString commandName = command.split(' ').first();

    const char* themeIcon = nullptr;

    if (commandName.endsWith(".exe"))
    {
        themeIcon = "wine";
    }
    else if (commandName == "bash" || commandName.endsWith(".sh"))
    {
        themeIcon = "bash";
    }
    else
    {
        themeIcon = "application-x-executable";
    }

    return QIcon::fromTheme(themeIcon);
}

void BuildStepsList::updateCommand(QTreeWidgetItem *item, int column)
{
    if (column == 1)
    {
        item->setIcon(1, commandIcon(item->text(1)));
    }

    emit updated();
}

void BuildStepsList::on_buttonAdd_clicked()
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setFlags((item->flags() & ~Qt::ItemIsDropEnabled) | Qt::ItemIsEditable);
    item->setFont(1, QFontDatabase::systemFont(QFontDatabase::FixedFont));

    item->setIcon(1, commandIcon(QString()));

    item->setText(1, QString());
    item->setData(0, Qt::UserRole + 1, BuildStep::IF_ANY_FILE_BUILDS);
    item->setData(0, Qt::DisplayRole, BuildStepConditionDelegate::buildStepConditionString(BuildStep::IF_ANY_FILE_BUILDS, isFile()));

    ui->tree->addTopLevelItem(item);

    foreach (QTreeWidgetItem* item, ui->tree->selectedItems())
    {
        item->setSelected(false);
    }

    item->setSelected(true);

    ui->tree->editItem(item, 1);

    emit updated();
}

void BuildStepsList::on_buttonRemove_clicked()
{
    QList<QTreeWidgetItem*> items = ui->tree->selectedItems();

    foreach (QTreeWidgetItem* item, items)
    {
        delete item;
    }

    if (not items.isEmpty())
    {
        emit updated();
    }
}

void BuildStepsList::on_buttonMoveUp_clicked()
{
    int index = ui->tree->indexOfTopLevelItem(ui->tree->currentItem());
    if (index < 0)
    {
        return;
    }

    QTreeWidgetItem* item = ui->tree->takeTopLevelItem(index);

    ui->tree->insertTopLevelItem(index - 1, item);

    ui->tree->setCurrentItem(item);
}

void BuildStepsList::on_buttonMoveDown_clicked()
{
    int index = ui->tree->indexOfTopLevelItem(ui->tree->currentItem());
    if (index < 0)
    {
        return;
    }

    QTreeWidgetItem* item = ui->tree->takeTopLevelItem(index);

    ui->tree->insertTopLevelItem(index + 1, item);

    ui->tree->setCurrentItem(item);
}

void BuildStepsList::on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    ui->buttonRemove->setEnabled(current != nullptr);

    int index = ui->tree->indexOfTopLevelItem(current);

    ui->buttonMoveUp->setEnabled(index > 0);
    ui->buttonMoveDown->setEnabled(index > -1 && index < ui->tree->topLevelItemCount() - 1);
}

void BuildStepsList::updateList()
{
    on_tree_currentItemChanged(ui->tree->currentItem(), nullptr);
}
