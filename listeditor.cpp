#include "listeditor.h"
#include "ui_listeditor.h"

#include "dialoglisteditor.h"

ListEditor::ListEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListEditor),
    mSeparator(';')
{
    ui->setupUi(this);
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
