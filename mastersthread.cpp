#include "mastersthread.h"
#include <QThread>

MastersThread::MastersThread(QSerialPort* serialPort, QMutex* mutex, int* deviceAddress)
    : QThread()

{
    quit_ = false;
    serialPort_ = serialPort;
    deviceAddress_ = deviceAddress;
    mutex_ = mutex;
}

MastersThread::~MastersThread()
{
    serialPort_ = nullptr;
    mutex_ = nullptr;
    deviceAddress_ = nullptr;
}
void MastersThread::stop(){
    quit_ = true;
}
void MastersThread::run(){
    qDebug() << "Master thread has started";
    QByteArray request;

    mutex_->lock();
    request.append(*deviceAddress_ & 0xff);
    mutex_->unlock();
    request.append(messageFunctionCode_ & 0xff);
    request.append(QByteArray::fromRawData("\x9c\x41\x00\x25", 4));
    unsigned crc = getCrc(request);
    request.append(crc & 0xff);
    request.append((crc>>8) & 0xff);
    //qDebug() << getCrc(QByteArray::fromRawData("\x01\x03\x4A\x01\x00\x02\x00\x03\x00\x04\x00\x05\x00\x06\x00\x07\x00\x08\x00\x09\x00\x10\x00\x11\x00\x12\x00\x13\x00\x14\x00\x15\x00\x16\x00\x17\x00\x18\x00\x19\x00\x20\x00\x21\x00\x22\x00\x23\x00\x24\x00\x25\x00\x26\x00\x27\x00\x28\x00\x29\x00\x30\x00\x31\x00\x32\x00\x33\x00\x34\x00\x35\x00\x36\x00\x37\x00", 77));

    while(!quit_){
        request[0] = ((deviceAddress_ != nullptr)?*deviceAddress_:0x01) & 0xff;
        mutex_->lock();
        qint64 bytesWritten = serialPort_->write(request);
        if (bytesWritten >= 0){
            //qDebug () << bytesWritten;
            if (serialPort_->waitForBytesWritten(writeWaitTimeout_)) {
                // read response
                if (serialPort_->waitForReadyRead(readWaitTimeout_)) {
                    QByteArray responseData = serialPort_->readAll();
                    while (serialPort_->waitForReadyRead(10))
                        responseData += serialPort_->readAll();
                    mutex_->unlock();
                    if (!checkCrc(responseData))
                        emit error(QString("Неверная контрольная сумма в ответе"));
                    else if (!checkDeviceAddress(responseData))
                        emit error(QString("Неверный адрес устройства в ответе"));
                    else if (!checkFunctionCode(responseData))
                        emit error(QString("Неверный код функции в ответе"));
                    else if (!checkResponseLenght(responseData))
                        emit error(QString("Неверный формат ответа"));
                    else
                        emit response(parseResponse(responseData));
                } else{
                    emit timeout(QString("Таймаут чтения из порта"));
                    mutex_->unlock();
                }
            } else{
                emit timeout(QString("Таймаут записи в порт"));
                mutex_->unlock();
            }
        } else{
            emit error(QString("Критическая ошибка записи в порт"));
            mutex_->unlock();
        }
        msleep(sleepTime_);
    }
    serialPort_->close();
    qDebug() << "Master thread has stopped";
}
unsigned MastersThread::getCrc(const QByteArray& dataArray){
    unsigned sum = 0xffff;
    for (int i=0; i < dataArray.length(); ++i){
        sum ^= dataArray[i] & 0xff;
        for(int j = 0; j < 8; j++){
            if(sum & 1 )
                sum=(sum>>1) ^ 0xA001;
            else
                sum >>= 1;
        }
    }
    //qDebug() << sum;
    return sum;
} //79c8  c34c
bool MastersThread::checkCrc(QByteArray& message){
    if (message.length() < 2)
        return false;
    QByteArray tmpArray = message.right(2);
    message.remove(message.length() - 2, 2);
    unsigned messageCrc = getBigEndian(tmpArray);
    //qDebug() << "msg_crc=" << messageCrc << " getCrc=" << getCrc(message);
    return (messageCrc == getCrc(message));
}
unsigned MastersThread::getBigEndian(const QByteArray& byteArray){
    unsigned outUnsigned = 0;
    for (size_t i = 0; i < (size_t)byteArray.length(); ++i)
        outUnsigned += ((byteArray[i] & 0xff) << 8*i);
    return outUnsigned;
}
unsigned MastersThread::getLittleEndian(const QByteArray& byteArray){
    unsigned outUnsigned = 0;
    int byteArrayLen = byteArray.length();
    for (size_t i = 0; i < (size_t)byteArrayLen; ++i)
        outUnsigned += ((byteArray[byteArrayLen - i - 1] & 0xff) << 8*i);
    return outUnsigned;
}
bool MastersThread::checkDeviceAddress(QByteArray& message){
    if (message.length() < 1)
        return false;
    int messageDeviceAddress = message[0] & 0xff;
    message.remove(0,1);
    return (messageDeviceAddress == *deviceAddress_);
}
bool MastersThread::checkFunctionCode(QByteArray& message){
    if (message.length() < 1)
        return false;
    unsigned messageFunctionCode = message[0] & 0xff;
    message.remove(0,1);
    return (messageFunctionCode == messageFunctionCode_);
}
bool MastersThread::checkResponseLenght(QByteArray& message){
    if (message.length() < 1)
        return false;
    unsigned messageLenght = message[0] & 0xff;
    message.remove(0,1);
    return (messageLenght == (unsigned)message.length());
}
QVector<int> MastersThread::parseResponse(const QByteArray& message){
    QVector<int> responseVector;
    for (size_t i = 0; i < (size_t)message.length(); i=i+2)
        responseVector.push_back((int)getLittleEndian(message.mid(i, 2)));
    return responseVector;
}
