
#include <QtWidgets>

#include "batterywatch.h"


/** {{{ int main(int argc, char *argv[])
 */
int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(batterywatch);

	QApplication app(argc, argv);

	if ( ! QSystemTrayIcon::isSystemTrayAvailable() ) {
		QMessageBox::critical( 0, QObject::tr("BatteryWatch"), QObject::tr("I couldn't detect any system tray on this system.") );
		return 1;
	}
	QApplication::setQuitOnLastWindowClosed(false);

	BatteryWatch bw;

	return app.exec();

	return 0;
}
/* }}} */


