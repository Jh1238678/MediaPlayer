#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H
#include "INetMediator.h"
#include "TcpClientMediator.h"
#include <QDialog>
#include<QObject>
#include "videoplayer.h"
#include"packdef.h"
#include"imageWidget.h"
namespace Ui {
class UpLoadDialog;
}

class UpLoadDialog : public QDialog
{
    Q_OBJECT

signals:
    void SIG_GetOnePicture(QImage);
    void SIG_UploadFile(QString filePath,QString imgPath,Hobby hy);
    void SIG_updataProcess(qint64,qint64);
public:
    explicit UpLoadDialog(QWidget *parent = nullptr);
    ~UpLoadDialog();

    void clear();
    QString SaveVideoJpg(QString FilePath);

private slots:
    void on_pb_view_clicked();

    void on_pb_beginupload_clicked();
public slots:
    void slot_updataProcess(qint64 cur,qint64 max);

private:
    Ui::UpLoadDialog *ui;
public:
    //INetMediator* m_tcpclient;
    VideoPlayer* m_player;

    QString filePath;
    QString imgPath;
};


#endif // UPLOADDIALOG_H
