#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->server = new Server();
//	connect(server,&Server::sendSize,this,&MainWindow::getSize);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_ButtonStart_clicked()
{
    if(ui->lineEdit->text().isEmpty())
	    {
		    return;
	    }
    
    QString ip =ui->lineServerIP->text();
    QString port = ui->lineServerPort->text();
    if(ip.isEmpty() || port.isEmpty())
	    {
		    qDebug()<<"null";
		    return;
	    }
    else
	    {
		    server->startServer("127.0.0.1",ui->lineEdit->text().toInt());
		    this->server->setInfo(ip,port.toInt());
	    }
    
    ui->ButtonStart->setDisabled(true);
    ui->ButtonStop->setEnabled(true);
}

void MainWindow::on_ButtonStop_clicked()
{
    server->stopServer();
    ui->ButtonStop->setDisabled(true);
    ui->ButtonStart->setEnabled(true);
}

void MainWindow::on_ButtonGetSize_clicked()
{
	ui->label_2->setText(QString::number(this->size));
}

void MainWindow::getSize(int size)
{
	this->size += size;
	ui->label_2->setText(QString::number(this->size));
}
