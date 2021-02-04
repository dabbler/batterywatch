
#include <QtWidgets>

#include "preferences.h"
#include "tmp/ui/ui_preferences.h"
#include "batterywatch.h"

#if defined(_WIN32)
#include <windows.h>
#endif



Preferences::Preferences(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Preferences)
{
	ui->setupUi(this);

	connect( this, SIGNAL(accepted()), this, SLOT(accept()) );

    QSettings settings("BatteryWatch", "Config");

	ui->efBatLvlMax->setValue( settings.value("BatLvlMax").toInt() );
	ui->efBatLvlMin->setValue( settings.value("BatLvlMin").toInt() );
    ui->efWemoHost->setText( settings.value("WemoHost").toString() );
    ui->efMqttHost->setText( settings.value("MqttHost").toString() );
    ui->efMqttUsername->setText( settings.value("MqttUsername").toString() );
    ui->efMqttPassword->setText( settings.value("MqttPassword").toString() );
    ui->efMqttClient->setText( settings.value("MqttClient").toString() );
    ui->efMqttTopic->setText( settings.value("MqttTopic").toString() );
    ui->efMqttTurnOff->setText( settings.value("MqttTurnOff").toString() );
    ui->efMqttTurnOn->setText( settings.value("MqttTurnOn").toString() );

    ui->cbIsDisplaySystemNotifications->setCheckState( settings.value("IsDisplaySystemNotifications").toInt() ? Qt::Checked : Qt::Unchecked );
    ui->cbIsMqttEnabled->setCheckState( settings.value("IsMqttEnabled").toInt() ? Qt::Checked : Qt::Unchecked );
    ui->cbIsWemoEnabled->setCheckState( settings.value("IsWemoEnabled").toInt() ? Qt::Checked : Qt::Unchecked );
    ui->cbIsExecutablesEnabled->setCheckState( settings.value("IsExecutablesEnabled").toInt() ? Qt::Checked : Qt::Unchecked );

    ui->efExecutableWhenAboveMax->setText( settings.value("ExecutableWhenAboveMax").toString() );
    ui->efExecutableWhenBelowMin->setText( settings.value("ExecutableWhenBelowMin").toString() );
    ui->efExecutableArgsWhenAboveMax->setText( settings.value("ExecutableArgsWhenAboveMax").toString() );
    ui->efExecutableArgsWhenBelowMin->setText( settings.value("ExecutableArgsWhenBelowMin").toString() );

    ui->efLinuxFileBatteryLevel->setText( settings.value("LinuxFileBatteryLevel", "/sys/class/power_supply/BAT0/capacity").toString() );

	ui->frameLinuxCmdBatteryLevel->hide();	/* usually hide this, except for Linux where it's shown below */

#ifdef __APPLE__
	ui->lblBatLvlMin->setText("Turn On when Minutes Remaining go Below");
	ui->lblBatLvlMax->setText("Turn Off when Minutes Remaining go Above");
	ui->efBatLvlMax->setMaximum( 24 * 60 );
#elif defined _WIN32 || defined _WIN64
#else
	ui->frameLinuxCmdBatteryLevel->show();	/* show this only for linux */
#endif

}



Preferences::~Preferences()
{
	delete ui;
}



void Preferences::changeEvent(QEvent *e)
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



QString Preferences::getUserName()
{
    QSettings settings("BatteryWatch", "Config");

	QString userName = settings.value("name").toString();

	if ( userName.isEmpty() ) {
#if defined(_WIN32)
		char winUserName[256];
		DWORD winUserNameSize = sizeof(winUserName);
		GetUserNameA( winUserName, &winUserNameSize );
		userName = QString::fromLocal8Bit( winUserName ).toLower();
#else
		userName = QString(getenv("USER")).toLower();
#endif
		settings.setValue("name", userName );
	}

	// qDebug() << "getenv(USER) =" << userName;
	return userName;
}


void Preferences::on_btnChooseExeWhenBelowMin_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select Executable File"), "C:/Program Files" );
	ui->efExecutableWhenBelowMin->setText( fileName );
}

void Preferences::on_btnChooseExeWhenAboveMax_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select Executable File"), "C:/Program Files" );
	ui->efExecutableWhenAboveMax->setText( fileName );
}



void Preferences::accept()
{
    QSettings settings("BatteryWatch", "Config");

	settings.setValue("BatLvlMax", ui->efBatLvlMax->value() );
	settings.setValue("BatLvlMin", ui->efBatLvlMin->value() );
    settings.setValue("WemoHost", ui->efWemoHost->text() );
    settings.setValue("MqttHost", ui->efMqttHost->text() );
    settings.setValue("MqttUsername", ui->efMqttUsername->text() );
    settings.setValue("MqttPassword", ui->efMqttPassword->text() );
    settings.setValue("MqttClient", ui->efMqttClient->text() );
    settings.setValue("MqttTopic", ui->efMqttTopic->text() );
    settings.setValue("MqttTurnOff", ui->efMqttTurnOff->text() );
    settings.setValue("MqttTurnOn", ui->efMqttTurnOn->text() );

    settings.setValue("IsDisplaySystemNotifications", (ui->cbIsDisplaySystemNotifications->checkState() == Qt::Checked) ? 1 : 0 );

    settings.setValue("IsMqttEnabled", (ui->cbIsMqttEnabled->checkState() == Qt::Checked) ? 1 : 0 );
    settings.setValue("IsWemoEnabled", (ui->cbIsWemoEnabled->checkState() == Qt::Checked) ? 1 : 0 );
    settings.setValue("IsExecutablesEnabled", (ui->cbIsExecutablesEnabled->checkState() == Qt::Checked) ? 1 : 0 );

    settings.setValue("ExecutableWhenAboveMax", ui->efExecutableWhenAboveMax->text() );
    settings.setValue("ExecutableWhenBelowMin", ui->efExecutableWhenBelowMin->text() );
    settings.setValue("ExecutableArgsWhenAboveMax", ui->efExecutableArgsWhenAboveMax->text() );
    settings.setValue("ExecutableArgsWhenBelowMin", ui->efExecutableArgsWhenBelowMin->text() );

    settings.setValue("LinuxFileBatteryLevel", ui->efLinuxFileBatteryLevel->text() );

	close();
	return;
}



