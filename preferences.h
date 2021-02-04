#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
    class Preferences;
}

class Preferences : public QDialog
{
    Q_OBJECT

public:
    Preferences(QWidget *parent = 0);
    ~Preferences();

	static QString getUserName();

public slots:
	void accept();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_btnChooseExeWhenBelowMin_clicked();
    void on_btnChooseExeWhenAboveMax_clicked();

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
