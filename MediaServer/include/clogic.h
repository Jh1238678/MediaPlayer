#ifndef CLOGIC_H
#define CLOGIC_H

#include<iostream>
#include<string>
#include<cstdlib>
#include"TCPKernel.h"
#include"packdef.h"
class CLogic
{
public:
    CLogic( TcpKernel* pkernel )
    {
        m_pKernel = pkernel;
        m_sql = pkernel->m_sql;
        m_tcp = pkernel->m_tcp;
    }
    ~CLogic();
public:
    //static int count;
    //设置协议映射
    void setNetPackMap();
    /************** 发送数据*********************/
    void SendData( sock_fd clientfd, char*szbuf, int nlen )
    {
        m_pKernel->SendData( clientfd ,szbuf , nlen );
    }
    /************** 网络处理 *********************/
    //注册
    void RegisterRq(sock_fd clientfd, char*szbuf, int nlen);
    //登录
    void LoginRq(sock_fd clientfd, char*szbuf, int nlen);

    /*******************************************/

    //上传文件请求
    void UploadRq(sock_fd clientfd, char*szbuf, int nlen);
    //上传文件快
    void UploadBlockRq(sock_fd clientfd, char*szbuf, int nlen);
    //下载请求
    void DownloadRq(sock_fd clientfd, char*szbuf, int nlen);
    //获取列表
    void GetFileList(list<FileInfo *> &fileList, int userid);
    //将rtmp转换成hls函数
    bool transcode(const std::string& inputFile, const std::string& outputFile);
private:
    TcpKernel* m_pKernel;
    CMysql * m_sql;
    Block_Epoll_Net * m_tcp;

    map<int,FileInfo*> m_mapFileIdToFileInfo;
};

#endif // CLOGIC_H
