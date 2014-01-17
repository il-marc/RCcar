#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define FORWARD  'w'
#define BACKWARD 's'
#define STOP     ' '
#define LEFT     'a'
#define RIGHT    'd'
#define CENTER   'q'
#define VOLTAGE  'v'

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
    void measureVoltage();
    void getSerialMessage();

private:
    void stateProcess();
    QTimer *timer;
    Ui::MainWindow *ui;
    bool DownW, DownS, DownA, DownD;
    char move, turn;
    QSerialPort *serial;
    bool connected;

};

#endif // MAINWINDOW_H
