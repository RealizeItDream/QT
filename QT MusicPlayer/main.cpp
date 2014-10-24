#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //获取系统编码，否则移植会出现乱码
        QTextCodec *codec = QTextCodec::codecForName("System");

        //设置和对本地文件系统读写时候的默认编码格式
        QTextCodec::setCodecForLocale(codec);

        //设置传给tr函数时的默认字符串编码
        QTextCodec::setCodecForTr(codec);

        //用在字符常量或者QByteArray构造QString对象时使用的一种编码方式
        QTextCodec::setCodecForCStrings(codec);
    MainWindow w;
    w.show();

    return a.exec();
}
