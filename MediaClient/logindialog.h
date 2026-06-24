#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include"packdef.h"
#include"uploaddialog.h"
#include<QThread>
namespace Ui {
class loginDialog;
}

class UploadWorker:public QObject
{
    Q_OBJECT
public slots:
    void slot_UploadFile(QString filePath,QString imgPath,Hobby hy);
};

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();

    void setLoginInfo(char* data,int len,unsigned long from);


    void UploadFile(QString filePath,Hobby hy,QString gifName="");
signals:
    void sig_openRegister();
    void SIG_sendBlock(char* data,int len,unsigned long from);
    void SIG_UploadRq(char* data,int len,unsigned long from);
    void SIG_Fresh();
    //播放信号传递
    void SIG_openLogin(QString url);
public slots:
    void slot_UploadFile(QString filePath,QString imgPath,Hobby hy);
    void slot_fileBlockRq(char *buf,QMap<int,FileInfo*>& m_mapVideoIdToFileInfo);
private slots:
    void on_pb_login_clicked();

    void on_pb_upload_clicked();

    void on_pb_hasupload_clicked();
    void on_pb_fresh_clicked();

private:
    Ui::loginDialog *ui;
public:
    UpLoadDialog* m_uploadDlg;
    int m_id;

    QThread * m_uploadThread;
    UploadWorker* m_uploadWorker;
    static loginDialog* m_login;
};

#endif // LOGINDIALOG_H
