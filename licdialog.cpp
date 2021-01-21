#include "licdialog.h"
#include "ui_licdialog.h"

LicDialog::LicDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicDialog)
{
    ui->setupUi(this);
}

LicDialog::~LicDialog()
{
    delete ui;
}
