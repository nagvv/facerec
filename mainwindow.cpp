#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow( Base *base, QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow )
{
	ui->setupUi( this );

	this->base = base;
	connect( this->base, SIGNAL( imgObjUpdated( const ImgObj * ) ), this, SLOT( onImgObjUpdated( const ImgObj * ) ) );
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::onImgObjUpdated( const ImgObj *imgObj )
{
	auto items = ui->listWidget->findItems( QFileInfo(imgObj->filepath).fileName(), Qt::MatchExactly );
	for ( auto &item : items )
		item->setBackground(Qt::green);

	ui->viewer->repaint();
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

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
	if ( currentRow < 0 )
		return;
	ui->viewer->setTarget(base->getImgObj( ui->listWidget->item(currentRow)->data( Qt::ToolTipRole ).toString() ) );
	ui->viewer->repaint();
}
