
#include <QtCore>
#include <QtNetwork>

#define WEMO_PORT	(49153)


class WemoClient : public QObject
{
    Q_OBJECT
public:
    explicit WemoClient( QString wemoIpAddr );
    ~WemoClient();

	void connectToWemoPort();

public slots:
    void turn( int on );
    void turnOn() { turn(1); }
    void turnOff() { turn(0); }

private:
	QTcpSocket *socket;

	QString m_host;
};

