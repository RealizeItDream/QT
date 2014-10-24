#include "myplaylist.h"
#include <QContextMenuEvent>
#include <QMenu>


MyPlaylist::MyPlaylist(QWidget *parent) :
    QTableWidget(parent)
{

    resize(325, 350);

    setRowCount(0);             //初始的行数为0
    setColumnCount(3);          //初始的列数为1

    //设置第一个标签
    QStringList list;
    list << tr("标题") << tr("歌手") << tr("长度");
    setHorizontalHeaderLabels(list);

    setSelectionMode(QAbstractItemView::SingleSelection);//设置只能选择单行
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setShowGrid(false);//设置不显示网格


}


void MyPlaylist::clear_play_list()
{
    while(rowCount())
        removeRow(0);
    emit play_list_clean();//删除完后，发送清空成功信号

}
void MyPlaylist::add_File_slot()
{
    emit add_File_signal();
}


void MyPlaylist::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    menu.addAction(tr("添加歌曲"),this,SLOT(add_File_slot()));
    if(this->rowCount()!= 0)
    {
        menu.addAction(tr("清空列表"), this, SLOT(clear_play_list()));//可以直接在这里指定槽函数
    }

    menu.setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                       "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                       "QMenu::item:selected:!enabled{background:transparent;}"
                       "QMenu::item:selected:enabled{background:lightgray;color:white;}");
    menu.exec(event->globalPos());//返回鼠标指针的全局位置


}


void MyPlaylist::closeEvent(QCloseEvent *event)
{
    if(isVisible()) {
        hide();
        event->ignore();//清零接收标志
    }
}


