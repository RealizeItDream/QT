#include "MySystemTray.h"
#include <QtGui>
SystemTray::SystemTray(QWidget *parent ):
        QSystemTrayIcon(parent)
{
    this->createActions();
    this->createPlayAction();
    this->translateLanguage();
    this->addActions();
    this->setContextMenu(pop_menu);
    this->setToolTip(tr("M音乐"));
    no_vol_flag = 0;
    //this->showMessage(tr("M音乐"),tr("给你不一样的音乐体验"),QSystemTrayIcon::Information,4000);

}
SystemTray::~SystemTray()
{

}

void SystemTray::translateLanguage()
{
    show_lrc_action->setText(tr("显示桌面歌词"));
    show_action->setText(tr("显示主界面"));
    addfile_action->setText(tr("添加歌曲"));
    clearlist_action->setText(tr("清空列表"));
    quit_action->setText(tr("退出"));
}

void SystemTray::createActions()
{
    this->setIcon(QIcon(":/images/ico.png"));
    pop_menu = new QMenu();
    show_lrc_action = new QAction(pop_menu);
    show_action = new QAction(pop_menu);
    addfile_action = new QAction(pop_menu);
    clearlist_action = new QAction(pop_menu);
    quit_action = new QAction(pop_menu);


    connect(show_lrc_action,SIGNAL(triggered()),this,SLOT(show_lrc_slot()));
    connect(show_action,SIGNAL(triggered()),this,SLOT(show_action_slot()));
    connect(addfile_action,SIGNAL(triggered()),this,SLOT(addfile_slot()));
    connect(clearlist_action,SIGNAL(triggered()),this,SLOT(clearlist_slot()));
    connect(quit_action,SIGNAL(triggered()),this,SLOT(quit_slot()));
}

void SystemTray::play_slot()
{
    play_button->setIcon(QIcon(":/images/trayplay2.png"));
    emit    play_signal();
}
void SystemTray::forward_slot()
{
    emit    forward_signal();
}
void SystemTray::backward_slot()
{
    emit    backward_signal();
}
void SystemTray::addfile_slot()
{
    emit addfile_signal();
}
void SystemTray::clearlist_slot()
{
    emit clearlist_signal();
}

void SystemTray::show_action_slot()
{
    emit    show_action_signal();
}
void SystemTray::show_lrc_slot()
{
    emit    show_lrc_signal();
}
void SystemTray::quit_slot()
{
    emit    quit_signal();
}
void SystemTray::set_song_name(QString s)
{
    music_name_label->setText(s);
    this->setToolTip(s);
}
void SystemTray::re_song_name()
{
    music_name_label->setText("M音乐，传播好音乐");
    this->setToolTip(tr("M音乐"));
}

void SystemTray::createPlayAction()
{
    play_button = new QPushButton();
    backward_button = new QPushButton();
    forward_button = new QPushButton();
    music_name_label = new QLabel();

    vol_slider  = new QSlider();
    vol_button  = new QToolButton();
    vol_widget  = new QWidget();

    //play_button->setToolTip(tr("播放"));
    //backward_button->setToolTip(tr("上一首"));
    //forward_button->setToolTip(tr("下一首"));

    //设置播放按钮的样式
    play_button->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    backward_button->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    forward_button->setStyleSheet("background-color: rgba(255, 255, 255, 0);");

    play_button->setIcon(QIcon(":/images/trayplay1.png"));
    backward_button->setIcon(QIcon(":/images/traylast.png"));
    forward_button->setIcon(QIcon(":/images/traynext.png"));

    //vol_button->setToolTip(tr("音量"));
    vol_button->setCursor(Qt::PointingHandCursor);
    vol_button->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    vol_button->setIcon(QIcon(":/images/traysound1.png"));
    vol_slider_widgetAction = new QWidgetAction(pop_menu);

    vol_slider->setOrientation(Qt::Horizontal);
    vol_slider->setValue(90);
    vol_slider->setRange(0,100);

    vol_slider->setStyleSheet("QSlider::groove:horizontal{border:0px;height:4px;}"
                              "QSlider::sub-page:horizontal{background:rgb(170, 170, 170);}"
                              "QSlider::add-page:horizontal{background:lightgray;}"
                              "QSlider::handle:horizontal{background:rgb(179, 179, 134);width:10px;border-radius:2px;margin:-3px 0px -3px 0px;}"
                              "QSlider::handle:hover{background:rgb(0, 57, 84);width:10px;border-radius:2px;margin:-3px 0px -3px 0px;}");
    QHBoxLayout *vol_layout = new QHBoxLayout();

    vol_layout->addWidget(vol_button);
    vol_layout->addWidget(vol_slider);
    vol_widget->setLayout(vol_layout);
    vol_slider_widgetAction->setDefaultWidget(vol_widget);

    play_button->setFixedSize(50,50);
    backward_button->setFixedSize(40,40);
    forward_button->setFixedSize(40,40);

    play_widget = new QWidget();
    play_widget_action = new QWidgetAction(pop_menu);
    backward_button->setCursor(Qt::PointingHandCursor);
    play_button->setCursor(Qt::PointingHandCursor);
    forward_button->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(backward_button);
    layout->addWidget(play_button);
    layout->addWidget(forward_button);
    layout->setSpacing(0);
    layout->setContentsMargins(20, 0, 20, 0);

    QVBoxLayout *layout2 = new QVBoxLayout();
    layout2->addLayout(layout);
    layout2->addWidget(music_name_label, 0, Qt::AlignCenter);
    layout2->setSpacing(5);
    layout2->setContentsMargins(0, 0, 0, 5);
    music_name_label->setText("M音乐，传播好音乐");
    music_name_label->setEnabled(false);
    music_name_label->setFixedSize(180,20);

    music_name_label->setAlignment(Qt::AlignCenter);
    play_widget->resize(play_widget->width(),play_widget->height()+40);
    play_widget->setLayout(layout2);

    play_widget_action->setDefaultWidget(play_widget);

    connect(play_button,SIGNAL(clicked()),this,SLOT(play_slot()));
    connect(backward_button,SIGNAL(clicked()),this,SLOT(backward_slot()));
    connect(forward_button,SIGNAL(clicked()),this,SLOT(forward_slot()));
    connect(vol_button,SIGNAL(clicked()),this,SLOT(no_vol_slots()));
    connect(vol_slider,SIGNAL(valueChanged(int)),this,SLOT(changevol_slot(int)));
}
void SystemTray::addActions()
{
    pop_menu->addAction(play_widget_action);
    pop_menu->addSeparator();
    pop_menu->addAction(vol_slider_widgetAction);
    pop_menu->addSeparator();
    pop_menu->addAction(show_action);

    pop_menu->addAction(show_lrc_action);
    pop_menu->addSeparator();
    pop_menu->addAction(addfile_action);
    pop_menu->addAction(clearlist_action);
    pop_menu->addSeparator();
    pop_menu->addAction(quit_action);

    pop_menu->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                            "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                            "QMenu::item:selected:!enabled{background:transparent;}"
                            "QMenu::item:selected:enabled{background:lightgray;color:white;}"
                            "QMenu::separator{height:1px;background:lightgray;margin:5px 0px 5px 0px;}");
}
//点击音量按钮时触发
void SystemTray::no_vol_slots()
{

    if(no_vol_flag == 0)
    {
        no_vol_flag = 1;
        vol_button->setIcon(QIcon(":/images/traysound2.png"));
    }
    else
    {
        no_vol_flag = 0;
        vol_button->setIcon(QIcon(":/images/traysound1.png"));
    }
    emit no_vol_signal();

}
//音量滑条滑动时触发
void SystemTray::changevol_slot(int vol)
{

    qreal dvol = vol/100.0;
    if(vol == 0)
    {
        no_vol_flag = 1;
        vol_button->setIcon(QIcon(":/images/traysound2.png"));
    }
    else
    {
        no_vol_flag = 0;
        vol_button->setIcon(QIcon(":/images/traysound1.png"));
    }
    if(vol <50&&vol!=0)

         vol_button->setIcon(QIcon(":/images/traysoundless.png"));

    if(vol>50)
        vol_button->setIcon(QIcon(":/images/traysound1.png"));

    emit changevol_signal(dvol);
}
//音量改变时改变滑条
void SystemTray::changevolSlider_slot(qreal vol)
{
    vol_slider->setValue(vol*100);
}
//根据当前音乐播放状态来更改播放按钮图标
void SystemTray::state(Phonon::State state)
{
    switch(state)
    {
    case Phonon::PlayingState:
        play_button->setIcon(QIcon(":/images/trayplay2.png"));
        break;
    case Phonon::PausedState:
    case Phonon::StoppedState:
        play_button->setIcon(QIcon(":/images/trayplay1.png"));
        break;
    default:
    ;
    }
}
