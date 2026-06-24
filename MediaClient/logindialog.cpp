#include "logindialog.h"
#include "ui_logindialog.h"
#include"packdef.h"
#include<QDebug>
#include<QMessageBox>
#include<QTime>
loginDialog* loginDialog::m_login = nullptr;
loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    m_id = 0;
    qRegisterMetaType<Hobby>("Hobby");
    m_uploadDlg = new UpLoadDialog;
    qsrand(QTime(0,0,0).msecsTo(QTime::currentTime()));

    m_login = this;
    m_uploadWorker = new UploadWorker;
    m_uploadThread = new QThread;
    connect(m_uploadDlg,&UpLoadDialog::SIG_UploadFile,
            m_uploadWorker,&UploadWorker::slot_UploadFile);

    connect(ui->pb_play1,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play2,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play3,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play4,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play5,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play6,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play7,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play8,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play9,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play10,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play11,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    connect(ui->pb_play12,&QMymovieLabel::SIG_labelClicked,
            this,&loginDialog::SIG_openLogin);
    m_uploadWorker->moveToThread(m_uploadThread);
    m_uploadThread->start();
}

loginDialog::~loginDialog()
{
    delete ui;
    if(m_uploadDlg)
    {
        delete  m_uploadDlg;
        m_uploadDlg = nullptr;
    }
    if(m_uploadWorker)
    {
        delete  m_uploadWorker;
        m_uploadWorker = nullptr;
    }
    if(m_uploadThread)
    {
        m_uploadThread->quit();
        m_uploadThread->wait(100);
        if(m_uploadThread->isRunning())
        {
            m_uploadThread->terminate();
            m_uploadThread->wait(100);
        }
        delete m_uploadThread;
        m_uploadThread = nullptr;
    }
}

void loginDialog::setLoginInfo(char *data, int len, unsigned long from)
{
    STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)data;
    ui->lb_welcom->setText(QString("您好，%1").arg(rs->name));
}
//上传文件响应
void loginDialog::slot_UploadFile(QString filePath, QString imgPath, Hobby hy)
{
    //上传
    qDebug()<<"上传开始";

    UploadFile(imgPath,hy);
    UploadFile(filePath,hy,imgPath);
}

void loginDialog::slot_fileBlockRq(char *buf,QMap<int,FileInfo*>& m_mapVideoIdToFileInfo)
{
    STRU_FILEBLOCK_RQ *rq = (STRU_FILEBLOCK_RQ*)buf;
    auto ite = m_mapVideoIdToFileInfo.find(rq->m_nFileId);
    if(ite == m_mapVideoIdToFileInfo.end())return;

    FileInfo* info = m_mapVideoIdToFileInfo[rq->m_nFileId];

    int64_t res = info->pFile->write(rq->m_szFileContent,rq->m_nBlockLen);
    info->filePos += res;

    if(info->filePos >= info->fileSize)
    {
        //关闭文件
        info->pFile->close();
        //删除该节点
        m_mapVideoIdToFileInfo.erase(ite);
        //设置到控件
        QString pbNum = QString("pb_play%1").arg(info->fileId);
        QMymovieLabel* pb_play = ui->widget->findChild<QMymovieLabel*>(pbNum);

        QMovie* LastMovie = pb_play->movie();
        if(LastMovie && LastMovie->isValid())
        {
            delete LastMovie;
        }
        QMovie * movie = new QMovie(info->filePath);
        pb_play->setMovie(movie);
        pb_play->setRtmpUrl(info->rtmpUrl);
        //回收info
        delete info;
        info = nullptr;
    }
}
#include<QFileInfo>
//上传文件
void loginDialog::UploadFile(QString filePath, Hobby hy,QString gifName)
{
    //兼容中文
    QFileInfo info(filePath);
    std::string strName = info.fileName().toStdString();
    const char* file_name = strName.c_str();


    STRU_USER_UPLOAD_RQ rq;
    rq.m_nFileId = qrand()%10000;
    rq.m_nFileSize = info.size();
    strcpy_s(rq.m_szFileName,_MAX_PATH,file_name);

    QByteArray bt = filePath.right(filePath.length()-filePath.lastIndexOf('.')-1).toLatin1();

    memcpy(rq.m_szFileType,bt.data(),bt.length());

    if(!gifName.isEmpty())
    {
        QFileInfo info(gifName);
        strcpy_s(rq.m_szGifName,_MAX_PATH,info.fileName().toLocal8Bit().data());
    }
    memcpy(rq.m_szHobby,&hy,sizeof (hy));
    rq.m_UserId = m_id;

    Q_EMIT SIG_UploadRq((char*)&rq,sizeof(rq),8000);

    FileInfo fi;
    fi.fileId = rq.m_nFileId;
    fi.fileName = rq.m_szFileName;
    fi.filePath = filePath;
    fi.filePos = 0;
    fi.fileSize = rq.m_nFileSize;
    fi.pFile = new QFile(filePath);

    if(fi.pFile->open(QIODevice::ReadOnly))
    {
        while(1)
        {
            STRU_FILEBLOCK_RQ blockrq;
            int64_t res = fi.pFile->read(blockrq.m_szFileContent,_DEF_CONTENT_SIZE);
            fi.filePos += res;
            blockrq.m_nBlockLen = res;
            blockrq.m_nFileId = rq.m_nFileId;
            blockrq.m_nUserId = m_id;

            qDebug()<<"m_nBlockLen: "<<blockrq.m_nBlockLen;
            Q_EMIT SIG_sendBlock((char*)&blockrq,sizeof (blockrq),8000);
            //m_tcp->SendData((char*)&blockrq,sizeof (blockrq));
            av_usleep(1000);
            Q_EMIT m_uploadDlg->SIG_updataProcess(fi.filePos,fi.fileSize);

            if(fi.filePos >= fi.fileSize)
            {
                //av_usleep(1000);
                fi.pFile->close();
                delete fi.pFile;
                fi.pFile = NULL;
                break;
            }
        }
    }

}

void loginDialog::on_pb_login_clicked()
{
    Q_EMIT sig_openRegister();
}


void loginDialog::on_pb_upload_clicked()
{
    if(m_id == 0)
    {
        QMessageBox::about(this,"提示","先登录！");
        return;
    }
    m_uploadDlg->clear();
    m_uploadDlg->show();
}


void loginDialog::on_pb_hasupload_clicked()
{

}


void UploadWorker::slot_UploadFile(QString filePath, QString imgPath, Hobby hy)
{
    loginDialog::m_login->slot_UploadFile(filePath,imgPath,hy);
}

void loginDialog::on_pb_fresh_clicked()
{
    if(!m_id)
    {
        QMessageBox::about(this,"提示","先登录");
        return;
    }

    Q_EMIT SIG_Fresh();

}

