#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(Base *base, QWidget *parent) : QDialog(parent), base(base), ui(new Ui::LoginDialog)
{
	setWindowFlag( Qt::WindowContextHelpButtonHint, false );
	ui->setupUi( this );

	connect( this->base, &Base::getTokenFinished, this, &LoginDialog::onGetTokenFinished );
	connect( this->base, &Base::createUserFinished, this, &LoginDialog::onCreateUserFinished );

	if ( base->isAuthorized() )
	{
		ui->singupBtn->setEnabled( false );
		ui->loginBtn->setEnabled( false );
		ui->statusLabel->setText( "You are already logged in" );
		auto palette = ui->statusLabel->palette();
		palette.setColor( QPalette::WindowText, Qt::green );
		ui->statusLabel->setPalette( palette );
	}
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

void LoginDialog::on_singupBtn_clicked()
{
	base->createDemoUser( ui->emailInput->text(), ui->passwdInput->text() );
}

void LoginDialog::on_loginBtn_clicked()
{
	base->getToken( ui->emailInput->text(), ui->passwdInput->text() );
	ui->singupBtn->setEnabled( false );
	ui->loginBtn->setEnabled( false );
}

void LoginDialog::onGetTokenFinished( const QString &token )
{
	if ( !token.isEmpty() )
	{
		base->setToken( token );
		ui->statusLabel->setText( "Successfully logged in" );
		auto palette = ui->statusLabel->palette();
		palette.setColor( QPalette::WindowText, Qt::green );
		ui->statusLabel->setPalette( palette );

		done( 0 );
		return;
	}
	ui->singupBtn->setEnabled( true );
	ui->loginBtn->setEnabled( true );
	ui->statusLabel->setText( "Failed to log in" );
	auto palette = ui->statusLabel->palette();
	palette.setColor( QPalette::WindowText, Qt::red );
	ui->statusLabel->setPalette( palette );
}

void LoginDialog::onCreateUserFinished( bool success )
{
	if ( success )
	{
		ui->statusLabel->setText( "User created successfully" );
		auto palette = ui->statusLabel->palette();
		palette.setColor( QPalette::WindowText, Qt::green );
		ui->statusLabel->setPalette( palette );
		return;
	}
	ui->singupBtn->setEnabled( true );
	ui->loginBtn->setEnabled( true );
	ui->statusLabel->setText( "Failed to create user" );
	auto palette = ui->statusLabel->palette();
	palette.setColor( QPalette::WindowText, Qt::red );
	ui->statusLabel->setPalette( palette );
}
