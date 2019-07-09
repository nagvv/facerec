#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "base.h"

namespace Ui {
	class LoginDialog;
}

class LoginDialog : public QDialog
{
private:
	Q_OBJECT
	Base *base;

public:
	explicit LoginDialog(Base *base, QWidget *parent = nullptr);
	~LoginDialog();

private slots:
	void on_singupBtn_clicked();

	void on_loginBtn_clicked();

	void onGetTokenFinished( const QString &token );

	void onCreateUserFinished( bool success );

private:
	Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
