//自定义系统托盘类
#ifndef MYSYSTEMTRAY_H
#define MYSYSTEMTRAY_H

#include <QtGui>
#include <phonon>

class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT
public:

    explicit SystemTray(QWidget *parent = 0);
    ~SystemTray();
    void translateLanguage();
    void createActions();
    void createPlayAction();
    void addActions();

private:
    QMenu *pop_menu;                    //菜单
    QAction *show_lrc_action;           //显示歌词
    QAction *show_action ;              //显示主窗口
    QAction *addfile_action;            //添加文件
    QAction *clearlist_action;          //清空列表
    QAction *quit_action;               //退出
    QWidget *play_widget;
    QWidgetAction *play_widget_action ; //播放按钮的widget
    QWidget *vol_widget;
    QWidgetAction *vol_slider_widgetAction;   //音量滑条的widget

    QPushButton *play_button ;          //播放按钮
    QPushButton *backward_button ;      //上一首
    QPushButton *forward_button ;       //下一首
    QLabel *music_name_label ;          //当前歌曲名字
    QSlider *vol_slider;                //音量滑条
    QToolButton *vol_button;            //音量按钮

    int no_vol_flag;                    //标记当前是否静音


signals:
    void show_action_signal();
    void play_signal();
    void backward_signal();
    void forward_signal();
    void show_lrc_signal();
    void quit_signal();
    void no_vol_signal();
    void changevol_signal(qreal vol);
    void addfile_signal();
    void clearlist_signal();
  //  void showMain_signal();

public slots:
    void show_action_slot();
    void play_slot();
    void backward_slot();
    void forward_slot();
    void show_lrc_slot();
    void quit_slot();
    void set_song_name(QString s);
    void re_song_name();
    void no_vol_slots();
    void changevol_slot(int vol);
    void changevolSlider_slot(qreal vol);
    void addfile_slot();
    void clearlist_slot();
    void state(Phonon::State state);
    //void showMain_slot();

};

#endif // MYSYSTEMTRAY_H
