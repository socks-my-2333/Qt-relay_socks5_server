#include "mainwindow.h"
#include <QApplication>
#include "server.h"
#include <QUdpSocket>
#include <QDebug>
#include "windows.h"
//#include "qaeswrap.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	
	/*
	QByteArray key = "1234567891234567";
	QAesWrap aes(key,key,QAesWrap::AesBit::AES_128);
	QByteArray plainText = "这个中文什么都没0有     ";
	
	qDebug()<<"加密前:"<<plainText;
	QByteArray encodedText = aes.encrypt(plainText,QAesWrap::AesMode::AES_ECB);
	qDebug()<<"加密后:"<<encodedText;
	QByteArray decodedText = aes.decrypt(encodedText,QAesWrap::AesMode::AES_ECB);
	qDebug()<<"解密后:"<<decodedText;
	*/
	
	
	MainWindow w;
	w.show();
	return a.exec();
}
