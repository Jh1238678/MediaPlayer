#ifndef MEDIAPLAYERDIALOG_H
#define MEDIAPLAYERDIALOG_H

#include <QDialog>
#include<QTimer>
#include<QDebug>
#include "videoplayer.h"
#include"ckernel.h"
#include"qmymovielabel.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MediaPlayerDialog; }
QT_END_NAMESPACE

class MediaPlayerDialog : public QDialog
{
    Q_OBJECT

public:
    MediaPlayerDialog(QWidget *parent = nullptr);
    ~MediaPlayerDialog();

private slots:
    void on_pb_start_clicked();

    void slot_setImage(QImage);
    void on_pb_resume_clicked();

    void on_pb_pause_clicked();
    void slot_PlayerStateChanged(int state);
    void on_pb_stop_clicked();
    void slot_TimerTimeOut();
    void slot_getTotalTime(qint64 uSec);
    void on_pb_online_clicked();

    //播放url
    void slot_OpenUrl(QString url);
private:
    Ui::MediaPlayerDialog *ui;
    VideoPlayer * m_player;

    QTimer m_timer;
    CKernel* m_kernel;
    //QMymovieLabel* m_movielb;

public:
    bool isStop;//代表当前是否是停止状态
    bool eventFilter(QObject *obj,QEvent *event);

};
#endif // MEDIAPLAYERDIALOG_H
