
#include <QtWidgets>
#include <QFile>
#include <QDebug>

#include <time.h>

#include "batterywatch.h"
#include "tmp/ui/ui_batterywatch.h"
#include "preferences.h"
#include "wemoclient.h"
#include "MQTTClient.h"
#include "version.h"

#ifdef __APPLE__
/* First make sure you add IOKit.framework */
#import <IOKit/ps/IOPowerSources.h>
#elif defined _WIN32 || defined _WIN64
#include <windows.h>
#else
#endif



/** {{{ BatteryWatch::BatteryWatch(QWidget *parent) : QDialog(parent), ui(new Ui::BatteryWatch)
 *  @brief This is constructor for the dialog for choosing drive letter and browsing for folder to mount
 *
 *  BatteryWatch appears when user click Configure in the taskbar icon menu
 */
BatteryWatch::BatteryWatch(QWidget *parent) :
        QDialog(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint),
        ui(new Ui::BatteryWatch)
{
    ui->setupUi(this);

    srand( time(0L) );

    // this->setStyleSheet(" #BatteryWatch { border-image: url(images/background.png); } ");

    readSettings();

    ui->dtClockInTime->setCalendarPopup(true);
    ui->dtClockOutTime->setCalendarPopup(true);
    ui->dtClockInTime->setDisplayFormat("yyyy-MM-dd hh:mm");
    ui->dtClockOutTime->setDisplayFormat("yyyy-MM-dd hh:mm");


    createActions();
    createTrayIcon();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    setIcon();
    trayIcon->show();

	checkBatteryLevel();	/* sets the tooltip to the current value immediate */

	connect( &timerCheckBattery, SIGNAL(timeout()), this, SLOT(checkBatteryLevel()) );
    timerCheckBattery.start( TIME_CHECKING_FOR_BATTERY_LEVEL );
}
/* }}} */




/** {{{ BatteryWatch::~BatteryWatch()
  * @brief BatteryWatch destructor
  *
  */
BatteryWatch::~BatteryWatch()
{
    delete ui;
}
/* }}} */


/** {{{ void BatteryWatch::changeEvent(QEvent *e)
  * @brief changeEvent(QEvent*)
  *
  */
void BatteryWatch::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}
/* }}} */


/** {{{ void BatteryWatch::quitApp()
  * @brief Unmount drive on quit and decrease file size
  */
void BatteryWatch::quitApp()
{
    qApp->quit();
}
/* }}} */


/** {{{ void BatteryWatch::about()
  * @brief Unmount drive on quit and decrease file size
  */
void BatteryWatch::about()
{
    QMessageBox msgbox;
    msgbox.setWindowTitle( "BatteryWatch" );
    msgbox.setText(
            QString("Version:  ") + glbRevision + "\n" +
            QString("Last Updated on:  ") + glbDateLastModified +
			QString("\n\n") +
			QString("The software included in this product contains copyrighted software that is licensed under the GPLv3.\n\nYou may obtain the complete Corresponding Source code from our distribution site at https://github.com/dabbler/batterywatch.git")
     );
    msgbox.setInformativeText( "\n(C) Copyright 2021 Peregrine LLC\n" );
    msgbox.exec();
}
/* }}} */




/** {{{ void BatteryWatch::setIcon()
 *  @brief Set Icon
 *
 *  Sets icon in the tray
 *
*/
void BatteryWatch::setIcon()
{
    QIcon icon;
    icon.addFile(":/images/battery.png");
    trayIcon->setIcon(icon);
}
/* }}} */


/** {{{ void BatteryWatch::iconActivated(QSystemTrayIcon::ActivationReason reason)
 *  @brief TrayIcon activation reason
 *
*/
void BatteryWatch::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch ( reason ) {
		case QSystemTrayIcon::Trigger:
			// qDebug() << "BatteryWatch:" << "iconActivated(Trigger)";
			checkBatteryLevel();
			break;
		case QSystemTrayIcon::DoubleClick:
			// qDebug() << "BatteryWatch:" << "iconActivated(DoubleClick)";
			{
				static int toggle = 0;
				setCharger( toggle );
				toggle = ! toggle;
			}
			break;
		case QSystemTrayIcon::MiddleClick:
			// qDebug() << "BatteryWatch:" << "iconActivated(MiddleClick)";
			editPreferences();
			readSettings();
			break;
		case QSystemTrayIcon::Context:
			// qDebug() << "BatteryWatch:" << "iconActivated(Context)";
			break;
		default:
			break;
	}
}
/* }}} */


int BatteryWatch::checkBatteryLevel()
{
	static int batteryLevelPreviously = -100;
	int batteryLevelNow = -100;

#ifdef __APPLE__
	CFTimeInterval timeRemaining = IOPSGetTimeRemainingEstimate();

	if (timeRemaining == kIOPSTimeRemainingUnlimited) {
		// connected to outlet
		batteryLevelNow = 100;
	} else if (timeRemaining == kIOPSTimeRemainingUnknown){
		// time remaining unknown (recently unplugged)
		batteryLevelNow = 100;
	} else if ((timeRemaining / 60) < 30){
		// less than 30 minutes remaining
		batteryLevelNow = timeRemaining / 60;
	}
qDebug() << "BatteryWatch:" << "Mac minutes remaining: " << batteryLevelNow;

#elif defined _WIN32 || defined _WIN64

	SYSTEM_POWER_STATUS systemPowerStatus;
	GetSystemPowerStatus( &systemPowerStatus );
	qDebug() << "BatteryWatch:" << "battery: " << systemPowerStatus.BatteryLifePercent << "%" << " at " << QDateTime::currentDateTime().toString();

	batteryLevelNow = systemPowerStatus.BatteryLifePercent;
#else
	QString fnameBatteryLevel(linuxFileBatteryLevel);
	QFile file(fnameBatteryLevel);
	file.open(QFile::ReadOnly | QFile::Text);
	batteryLevelNow = QString( file.readAll() ).toInt();
	qDebug() << "BatteryWatch:" << "battery: " << batteryLevelNow << "%" << " at " << QDateTime::currentDateTime().toString();
#endif

	if ( batteryLevelPreviously != -100 ) {

		/* if battery is growing and is above the threshold */
		if ( (batteryLevelNow > batteryLevelPreviously) && (batteryLevelNow > batLvlMax) ) {
			setCharger( OFF );
			if ( isDisplaySystemNotifications ) {
				showMessage( "Adjust Charging", "Turning Charging Off", 0 );
			}
		}

		/* if battery is draining and is below the threshold */
		if ( (batteryLevelNow < batteryLevelPreviously) && (batteryLevelNow < batLvlMin) ) {
			setCharger( ON );
			if ( isDisplaySystemNotifications ) {
				showMessage( "Adjust Charging", "Turning Charging On", 0 );
			}
		}
	}

    trayIcon->setToolTip( QString("Battery level: %1%").arg( batteryLevelNow ) );

	batteryLevelPreviously = batteryLevelNow;

	return batteryLevelNow;
}


void BatteryWatch::setCharger( int turnOnCharger )
{
	if ( isMqttEnabled ) {
		sendMQTT( mqttTopic.toLatin1().data(), (turnOnCharger ? mqttTurnOn : mqttTurnOff).toLatin1().data() );
	}
	if ( isWemoEnabled ) {
		WemoClient wm( wemoHost.toLatin1().data() );
		wm.turn( turnOnCharger );
	}
	if ( isExecutablesEnabled ) {
		if ( turnOnCharger ) {
			if ( ! executableWhenBelowMin.isEmpty() ) {
				qDebug() << "BatteryWatch:" << "setCharger(" << turnOnCharger << ") " << "system(" + executableWhenBelowMin + ")";
				QProcess process;
				process.execute( QFileInfo(executableWhenBelowMin).absoluteFilePath(), QStringList() << executableArgsWhenBelowMin.split(" "));
			}
		} else {
			if ( ! executableWhenAboveMax.isEmpty() ) {
				qDebug() << "BatteryWatch:" << "setCharger(" << turnOnCharger << ") " << "system(" + executableWhenAboveMax + ")";
				QProcess process;
				process.execute(QFileInfo(executableWhenAboveMax).absoluteFilePath(), QStringList() << executableArgsWhenAboveMax.split(" "));
			}
		}
	}
}


void BatteryWatch::sendMQTT( char * topic, char * payload )
{
#if defined _WIN32 || defined _WIN64
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc;

	MQTTClient_create(&client, mqttHost.toLatin1().data(), mqttClient.toLatin1().data(), MQTTCLIENT_PERSISTENCE_NONE, NULL);

	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.username = mqttUsername.toLatin1().data();
	conn_opts.password = mqttPassword.toLatin1().data();

	if ( (rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS ) {
		qDebug() << "BatteryWatch:" << QString("Failed to connect, return code %1").arg(rc);
		return;
	}

	pubmsg.payload = payload;
	pubmsg.payloadlen = (int)strlen(payload);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;

	rc = MQTTClient_publishMessage(client, topic, &pubmsg, &token);

	qDebug() << "BatteryWatch:" << QString("publish( %1, %2, %3 ) => %4").arg(mqttHost).arg(mqttTopic).arg(payload).arg(rc);

	rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

	// qDebug() << "BatteryWatch:" << QString("Message with token of %1 is delivered.  MQTTClient_waitForCompletion() => %2").arg(token).arg(rc);

	MQTTClient_disconnect( client, TIMEOUT );
	MQTTClient_destroy(&client);
#else
/*
	mosquitto_pub -h home.beccue.com -u dab -P 13131313 -t Beccue/Home/sonoff_060/switch -m off
*/
	// qDebug() << QString("mosquitto_pub -h %1 -u %2 -P %3 -t %4 -m %5 ").arg(mqttHost).arg(mqttUsername).arg(mqttPassword).arg(topic).arg(payload).toLatin1().constData();
	system( QString("mosquitto_pub -h %1 -u %2 -P %3 -t %4 -m %5 ").arg(mqttHost).arg(mqttUsername).arg(mqttPassword).arg(topic).arg(payload).toLatin1().constData() );
#endif
}


/** {{{ void BatteryWatch::createActions()
 *  @brief Create Actions
 *
*/
void BatteryWatch::createActions()
{
    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quitApp()));

    aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    configAction = new QAction (tr("&Configure"), this);
    connect(configAction, SIGNAL(triggered()), this, SLOT(editPreferences()));

}
/* }}} */


/** {{{ void BatteryWatch::createTrayIcon()
 *  @brief Create Tray Icon Menu
 *
*/
void BatteryWatch::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(configAction);
    trayIconMenu->addAction(aboutAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
/* }}} */



/** {{{ void BatteryWatch::editPreferences()
 *  @brief Shows the configuration dialog when Configure is selected
 *
*/
void BatteryWatch::editPreferences()
{
    Preferences prefs(this);

    prefs.exec();

    readSettings();
}
/* }}} */


/** {{{ void BatteryWatch::readSettings()
 */
void BatteryWatch::readSettings()
{
	QSettings settings("BatteryWatch", "Config");

	batLvlMax = settings.value("BatLvlMax").toInt();
	batLvlMin = settings.value("BatLvlMin").toInt();
	wemoHost = settings.value("WemoHost").toString();
	mqttHost = settings.value("MqttHost").toString();
	mqttUsername = settings.value("MqttUsername").toString();
	mqttPassword = settings.value("MqttPassword").toString();
	mqttTopic = settings.value("MqttTopic").toString();
	mqttTurnOff = settings.value("MqttTurnOff").toString();
	mqttTurnOn = settings.value("MqttTurnOn").toString();
	isDisplaySystemNotifications = settings.value("IsDisplaySystemNotifications").toInt();
	isWemoEnabled = settings.value("IsWemoEnabled").toInt();
	isMqttEnabled = settings.value("IsMqttEnabled").toInt();
	isExecutablesEnabled = settings.value("IsExecutablesEnabled").toInt();
	executableWhenAboveMax = settings.value("ExecutableWhenAboveMax").toString();
	executableWhenBelowMin = settings.value("ExecutableWhenBelowMin").toString();
	executableArgsWhenAboveMax = settings.value("ExecutableArgsWhenAboveMax").toString();
	executableArgsWhenBelowMin = settings.value("ExecutableArgsWhenBelowMin").toString();
    linuxFileBatteryLevel = settings.value("LinuxFileBatteryLevel", "/sys/class/power_supply/BAT0/capacity").toString();

}
/* }}} */



/** {{{ void BatteryWatch::showMessage(QString msgTitle, QString msgBody, int iconType)
  * @brief Show Status Message
  */
void BatteryWatch::showMessage(QString msgTitle, QString msgBody, int iconType)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "BatteryWatch:" << "showMessage(" << msgTitle << "," << msgBody << ")";
#endif
    QSystemTrayIcon::MessageIcon mIcon = QSystemTrayIcon::MessageIcon(iconType);
    trayIcon->showMessage(msgTitle, msgBody, mIcon, 2000);
}
/* }}} */



