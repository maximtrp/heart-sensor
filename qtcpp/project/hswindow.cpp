#include "hswindow.h"
#include "ui_hswindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QSignalMapper>
#include <QtCharts>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace QtCharts;

void HSWindow::serialOpen() {
    QString portName = ui->serialPorts->currentText();
    serialPort->setPortName(portName);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->open(QSerialPort::ReadWrite);
    QThread::msleep(2000);
}

void HSWindow::serialRead() {

    //QCoreApplication::processEvents();
    QByteArray receivedData = serialPort->readAll();
    readData.append(receivedData);
    QList<QByteArray> dataSplit = receivedData.split('\n');
    dataSplit.removeFirst();
    dataSplit.removeLast();

    this->setXMin(&dataSplit.first());
    this->updateChart(&dataSplit);

    if (this->isEnough(&dataSplit.last())) {
        this->sendStopBit();
        this->saveFile();
        ui->recButton->setText("Start recording");
        receivedData.clear();
        serialPort->close();
        isRecording = false;
    }
}

void HSWindow::updateChart(QList<QByteArray> *dataSplit) {

    for (int i = 0; i < dataSplit->size(); ) {
        QList<QByteArray> values = dataSplit->at(i).trimmed().split(' ');
        QByteArray x = values.first();
        QByteArray y = values.last();
        series->append(qreal(x.toLong() - xMin), qreal(y.toLong()));
        //qDebug() << x.toLong() - xMin;
        i += 10;
    }
    //qDebug() << series->count();
    chartView->repaint();

    QList<QByteArray> values = dataSplit->last().trimmed().split(' ');
    if (series->at(series->count() - 1).x() - series->at(0).x() > 50000) {
        series->clear();
        long x = values.first().toLong();
        auto xAxis = chart->axes(Qt::Horizontal).back();
        auto yAxis = chart->axes(Qt::Vertical).back();
        xAxis->setRange(QVariant::fromValue(x / 50000 * 50000), QVariant::fromValue((x / 50000 + 1) * 50000));
        yAxis->setRange(0, 1000);
    }
}

void HSWindow::startRecording() {
    if (isRecording) {
        isRecording = false;
        ui->recButton->setText("Start recording");

        this->sendStopBit();
        serialPort->close();
    } else {
        isRecording = true;
        ui->recButton->setText("Stop recording");
        series->clear();
        readData.clear();
        auto xAxis = chart->axes(Qt::Horizontal).back();
        xAxis->setRange(0, 50000);
        QCoreApplication::processEvents();

        this->serialOpen();
        serialPort->clear();
        this->sendStartBit();
    }
}

void HSWindow::setXMin(QByteArray *value) {
    QList<QByteArray> xy = value->split(' ');
    long x = xy.first().toLong();
    if (xMin > x || xMin == 0) xMin = x;
}

bool HSWindow::isEnough(QByteArray *value) {
    QList<QByteArray> xy = value->split(' ');
    long x = xy.first().toLong();
    int duration = ui->recordDuration->value() * 1000;
    return (x > xMin + duration) ? true : false;
}

void HSWindow::sendStartBit() {
    const char *startBit = "1";
    serialPort->write(startBit);
    serialPort->waitForBytesWritten(1000);
}

void HSWindow::sendStopBit() {
    const char *stopBit = "0";
    serialPort->write(stopBit);
    serialPort->waitForBytesWritten(1000);
}

void HSWindow::saveFile() {
    auto t = std::time(nullptr);
    auto lt = *std::localtime(&t);
    std::ostringstream dt_stream;
    dt_stream << std::put_time(&lt, "%Y-%m-%d %H-%M-%S ECG.csv");

    QString outputFilename = QString::fromStdString(dt_stream.str());
    QFile outputFile(outputFilename);
    outputFile.open(QIODevice::WriteOnly);

    QTextStream outStream(&outputFile);
    int startLen = readData.indexOf(QString("\r\n").toUtf8()) + 2;
    int endIdx = readData.lastIndexOf(QString("\r\n").toUtf8());
    int endLen = readData.count() - endIdx;
    readData.remove(endIdx, endLen);
    readData.remove(0, startLen);
    //qDebug() << endIdx;
    outStream << readData;
    outputFile.close();
}

void HSWindow::serialRefresh() {
    ui->serialPorts->clear();
    const auto serialPortInfos = QSerialPortInfo::availablePorts();
    QString portName;
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        portName = serialPortInfo.portName();
        ui->serialPorts->addItem(portName);
    }
}

HSWindow::HSWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HSWindow)
{
    ui->setupUi(this);
    this->serialRefresh();
    this->setWindowTitle("Heart Sensor");
    QPushButton *recButton = ui->recButton;
    QPushButton *refreshSerialButton = ui->refreshSerialButton;

    series = new QLineSeries();
    series->setUseOpenGL(true);
    QPen pen = series->pen();
    pen.setWidth(1);
    series->setPen(pen);

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Heart Sensor Recording");
//    chart->createDefaultAxes();
//    auto xAxis = chart->axes(Qt::Horizontal).back();
//    auto yAxis = chart->axes(Qt::Vertical).back();
//    xAxis->setRange(0, 50000);
//    yAxis->setRange(0, 1000);

    QValueAxis *xAxis = new QValueAxis;
    xAxis->setRange(0, 50000);
    xAxis->setTickCount(6);
    xAxis->setLabelFormat("%d");
    xAxis->setTitleText("Milliseconds");

    QValueAxis *yAxis = new QValueAxis;
    yAxis->setRange(0, 1000);
    yAxis->setTickCount(11);
    yAxis->setLabelFormat("%d");
    yAxis->setTitleText("Signal");

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);

    chartView = ui->heartChart;
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing, true);

    serialPort = new QSerialPort;
    QObject::connect(recButton, SIGNAL(clicked()), this, SLOT(startRecording()));
    QObject::connect(refreshSerialButton, SIGNAL(clicked()), this, SLOT(serialRefresh()));
    QObject::connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialRead()));

}

HSWindow::~HSWindow()
{
    delete ui;
}

