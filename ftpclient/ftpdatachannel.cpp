#include "ftpdatachannel.h"

FtpDataChannel::FtpDataChannel(QObject *parent) : QObject(parent)
{
    connect(&m_server, &QTcpServer::newConnection, this, [this]() {
        m_socket.reset(m_server.nextPendingConnection());
        connect(m_socket.data(), &QTcpSocket::readyRead, [this]() {
            emit dataReceived(m_socket->readAll());
        });
    });
}

void FtpDataChannel::listen(const QHostAddress &address)
{
    m_server.listen(address, 10241);
    qDebug() << address << "<-address";
}

void FtpDataChannel::sendData(const QByteArray &data)
{
    if (m_socket)
        m_socket->write(QByteArray(data).replace("\n", "\r\n"));
}

void FtpDataChannel::close()
{
    if (m_socket)
        m_socket->disconnectFromHost();
}

QString FtpDataChannel::portspec() const
{
    // Yes, this is a weird format, but say hello to FTP.
    QString portSpec;
    quint32 ipv4 = m_server.serverAddress().toIPv4Address();
    quint16 port = 0;
    port = m_server.serverPort();
    qDebug() << port << " <- port";
    portSpec += QString::number((ipv4 & 0xff000000) >> 24);
    portSpec += ',' + QString::number((ipv4 & 0x00ff0000) >> 16);
    portSpec += ',' + QString::number((ipv4 & 0x0000ff00) >> 8);
    portSpec += ',' + QString::number(ipv4 & 0x000000ff);
    portSpec += ',' + QString::number((port & 0xff00) >> 8);
    portSpec += ',' + QString::number(port &0x00ff);

    //portSpec += ",4,0";
    qDebug() << portSpec << " <- portSpec";
    return portSpec;
}
