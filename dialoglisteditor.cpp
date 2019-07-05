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
