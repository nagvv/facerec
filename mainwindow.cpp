/*
 * Copyright 2019 nagvv ( https://github.com/nagvv )
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *    http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"

MainWindow::MainWindow( Base *base, QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow )
{
	ui->setupUi( this );

	this->base = base;
	connect( this->base, &Base::imgObjUpdated, this, &MainWindow::onImgObjUpdated );
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::detectItem( QListWidgetItem *item )
{
	if ( item->background() == Qt::green ) // img has succesfully detected
		return;

	if ( base->detectImage( item->data( Qt::ToolTipRole ).toString() /*,
							[this](qint64 bytesSent, qint64 bytesTotal)
							{

							}*/) )
	{
		pushProcessing();
		item->setBackground( Qt::yellow );
		return;
	}

	item->setBackground( Qt::red );
}

void MainWindow::onImgObjUpdated( const ImgObj *imgObj )
{
	auto items = ui->listWidget->findItems( QFileInfo( imgObj->filepath ).fileName(), Qt::MatchExactly );

	for ( auto &item : items )
		item->setBackground( imgObj->success ? Qt::green : Qt::red );

	popProcessing();

	ui->viewer->repaint();
}

void MainWindow::pushProcessing()
{
	nowProcessing++;
	if ( nowProcessing + finishedProcessing == 1 )
		ui->progressBar->setMaximum( 0 ); // The progress bar "rewinds" and shows no progress
	else // ( nowProcessing + finishedProcessing > 1)
		ui->progressBar->setMaximum( nowProcessing + finishedProcessing );
}

void MainWindow::popProcessing()
{
	nowProcessing--;
	finishedProcessing++;
	ui->progressBar->setValue( finishedProcessing );

	if ( nowProcessing == 0 ) // all job finished
	{
		finishedProcessing = 0;
		ui->progressBar->setValue( 0 );
		ui->progressBar->setMaximum( 1 );
	}
}

void MainWindow::on_addImageBtn_clicked()
{
	auto keyboardModifiers = QGuiApplication::keyboardModifiers();
	if ( keyboardModifiers & Qt::ControlModifier )
		return addImagesFromDirectory();

	QStringList filePaths = QFileDialog::getOpenFileNames( this,
	                                                       "Select one or more images",
	                                                       QString(),
	                                                       "Images (*.jpeg *.jpg *.png *.bmp)" );

	for ( auto &fpath: filePaths )
	{
		if ( !base->addFile( fpath ) )
			continue;
		auto temp = QFileInfo( fpath );
		auto item = new QListWidgetItem( ui->listWidget, 0 );
		item->setData( Qt::ToolTipRole, fpath );
		item->setText( temp.fileName() );
		ui->listWidget->addItem( item );
	}

	ui->detectAllBtn->setEnabled( true );
	if ( ui->listWidget->currentRow() < 0 )
		ui->listWidget->setCurrentRow( 0 );
}

void MainWindow::addImagesFromDirectory()
{
	QString dirPath = QFileDialog::getExistingDirectory( this );

	if ( dirPath.isEmpty() )
		return;

	QStringList fileNames = QDir( dirPath ).entryList( { "*.jpg", "*jpeg", "*.png", "*.bmp" }, QDir::Files );

	for ( auto &fname: fileNames )
	{
		QString fpath = dirPath + QDir::separator() + fname;
		if ( !base->addFile( fpath ) )
			continue;
		auto item = new QListWidgetItem( ui->listWidget, 0 );
		item->setData( Qt::ToolTipRole, fpath );
		item->setText( fname );
		ui->listWidget->addItem( item );
	}

	ui->detectAllBtn->setEnabled( true );
	if ( ui->listWidget->currentRow() < 0 )
		ui->listWidget->setCurrentRow( 0 );
}


void MainWindow::on_removeImageBtn_clicked()
{
	auto selected = ui->listWidget->selectedItems();
	for ( auto &item : selected )
	{
		base->removeFile( item->data( Qt::ToolTipRole ).toString() );
		delete item;
	}

	if ( ui->listWidget->count() == 0 )
		ui->detectAllBtn->setEnabled( false );
}

void MainWindow::on_detectBtn_clicked()
{
	auto selected = ui->listWidget->selectedItems();
	for ( auto item : selected )
		detectItem( item );
}

void MainWindow::on_detectAllBtn_clicked()
{
	for ( int i = 0; i < ui->listWidget->count(); ++i )
		detectItem( ui->listWidget->item( i ) );
}

void MainWindow::on_listWidget_itemSelectionChanged()
{
	ui->listWidget->setItemSelected( ui->listWidget->currentItem(), true );
	if ( ui->listWidget->selectedItems().isEmpty() )
	{
		ui->detectBtn->setEnabled( false );
		ui->removeImageBtn->setEnabled( false );
		return;
	}
	ui->detectBtn->setEnabled( true );
	ui->removeImageBtn->setEnabled( true );
}

void MainWindow::on_listWidget_currentRowChanged( int currentRow )
{
	if ( currentRow < 0 )
	{
		ui->viewer->setTarget( nullptr );
		ui->helpText->setVisible( true );
		ui->viewer->repaint();
		return;
	}
	ui->viewer->setTarget( base->getImgObj( ui->listWidget->item( currentRow )->data( Qt::ToolTipRole ).toString() ) );
	ui->helpText->setVisible( false );
	ui->viewer->repaint();
}

void MainWindow::on_loginBtn_clicked()
{
	LoginDialog login( base, this );
	login.exec();
	if ( base->isAuthorized() )
	{
		ui->loginBtn->setEnabled( false );
		ui->logoutBtn->setEnabled( true );
	}
}

void MainWindow::on_logoutBtn_clicked()
{
	base->setToken( QString() );
	ui->loginBtn->setEnabled( true );
	ui->logoutBtn->setEnabled( false );
}
