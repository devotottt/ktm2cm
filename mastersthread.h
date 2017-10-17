#ifndef MASTERSTHREAD_H
#define MASTERSTHREAD_H


#include <QSerialPort>
#include <QDebug>
#include <QMutex>
#include <QThread>

class MastersThread :  public QThread
{
    Q_OBJECT

public:
    explicit MastersThread(QSerialPort* serialPort,  QMutex* mutex, int* deviceAddress);
    ~MastersThread();

    unsigned getCrc(const QByteArray& dataArray);
    bool checkCrc(QByteArray& message);
    unsigned getBigEndian(const QByteArray& byteArray);
    unsigned getLittleEndian(const QByteArray& byteArray);
    bool checkDeviceAddress(QByteArray& message);
    bool checkFunctionCode(QByteArray& message);
    bool checkResponseLenght(QByteArray& message);
    QVector<int> parseResponse(const QByteArray& message);

public slots:
    void run();
    void stop();

signals:
    void response(const QVector<int>);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    QSerialPort* serialPort_;
    bool quit_;
    QMutex* mutex_;
    int* deviceAddress_;
    const int writeWaitTimeout_ = 100; //ms
    const int readWaitTimeout_ = 300; //ms
    const int sleepTime_ = 250; //ms
    const unsigned messageFunctionCode_ = 0x03;
};

#endif // MASTERSTHREAD_H
