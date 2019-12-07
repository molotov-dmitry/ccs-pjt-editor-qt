#include "listeditor.h"
#include "ui_listeditor.h"

#include <QFontDatabase>

#include "dialoglisteditor.h"

ListEditor::ListEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListEditor),
    mSeparator(';')
{
    ui->setupUi(this);

    connect(ui->edit, SIGNAL(textChanged(QString)), this, SIGNAL(listUpdated()));
    connect(ui->edit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));

    ui->edit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

ListEditor::~ListEditor()
{
    delete ui;
}

void ListEditor::setText(const QString &text)
{
    ui->edit->setText(text);
}

QString ListEditor::text() const
{
    return ui->edit->text();
}

QStringList ListEditor::items() const
{
    return text().split(mSeparator);
}

void ListEditor::setItems(const QStringList& items)
{
    setText(items.join(mSeparator));
}

void ListEditor::addItem(const QString& item, bool insertMulti)
{
    QStringList itemList = items();

    if (insertMulti || not itemList.contains(item))
    {
        itemList.append(item);
        setItems(itemList);
    }
}

void ListEditor::removeItem(const QString& item)
{
    QStringList itemList = items();

    if (itemList.contains(item))
    {
        itemList.removeAll(item);
        setItems(itemList);
    }
}

void ListEditor::clear()
{
    ui->edit->clear();
}

void ListEditor::setDragEnabled(bool enabled)
{
    ui->edit->setDragEnabled(enabled);
}

bool ListEditor::dragEnabled() const
{
    return ui->edit->dragEnabled();
}

void ListEditor::setReadOnly(bool readOnly)
{
    ui->edit->setReadOnly(readOnly);
}

bool ListEditor::isReadOnly() const
{
    return ui->edit->isReadOnly();
}

void ListEditor::setPlaceholderText(const QString &text)
{
    ui->edit->setPlaceholderText(text);
}

QString ListEditor::placeholderText() const
{
    return ui->edit->placeholderText();
}

void ListEditor::setClearButtonEnabled(bool enabled)
{
    ui->edit->setClearButtonEnabled(enabled);
}

bool ListEditor::isClearButtonEnabled() const
{
    return ui->edit->isClearButtonEnabled();
}

void ListEditor::setSeparator(QChar sep)
{
    mSeparator = sep;
}

QChar ListEditor::separator() const
{
    return mSeparator;
}

void ListEditor::on_buttonEditAsList_clicked()
{
    DialogListEditor listEditor;

    if (not ui->edit->placeholderText().isEmpty())
    {
        listEditor.setWindowTitle(ui->edit->placeholderText());
    }

    listEditor.setItemList(ui->edit->text(), mSeparator);

    if (listEditor.exec() == QDialog::Accepted)
    {
        ui->edit->setText(listEditor.itemList(mSeparator));
    }
}
