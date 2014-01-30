#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "portdialog.h"
#include "QKeyEvent"
#include "QCloseEvent"
#include "QMessageBox"
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
    motorSpeed = 60;
    portDialog p;
    p.exec();
    ui->setupUi(this);
    if (!p.isOK)
    {
        exit(EXIT_FAILURE);
    }
    voltageTimer = new QTimer(this);
    connect(ui->motorSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(setMotorSpeed()));
    connect(voltageTimer, SIGNAL(timeout()), this, SLOT(measureVoltage()));
    serialConnect(p.portName);
    stateProcess();
}
void MainWindow::serialConnect(QString portName)
{
    serial = new QSerialPort (this);
    serial->setPortName(portName);
    if (serial->open(QIODevice::ReadWrite)) {
        if (serial->setBaudRate(QSerialPort::Baud9600)
                && serial->setDataBits(QSerialPort::Data8)
                && serial->setParity(QSerialPort::NoParity)
                && serial->setStopBits(QSerialPort::OneStop)
                && serial->setFlowControl(QSerialPort::SoftwareControl)) {
            connected = true;
            connect(serial, SIGNAL(readyRead()), this, SLOT(getSerialMessage()));
            connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(serialError(QSerialPort::SerialPortError)));
            voltageTimer->start(100);
            stateProcess();
        } else {
            serial->close();
            QMessageBox msgBox;
            msgBox.setText(serial->errorString());\
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
    } else {
        QMessageBox msgBox;
        msgBox.setText(serial->errorString());\
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}
void MainWindow::serialError(QSerialPort::SerialPortError)
{
    QMessageBox msgBox;
    msgBox.setText(serial->errorString());\
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
    exit(EXIT_FAILURE);
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
                }
            } else {
                if (move != BACKWARD) {
                    move = BACKWARD;
                    serial->putChar(COMMAND_BACKWARD);
                    serial->putChar(motorSpeed);
                }
            }
        } else {
            if (move != STOP) {
                move = STOP;
                serial->putChar(COMMAND_STOP);
                serial->putChar(0);
            }
        }
        if (downA ^ downD) {
            if(downA) {
                if (turn != LEFT ) {
                    turn = LEFT;
                    serial->putChar(COMMAND_TURN);
                    serial->putChar(leftPos);
                }
            } else {
                if (turn != RIGHT ) {
                    turn = RIGHT;
                    serial->putChar(COMMAND_TURN);
                    serial->putChar(rightPos);
                }
            }
        } else {
            if (turn != CENTER ) {
                turn = CENTER;
                serial->putChar(COMMAND_TURN);
                serial->putChar(centerPos);
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
void MainWindow::setMotorSpeed()
{
    motorSpeed = ui->motorSpeedSlider->value();
    if (move == FORWARD)
    {
        serial->putChar(COMMAND_FORWARD);
        serial->putChar(motorSpeed);
    }
    if (move == BACKWARD)
    {
        serial->putChar(COMMAND_BACKWARD);
        serial->putChar(motorSpeed);
    }
}
void MainWindow::getSerialMessage()
{
    if (serial->bytesAvailable() == 3) {
        QByteArray msg = serial->readAll();
        switch (msg.at(0)) {
        case 'V':
            char byte = msg.at(1);
            float voltage = ((unsigned char)byte)/(182.0/12.0);
            ui->lcdNumber_2->display(voltage);
            break;
        }
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
