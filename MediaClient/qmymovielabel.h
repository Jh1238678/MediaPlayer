#ifndef QMYMOVIELABEL_H
#define QMYMOVIELABEL_H

#include <QWidget>
#include<QMovie>
#include<QEvent>
#include<QLabel>
namespace Ui {
class QMymovieLabel;
}

class QMymovieLabel : public QWidget
{
    Q_OBJECT

public:
    explicit QMymovieLabel(QWidget *parent = nullptr);
    ~QMymovieLabel();
    QMovie *movie() const;

signals:
    void SIG_labelClicked(QString url);
public slots:
    void setMovie(QMovie* movie);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void setRtmpUrl(QString url);
    virtual bool eventFilter(QObject* watched,QEvent* event);
private:
    Ui::QMymovieLabel *ui;
    QMovie* m_movie;
    QString m_rtmpUrl;
};

#endif // QMYMOVIELABEL_H
