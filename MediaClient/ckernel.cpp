#include "ckernel.h"
#include"TcpClientMediator.h"
#include<QDebug>
#include<QMessageBox>
#include<QDir>
#include"qmymovielabel.h"
CKernel::CKernel(QObject *parent) : QObject(parent),m_id(0)
{
    setProtocolNum();

    m_pLoginDlg = new loginDialog;
    m_regisDialog = new registerDialog;
    m_pMediator = new TcpClientMediator;
    connect(m_regisDialog,&registerDialog::sig_registerData,
            this,&CKernel::slot_registerData);
    connect(m_regisDialog,&registerDialog::sig_loginData,
            this,&CKernel::slot_loginData);

    connect(m_pLoginDlg,&loginDialog::sig_openRegister,
            this,&CKernel::slot_openRegister);
    connect(m_pMediator,&TcpClientMediator::SIG_ReadyData
            ,this,&CKernel::slot_dealData);
    connect(m_pLoginDlg,&loginDialog::SIG_sendBlock,
            this,&CKernel::slot_sendBlock);
    connect(m_pLoginDlg,&loginDialog::SIG_UploadRq,
            this,&CKernel::slot_sendUploadRq);
    connect(this,&CKernel::SIG_fileBlockRq,
            m_pLoginDlg,&loginDialog::slot_fileBlockRq);
    connect(m_pLoginDlg,&loginDialog::SIG_Fresh,
            this,&CKernel::slot_fresh);

    connect(m_pLoginDlg,&loginDialog::SIG_openLogin,
            this,&CKernel::SIG_openKernel);

    if(!m_pMediator->OpenNet("192.168.134.138",8000))
    {
        QMessageBox::about(m_pLoginDlg,"提示","打开网络失败");
        exit(1);
    }
}

CKernel::~CKernel()
{
    if(m_pLoginDlg)
    {
        m_pLoginDlg->hide();
        delete m_pLoginDlg;
        m_pLoginDlg = nullptr;
    }
    if(m_regisDialog)
    {
        m_regisDialog->hide();
        delete m_regisDialog;
        m_regisDialog = nullptr;
    }
    if(m_pMediator)
    {
        m_pMediator->CloseNet();
        delete m_pMediator;
        m_pMediator =nullptr;
    }
}

void CKernel::setProtocolNum()
{
    m_verProtocolFun[_DEF_PACK_REGISTER_RS    -   _DEF_PACK_BASE - 1] =&CKernel::dealRegisterRs;
    m_verProtocolFun[_DEF_PACK_LOGIN_RS       -   _DEF_PACK_BASE - 1] =&CKernel::dealLoginRs;
    m_verProtocolFun[_DEF_UPLOAD_RS           -   _DEF_PACK_BASE - 1] =&CKernel::dealUploadRs;
    m_verProtocolFun[DEF_PACK_DOWNLOAD_RS     -   _DEF_PACK_BASE - 1] =&CKernel::dealDownloadRs;
    m_verProtocolFun[_DEF_FILE_PACK_BLOCK     -   _DEF_PACK_BASE - 1] =&CKernel::dealfileBlockRq;
}

void CKernel::dealRegisterRs(char* buf,int len,unsigned long from)
{
    qDebug()<<__func__;
    STRU_REGISTER_RS* rs = (STRU_REGISTER_RS*)buf;
    switch(rs->result)
    {
    case user_is_exist:
        QMessageBox::about(m_regisDialog,"提示","注册用户已存在！");
        break;
    case register_success:
        QMessageBox::about(m_regisDialog,"提示","注册成功！");
        //m_regisDialog->hide();
        break;
    default:
        break;
    }

}
void CKernel::dealLoginRs(char* buf,int len,unsigned long from)
{
    qDebug()<<__func__;
    STRU_LOGIN_RS* rs = (STRU_LOGIN_RS*)buf;
    switch(rs->result)
    {
    case password_error:
        QMessageBox::about(m_regisDialog,"提示","密码错误！");
        break;
    case user_not_exist:
        QMessageBox::about(m_regisDialog,"提示","用户不存在！");
        break;
    case login_success:
        QMessageBox::about(m_regisDialog,"提示","登陆成功!");
        m_id = rs->userid;
        m_pLoginDlg->m_id = rs->userid;
        m_pLoginDlg->setLoginInfo((char*)&rs,sizeof (rs),8000);
        m_regisDialog->hide();

        //下载列表文件
        slot_fresh();

        break;
    }
}

void CKernel::dealUploadRs(char *buf, int len, unsigned long from)
{
    STRU_USER_UPLOAD_RS* rs = (STRU_USER_UPLOAD_RS*)buf;
    switch(rs->result)
    {
        case 1:
            QMessageBox::about(m_pLoginDlg,"提示","上传成功！");
        break;
    default:
            QMessageBox::about(m_pLoginDlg,"提示","上传失败！");
        break;
    }
}

void CKernel::slot_registerData(QString name, QString password,Hobby* hy)
{
    qDebug()<<__func__;
    //注册请求
    STRU_REGISTER_RQ rq;
    strcpy_s(rq.name,sizeof(rq.name),name.toStdString().c_str());
    strcpy_s(rq.password,sizeof(rq.password),password.toStdString().c_str());
    rq.hy = *hy;
    //发送注册请求
    m_pMediator->SendData(8000,(char*)&rq,sizeof (rq));
    if(hy)
    {
        delete hy;
        hy = nullptr;
    }
}

void CKernel::slot_loginData(QString name,QString password)
{
    qDebug()<<__func__;
    //登录请求
    STRU_LOGIN_RQ rq;
    strcpy_s(rq.name,sizeof(rq.name),name.toStdString().c_str());
    strcpy_s(rq.password,sizeof(rq.password),password.toStdString().c_str());
    //发送登录请求
    m_pMediator->SendData(8000,(char*)&rq,sizeof (rq));
}

void CKernel::slot_dealData(char *buf, int len, unsigned long from)
{
    qDebug()<<__func__;
    int type = *(int*)buf;
    int index = type - _DEF_PACK_BASE - 1;
    if(index >= 0 && index < _DEF_PACK_COUNT)
    {
        PFUN pf = m_verProtocolFun[index];
        //如果函数指针pf不为空，说明有对应函数
        if(pf)
        {
            (this->*pf)(buf,len,from);
        }else{
            qDebug()<<"函数为空，index"<<index;
        }
    }
    //如果type越界，检查定义
    else{
        qDebug()<<"type error :"<<type;
    }
}

void CKernel::slot_openRegister()
{
    m_regisDialog->show();
}

void CKernel::slot_sendUploadRq(char *buf, int len, unsigned long from)
{
    m_pMediator->SendData(from,buf,len);
}

void CKernel::slot_sendBlock(char *buf, int len, unsigned long from)
{
    m_pMediator->SendData(from,buf,len);
}

void CKernel::dealDownloadRs(char *buf, int len, unsigned long from)
{
    STRU_DOWNLOAD_RS* rs = (STRU_DOWNLOAD_RS*)buf;
    //文件头

    //给FileInfo赋值

    FileInfo* info = new FileInfo;
    //videoid 作为文件的表示，fileid用来区分不同控件
    info->VideoId = rs->m_nVideoId;
    info->fileId = rs->m_nFileId;
    info->fileName = rs->m_szFileName;

    QDir dir;
    if(!dir.exists(QDir::currentPath() + "/temp/"))
    {
        dir.mkpath(QDir::currentPath() + "/temp/");
    }
    info->filePath = QString("./temp/%1").arg(rs->m_szFileName);

    info->filePos = 0;
    info->fileSize = rs->m_nFileSize;

    //info->rtmpUrl = QString("rtmp://%1/vod%2").arg(_DEF_SERVER_IP).arg(rs->m_rtmp);
    //由上面的rtmp变为hls进行推流播放
    info->rtmpUrl = QString("http://%1:80/hls/%2").arg(_DEF_SERVER_IP).arg(rs->m_rtmp);

    info->pFile = new QFile(info->filePath);
    if(info->pFile->open(QIODevice::WriteOnly))
    {
        m_mapVideoIdToFileInfo[info->VideoId] = info;
    }else{
        delete info;
    }
    //m_mapVideoIdToFileInfo
}
//下载文件快
void CKernel::dealfileBlockRq(char *buf, int len, unsigned long from)
{
    Q_EMIT SIG_fileBlockRq(buf,m_mapVideoIdToFileInfo);
}

void CKernel::slot_fresh()
{
    STRU_DOWNLOAD_RQ rq;
    rq.m_nUserId = m_id;
    m_pMediator->SendData(8000,(char*)&rq,sizeof (rq));
}
