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
#include <iostream>

MainWindow::MainWindow( Base *base, QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow )
{
	ui->setupUi( this );

	this->base = base;
	connect( this->base, SIGNAL( imgObjUpdated( const ImgObj * ) ),
			 this, SLOT( onImgObjUpdated( const ImgObj * ) ) );
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onImgObjUpdated( const ImgObj *imgObj )
{
	auto items = ui->listWidget->findItems( QFileInfo( imgObj->filepath ).fileName(), Qt::MatchExactly );
	for ( auto &item : items )
		item->setBackground( Qt::green ); // TODO: red if error occured

	ui->viewer->repaint();
}

void MainWindow::on_addImageBtn_clicked()
{
	QStringList filePaths = QFileDialog::getOpenFileNames( this,
	                                                       "Choose images",
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
	for ( auto &item : selected )
	{
		// TODO: redetect or skip already detected images
		if ( base->detectImage( item->data( Qt::ToolTipRole ).toString() ) )
		{
			item->setBackground( Qt::yellow );
			continue;
		}
		item->setBackground( Qt::red );
	}
}

void MainWindow::on_detectAllBtn_clicked()
{
	for ( int i = 0; i < ui->listWidget->count(); ++i )
	{
		auto item = ui->listWidget->item( i );
		if ( base->detectImage( item->data( Qt::ToolTipRole ).toString() ) )
		{
			item->setBackground( Qt::yellow );
			continue;
		}
		item->setBackground( Qt::red );
	}
}

void MainWindow::on_listWidget_itemSelectionChanged()
{
	if ( ui->listWidget->selectedItems().empty() )
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
		ui->viewer->repaint();
		return;
	}
	ui->viewer->setTarget( base->getImgObj( ui->listWidget->item( currentRow )->data( Qt::ToolTipRole ).toString() ) );
	ui->viewer->repaint();
}
