#ifndef BATTERYWATCH_H
#define BATTERYWATCH_H

#include <QSystemTrayIcon>
#include <QDialog>
#include <QTimer>

#define OFF			(0)
#define ON			(1)

#define QOS         1
#define TIMEOUT     (10 * 1000L)

#define TIME_CHECKING_FOR_BATTERY_LEVEL	(1 * 60 * 1000)


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
QT_END_NAMESPACE


namespace Ui {
  class BatteryWatch;
}

class BatteryWatch : public QDialog
{
    Q_OBJECT

public:
    BatteryWatch(QWidget *parent = 0);
    ~BatteryWatch();

    Ui::BatteryWatch *ui;

private:
	QTimer timerCheckBattery;


	void setCharger( int turnOnCharger );

	int batLvlMax;
	int batLvlMin;
	bool isMqttEnabled;
	bool isWemoEnabled;
	bool isExecutablesEnabled;
	bool isDisplaySystemNotifications;
	QString wemoHost;
	QString mqttHost;
	QString mqttUsername;
	QString mqttPassword;
	QString mqttTopic;
	QString mqttClient;
	QString mqttTurnOff;
	QString mqttTurnOn;
	QString executableWhenAboveMax;
	QString executableWhenBelowMin;
	QString executableArgsWhenAboveMax;
	QString executableArgsWhenBelowMin;
	QString linuxFileBatteryLevel;

private slots:
    void setIcon();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void showMessage(QString msgTitle, QString msgBody, int iconType);
    void editPreferences();

	void sendMQTT( char * topic, char * payload );

public slots:

	int checkBatteryLevel();

    void about();
    void quitApp();

protected:
    void changeEvent(QEvent *e);
    void readSettings();

private:

    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();

    QAction *quitAction;
    QAction *aboutAction;
    QAction *configAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // BATTERYWATCH_H
