#include "mylrc.h"
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>


MyLrc::MyLrc(QWidget *parent) :
    QLabel(parent)
{
    //FramelessWindowHint为无边界的窗口
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setText(tr("M音乐"));
    // 固定显示区域大小
    setMaximumSize(800, 60);
    setMinimumSize(800, 60);

    //歌词的线性渐变填充
    linear_gradient.setStart(0, 10);//填充的起点坐标
    linear_gradient.setFinalStop(0, 40);//填充的终点坐标
    //第一个参数终点坐标，相对于我们上面的区域而言，按照比例进行计算
    linear_gradient.setColorAt(0.1, QColor(14, 179, 255));
    linear_gradient.setColorAt(0.5, QColor(114, 232, 255));
    linear_gradient.setColorAt(0.9, QColor(14, 179, 255));

    // 遮罩的线性渐变填充
    mask_linear_gradient.setStart(0, 10);
    mask_linear_gradient.setFinalStop(0, 40);
    mask_linear_gradient.setColorAt(0.1, QColor(222, 54, 4));
    mask_linear_gradient.setColorAt(0.5, QColor(255, 72, 16));
    mask_linear_gradient.setColorAt(0.9, QColor(222, 54, 4));

    // 设置字体
    font.setFamily("Times New Roman");
    font.setBold(true);
    font.setPointSize(30);

    // 设置定时器
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    lrc_mask_width = 0;
    lrc_mask_width_interval = 0;
}


// 开启遮罩，需要指定当前歌词开始与结束之间的时间间隔
void MyLrc::start_lrc_mask(qint64 intervaltime)
{
    // 这里设置每隔30毫秒更新一次遮罩的宽度，因为如果更新太频繁
    // 会增加CPU占用率，而如果时间间隔太大，则动画效果就不流畅了
    qreal count = intervaltime / 30;
    // 获取遮罩每次需要增加的宽度，这里的800是部件的固定宽度
    lrc_mask_width_interval = 800 / count;
    lrc_mask_width = 0;
    timer->start(30);
}

void MyLrc::stop_lrc_mask()
{
    timer->stop();
    lrc_mask_width = 0;
    update();
}


void MyLrc::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setFont(font);

    // 先绘制底层文字，作为阴影，这样会使显示效果更加清晰，且更有质感
    painter.setPen(QColor(0, 0, 0, 200));
    painter.drawText(1, 1, 800, 60, Qt::AlignLeft, text());//左对齐

    // 再在上面绘制渐变文字
    painter.setPen(QPen(linear_gradient, 0));
    painter.drawText(0, 0, 800, 60, Qt::AlignLeft, text());

    // 设置歌词遮罩
    painter.setPen(QPen(mask_linear_gradient, 0));
    painter.drawText(0, 0, lrc_mask_width, 60, Qt::AlignLeft, text());

}

//左击操作
void MyLrc::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        offset = event->globalPos() - frameGeometry().topLeft();
}


void MyLrc::mouseMoveEvent(QMouseEvent *event)
{
    //移动鼠标到歌词上时，会显示手型
    //event->buttons()返回鼠标点击的类型，分为左击，中击，右击
    //这里用与操作表示是左击
    if (event->buttons() & Qt::LeftButton) {
        setCursor(Qt::PointingHandCursor);
        //实现移动操作
        move(event->globalPos() - offset);
       }
}

//右击事件
void MyLrc::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                       "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                       "QMenu::item:selected:!enabled{background:transparent;}"
                       "QMenu::item:selected:enabled{background:lightgray;color:white;}"
);
    menu.addAction(tr("隐藏"), this, SLOT(myhide_slot()));
    menu.exec(event->globalPos());//globalPos()为当前鼠标的位置坐标
}


void MyLrc::timeout()
{
    //每隔一段固定的时间笼罩的长度就增加一点
    lrc_mask_width += lrc_mask_width_interval;
    update();//更新widget，但是并不立即重绘，而是安排一个Paint事件，当返回主循环时由系统来重绘
}
void MyLrc::myhide_slot()
{
    emit myhide_signal(false);
    this->hide();

}

