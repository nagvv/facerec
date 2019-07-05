#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( std::shared_ptr<Base> base, QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow )
{
	ui->setupUi( this );

	this->base = base;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_addImageBtn_clicked()
{
	QStringList fileNames = QFileDialog::getOpenFileNames( this,
	                                                       "Choose images",
	                                                       QString(),
	                                                       "JPEG Images (*.jpeg *.jpg)" );

	for ( auto &name: fileNames )
	{
		if ( !base->addFile( name ) )
			continue;
		auto temp = QFileInfo( name );
		QListWidgetItem *item = new QListWidgetItem( ui->listWidget, 0 );
		item->setData( Qt::ToolTipRole, name );
		item->setText( temp.fileName() );
		ui->listWidget->addItem( item );
	}

	ui->detectAllBtn->setEnabled( true );
}

#include <iostream>

void MainWindow::on_detectBtn_clicked()
{
	auto selected = ui->listWidget->selectedItems();
	for ( auto &item : selected )
	{
		// TODO: redetect or skip already detected images
		if ( base->detectImage( item->data( Qt::ToolTipRole ).toString() ) )
		{
			item->setBackground(Qt::yellow);
			continue;
		}
		item->setBackground(Qt::red);
	}
}

void MainWindow::on_listWidget_itemSelectionChanged()
{
	if ( ui->listWidget->selectedItems().empty() )
		ui->detectBtn->setEnabled( false );
	else
		ui->detectBtn->setEnabled( true );
}
