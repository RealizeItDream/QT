#ifndef MYPLAYLIST_H
#define MYPLAYLIST_H

#include <QTableWidget>

class MyPlaylist : public QTableWidget
{
    Q_OBJECT
public:
    explicit MyPlaylist(QWidget *parent = 0);


signals:
    void play_list_clean();
    void add_File_signal();

public slots:


protected:
    void contextMenuEvent(QContextMenuEvent *);
    void closeEvent(QCloseEvent *);

private slots:
    void clear_play_list();
    void add_File_slot();

};
#endif // MYPLAYLIST_H
