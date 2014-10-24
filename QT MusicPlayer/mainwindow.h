#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <phonon>
#include "myplaylist.h"
#include "MySystemTray.h"

class MyLrc;
class QPoint;
class QToolButton;
class QSystemTrayIcon;
class QAction;
class QMenu;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //鼠标响应事件
    void mousePressEvent(QMouseEvent *event);           //鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *event);         //鼠标释放事件
    void mouseMoveEvent(QMouseEvent *event);            //鼠标移动事件

    void MyWindowStyle();                               //主窗体相关属性设置
    void MyWindowButtonStyle();                         //主窗体按钮属性设置
    void MyTrayIcon();                                  //系统托盘设置

    void initTableWidget();
    void paintEvent(QPaintEvent *e);
    void initRightMenu();                                           //初始化右键菜单
    void contextMenuEvent(QContextMenuEvent *event);                //右键菜单

    void keyPressEvent(QKeyEvent *e);
    void initEventFilter();                                         //初始化事件过滤器
    bool eventFilter(QObject *obj, QEvent *e);                      //事件过滤器实现

    void initMusic();                                               //初始化音频信息
    void initFlag();                                                //初始化标记

    void SkipBackward();                                            //上一首
    void SkipForward();                                             //下一首

    void readfile();
    void writefile();

    void setPlaymodebuttontext(int mode);
    void setWindowHeight(int flag);
signals:
    void set_tray_songnamelabel(QString s);
    void re_tray_songnamelabel();
    void vol_change_signal(qreal vol);
    void changethetraystate(Phonon::State new_state);





private:
    Ui::MainWindow *ui;

    QPoint move_point;                  //鼠标移动的距离
    bool mouse_press;                   //鼠标是否按下

    QSystemTrayIcon *trayIcon;          //系统托盘

    //QAction *loadAction;                //系统托盘登陆项
    QAction *quitAction;                //系统托盘退出项
    QMenu   *trayIconMenu;              //系统托盘菜单
    QMenu   *rightmenu;                 //右键菜单
    MyPlaylist *playlist;
    SystemTray *mytrayIcon;
    int flag;                           //标记当前播放图标状态
    int playflag ;                      //标记当前播放状态
    qreal vol;

    int songNo;
    int playsongNO;
    int playmode;                       //标记播放模式
    int firsttime;                      //标记第一次启动
    int thelistopenflag;

    MyLrc *lrc;
    QMap<qint64, QString> lrc_map;
    void resolve_lrc(const QString &source_file_name);

    //媒体类
    Phonon::SeekSlider *seekSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;
    Phonon::VolumeSlider *volumeSlider;
    Phonon::AudioOutput *metaInformationResolveraudioOutput;
    QList<Phonon::MediaSource> sources;

private slots:



    void on_toolButton_2_clicked();
    void on_toolButton_clicked();
    void on_checkBox_lrc_stateChanged(int );
    void on_checkBox_list_stateChanged(int );
   // void on_checkBox_list_clicked();
    void on_nextButton_clicked();
  //  void on_checkBox_list_stateChanged(int );
    void on_iconbutton_clicked();

    void on_playmode_clicked();
    void on_playButton_clicked();
    void on_styleButton_clicked();
    void on_lastButton_clicked();
    void myclose();
    void addfile();                                                 //添加音乐文件


    void updateTime(qint64 time);

    void setSilent();                                               //静音
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);   //托盘点击信息
    void AboutToFinish();
    void StateChanged(Phonon::State new_state, Phonon::State old_state);
    void MetaStateChanged(Phonon::State new_state, Phonon::State old_state);
    void SourceChanged(const Phonon::MediaSource &source);
    void ClearSources();
    void TableClicked(QTableWidgetItem* item);
    void traytochangvol(qreal vol);
    void volchange_slot(qreal qvol);
    void traytoclearlist_slot();

    void addvol_slot();
    void devvol_slot();
    void showThisWindow();

    void SetLrcShown();

    void playmode_sc_slot();
    void playmode_sp_slot();
    void playmode_lp_slot();
    void playmode_lc_slot();

    void showAuthor();
    void showKey();

};

#endif // MAINWINDOW_H
