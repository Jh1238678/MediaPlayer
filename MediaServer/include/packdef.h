#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "err_str.h"
#include <malloc.h>

#include<iostream>
#include<map>
#include<list>

#include<QString>
#include<QFile>
//边界值
#define _DEF_SIZE           45
#define _DEF_BUFFERSIZE     1000
#define _DEF_PORT           8000
#define _DEF_SERVERIP       "0.0.0.0"
#define _DEF_LISTEN         128
#define _DEF_EPOLLSIZE      4096
#define _DEF_IPSIZE         16
#define _DEF_COUNT          10
#define _DEF_TIMEOUT        10
#define _DEF_SQLIEN         400
#define TRUE                true
#define FALSE               false



/*-------------数据库信息-----------------*/
#define _DEF_DB_NAME    "VideoServer"
#define _DEF_DB_IP      "localhost"
#define _DEF_DB_USER    "xjn"
#define _DEF_DB_PWD     "123"
/*--------------------------------------*/
#define _MAX_PATH           (260)
#define _DEF_BUFFER         (4096)
#define _DEF_CONTENT_SIZE	(1024)
#define _MAX_SIZE           (40)

//自定义协议   先写协议头 再写协议结构
//登录 注册 获取好友信息 添加好友 聊天 发文件 下线请求
#define _DEF_PACK_BASE	(10000)
#define _DEF_PACK_COUNT (100)
#define _DEF_HOBBY_COUNT (8)
//注册
#define _DEF_PACK_REGISTER_RQ	(_DEF_PACK_BASE + 0 )
#define _DEF_PACK_REGISTER_RS	(_DEF_PACK_BASE + 1 )
//登录
#define _DEF_PACK_LOGIN_RQ	(_DEF_PACK_BASE + 2 )
#define _DEF_PACK_LOGIN_RS	(_DEF_PACK_BASE + 3 )

//上传视频
#define _DEF_UPLOAD_RQ (_DEF_PACK_BASE + 4 )
#define _DEF_UPLOAD_RS (_DEF_PACK_BASE + 5)
#define _DEF_FILE_PACK_BLOCK (_DEF_PACK_BASE + 6)
//添加协议头(下载文件)
#define DEF_PACK_DOWNLOAD_RQ (_DEF_PACK_BASE + 7)
#define DEF_PACK_DOWNLOAD_RS (_DEF_PACK_BASE + 8)

//根路径
#define ROOT_PATH "/home/ros/video/"

//sql语句长度
#define _DEF_SQLLEN 512

//返回的结果
//注册请求的结果
#define user_is_exist		(0)
#define register_success	(1)
//登录请求的结果
#define user_not_exist		(0)
#define password_error		(1)
#define login_success		(2)


typedef int PackType;

struct Hobby
{
    int dance;
    int edu;
    int good;
    int food;
    int funny;
    int music;
    int outdoor;
    int film;
};
//协议结构
//注册
typedef struct STRU_REGISTER_RQ
{
    STRU_REGISTER_RQ():type(_DEF_PACK_REGISTER_RQ)
    {
        //memset( tel  , 0, sizeof(tel));
        memset( name  , 0, sizeof(name));
        memset( password , 0, sizeof(password) );
    }
    //需要手机号码 , 密码, 昵称
    PackType type;
    //char tel[_MAX_SIZE];
    char name[_MAX_SIZE];
    char password[_MAX_SIZE];
    Hobby hy;
}STRU_REGISTER_RQ;

typedef struct STRU_REGISTER_RS
{
    //回复结果
    STRU_REGISTER_RS(): type(_DEF_PACK_REGISTER_RS) , result(register_success)
    {
    }
    PackType type;
    int result;

}STRU_REGISTER_RS;

//登录
typedef struct STRU_LOGIN_RQ
{
    //登录需要: 手机号 密码
    STRU_LOGIN_RQ():type(_DEF_PACK_LOGIN_RQ)
    {
        memset( name , 0, sizeof(name) );
        memset( password , 0, sizeof(password) );
    }
    PackType type;
    char name[_MAX_SIZE];
    char password[_MAX_SIZE];

}STRU_LOGIN_RQ;

typedef struct STRU_LOGIN_RS
{
    // 需要 结果 , 用户的id
    STRU_LOGIN_RS(): type(_DEF_PACK_LOGIN_RS) , result(login_success),userid(0)
    {
        memset( name , 0, sizeof(name) );
    }
    PackType type;
    char name[_MAX_SIZE];
    int result;
    int userid;

}STRU_LOGIN_RS;

//用户上传视频的请求
typedef struct STRU_USER_UPLOAD_RQ{

    STRU_USER_UPLOAD_RQ(){
        type = _DEF_UPLOAD_RQ;
        m_nFileId = 0;
        m_UserId = 0;
        m_nFileSize = 0;

        memset(m_szGifName,0,sizeof(m_szGifName));
        memset(m_szFileType,0,sizeof(m_szFileType));
        memset(m_szFileName,0,sizeof(m_szFileName));
    }
    PackType type;//包类型
    int m_UserId;//用于查询数据库，获取用户名字，拼接二路径
    int m_nFileId;//区分不同文件可采用md5 或 随机数 用户同时只能传一个所以相同概率较低
    long long m_nFileSize;//文件大小，用于文件传输结束
    int m_szHobby[_DEF_HOBBY_COUNT];//喜好标签
    char m_szGifName[_MAX_PATH];//gif文件名，用于数据库写表
    char m_szFileName[_MAX_PATH];//文件名，用于存储文件
    char m_szFileType[_MAX_SIZE];//用于区分视频和图片
}STRU_USER_UPLOAD_RQ;

//用户上传视频的回复
typedef struct STRU_USER_UPLOAD_RS{

    STRU_USER_UPLOAD_RS(){
        type = _DEF_UPLOAD_RS;
        result = 0;
    }
    PackType type;
    //结果
    int result;
}STRU_USER_UPLOAD_RS;

//文件快
typedef struct STRU_FILEBLOCK_RQ
{
    STRU_FILEBLOCK_RQ()
    {
        type = _DEF_FILE_PACK_BLOCK;
        m_nUserId = 0;
        m_nFileId = 0;
        m_nBlockLen = 0;
        memset(m_szFileContent,0,sizeof(m_szFileContent));
    }
    PackType type;
    int m_nUserId;//用户id
    int m_nFileId;//文件id 用于区分文件
    int m_nBlockLen;//文件写入大小
    char m_szFileContent[_DEF_CONTENT_SIZE];//文件块内容
}STRU_FILEBLOCK_RQ;

//struct FileInfo
//{
//    int fileId;
//    long long filePos;
//    long long fileSize;
//    QString filePath;
//    QString fileName;
//    QFile* pFile;
//};

//文件信息
typedef struct STRU_FILEINFO
{
public:
    STRU_FILEINFO():m_nFileID(0),m_VideoID(0),m_nFileSize(0),m_nPos(0),
        m_nUserId(0),pFile(0)
    {
        memset(m_szFilePath, 0 , _MAX_PATH);
        memset(m_szFileName, 0 , _MAX_PATH);
        memset(m_szGifPath , 0 , _MAX_PATH );
        memset(m_szGifName, 0 , _MAX_PATH);
        memset(m_szFileType, 0 , _MAX_SIZE);
        memset(m_Hobby, 0 , _DEF_HOBBY_COUNT);
        memset(m_UserName, 0 , _MAX_SIZE);
        memset(m_szRtmp ,0 , _MAX_PATH);
    }
    int m_nFileID;//下载的时候是用来做 UI 控件编号的， 上传的时候是一个随机数， 区分文件。
    int m_VideoID;//真是文件 ID 與 Mysql 的一致
    int64_t m_nFileSize;
    int64_t m_nPos;
    int m_nUserId;
    FILE* pFile;
    char m_szFilePath[_MAX_PATH];
    char m_szFileName[_MAX_PATH];
    char m_szGifPath[_MAX_PATH];
    char m_szGifName[_MAX_PATH];
    char m_szFileType[_MAX_SIZE];
    int   m_Hobby[_DEF_HOBBY_COUNT];
    char m_UserName[_MAX_SIZE];
    char m_szRtmp[_MAX_PATH];
}FileInfo;


//下载文件请求
typedef struct STRU_DOWNLOAD_RQ
{
    STRU_DOWNLOAD_RQ()
    {
        m_nType = DEF_PACK_DOWNLOAD_RQ;
        m_nUserId = 0;
    }
    PackType m_nType; //包类型
    int m_nUserId; //用户 ID
}STRU_DOWNLOAD_RQ;
//下载文件回复
typedef struct STRU_DOWNLOAD_RS
{
    STRU_DOWNLOAD_RS()
    {
        m_nType = DEF_PACK_DOWNLOAD_RS;
        m_nFileId = 0;
        memset(m_szFileName , 0 ,_MAX_PATH);
        memset(m_rtmp , 0 ,_MAX_PATH);
    }
    PackType m_nType; //包类型
    int m_nFileId;
    int64_t m_nFileSize;
    int m_nVideoId;
    char m_szFileName[_MAX_PATH];
    char m_rtmp[_MAX_PATH]; // 播放地址 如//1/103.MP3 用户本地需要转化为 rtmp://服务器 ip/app 名/ + 这个字符串 //本项目 app 名为 vod
}STRU_DOWNLOAD_RS;

