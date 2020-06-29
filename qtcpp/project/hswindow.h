#ifndef HSWINDOW_H
#define HSWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSerialPort>
#include <QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui { class HSWindow; }
QT_END_NAMESPACE

class HSWindow : public QMainWindow
{
    Q_OBJECT

public:
    QSerialPort *serialPort;
    QChart *chart;
    QLineSeries *series;
    QChartView *chartView;
    QByteArray readData;

    bool isRecording = false;
    long xMin = 0;

    bool isEnough(QByteArray*);
    void updateChart(QList<QByteArray>*);
    void setXMin(QByteArray*);
    void saveFile();
    void sendStopBit();
    void sendStartBit();
    void serialOpen();

    HSWindow(QWidget *parent = nullptr);
    ~HSWindow();

public slots:
    void startRecording();
    void serialRead();
    void serialRefresh();

private:
    Ui::HSWindow *ui;
};
#endif // HSWINDOW_H
