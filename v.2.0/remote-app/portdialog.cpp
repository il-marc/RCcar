#include "portdialog.h"
#include "ui_portdialog.h"

portDialog::portDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::portDialog)
{
    isOK = false;

    ui->setupUi(this);
    setFixedSize(200, 120);
    setWindowFlags(Qt::WindowTitleHint);
    ui->applyButton->setDisabled(true);
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.description().contains(QRegExp("Bluetooth"))) {
            QRadioButton * radioButton = new QRadioButton(info.portName());
            connect(radioButton, SIGNAL(pressed()), this, SLOT(radioButtonPressed()));
            ui->portRadioButtonsLayout->addWidget(radioButton);
        }
    }
    ui->portRadioButtonsLayout->addSpacerItem(new QSpacerItem(20,40,QSizePolicy::Minimum,QSizePolicy::Expanding));
    connect(ui->applyButton, SIGNAL(released()), this, SLOT(applyButtonPressed()));
    connect(ui->cencelButton, SIGNAL(released()), this, SLOT(cencelButtonPressed()));
}
void portDialog::radioButtonPressed()
{
    QRadioButton* radioButton = qobject_cast<QRadioButton *>(QObject::sender());
    ui->applyButton->setEnabled(true);
    portName = radioButton->text();
}
void portDialog::applyButtonPressed()
{
    isOK = true;
    close();
}
void portDialog::cencelButtonPressed()
{
    portName.clear();
    close();
}

portDialog::~portDialog()
{
    delete ui;
}
