#include "mediaplayerdialog.h"
#include "ui_mediaplayerdialog.h"
#include "videoplayer.h"
#include<QFileDialog>
//#define _def_path "E:/BaiduNetdiskDownload/17.mp4"
//#define _DEF_PATH "rtmp://192.168.134.138:1935/vod//yxlm.mp4"
#define _DEF_PATH "http://111.40.196.9/PLTV/88888888/224/3221225628/index.m3u8"
#define _DEF_LIVE_PATH "rtmp://192.168.134.138:1935/videotest/user=100"
MediaPlayerDialog::MediaPlayerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MediaPlayerDialog)
{
    ui->setupUi(this);
    m_player = new VideoPlayer;

    m_kernel = new CKernel;

    //m_movielb = new QMymovieLabel;
    connect(m_kernel,SIGNAL(SIG_openKernel(QString)),this,SLOT(slot_OpenUrl(QString)));
    connect(m_player,SIGNAL(SIG_getOneImage(QImage)),this,SLOT(slot_setImage(QImage)));
    slot_PlayerStateChanged(VideoPlayer::PlayerState::Stop);
    connect( m_player, SIGNAL( SIG_PlayerStateChanged(int)) ,
     this ,SLOT(slot_PlayerStateChanged(int)) );
    //测试
    //m_player->setFileName(_def_path);
    //connect(m_timer,SIGNAL(timeout()),this,SLOT());
    connect( m_player,SIGNAL( SIG_TotalTime(qint64)),this,SLOT(slot_getTotalTime(qint64)),/*Qt::BlockingQueuedConnection原来*/Qt::QueuedConnection);
    //qDebug()<<"TotalTime connect:"<<tmp;
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(slot_TimerTimeOut()));
    m_timer.setInterval(500);//超时时间500ms
    //安装事件过滤器，让该对象成功被观察  //this 去执行函数
    ui->slider_progress->installEventFilter( this );
}

MediaPlayerDialog::~MediaPlayerDialog()
{
    delete ui;
    if(m_player)
    {
        delete m_player;
        m_player = nullptr;
    }
    if(m_kernel)
    {
        delete m_kernel;
        m_kernel = nullptr;
    }
//    if(m_movielb)
//    {
//        delete m_movielb;
//        m_movielb = nullptr;
//    }
}

//获取视频全部时间
void MediaPlayerDialog::slot_getTotalTime(qint64 uSec)
{
    qint64 Sec = uSec/1000000;
    ui->slider_progress->setRange(0,Sec);//精确到秒
    QString hStr = QString("00%1").arg(Sec/3600);
    QString mStr = QString("00%1").arg(Sec/60%60);
    QString sStr = QString("00%1").arg(Sec%60);
    QString str = QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
    ui->lb_totalTime->setText(str);
    qDebug()<<"setTotalTime";
}

//QT线程
//QThread 定义子类 start（） -> run（）;
void MediaPlayerDialog::on_pb_start_clicked()
{
    //开始播放 -> 一段时间内 获取图片
    //m_player->start();

    //首先 要先关闭 判断 当前的状态 stop
    if(m_player->playerState() !=
            VideoPlayer::PlayerState::Stop){
        m_player->stop(true);
    }
    //打开游览选择文件
    QString path = QFileDialog::getOpenFileName(this,"打开文件","./"
        ,"视频文件 (*.flv *.rmvb *.avi *.MP4 *.mkv);; 所有文件(*.*);;");

    //判断
    if(path.isEmpty())return;
    //设置 m_play filename
    m_player->setFileName(path);
//    m_player->setFileName(_DEF_PATH);
    //m_player->setFileName(_DEF_LIVE_PATH);
    //play
//    int total_time = m_player->getTotalTime();
//    slot_getTotalTime(total_time);

    slot_PlayerStateChanged(VideoPlayer::PlayerState::Playing);

}

void MediaPlayerDialog::slot_setImage(QImage img)
{
    //pixmap image
    //缩放
//    QPixmap pixmap;
//    if(!img.isNull())
//        pixmap = QPixmap::fromImage(img.scaled(ui->lb_show->size(),Qt::KeepAspectRatio));
//    else
//        pixmap = QPixmap::fromImage(img);

    //视频加速渲染 OpenGL
    //ui->lb_show->setPixmap(pixmap);
    ui->wdg_show->slot_setImage(img);
}


void MediaPlayerDialog::on_pb_resume_clicked()
{
    if(m_player->playerState() != VideoPlayer::PlayerState::Pause)return;

    m_player->play();

    //切换
    ui->pb_resume->hide();
    ui->pb_pause->show();
}


void MediaPlayerDialog::on_pb_pause_clicked()
{
    if(m_player->playerState() != VideoPlayer::PlayerState::Playing)return;

    m_player->pause();

    //切换
    ui->pb_resume->show();
    ui->pb_pause->hide();
}




void MediaPlayerDialog::on_pb_stop_clicked()
{
    m_player->stop(true);
}

//播放状态切换槽函数
void MediaPlayerDialog::slot_PlayerStateChanged(int state)
{
    switch( state )
    {
    case VideoPlayer::PlayerState::Stop:
        qDebug()<< "VideoPlayer::Stop";
        m_timer.stop();
        ui->slider_progress->setValue(0);
        ui->lb_totalTime->setText("00:00:00");
        ui->lb_curTime->setText("00:00:00");
        //ui->lb_curName->setText("");

        ui->pb_pause->hide();
        ui->pb_resume->show();
//    {
//        QImage img;
//        img.fill( Qt::black);
//        slot_setImage( img );
//    }
        this->update();
        isStop = true;
        break;
    case VideoPlayer::PlayerState::Playing:
        qDebug()<< "VideoPlayer::Playing";
        ui->pb_resume->hide();
        ui->pb_pause->show();
        m_timer.start();
        this->update();
        isStop = false;
        break;
    }
}

//获取当前视频时间定时器
void MediaPlayerDialog::slot_TimerTimeOut()
{
    if (QObject::sender() == &m_timer)
    {
        qint64 Sec = m_player->getCurrentTime()/1000000;
        ui->slider_progress->setValue(Sec);
        QString hStr = QString("00%1").arg(Sec/3600);
        QString mStr = QString("00%1").arg(Sec/60%60);
        QString sStr = QString("00%1").arg(Sec%60);
        QString str = QString("%1:%2:%3").arg(hStr.right(2)).arg(mStr.right(2)).arg(sStr.right(2));
        ui->lb_curTime->setText(str);
        if(ui->slider_progress->value() == ui->slider_progress->maximum()
                && m_player->playerState() == VideoPlayer::PlayerState::Stop)
        {
            slot_PlayerStateChanged( VideoPlayer::PlayerState::Stop );
        }else if(ui->slider_progress->value() + 1 ==
                 ui->slider_progress->maximum()
                 && m_player->playerState() == VideoPlayer::PlayerState::Stop)
        {
            slot_PlayerStateChanged( VideoPlayer::PlayerState::Stop );
        }
    }
}
#include <QMouseEvent>
#include <QStyle>
bool MediaPlayerDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->slider_progress) {
             if (event->type() == QEvent::MouseButtonPress) {
                 QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                 int min = ui->slider_progress->minimum();
                 int max = ui->slider_progress->maximum();
                 int value = QStyle::sliderValueFromPosition(
                 min , max , mouseEvent->pos().x(), ui->slider_progress->width());

                 ui->slider_progress->setValue(value);
                 m_player->seek((qint64)value*1000000); // value 秒
                 return true;
             } else {
                 return false;
             }
         }
    //空格 暂停/回复 左右 退回/快进 上下 音量调整
    //pass the event on to the parent calss
    return QDialog::eventFilter(obj,event);
}

void MediaPlayerDialog::on_pb_online_clicked()
{
    m_kernel->m_pLoginDlg->show();

}

void MediaPlayerDialog::slot_OpenUrl(QString url)
{
    //首先 要先关闭 判断 当前的状态 stop
    if(m_player->playerState() !=
            VideoPlayer::PlayerState::Stop){
        m_player->stop(true);
    }
    //打开游览选择文件
    QString path = url;

    qDebug()<<"url: "<<url;
    //判断
    if(path.isEmpty())return;
    //设置 m_play filename
    m_player->setFileName(path);

    slot_PlayerStateChanged(VideoPlayer::PlayerState::Playing);
}

