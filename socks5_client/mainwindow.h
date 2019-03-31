#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
private slots:
	void on_ButtonStart_clicked();
	
	void on_ButtonStop_clicked();
	
	void on_ButtonGetSize_clicked();
	
	void getSize(int size);
private:
	Ui::MainWindow *ui;
	Server *server;
	int size = 0;
};

#endif // MAINWINDOW_H
