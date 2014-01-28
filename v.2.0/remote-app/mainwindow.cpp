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
    downA = false;
    downW = false;
    downS = false;
    downD = false;
    turn = CENTER;
    move = STOP;
    connected = false;

    ui->setupUi(this);

    //waitingVoltage = false;
    //waitingSpeed = false;

    voltageTimer = new QTimer(this);
    speedTimer = new QTimer(this);

    connect(voltageTimer, SIGNAL(timeout()), this, SLOT(measureVoltage()));
    connect(speedTimer, SIGNAL(timeout()), this, SLOT(measureSpeed()));

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->plainTextEdit->appendHtml("<p><b>Serial</b> : Detected "+
                                      info.portName() + " (" +
                                      info.description() + ")</p>");
        QAction * newAct = new QAction(info.portName(), this);
        connect(newAct, SIGNAL(triggered()), this, SLOT(serialConnect()));
        ui->menuPort->addAction(newAct);
    }
    //connect(ui->menuDiconnect, SIGNAL(triggered(QAction*)), this, SLOT(serialDisconnect()));
    //ui->menuDiconnect->setDisabled(true);
    stateProcess();
}
void MainWindow::serialConnect()
{
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
            ui->menuPort->setDisabled(true);
            //ui->menuDiconnect->setEnabled(true);
            connected = true;
            connect(serial, SIGNAL(readyRead()), this, SLOT(getSerialMessage()));
            voltageTimer->start(1000);
            //speedTimer->start(100);
            stateProcess();
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
void MainWindow::serialDisconnect() {
    ui->menuPort->setEnabled(true);
    //ui->menuDiconnect->setDisabled(false);
    connected = false;
    serial->close();
}

void MainWindow::stateProcess()
{
    if (connected) {
        if (downW ^ downS) {
            if(downW) {
                if (move != FORWARD) {
                    move = FORWARD;
                    serial->putChar(COMMAND_FORWARD);
                    serial->putChar(motorSpeed);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Forward</p>");
                }
            } else {
                if (move != BACKWARD) {
                    move = BACKWARD;
                    serial->putChar(COMMAND_BACKWARD);
                    serial->putChar(motorSpeed);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Backward</p>");
                }
            }
        } else {
            if (move != STOP) {
                move = STOP;
                serial->putChar(COMMAND_STOP);
                serial->putChar(0);
                ui->plainTextEdit->appendHtml("<p><b>Control</b> : Stop</p>");
            }
        }
        if (downA ^ downD) {
            if(downA) {
                if (turn != LEFT ) {
                    turn = LEFT;
                    serial->putChar(COMMAND_TURN);
                    serial->putChar(leftPos);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Left</p>");
                }
            } else {
                if (turn != RIGHT ) {
                    turn = RIGHT;
                    serial->putChar(COMMAND_TURN);
                    serial->putChar(rightPos);
                    ui->plainTextEdit->appendHtml("<p><b>Control</b> : Right</p>");
                }
            }
        } else {
            if (turn != CENTER ) {
                turn = CENTER;
                serial->putChar(COMMAND_TURN);
                serial->putChar(centerPos);
                ui->plainTextEdit->appendHtml("<p><b>Control</b> : Center</p>");
            }
        }
        QString imageUrl = "background-image: url(:/images/states/";
        switch (move) {
        case BACKWARD:
            imageUrl.append("back_");
            break;
        case STOP:
            imageUrl.append("stop_");
            break;
        case FORWARD:
            imageUrl.append("forward_");
            break;
        default:
            break;
        }
        switch (turn) {
        case LEFT:
            imageUrl.append("left.png)");
            break;
        case CENTER:
            imageUrl.append("center.png)");
            break;
        case RIGHT:
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
            downW = 1;
            break;
        case Qt::Key_S:
            downS = 1;
            break;
        case Qt::Key_A:
            downA = 1;
            break;
        case Qt::Key_D:
            downD = 1;
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
            downW = 0;
            break;
        case Qt::Key_S:
            downS = 0;
            break;
        case Qt::Key_A:
            downA = 0;
            break;
        case Qt::Key_D:
            downD = 0;
            break;
        default:
            break;
        }
        stateProcess();
    }
}
void MainWindow::measureVoltage()
{
    serial->putChar(COMMAND_VOLTAGE);
    serial->putChar(0);
}
void MainWindow::measureSpeed()
{
    serial->putChar(COMMAND_SPEED);
    serial->putChar(0);
}
void MainWindow::getSerialMessage()
{
    //voltageTimer->stop();
    //speedTimer->stop();
    //QByteArray msg;
    if (serial->bytesAvailable() == 3) {
        QByteArray msg = serial->readAll();
        switch (msg.at(0)) {
        case 'S':

            break;
        case 'V':
        {
            char byte = msg.at(1);
            float voltage = ((unsigned char)byte)/(182.0/12.0);
            ui->lcdNumber_2->display(voltage);
            break;
        }
        default:
            ui->plainTextEdit->appendHtml("<p><b>Serial</b> : "+msg+"</p>");
        }
    }
    //voltageTimer->start(10000);
    //speedTimer->start(100);
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
