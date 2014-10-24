#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QColorDialog>
#include <QFileDialog>
#include <QActionGroup>
#include <phonon>
#include <QFileInfo>
#include <QDebug>
#include "myplaylist.h"
#include "no_focus_delegate.h"
#include <QUrl>
#include <QList>
#include "mylrc.h"
//#include <QKeySequence>
#include <QAction>
#include <QFile>
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MyWindowStyle();
    MyWindowButtonStyle();
    MyTrayIcon();
    initMusic();
    initEventFilter();
    initTableWidget();
    initFlag();

    songNo = 0;

    lrc = new MyLrc(this);

    readfile();
    connect(lrc,SIGNAL(myhide_signal(bool)),ui->checkBox_lrc,SLOT(setChecked(bool)));

    //改变托盘显示的歌曲名
    connect(this,SIGNAL(set_tray_songnamelabel(QString)),mytrayIcon,SLOT(set_song_name(QString)));
    //重置标题
    connect(this,SIGNAL(re_tray_songnamelabel()),mytrayIcon,SLOT(re_song_name()));
    //声音改变时发送信号
    connect(this,SIGNAL(vol_change_signal(qreal)),mytrayIcon,SLOT(changevolSlider_slot(qreal)));
    //媒体播放状态改变是发送信号
    connect(this,SIGNAL(changethetraystate(Phonon::State)),mytrayIcon,SLOT(state(Phonon::State)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


//重写鼠标事件,因为主窗体设置为无边框模式，需重写鼠标响应事件，实现窗体的拖动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
        //鼠标相对于窗体的位置
        move_point = event->pos();
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //若鼠标左键被按下
    if(mouse_press)
    {
        //鼠标相对于屏幕的位置
        QPoint move_pos = event->globalPos();

        //移动主窗体位置
        this->move(move_pos - move_point);
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    //设置鼠标为未被按下
    mouse_press = false;
}

void MainWindow::myclose()                  //窗体关闭
{
    //int msg = QMessageBox::warning(NULL, "warning", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    //if(msg == QMessageBox::No) return;
    //如果在播放音乐，先停止
    if(mediaObject->state() == Phonon::PlayingState)
    {
        mediaObject->stop();

    }
    writefile();
    //界面淡出
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, SIGNAL(finished()), this, SLOT(close()));
}
void MainWindow::MyWindowStyle()            //设置窗体属性
{
    //设置窗体为固定大小，高度190为不显示列表状态
    this->setFixedSize(this->width(),190);

    //设置图标
    QIcon icon = QIcon(":/images/ico.png");
    this->setWindowIcon(icon);

    //设置界面无边框并位于状态栏上
    this->setWindowFlags(Qt::FramelessWindowHint );
    this->setMouseTracking(true);


    //设置能接收拖动
    this->setAcceptDrops(true);

    //设置初始背景颜色，深蓝色
    QPalette p;
    p.setBrush(this->backgroundRole(),QBrush(QColor(0,0,127,255)));
    this->setPalette(p);
    this->setAutoFillBackground(true);

    ui->songtotal->setText("");
    ui->songnum->setText("");

    playmode = 3;

    //界面渐变出现
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}
void MainWindow::MyWindowButtonStyle()      //设置窗体上最小化，关闭按钮属性
{
    ui->playButton->setCursor(Qt::PointingHandCursor);
    ui->nextButton->setCursor(Qt::PointingHandCursor);
    ui->lastButton->setCursor(Qt::PointingHandCursor);
    //获取最小化、关闭按钮
   // QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarShadeButton);
    //QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

    //设置最小化、关闭按钮图标
    //ui->minButton->setIcon(minPix);
    //ui->closeButton->setIcon(closePix);

    //设置最小化、关闭按钮在界面的位置
    ui->minButton->setGeometry(this->width()-46,8,20,20);
    ui->closeButton->setGeometry(this->width()-25,5,20,20);

    //设置最小化、关闭按钮的鼠标形状
    ui->minButton->setCursor(Qt::PointingHandCursor);
    ui->closeButton->setCursor(Qt::PointingHandCursor);


    //设置最小化、关闭按钮的样式
    ui->minButton->setStyleSheet("background-color:transparent;");
    ui->closeButton->setStyleSheet("background-color:transparent;");


    //最小化、关闭按钮响应事件
    connect(ui->minButton,SIGNAL(clicked()),SLOT(showMinimized()));
    connect(ui->closeButton,SIGNAL(clicked()),SLOT(myclose()));


}
void MainWindow::initTableWidget()      //初始化列表
{
    playlist = new MyPlaylist(this);
    playlist->move(20,200);                                     //设置位置
    playlist->setFrameShape(QFrame::NoFrame);                   //设置无边框
    playlist->horizontalHeader()->setStretchLastSection(true);  //表头伸缩
    playlist->horizontalHeader()->resizeSection(0,200);         //设置表头第一项宽度
    playlist->horizontalHeader()->resizeSection(1,70);          //设置表头第二项宽度
    playlist->horizontalHeader()->setClickable(false);          //设置表头不能点击
    playlist->setItemDelegate(new QLineDelegate());             //设置选择行时不显示虚框

    QHeaderView *headerView = playlist->verticalHeader();
    headerView->setHidden(true);                                //隐藏垂直表头

   // playlist->setStyleSheet("selection-background-color: rgb(152, 171, 255);"
   //                         "selection-color:rgb(0,0,0);");
    //当双击列表中项的时候响应
    connect(playlist,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(TableClicked(QTableWidgetItem*)));
    //清除列表时响应
    connect(playlist, SIGNAL(play_list_clean()), this, SLOT(ClearSources()));
    //添加文件
    connect(playlist,SIGNAL(add_File_signal()),this,SLOT(addfile()));
    //connect(playlist,SIGNAL(cellEntered(int,int)))

}

void MainWindow::initMusic()                //初始化媒体对象，并设置相关控件属性
{
    //创建媒体对象
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    metaInformationResolveraudioOutput =  new Phonon::AudioOutput(Phonon::MusicCategory, this);

    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);
    Phonon::createPath(metaInformationResolver, metaInformationResolveraudioOutput);



    //绑定源和接收器
    Phonon::createPath(mediaObject, audioOutput);

    audioOutput->setVolume(0.9);
    //设置tick信号间断发送时间，1000毫秒，用于更新时间label
    mediaObject->setTickInterval(1000);

    //创建滑条并设置相关属性
    seekSlider = new Phonon::SeekSlider(mediaObject);
    seekSlider->setCursor(Qt::PointingHandCursor);
    ui->verticalLayout_slider->addWidget(seekSlider);
    seekSlider->setStyleSheet("QSlider::groove:horizontal{border:0px;height:4px;}"
                              "QSlider::sub-page:horizontal{background:white;}"
                              "QSlider::add-page:horizontal{background:lightgray;}"
                              "QSlider::handle:horizontal{background:white;width:10px;border-radius:5px;margin:-3px 0px -3px 0px;}");
    //创建音量条并设置相关属性
    volumeSlider = new Phonon::VolumeSlider();
    volumeSlider->setCursor(Qt::PointingHandCursor);
    ui->verticalLayout->addWidget(volumeSlider);
    volumeSlider->setAudioOutput(audioOutput);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal{border:0px;height:4px;}"
                                "QSlider::sub-page:horizontal{background:white;}"
                                "QSlider::add-page:horizontal{background:lightgray;}"
                                "QSlider::handle:horizontal{background:white;width:10px;border-radius:5px;margin:-3px 0px -3px 0px;}");



    //关联媒体对象的tick信号来更新播放时间的显示
    connect(mediaObject,SIGNAL(tick(qint64)),this,SLOT(updateTime(qint64)));
    //当前歌曲播放完事触发的动作
    connect(mediaObject,SIGNAL(aboutToFinish()),this,SLOT(AboutToFinish()));
    //当媒体状态发生改变时，触发函数
    connect(mediaObject,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,SLOT(StateChanged(Phonon::State,Phonon::State)));
    connect(metaInformationResolver,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,SLOT(MetaStateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject,SIGNAL(currentSourceChanged(Phonon::MediaSource)),this,SLOT(SourceChanged(Phonon::MediaSource)));

    //当音量发生改变时触发
    connect(audioOutput,SIGNAL(volumeChanged(qreal)),this,SLOT(volchange_slot(qreal)));
}
void MainWindow::initFlag()                 //初始化标记
{
    flag = 0;
    playflag = 0;
    firsttime = 0;
}

void MainWindow::MyTrayIcon()               //实现托盘功能
{

    QString title = tr("M音乐");
    QString text = tr("给你不一样的音乐体验");

    mytrayIcon = new SystemTray(this);

    mytrayIcon->show();
    mytrayIcon->showMessage(title,text,QSystemTrayIcon::Information,4000);

    connect(mytrayIcon,SIGNAL(no_vol_signal()),this,SLOT(setSilent()));
    connect(mytrayIcon,SIGNAL(play_signal()),this,SLOT(on_playButton_clicked()));
    connect(mytrayIcon,SIGNAL(forward_signal()),this,SLOT(on_nextButton_clicked()));
    connect(mytrayIcon,SIGNAL(backward_signal()),this,SLOT(on_lastButton_clicked()));
    connect(mytrayIcon,SIGNAL(quit_signal()),this,SLOT(myclose()));
    connect(mytrayIcon,SIGNAL(changevol_signal(qreal)),this,SLOT(traytochangvol(qreal)));
    connect(mytrayIcon,SIGNAL(addfile_signal()),this,SLOT(addfile()));
    connect(mytrayIcon,SIGNAL(clearlist_signal()),this,SLOT(traytoclearlist_slot()));
    connect(mytrayIcon,SIGNAL(show_action_signal()),this,SLOT(showThisWindow()));
    connect(mytrayIcon,SIGNAL(show_lrc_signal()),this,SLOT(SetLrcShown()));
    //托盘点击响应
    connect(mytrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));


}
void MainWindow::iconIsActived(QSystemTrayIcon::ActivationReason reason)    //托盘点击响应事件
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:          //单击
    case QSystemTrayIcon::DoubleClick:      //双击
        this->showNormal();
        this->raise();
        break;
    default:
        break;
    }
}
void MainWindow::initEventFilter()          //为控件注册事件过滤器
{
    ui->playButton->installEventFilter(this);        //播放按钮
    ui->nextButton->installEventFilter(this);        //下一首按钮
    ui->lastButton->installEventFilter(this);        //上一首按钮
    ui->styleButton->installEventFilter(this);       //换肤按钮
    this->installEventFilter(this);                  //主窗体
}
bool MainWindow::eventFilter(QObject *obj, QEvent *e)//事件过滤器，实现播放/暂停按钮、上/下一首按钮，换肤按钮等图标切换。
{

    if(obj == this)
    {
        QString s;
        if(e->type() == QEvent::DragEnter)
        {
            QDragEnterEvent *dee = dynamic_cast<QDragEnterEvent *>(e);
                      dee->acceptProposedAction();
                        return true;
        }
        else if(e->type() == QEvent::Drop)
        {
            //将拖入的文件加入列表
            QDropEvent *de = dynamic_cast<QDropEvent *>(e);
            QList<QUrl> urls = de->mimeData()->urls();
            int index = sources.size();

            if (urls.isEmpty()) { return true; }
            for (int i = 0; i < urls.size(); ++i) {

                s = urls.at(i).toString().replace("file:///","");
                Phonon::MediaSource source(s);
                sources.append(source);

             }
            if(!sources.isEmpty())
            {
                //如果媒体源列表不为空，则将新加入的第一个媒体源作为当前媒体源
                metaInformationResolver->setCurrentSource(sources.at(index));
            }

        }
    }

    if(obj == ui->playButton)               //当前对象为播放按钮
    {
        if(e->type() == QEvent::Enter)      //鼠标进入按钮时动作
        {
            if(flag == 1)                   //flag为1，暂停状态，为0，播放状态
            {
                //切换按钮背景图
                ui->playButton->setIcon(QPixmap(":/images/play1.png"));
            }
            if(flag == 0)
            {
                ui->playButton->setIcon(QPixmap(":/images/play4.png"));
            }

        }
        if(e->type() == QEvent::Leave)              //鼠标离开按钮时动作
        {
            if(flag == 1)
            {
                ui->playButton->setIcon(QPixmap(":/images/play2.png"));
            }
            if(flag == 0)
            {
                ui->playButton->setIcon(QPixmap(":/images/play5.png"));
            }

        }
    }
    if(obj == ui->nextButton)               //对象为下一首按钮
    {
        if(e->type() == QEvent::Enter)
        {
            ui->nextButton->setIcon(QPixmap(":/images/next.png"));
        }
        if(e->type() == QEvent::Leave)
        {
            ui->nextButton->setIcon(QPixmap(":/images/next1.png"));
        }
    }
    if(obj == ui->lastButton)               //对象为上一首按钮
    {
        if(e->type() == QEvent::Enter)
        {
            ui->lastButton->setIcon(QPixmap(":/images/last.png"));
        }
        if(e->type() == QEvent::Leave)
        {
            ui->lastButton->setIcon(QPixmap(":/images/last1.png"));
        }
    }
    if(obj == ui->styleButton)              //换肤按钮
    {
        if(e->type() == QEvent::Enter)
        {
            ui->styleButton->setIcon(QPixmap(":/images/picstyle2.png"));
        }
        if(e->type() == QEvent::Leave)
        {
            ui->styleButton->setIcon(QPixmap(":/images/picstyle.png"));
        }
    }

    return   QMainWindow::eventFilter(obj,e);

}

void MainWindow::on_lastButton_clicked()
{
    SkipBackward();
}
void MainWindow::on_styleButton_clicked()
{

    QColor color;
    color = QColorDialog::getColor(Qt::white, this);
    if(color.isValid())     //判断是否有效颜色，判断用户是否直接关闭对话框
    {
        QPalette p;
        p.setBrush(this->backgroundRole(),QBrush(color));
        this->setPalette(p);
        this->setAutoFillBackground(true);
        //qDebug()<<color.red();
        //qDebug()<<color.green();
        //qDebug()<<color.blue();
    }

}
void MainWindow::on_playButton_clicked()
{
    //如果播放状态为暂停，则播放
    if(mediaObject->state() == Phonon::PlayingState)
    {

        mediaObject->pause();;

    }
    else
    {
        //判断资源是否为空
        if(sources.isEmpty()){
            QMessageBox::information(this, tr("Waring"), tr("列表空！"));
            return ;
        }

        mediaObject->play();
    }
}
void MainWindow::on_iconbutton_clicked()    //主窗体M图标点击响应
{
    initRightMenu();
}
void MainWindow::on_playmode_clicked()
{
    QCursor cur = this->cursor();
    QMenu *menu = new QMenu(this);

    QAction *sc = new QAction(tr("单曲循环"),this);    //单曲循环
    QAction *sp = new QAction(tr("单曲播放"),this);    //单曲播放
    QAction *lp = new QAction(tr("顺序播放"),this);    //顺序播放
    QAction *lc = new QAction(tr("列表循环"),this);    //列表循环


    menu->addAction(sc);
    menu->addAction(sp);
    menu->addAction(lp);
    menu->addAction(lc);

    connect(sc,SIGNAL(triggered()),this,SLOT(playmode_sc_slot()));
    connect(sp,SIGNAL(triggered()),this,SLOT(playmode_sp_slot()));
    connect(lp,SIGNAL(triggered()),this,SLOT(playmode_lp_slot()));
    connect(lc,SIGNAL(triggered()),this,SLOT(playmode_lc_slot()));

    //设置菜单样式
    menu->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                        );

    menu->exec(cur.pos());  //在鼠标当前位置显示菜单

}
void MainWindow::on_checkBox_list_stateChanged(int )  //扩展对话框，是否显示列表
{
    if(ui->checkBox_list->checkState()==Qt::Checked)
    {
        this->setFixedSize(this->width(),613);
        thelistopenflag = 1;
    }
    else
    {

        this->setFixedSize(this->width(),190);
        thelistopenflag = 0;
    }
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    //重写paintEvent，用在主窗体边框圆角
    QPainter painter(this);
    painter.setPen(Qt::black);  //边框色
    painter.drawRoundedRect(this->rect(), 5, 5); //圆角5像素
}
void MainWindow::initRightMenu()
{


    QCursor cur = this->cursor();
    rightmenu = new QMenu(this);
    QMenu *file = new QMenu(tr("文件"),this);
    QMenu *play = new QMenu(tr("播放"),this);
    QMenu *playmode_menu = new QMenu(tr("播放模式"),this);

    //QAction *load = new QAction(tr("登陆"),this);

    QAction *showlrc = new QAction(tr("显示桌面歌词"),this);
    QAction *skin = new QAction(tr("换肤"),this);
    QMenu *help = new QMenu(tr("帮助"),this);
    QAction *quit = new QAction(tr("退出"),this);
    QAction *load_file = new QAction(tr("添加本地歌曲..."),this);
    QAction *load_files = new QAction(tr("添加本地歌曲文件夹..."),this);
    QAction *play2 = new QAction(tr("播放/暂停"),this);
    QAction *last = new QAction(tr("上一首"),this);
    QAction *next = new QAction(tr("下一首"),this);
    QAction *addvol = new QAction(tr("增加音量"),this);
    QAction *dvol = new QAction(tr("减少音量"),this);
    QAction *novol = new QAction(tr("静音"),this);

    QAction *author = new QAction(tr("作者"),this);
    QAction *key    = new QAction(tr("快捷键"),this);

   // addvol->setShortcut(QKeySequence(Qt::Key_Up));
   // dvol->setShortcut(QKeySequence(Qt::Key_Down));
   // novol->setShortcut(QKeySequence("F6"));

    connect(addvol,SIGNAL(triggered()),this,SLOT(addvol_slot()));
    connect(dvol,SIGNAL(triggered()),this,SLOT(devvol_slot()));

    /* QActionGroup *skin_g = new QActionGroup(this);
    QActionGroup *playmode_g = new QActionGroup(this);
*/
    QAction *sc = new QAction(tr("单曲循环"),this);    //单曲循环
    QAction *sp = new QAction(tr("单曲播放"),this);    //单曲播放
    QAction *lp = new QAction(tr("顺序播放"),this);    //顺序播放
    QAction *lc = new QAction(tr("列表循环"),this);    //列表循环

    playmode_menu->addAction(sc);
    playmode_menu->addAction(sp);
    playmode_menu->addAction(lp);
    playmode_menu->addAction(lc);

    play->addAction(play2);
    play->addSeparator();
    play->addAction(last);
    play->addAction(next);
    play->addSeparator();
    play->addAction(addvol);
    play->addAction(dvol);
    play->addAction(novol);
    play->addSeparator();
    play->addMenu(playmode_menu);


    file->addAction(load_file);
    file->addAction(load_files);

    help->addAction(author);
    help->addAction(key);

    //rightmenu->addAction(load);
   // rightmenu->addSeparator();
    rightmenu->addMenu(file);
    rightmenu->addMenu(play);
    rightmenu->addSeparator();

    rightmenu->addAction(showlrc);
    rightmenu->addSeparator();
    rightmenu->addAction(skin);
    rightmenu->addSeparator();
    rightmenu->addMenu(help);
    rightmenu->addAction(quit);

    connect(load_file,SIGNAL(triggered()),this,SLOT(addfile()));
    connect(load_files,SIGNAL(triggered()),this,SLOT(addfile()));
    connect(skin,SIGNAL(triggered()),this,SLOT(on_styleButton_clicked()));
    connect(play2,SIGNAL(triggered()),this,SLOT(on_playButton_clicked()));
    connect(last,SIGNAL(triggered()),this,SLOT(on_lastButton_clicked()));
    connect(next,SIGNAL(triggered()),this,SLOT(on_nextButton_clicked()));
    connect(novol,SIGNAL(triggered()),this,SLOT(setSilent()));

    //connect(author,SIGNAL(triggered()),this,SLOT(showAuthor()));
    connect(key,SIGNAL(triggered()),this,SLOT(showKey()));

    help->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"
                        );
    rightmenu->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                             "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                             "QMenu::item:selected:!enabled{background:transparent;}"
                             "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                             );
    play->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                        );
    file->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                        );
    playmode_menu->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                                 "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                                 "QMenu::item:selected:!enabled{background:transparent;}"
                                 "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                                 );
    rightmenu->exec(cur.pos());

}
void MainWindow::contextMenuEvent(QContextMenuEvent *event) //主窗体右键菜单
{
    initRightMenu();
}

void MainWindow::addfile()      //添加文件
{

    QStringList files = QFileDialog::getOpenFileNames(this, tr("打开音乐文件"),
                        QCoreApplication::applicationDirPath(),tr("*.mp3;*.wma;*.wav"));


    if (files.isEmpty())
    {
        return;
    }
    int index = sources.size();
    foreach (QString string, files)
    {
        Phonon::MediaSource source(string);
        sources.append(source);

    }

    if(!sources.isEmpty())
    {
        //如果媒体源列表不为空，则将新加入的第一个媒体源作为当前媒体源
        metaInformationResolver->setCurrentSource(sources.at(index));
    }

}

//更新当前时间
void MainWindow::updateTime(qint64 time)
{
    //直接获取该音频文件的总时长参数，单位为毫秒
    qint64 total_time_value = mediaObject->totalTime();

    //这3个参数分别代表了时，分，秒；60000毫秒为1分钟，所以分钟第二个参数是先除6000,第3个参数是直接除1s
    QTime total_time(0, (total_time_value/60000)%60, (total_time_value/1000)%60);

    //参数time表示当前播放时间
    QTime cur_time(0,(time/60000)%60, (time/1000)%60);

    //格式
    QString total_time_str = total_time.toString("mm:ss");
    QString cur_time_str = cur_time.toString("mm:ss");

    //label设置
    ui->songtotaltime->setText(total_time_str);
    ui->songcurtime->setText(cur_time_str);

    // 获取当期时间对应的歌词
    if(!lrc_map.isEmpty()) {
        // 获取当前时间在歌词中的前后两个时间点
        qint64 previous = 0;
        qint64 later = 0;
        //keys()方法返回lrc_map列表
        foreach (qint64 value, lrc_map.keys()) {
            if (time >= value) {
                previous = value;
            } else {
                later = value;
                break;
            }
        }

        // 达到最后一行,将later设置为歌曲总时间的值
        if (later == 0)
            later = total_time_value;

        // 获取当前时间所对应的歌词内容
        QString current_lrc = lrc_map.value(previous);

//        // 没有内容时
//        if(current_lrc.length() < 2)
//            current_lrc = tr("简易音乐播放器");

        // 如果是新的一行歌词，那么重新开始显示歌词遮罩
        if(current_lrc != lrc->text()) {
            lrc->setText(current_lrc);
            //top_label->setText(current_lrc);
            qint64 interval_time = later - previous;
            lrc->start_lrc_mask(interval_time);
        }
    } else {  // 如果没有歌词文件，则在顶部标签中显示歌曲标题
        //top_label->setText(QFileInfo(mediaObject->
        //                            currentSource().fileName()).baseName());
    }
}
//静音
void MainWindow::setSilent()
{
    if(audioOutput->volume()==0)
    {
        audioOutput->setVolume(0.8);
    }
    else
    {
        audioOutput->setVolume(0);
    }
}

//当媒体播放快结束时，会发送aboutToFinish()信号，从而触发该槽函数
void MainWindow::AboutToFinish()
{
    int index = sources.indexOf(mediaObject->currentSource())+1;
    switch(playmode)
    {
    case 1:         //单曲播放
        mediaObject->stop();
        flag = 0;
        ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));
        playflag = 0;
        break;
    case 2:         //单曲循环
        mediaObject->enqueue(sources.at(index-1));        //添加下一首歌到列表
        mediaObject->seek(mediaObject->totalTime());      //跳到当前歌的最后
        // mediaObject->stop();
        mediaObject->setCurrentSource(sources.at(index-1));
        mediaObject->play();
        //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

        lrc->stop_lrc_mask();
        resolve_lrc(sources.at(index-1).fileName());
        break;
    case 3:             //顺序播放
        if(sources.size()>index)
        {
            mediaObject->enqueue(sources.at(index));        //添加下一首歌到列表
            mediaObject->seek(mediaObject->totalTime());      //跳到当前歌的最后
            // mediaObject->stop();
            mediaObject->setCurrentSource(sources.at(index));
            mediaObject->play();
            //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

            lrc->stop_lrc_mask();
            resolve_lrc(sources.at(index).fileName());
        }
        else
        {

            mediaObject->stop();        //如果已经是打开音频文件的最后一首歌了，就直接停止
            //相关的属性
            flag = 0;
            ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));
            playflag = 0;
        }
        break;
      case 4:
        if(sources.size()>index)
        {
            mediaObject->enqueue(sources.at(index));        //添加下一首歌到列表
            mediaObject->seek(mediaObject->totalTime());      //跳到当前歌的最后
            // mediaObject->stop();
            mediaObject->setCurrentSource(sources.at(index));
            mediaObject->play();
            //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

            lrc->stop_lrc_mask();
            resolve_lrc(sources.at(index).fileName());
        }
        else
        {

            mediaObject->enqueue(sources.at(0));        //添加下一首歌到列表
            mediaObject->seek(mediaObject->totalTime());      //跳到当前歌的最后
            // mediaObject->stop();
            mediaObject->setCurrentSource(sources.at(0));
            mediaObject->play();
            //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

            lrc->stop_lrc_mask();
            resolve_lrc(sources.at(0).fileName());
        }


    }



}
//上一首
void MainWindow::SkipBackward()
{
    lrc->stop_lrc_mask();
    int index = sources.indexOf(mediaObject->currentSource());
    if(index>0)
    {
        //mediaObject->stop();
        mediaObject->setCurrentSource(sources.at(index-1));
        mediaObject->play();
    }
}
//下一首
void MainWindow::SkipForward()
{
    lrc->stop_lrc_mask();
    int index = sources.indexOf(mediaObject->currentSource());
    if(sources.size()>index+1)
    {
        //mediaObject->stop();
        mediaObject->setCurrentSource(sources.at(index+1));
        mediaObject->play();
    }
}

void MainWindow::on_nextButton_clicked()
{
    SkipForward();
}
//媒体播放状态改变时响应
void MainWindow::StateChanged(Phonon::State new_state, Phonon::State old_state)
{
    QString num = tr("%1").arg(playsongNO+1);
    switch(new_state)
    {
    case Phonon::ErrorState:
        if(mediaObject->errorType() == Phonon::FatalError)
        {
            QMessageBox::warning(this, tr("致命错误"), mediaObject->errorString());//显示其错误
        }
        else
        {
            QMessageBox::warning(playlist, tr("错误"), mediaObject->errorString());//显示普通错误
        }

        break;
     case Phonon::PausedState:
          //设置按钮图片
          flag = 0;
          ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));

          playflag = 0;
          emit changethetraystate(Phonon::PausedState);
          ui->songnum->setText(tr("曲目: ")+num);
          if (!lrc_map.isEmpty()) {
              lrc->stop_lrc_mask();
              lrc->setText(ui->songnamelabel->text());
          }
          break;
    case Phonon::StoppedState:
          flag = 0;
          ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));

          playflag = 0;
          ui->songnamelabel->setText("");
          ui->songtotaltime->setText("00:00");
          emit re_tray_songnamelabel();
          emit changethetraystate(Phonon::StoppedState);
          ui->songnum->setText("");
          lrc->stop_lrc_mask();
          lrc->setText(tr("M音乐"));

          break;


    case Phonon::PlayingState:
        //设置按钮图片
        flag = 1;
        ui->playButton->setIcon(QIcon(QPixmap(":/images/play1.png")));
        ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

        playflag = 1;
        emit set_tray_songnamelabel(QFileInfo(mediaObject->currentSource().fileName()).baseName());
         emit changethetraystate(Phonon::PlayingState);

         ui->songnum->setText(tr("曲目: ")+num);
         resolve_lrc(mediaObject->currentSource().fileName());
        firsttime = 1;
        break;

    case Phonon::BufferingState:
            break;
        default:
        ;
    }
}
void MainWindow::MetaStateChanged(Phonon::State new_state, Phonon::State old_state)
{
    // 错误状态，则从媒体源列表中除去新添加的媒体源
    if(new_state == Phonon::ErrorState) {
        QMessageBox::warning(this, tr("打开文件时出错"), metaInformationResolver->errorString());
        //takeLast()为删除最后一行并将其返回
        while (!sources.isEmpty() &&
               !(sources.takeLast() == metaInformationResolver->currentSource()))
        {};//只留下最后一行
        return;
    }
    // 如果既不处于停止状态也不处于暂停状态，则直接返回
    if(new_state != Phonon::StoppedState && new_state != Phonon::PausedState)
        return;
    // 如果媒体源类型错误，则直接返回
    if(metaInformationResolver->currentSource().type() == Phonon::MediaSource::Invalid)
        return;

    QMap<QString, QString> meta_data = metaInformationResolver->metaData();//获取媒体源中的源数据

    //获取文件标题信息
    QString title  = meta_data.value("TITLE");
    //如果媒体元数据中没有标题信息，则去该音频文件的文件名为该标题信息
    if(title == "") {
    QString str = metaInformationResolver->currentSource().fileName();
    title = QFileInfo(str).baseName();
    }
    QTableWidgetItem *title_item = new QTableWidgetItem(title);
    title_item->setFlags(title_item->flags() ^ Qt::ItemIsEditable);

    //获取艺术家信息
    QTableWidgetItem *artist_item = new QTableWidgetItem(meta_data.value("ARTIST"));
    artist_item->setFlags(artist_item->flags() ^ Qt::ItemIsEditable);

    //获取总时间信息
    qint64 total_time  = metaInformationResolver->totalTime();
    QTime time(0, (total_time/60000)%60, (total_time/10000)%60);
    QTableWidgetItem *time_item = new QTableWidgetItem(time.toString("mm:ss"));
    time_item->setFlags(time_item->flags() ^ Qt::ItemIsEditable);

    //插入播放列表
    int current_rows = playlist->rowCount();//返回列表中的行数
    playlist->insertRow(current_rows);
    playlist->setItem(current_rows, 0, title_item);
    playlist->setItem(current_rows, 1, artist_item);
    playlist->setItem(current_rows, 2, time_item);

    //sources为打开的所以音频文件列表,playlist为音乐播放列表表格对象
    int index = sources.indexOf(metaInformationResolver->currentSource())+1;
    if(sources.size() > index) //没有解析完
        metaInformationResolver->setCurrentSource(sources.at(index));
    else {
        //没有被选中的行
        if(playlist->selectedItems().isEmpty()) {
            // 如果现在没有播放歌曲则设置第一个媒体源为媒体对象的当前媒体源
            //（因为可能正在播放歌曲时清空了播放列表，然后又添加了新的列表）
            if(mediaObject->state() != Phonon::PlayingState && mediaObject->state() != Phonon::PausedState)
                mediaObject->setCurrentSource(sources.at(0));
            else {
                //如果正在播放歌曲，则选中播放列表的第一个曲目,并更改图标状态
                playlist->selectRow(0);

            }
        }

    }
    QString total = tr("%1").arg(playlist->rowCount());

    ui->songtotal->setText("总曲目: "+total);

}
//源发生改变
void MainWindow::SourceChanged(const Phonon::MediaSource &source)
{
    int index = sources.indexOf(source);
    //playsongNO标记当前歌曲号
    playsongNO = index;
    //playlist->selectRow(index);

    //设置当前的行高亮，行宽变大
    for(int i = 0;i<3;i++)
    {
        playlist->item(songNo,i)->setBackgroundColor(QColor(255,255,255));
        //playlist->item(songNo,i)->setTextColor(QColor(0,0,0));
    }
    for(int i = 0;i<3;i++)
    {
        playlist->item(index,i)->setBackgroundColor(QColor(225, 225, 225));
       // playlist->item(index,i)->setTextColor(QColor(0,255,255));

    }

    playlist->setRowHeight(songNo,30);
    playlist->setRowHeight(index,playlist->rowHeight(index)+30);

    songNo = index;
    /*if(playlist->rowCount()!=0)
    {
        QString total = tr("%1").arg(playlist->rowCount());

        ui->songtotal->setText("总曲目: "+total);
    }
    else ui->songtotal->setText("");
*/
}
//清除源
void MainWindow::ClearSources()
{
    sources.clear();
    ui->songtotal->setText("");
}
//列表项双击
void MainWindow::TableClicked(QTableWidgetItem* item)
{

    int row = item->row();
    bool was_palying = mediaObject->state() == Phonon::PlayingState;
    if(mediaObject->state() == Phonon::StoppedState)
    {
        mediaObject->setCurrentSource(sources.at(row));
        mediaObject->play();
        return;
    }
        mediaObject->stop();//停止当前播放的歌曲
        mediaObject->clearQueue();//清楚播放队列

        //如果单就的播放列表行号比媒体源中列表行号还打，则直接返回
        if(row >= sources.size())
            return;
        mediaObject->setCurrentSource(sources.at(row));
        if(was_palying)//如果选中前在播放歌曲，那么选中后也继续播放歌曲
            mediaObject->play();
}

void MainWindow::traytochangvol(qreal vol)
{
    audioOutput->setVolume(vol);
}
void MainWindow::traytoclearlist_slot()
{
    while(playlist->rowCount())
        playlist->removeRow(0);
    ClearSources();
}

void MainWindow::volchange_slot(qreal qvol)
{
    emit vol_change_signal(qvol);
}
void MainWindow::addvol_slot()
{
    volumeSlider->setFocus();
    audioOutput->setVolume(audioOutput->volume()+0.1);
   // ui->songnamelabel->setText("UP");
}
void MainWindow::devvol_slot()
{
    volumeSlider->setFocus();   //让控件获取焦点
    audioOutput->setVolume(audioOutput->volume()-0.1);
   // ui->songnamelabel->setText("DOWN");
}
void MainWindow::showThisWindow()
{
    this->showNormal();
    this->raise();
}
void MainWindow::SetLrcShown()
{
    if(lrc->isHidden())
        lrc->show();
    else
        lrc->hide();
}
// 解析LRC歌词，在stateChanged()函数的Phonon::PlayingState处和aboutToFinish()函数中调用了该函数
void MainWindow::resolve_lrc(const QString &source_file_name)
{
    lrc_map.clear();
    if(source_file_name.isEmpty())
        return;
    QString file_name = source_file_name;
    QString lrc_file_name = file_name.remove(file_name.right(3)) + "lrc";//把音频文件的后缀改成lrc后缀

    // 打开歌词文件
    QFile file(lrc_file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        lrc->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName()
                     + tr(" --- 未找到歌词文件！"));
        return ;
    }
    // 设置字符串编码
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QString all_text = QString(file.readAll());
    file.close();
    // 将歌词按行分解为歌词列表
    QStringList lines = all_text.split("\n");

    //这个是时间标签的格式[00:05.54]
    //正则表达式d{2}表示匹配2个数字
    QRegExp rx("\\[\\d{2}:\\d{2}\\.\\d{2}\\]");
    foreach(QString oneline, lines) {
        QString temp = oneline;
        temp.replace(rx, "");//用空字符串替换正则表达式中所匹配的地方,这样就获得了歌词文本
        // 然后依次获取当前行中的所有时间标签，并分别与歌词文本存入QMap中
        //indexIn()为返回第一个匹配的位置，如果返回为-1，则表示没有匹配成功
        //正常情况下pos后面应该对应的是歌词文件
        int pos = rx.indexIn(oneline, 0);
        while (pos != -1) { //表示匹配成功
            QString cap = rx.cap(0);//返回第0个表达式匹配的内容
            // 将时间标签转换为时间数值，以毫秒为单位
            QRegExp regexp;
            regexp.setPattern("\\d{2}(?=:)");
            regexp.indexIn(cap);
            int minute = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\.)");
            regexp.indexIn(cap);
            int second = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\])");
            regexp.indexIn(cap);
            int millisecond = regexp.cap(0).toInt();
            qint64 totalTime = minute * 60000 + second * 1000 + millisecond * 10;
            // 插入到lrc_map中
            lrc_map.insert(totalTime, temp);
            pos += rx.matchedLength();
            pos = rx.indexIn(oneline, pos);//匹配全部
        }
    }
    // 如果lrc_map为空
    if (lrc_map.isEmpty()) {
        lrc->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName()
                     + tr(" --- 歌词文件内容错误！"));
        return;
    }
}



void MainWindow::on_checkBox_lrc_stateChanged(int )
{
    this->SetLrcShown();
}

void MainWindow::playmode_sc_slot()
{
    playmode = 2;
    ui->playmode->setText("单曲循环");
}
void MainWindow::playmode_sp_slot()
{
    playmode = 1;
    ui->playmode->setText("单曲播放");
}
void MainWindow::playmode_lp_slot()
{
    playmode = 3;
    ui->playmode->setText("顺序播放");
}
void MainWindow::playmode_lc_slot()
{
    playmode = 4;
    ui->playmode->setText("列表循环");
}

void MainWindow::on_toolButton_clicked()
{
    this->addfile();
}

void MainWindow::on_toolButton_2_clicked()
{
    this->on_playmode_clicked();
}
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Up:
        addvol_slot();
        break;
    case Qt::Key_Down:
        devvol_slot();
        break;

    }
}
void MainWindow::showAuthor()
{
    QString s;
    s += "梁增国\n";
    s += "曾依婷\n";
    QMessageBox::information(NULL, QString("作者"),s);
}
void MainWindow::showKey()
{
    QString key ;
    key+="播放/暂停   空格\n";
    key+="下一首      →\n";
    key+="上一首      ←\n";
    key+="打开文件    ctrl+o\n";
    key+="音量增加    ↑\n";
    key+="音量减少    ↓\n\n\n";
    key+="把文件拖动到软件上能加入文件！\n";
    QMessageBox::information(NULL, QString("快捷键"),key);
}
void MainWindow::writefile()
{
    QFile file ("QTDATA.dat");
    file.resize(0);                 //写入前先清空文件内容
    QString data;
    QTextStream stream(&file);

    //写入背景颜色
    QPalette pal = this->palette();
    QBrush brush = pal.background();
    QColor col   = brush.color();

    data = QString::number(col.red(),10)+"\n";
    data += QString::number(col.green(),10)+"\n";
    data += QString::number(col.blue(),10)+"\n";

    //写入当前音量和播放模式
    data += QString::number(audioOutput->volume(),'f',1)+"\n";
    data += QString::number(playmode,10)+"\n";
    data += QString::number(thelistopenflag,10)+"\n";
    //写入载入歌曲的路径
    if(!sources.isEmpty())
    {
        for(int i = 0;i<sources.size();i++)
        {
            data += sources.at(i).url().toString().replace("file:///","")+"\n";
        }
    }
    file.open( QIODevice::WriteOnly| QIODevice::Text );

    stream<<data;
    file.close();
}
void MainWindow::readfile()
{
    QFile file ("QTDATA.dat");
    QString data;
    QTextStream stream(&file);
    int r,g,b,index = 0;
    double vol;

    if(!file.exists())
    {
        file.open( QIODevice::ReadWrite| QIODevice::Text );
    }
    else
    {
        file.open( QIODevice::ReadWrite| QIODevice::Text );
        while(!stream.atEnd())
        {
            data = stream.readLine();

            switch(index)
            {
            case 0:
                r = data.toInt();
                break;
            case 1:
                g = data.toInt();
                break;
            case 2:
                b = data.toInt();
                break;
            case 3:
                vol = data.toDouble();
                break;
            case 4:
                playmode = data.toInt();
                break;
            case 5:
                thelistopenflag = data.toInt();
                break;
            default:
                Phonon::MediaSource source(data);
                sources.append(source);
                if(!sources.isEmpty())
                {
                    metaInformationResolver->setCurrentSource(sources.at(0));
                }

            }
            index++;
        }

        file.close();


        QPalette p;
        p.setBrush(this->backgroundRole(),QBrush(QColor(r,g,b,255)));
        this->setPalette(p);
        this->setAutoFillBackground(true);

        setPlaymodebuttontext(playmode);
        setWindowHeight(thelistopenflag);
        audioOutput->setVolume(vol);
    }
}
void MainWindow::setPlaymodebuttontext(int mode)
{
    switch(mode)
    {
    case 1:
        ui->playmode->setText("单曲播放");
        break;
    case 2:
        ui->playmode->setText("单曲循环");
        break;
    case 3:
        ui->playmode->setText("顺序播放");
        break;
    case 4:
        ui->playmode->setText("列表循环");
        break;
    }


}
void MainWindow::setWindowHeight(int flag)
{
    if(flag == 1)
    {

        ui->checkBox_list->setChecked(true);
    }
    else
    {

        ui->checkBox_list->setChecked(false);
    }
}
