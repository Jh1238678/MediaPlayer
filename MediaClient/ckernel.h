#ifndef CKERNEL_H
#define CKERNEL_H
#include<QObject>
#include<QMap>
#include"INet.h"
#include"packdef.h"
#include"logindialog.h"
#include"registerdialog.h"
class CKernel;
typedef void(CKernel::*PFUN)(char* data,int len,unsigned long from);
class CKernel:public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = nullptr);
    ~CKernel();
signals:
    void SIG_fileBlockRq(char *buf,QMap<int,FileInfo*>& m_mapVideoIdToFileInfo);

    //打开rtmp
    void SIG_openKernel(QString url);
public:
    void setProtocolNum();
    void dealRegisterRs(char* buf,int len,unsigned long from);
    void dealLoginRs(char* buf,int len,unsigned long from);
    //处理上传回复
    void dealUploadRs(char* buf,int len,unsigned long from);
public slots:
    //把注册数据发给服务端
    void slot_registerData(QString name,QString password,Hobby* hy);

    //把登陆数据发给服务端
    void slot_loginData(QString name,QString password);

    //处理接收到的数据
    void slot_dealData(char* buf,int len,unsigned long from);

    //打开注册/登录界面
    void slot_openRegister();

    //传输文件上传请求
    void slot_sendUploadRq(char* buf,int len,unsigned long from);
    //传输文件块
    void slot_sendBlock(char* buf,int len,unsigned long from);

    //处理下载回复
    void dealDownloadRs(char* buf,int len,unsigned long from);
    //处理文件快请求
    void dealfileBlockRq(char* buf,int len,unsigned long from);
    //刷新
    void slot_fresh();
public:
    //保存当前用户的id
    int m_id;
    //保存当前登录用户的昵称
    QString m_name;
    INetMediator* m_pMediator;
    //主界面
    loginDialog* m_pLoginDlg;
    //函数指针数组
    PFUN m_verProtocolFun[_DEF_PACK_COUNT];
    //登录/注册界面
    registerDialog* m_regisDialog;
    //videoID映射到文件信息
    QMap<int,FileInfo*> m_mapVideoIdToFileInfo;
};

#endif // CKERNEL_H
