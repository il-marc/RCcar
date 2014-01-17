#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QKeyEvent"
#include "QCloseEvent"
#include "QtSerialPort/QSerialPort"
#include "QtSerialPort/QSerialPortInfo"
#include "QDebug"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    DownA = 0;
    DownW = 0;
    DownS = 0;
    DownD = 0;
    turn = 0;
    move = 0;
    connected = false;
    ui->setupUi(this);
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(measureVoltage()));

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->plainTextEdit->appendHtml("<p><b>Serial</b> : Detected "+
                                      info.portName() + " (" +
                                      info.description() + ")</p>");
        QAction * newAct = new QAction(info.portName(), this);
        connect(newAct, SIGNAL(triggered()), this, SLOT(serialConnect()));
        ui->menuPort->addAction(newAct);
    }
    stateProcess();
}
void MainWindow::serialConnect()
{
    ui->menuPort->setDisabled(true);
    ui->plainTextEdit->appendHtml("<p><b>Serial</b> : Connecting "+((QAction *)sender())->text()+"</p>");
    serial = new QSerialPort (this);
    serial->setPortName(((QAction *)sender())->text());
    if (serial->open(QIODevice::ReadWrite)) {
        ui->plainTextEdit->appendHtml("<p><b>Serial</b> : Opened</p>");
        if (serial->setBaudRate(QSerialPort::Baud9600)
                && serial->setDataBits(QSerialPort::Data8)
                && serial->setParity(QSerialPort::NoParity)
                && serial->setStopBits(QSerialPort::OneStop)
                && serial->setFlowControl(QSerialPort::SoftwareControl)) {
            ui->plainTextEdit->appendHtml("<p style='color:green'><b>Serial</b> : Connected!</p>");
            connected = true;
            connect(serial, SIGNAL(readyRead()), this, SLOT(getSerialMessage()));
            timer->start(1000);
        } else {
            serial->close();
            ui->plainTextEdit->appendHtml("<p><b>Serial</b> : Open error: "+ serial->errorString() + "</p>");
            ui->menuPort->setEnabled(true);
        }
    } else {
        ui->plainTextEdit->appendHtml("<p style='color:red'><b>Serial</b> : Configure error: "+ serial->errorString() + "</p>");
        ui->menuPort->setEnabled(true);
    }
}

void MainWindow::stateProcess()
{
    if (connected) {
        if (DownW ^ DownS) {
            if(DownW) {
                if (move != 1) {
                    move = 1;
                    serial->putChar(FORWARD);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Forward</p>");
                }
            } else {
                if (move != -1) {
                    move = -1;
                    serial->putChar(BACKWARD);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Backward</p>");
                }
            }
        } else {
            if (move != 0) {
                move = 0;
                serial->putChar(STOP);
                ui->plainTextEdit->appendHtml("<p><b>Control</b> : Stop</p>");
            }
        }
        if (DownA ^ DownD) {
            if(DownA) {
                if (turn != -1 ) {
                    turn = -1;
                    serial->putChar(LEFT);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Left</p>");
                }
            } else {
                if (turn != 1 ) {
                    turn = 1;
                    serial->putChar(RIGHT);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Right</p>");

                }
            }
        } else {
            if (turn != 0 ) {
                turn = 0;
                serial->putChar(CENTER);
                ui->plainTextEdit->appendHtml("<p><b>Control</b> : Center</p>");
            }
        }
        QString imageUrl = "background-image: url(:/images/states/";
        switch (move) {
        case -1:
            imageUrl.append("back_");
            break;
        case 0:
            imageUrl.append("stop_");
            break;
        case 1:
            imageUrl.append("forward_");
            break;
        default:
            break;
        }
        switch (turn) {
        case -1:
            imageUrl.append("left.png)");
            break;
        case 0:
            imageUrl.append("center.png)");
            break;
        case 1:
            imageUrl.append("right.png)");
            break;
        default:
            break;
        }
        ui->widget->setStyleSheet(imageUrl);
    } else {
        QString imageUrl = "background-image: url(:/images/states/disabled.png)";
        ui->widget->setStyleSheet(imageUrl);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        switch (event->key()) {
        case Qt::Key_W:
            DownW = 1;
            break;
        case Qt::Key_S:
            DownS = 1;
            break;
        case Qt::Key_A:
            DownA = 1;
            break;
        case Qt::Key_D:
            DownD = 1;
            break;
        default:
            break;
        }
        stateProcess();
    }
}
void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        switch (event->key()) {
        case Qt::Key_W:
            DownW = 0;
            break;
        case Qt::Key_S:
            DownS = 0;
            break;
        case Qt::Key_A:
            DownA = 0;
            break;
        case Qt::Key_D:
            DownD = 0;
            break;
        default:
            break;
        }
        stateProcess();
    }
}
void MainWindow::measureVoltage()
{
    serial->putChar(VOLTAGE);
}
void MainWindow::getSerialMessage()
{
    QByteArray msg = serial->readAll();
    unsigned char v = msg.at(0);
    if (v != 0) {
        float voltage = v/(182.0/12.0);
        ui->lcdNumber_2->display(voltage);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (connected) {
        serial->close();
    }
    event->accept();
}
