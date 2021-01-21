#ifndef LICDIALOG_H
#define LICDIALOG_H

#include <QDialog>

namespace Ui {
class LicDialog;
}

class LicDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LicDialog(QWidget *parent = nullptr);
    ~LicDialog();

private:
    Ui::LicDialog *ui;
};

#endif // LICDIALOG_H
