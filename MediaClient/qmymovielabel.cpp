#include "qmymovielabel.h"
#include "ui_qmymovielabel.h"
#include<QDebug>
QMymovieLabel::QMymovieLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QMymovieLabel)
{
    ui->setupUi(this);
    m_movie = nullptr;
    m_rtmpUrl = "";
    //安装事件监听器，方便检测鼠标的双击
    ui->lb_movie->installEventFilter(this);
}

QMymovieLabel::~QMymovieLabel()
{
    delete ui;
}
//设置动画
void QMymovieLabel::setMovie(QMovie *movie)
{
    m_movie = movie;
    ui->lb_movie->setMovie(movie);
    movie->start();
    movie->stop();
}
//鼠标移入
void QMymovieLabel::enterEvent(QEvent *event)
{
    if(m_movie)
    {
        m_movie->start();
    }
}
//鼠标移出
void QMymovieLabel::leaveEvent(QEvent *event)
{
    if(m_movie)
    {
        m_movie->stop();
    }
}
//设置控件播放的url
void QMymovieLabel::setRtmpUrl(QString url)
{
    m_rtmpUrl = url;
}
//事件过滤处理
bool QMymovieLabel::eventFilter(QObject *watched, QEvent *event)
{
    //动画点击
    if(watched == ui->lb_movie && event->type() == QEvent::MouseButtonPress)
    {
        qDebug()<<"mouse Press";
        //qDebug()<<"rtmpUrl: "<<m_rtmpUrl;
        Q_EMIT SIG_labelClicked(m_rtmpUrl);
        return true;
    }
    return false;
}

QMovie *QMymovieLabel::movie() const
{
    return m_movie;
}
