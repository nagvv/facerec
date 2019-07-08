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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QProgressBar>
#include <memory>
#include "base.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
private:
Q_OBJECT
	Base *base;
	int nowProcessing = 0;
	int finishedProcessing = 0;
	void detectItem( QListWidgetItem *item );
	void pushProcessing();
	void popProcessing();

public:
	explicit MainWindow( Base *base, QWidget *parent = nullptr );

	~MainWindow();

private slots:
	void onImgObjUpdated( const ImgObj * );

	void on_addImageBtn_clicked();

	void on_removeImageBtn_clicked();

	void on_detectBtn_clicked();

	void on_detectAllBtn_clicked();

	void on_listWidget_itemSelectionChanged();

	void on_listWidget_currentRowChanged( int currentRow );

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
