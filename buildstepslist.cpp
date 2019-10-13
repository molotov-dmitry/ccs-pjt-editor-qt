#include "buildstepslist.h"
#include "ui_buildstepslist.h"

#include <QFontDatabase>
#include <QComboBox>

#include "buildstepconditiondelegate.h"

#include "parser/buildstep.h"

BuildStepsList::BuildStepsList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BuildStepsList)
{
    ui->setupUi(this);

    ui->tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    BuildStepConditionDelegate* delegate = new BuildStepConditionDelegate(this);

    ui->tree->setItemDelegateForColumn(0, delegate);

    connect(ui->tree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(updateCommand(QTreeWidgetItem *, int)));
}

BuildStepsList::~BuildStepsList()
{
    delete ui;
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
    item->setData(0, Qt::DisplayRole, BuildStepConditionDelegate::buildStepConditionString(condition));

    ui->tree->addTopLevelItem(item);
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
    item->setData(0, Qt::DisplayRole, BuildStepConditionDelegate::buildStepConditionString(BuildStep::IF_ANY_FILE_BUILDS));

    ui->tree->addTopLevelItem(item);

    foreach (QTreeWidgetItem* item, ui->tree->selectedItems())
    {
        item->setSelected(false);
    }

    item->setSelected(true);

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
