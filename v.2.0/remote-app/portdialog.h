#ifndef PORTDIALOG_H
#define PORTDIALOG_H

#include <QDialog>
#include <QRadioButton>
//#include <QSpacerItem>
#include "QtSerialPort/QSerialPort"
#include "QtSerialPort/QSerialPortInfo"

namespace Ui {
class portDialog;
}

class portDialog : public QDialog
{
    Q_OBJECT
private slots:
    void radioButtonPressed();
    void applyButtonPressed();
    void cencelButtonPressed();
public:
    explicit portDialog(QWidget *parent = 0);
    ~portDialog();

    QString portName;
    bool isOK;

private:
    Ui::portDialog *ui;
};

#endif // PORTDIALOG_H
