#include "ftpcontrolchannel.h"
#include "ftpdatachannel.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDataStream>



#include <iostream>
using namespace std;


QByteArray cmdQBA;
char cmdc[100] = "";

FtpDataChannel dataChannel;
FtpControlChannel controlChannel;
int state;
char msg[100] = "pera";

int main(int argc, char *argv[])
{


    QCoreApplication app(argc, argv);

    QString server = "192.168.0.11";
    QString file = "ftp://stevinserver/ftpserver.txt";


    // Print all data retrieved from the server on the console.
    QObject::connect(&dataChannel, &FtpDataChannel::dataReceived, [](const QByteArray &data) {
        //print list of files on server
        if(state == -1)
        {
            cout << endl<< data.constData() << endl;
            state = 0;
        }
        //transfer file
        if(state != 0)
        {
            for(int i = 0; i < 1000000; i++){} //wait

            QString filename="d:\\";
            filename.append(cmdc);
            QFile file1( filename );
            if ( file1.open(QIODevice::WriteOnly | QIODevice::Append) )
            {
                QTextStream out( &file1 );
                out << data.constData();
                file1.close();
            }

            if(data.length() < 65536 && state != 1)
            {
                state = -1;
            }
            state++;
        }
        //if you want to transfer file type in new file or if you want to disconnect type "END"
        if(state == 0)
        {
            state = 1;
            //Send information to server on what port to connect data channel
            controlChannel.command("PORT", "192,168,0,11,4,1"); //4*256 + 1 = 1025

            cout << endl << "Type in name of file you want to download or if you want to disconnect type 'END':" << endl;
            cin >> cmdc;
            if(cmdc[0] == 'E' && cmdc[1] == 'N' && cmdc[2] == 'D' && cmdc[3] == '\0')
            {
               QCoreApplication::quit();
            }
            else
            {
                cmdQBA.clear();
                cmdQBA.append('/');
                cmdQBA.append(cmdc);
                controlChannel.command("RETR", cmdQBA);
                cout << "Transferring data..." << endl;
            }
        }
    });

    // Connect to our own local FTP server
    controlChannel.connectToServer(server);
    // Type in Username and send to server
    cout << "Type in username:" << endl;
    cin >> cmdc;
    cmdQBA.clear();
    cmdQBA.append(cmdc);
    controlChannel.command("USER", cmdQBA);

    // Type in Password and send to server
    cout << "Type in password:" << endl;
    cin >> cmdc;
    cmdQBA.clear();
    cmdQBA.append(cmdc);
    controlChannel.command("PASS", cmdQBA);

    //Send information to server on what port to connect data channel
    controlChannel.command("PORT", "192,168,0,11,4,1"); //4*256 + 1 = 1025

    QObject::connect(&controlChannel, &FtpControlChannel::opened,[&](const QHostAddress &address, int)
    {
         dataChannel.listen(address);
    });

    controlChannel.command("LIST", "");
    state = -1;

    return app.exec();


}
