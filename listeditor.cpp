#include "listeditor.h"
#include "ui_listeditor.h"

ListEditor::ListEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListEditor)
{
    ui->setupUi(this);
}

ListEditor::~ListEditor()
{
    delete ui;
}
