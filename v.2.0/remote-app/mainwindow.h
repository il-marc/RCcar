#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define COMMAND_FORWARD  'w'
#define COMMAND_BACKWARD 's'
#define COMMAND_STOP     ' '
#define COMMAND_TURN     'q'
#define COMMAND_VOLTAGE  'V'
#define COMMAND_SPEED    'S'
#define motorSpeed 60
#define leftPos 60
#define centerPos 90
#define rightPos 120


enum moving { BACKWARD, STOP, FORWARD};
enum turning { LEFT, CENTER, RIGHT};

#include <QMainWindow>
#include <QTimer>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *event);

private slots:
    void serialConnect();
    void serialDisconnect();
    void measureVoltage();
    void measureSpeed();
    void getSerialMessage();

private:
    void stateProcess();
    QTimer *voltageTimer;
    QTimer *speedTimer;
    //bool waitingVoltage;
    //bool waitingSpeed;
    Ui::MainWindow *ui;
    bool downW, downS, downA, downD;
    moving move;
    turning turn;
    QSerialPort *serial;
    bool connected;

};

#endif // MAINWINDOW_H
