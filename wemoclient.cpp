
#include "wemoclient.h"

#define TIMEOUT 200 /* msecs */

WemoClient::WemoClient( QString wemoIpAddr ) : QObject()
{
	m_host = wemoIpAddr;

    socket = new QTcpSocket(this);

	return;
}




WemoClient::~WemoClient()
{
	// qDebug() << "BatteryWatch:" << "~WemoClient()";
	if ( socket ) {
		socket->disconnectFromHost();
		// socket->waitForDisconnected(5 * 1000);
		socket->close();
		delete socket;
		socket = NULL;
	}
}


void WemoClient::turn( int on )
{
	// qDebug() << QString("BatteryWatch: WemoClient::turn(%1)").arg(on ? "On" : "Off").toLatin1().data();

	if ( socket ) {
		QFile file;
		if ( on ) {
			file.setFileName(":wemo-on-data.txt");
		} else {
			file.setFileName(":wemo-off-data.txt");
		}
		file.open(QFile::ReadOnly | QFile::Text);

		for ( int attempts = 0 ; attempts < 10 ; attempts++ ) {
			int portAttempt = WEMO_PORT + attempts;

			if ( socket->state() == QAbstractSocket::ConnectedState ) {
				socket->disconnectFromHost();
			}
			socket->connectToHost( m_host, portAttempt );

			if ( socket->waitForConnected(TIMEOUT) ) {

				file.seek(0);
				QByteArray baPostData = file.readAll();
				baPostData.replace("49153", QString::number(portAttempt).toLatin1().data() );
				baPostData.replace("192.168.1.69", m_host.toLatin1().data() );

				socket->write( baPostData );
				socket->waitForBytesWritten(TIMEOUT);

				if ( socket->waitForReadyRead(TIMEOUT) ) {
					QString dataRead = socket->readAll();
					if ( dataRead.contains("200 OK") ) {
						qDebug() << QString("BatteryWatch: Switched %1:%2 %3").arg(m_host).arg(portAttempt).arg(on ? "ON" : "off").toLatin1().data();
						break;
					}
				}
			}
		}
	}
}

