#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->server = new Server();
	connect(server,&Server::sendSize,this,&MainWindow::getSize);
	connect(server,&Server::sendSpeed,this,&MainWindow::getSpeed);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_ButtonStart_clicked()
{
	if(!isStart)
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
					ENCODE_TYPE type;
					if(ui->comboBoxEncodeType->currentText() =="NONE")
						{
							type = ENCODE_TYPE::NONE;
						}
					else if(ui->comboBoxEncodeType->currentText() =="XOR")
						{
							type = ENCODE_TYPE::XOR;
						}
					else if(ui->comboBoxEncodeType->currentText() =="AES")
						{
							type = ENCODE_TYPE::AES;
						}
					
					this->server->setInfo(ip,port.toInt(),type);
					server->startServer("127.0.0.1",ui->lineEdit->text().toInt());
					
				}
			ui->ButtonStart->setText("停止服务");
			isStart = true;
		}
	else
		{
			server->stopServer();
			ui->ButtonStart->setText("开始服务");
			isStart = false;
		}
}

void MainWindow::on_ButtonGetSize_clicked()
{
//	ui->label_2->setText(QString::number(this->size));
}

void MainWindow::getSize(int size)
{
	this->size += size;
	ui->LabelDownLoad->setText(QString::number(this->size/1000)+"k");
}

void MainWindow::getSpeed(int size)
{
	ui->LabelBandWidth->setText(QString::number(size/1000)+"/s");
}
