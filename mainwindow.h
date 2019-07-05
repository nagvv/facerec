#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
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
	std::shared_ptr<Base> base;

public:
	explicit MainWindow( std::shared_ptr<Base> base, QWidget *parent = nullptr );

	~MainWindow();

private slots:

	void on_addImageBtn_clicked();

	void on_detectBtn_clicked();

	void on_listWidget_itemSelectionChanged();

	void on_listWidget_currentRowChanged(int currentRow);

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
