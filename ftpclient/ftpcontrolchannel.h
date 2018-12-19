#ifndef FTPCONTROLCHANNEL_H
#define FTPCONTROLCHANNEL_H

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <iostream>

class FtpControlChannel : public QObject
{
    Q_OBJECT
public:
    explicit FtpControlChannel(QObject *parent = nullptr);

    // Connect to an FTP server
    void connectToServer(const QString &server);

    // Send a command to the server
    void command(const QByteArray &command, const QByteArray &params);

    void rcv(char *msg);

signals:

    // Connection established. Local address and port are known.
    void opened(const QHostAddress &localAddress, int localPort);

    // Connection closed
    void closed();

    // Informational message
    void info(const QByteArray &info);

    // Reply to a previously sent command
    void reply(int code, const QByteArray &parameters);

    // Something is wrong
    void invalidReply(const QByteArray &reply);

private:
    void onReadyRead();

    QTcpSocket m_socket;
    QByteArray m_buffer;
};

#endif // FTPCONTROLCHANNEL_H
