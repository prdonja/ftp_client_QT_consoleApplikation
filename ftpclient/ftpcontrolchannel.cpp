#include "ftpcontrolchannel.h"

FtpControlChannel::FtpControlChannel(QObject *parent) : QObject(parent)
{
    connect(&m_socket, &QIODevice::readyRead,
            this, &FtpControlChannel::onReadyRead);
    connect(&m_socket, &QAbstractSocket::disconnected,
            this, &FtpControlChannel::closed);
    connect(&m_socket, &QAbstractSocket::connected, this, [this]() {
        emit opened(m_socket.localAddress(), m_socket.localPort());
    });
}

void FtpControlChannel::connectToServer(const QString &server)
{
    m_socket.connectToHost(server, 111);
}

void FtpControlChannel::command(const QByteArray &command,
                                const QByteArray &params)
{
    QByteArray sendData = command;
    if (!params.isEmpty())
        sendData += " " + params;
    m_socket.write(sendData + "\r\n");
}

void FtpControlChannel::rcv(char *msg)
{
    qDebug() << m_socket.readAll();
    qDebug() << m_buffer << "<-m_buffer";

}

void FtpControlChannel::onReadyRead()
{
    m_buffer.append(m_socket.readAll());
    int rn = -1;
    while ((rn = m_buffer.indexOf("\r\n")) != -1) {
        QByteArray received = m_buffer.mid(0, rn);
        m_buffer = m_buffer.mid(rn + 2);
        int space = received.indexOf(' ');
        if (space != -1) {
            int code = received.mid(0, space).toInt();
            if (code == 0)
                emit info(received.mid(space + 1));
            else
                emit reply(code, received.mid(space + 1));
        } else {
            emit invalidReply(received);
        }
    }
}
