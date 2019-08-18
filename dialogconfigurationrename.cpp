#include "dialogconfigurationrename.h"
#include "ui_dialogconfigurationrename.h"

DialogConfigurationRename::DialogConfigurationRename(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfigurationRename)
{
    ui->setupUi(this);
}

DialogConfigurationRename::~DialogConfigurationRename()
{
    delete ui;
}

void DialogConfigurationRename::setName(const QString& name)
{
    ui->editName->setText(name);

    ui->editName->selectAll();
}

QString DialogConfigurationRename::name() const
{
    return ui->editName->text();
}
