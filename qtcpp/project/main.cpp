#include "hswindow.h"
#include <QApplication>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    HSWindow w;
    w.show();

    return app.exec();
}
