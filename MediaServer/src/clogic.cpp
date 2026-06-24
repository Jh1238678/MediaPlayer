#include "clogic.h"
#include "cmymd5.h"
CLogic::~CLogic()
{
    for(auto ite = m_mapFileIdToFileInfo.begin();ite != m_mapFileIdToFileInfo.end();++ite)
    {
        delete ite->second;
    }
    m_mapFileIdToFileInfo.clear();
}

void CLogic::setNetPackMap()
{
    NetPackMap(_DEF_PACK_REGISTER_RQ)    = &CLogic::RegisterRq;
    NetPackMap(_DEF_PACK_LOGIN_RQ)       = &CLogic::LoginRq;
    NetPackMap(_DEF_UPLOAD_RQ)           = &CLogic::UploadRq;
    NetPackMap(_DEF_FILE_PACK_BLOCK)     = &CLogic::UploadBlockRq;
    NetPackMap(DEF_PACK_DOWNLOAD_RQ)     = &CLogic::DownloadRq;
}

#define _DEF_COUT_FUNC_    cout << "clientfd:"<< clientfd << __func__ << endl;

//注册
void CLogic::RegisterRq(sock_fd clientfd,char* szbuf,int nlen)
{
    //cout << "clientfd:"<< clientfd << __func__ << endl;
    _DEF_COUT_FUNC_
   cout<<__func__<<endl;
   STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)szbuf;
   char szSql[1024];
   list<string> sllst;
   list<string> islst;
   sprintf(szSql,"select name from t_UserData where name='%s'",rq->name);
   cout<<"rq->name:"<<rq->name<<endl;
   cout<<"rq->passwd:"<<rq->password<<endl;
   STRU_REGISTER_RS rs;
   if(m_sql->SelectMysql(szSql,1,sllst))
   {
       if(sllst.size() > 0)
       {
           rs.result = user_is_exist;
           cout<<"name: "<<sllst.front()<<" had register!"<<endl;
           m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
           return;
       }
       sprintf(szSql,"insert into t_UserData (name,password,food,funny,ennegy,dance,music,video,outside,edu)"
                     " values('%s','%s','%d','%d','%d','%d','%d','%d','%d','%d')",
               rq->name,rq->password,rq->hy.food,rq->hy.funny,rq->hy.good,rq->hy.dance,rq->hy.music,
               rq->hy.film,rq->hy.outdoor,rq->hy.edu);
       if(m_sql->UpdataMysql(szSql))
       {
           cout<<"插入数据库成功！"<<endl;
           //给新注册的用户 创建一个路径
           char path[_MAX_PATH] = "";
           sprintf(path,"%sflv/%s",ROOT_PATH,rq->name);

           umask(0);
           mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
            //S_IRWXU\S_IRWXG\S_IRWXO,分别代表700、070、007权限，代表该文件所有者/用户组/其他用户，写和执行的权限

//           rs.result = register_success;
       }else{
           cout<<"插入数据库失败！"<<endl;
           return;
       }
   }

   m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));
}

//登录
void CLogic::LoginRq(sock_fd clientfd ,char* szbuf,int nlen)
{
//    cout << "clientfd:"<< clientfd << __func__ << endl;
    _DEF_COUT_FUNC_
    STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)szbuf;
    STRU_LOGIN_RS rs;
    char szSql[1024];
    list<string> sllst;
    sprintf(szSql,"select id,name,password from t_UserData where name='%s'",rq->name);
    if(m_sql->SelectMysql(szSql,3,sllst))
    {
        cout<<"登陆数据库，查询成功！"<<endl;
        if(sllst.size() == 3 && sllst.back() == rq->password)
        {
            rs.userid = atoi(sllst.front().c_str());
            sllst.pop_front();
            rs.result = login_success;
            strcpy(rs.name,sllst.front().c_str());
        }else if(sllst.size() == 3 && sllst.back() != rq->password)
        {
            rs.result = password_error;

        }else{
            rs.result = user_not_exist;
        }
    }else{
        cout<<"登陆时，数据库查询错误！"<<endl;
        return ;
    }
    //cout<<"user id:"<<atoi(sllst.front().c_str())<<endl;

    //rs.m_lResult = password_error;
    m_tcp->SendData( clientfd , (char*)&rs , sizeof (rs) );
}

void CLogic::UploadRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    STRU_USER_UPLOAD_RQ *rq = (STRU_USER_UPLOAD_RQ*)szbuf;

    FileInfo* info = new FileInfo;
    info->m_nPos = 0;
    memcpy(info->m_Hobby,rq->m_szHobby,_DEF_HOBBY_COUNT*sizeof (int));
    info->m_nUserId = rq->m_UserId;
    info->m_nFileID = rq->m_nFileId;
    info->m_VideoID = 0;
    info->m_nFileSize = rq->m_nFileSize;

    strcpy(info->m_szFileName,rq->m_szFileName);
    strcpy(info->m_szFileType,rq->m_szFileType);

    //找到用户名
    char szSql[_DEF_SQLLEN] = "";
    sprintf(szSql,"select name from t_UserData where id = %d;",info->m_nUserId);
    list<string> reslst;
    if(!m_sql->SelectMysql(szSql,1,reslst))
    {
        cout<<"SelectMysql error"<<szSql<<endl;
        delete info;
        return;
    }
    if(reslst.size()<=0)
    {
        delete info;
        return;
    }

    strcpy(info->m_UserName,reslst.front().c_str());
    cout<<"info->m_szFileSize:"<<info->m_nFileSize<<endl;
    sprintf(info->m_szFilePath,"%sflv/%s/%s",ROOT_PATH,info->m_UserName,info->m_szFileName);
    sprintf(info->m_szRtmp,"//%s/%s",info->m_UserName,info->m_szFileName);
    if(strcmp(rq->m_szFileType,"gif") != 0)
    {
        strcpy(info->m_szGifName,rq->m_szGifName);
        sprintf(info->m_szGifPath,"%sflv/%s/%s",ROOT_PATH,info->m_UserName,info->m_szGifName );
    }
    info->pFile = fopen(info->m_szFilePath,"w");

    m_mapFileIdToFileInfo[info->m_nFileID] = info;
}
//int CLogic::count = 0;
//上传文件快
void CLogic::UploadBlockRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_

    STRU_FILEBLOCK_RQ* rq = (STRU_FILEBLOCK_RQ*)szbuf;
    if(m_mapFileIdToFileInfo.find(rq->m_nFileId) == m_mapFileIdToFileInfo.end())
    {
        //cout<<"has no m_mapFileIdToFileInfo!"<<endl;
        return;
    }
    FileInfo* info = m_mapFileIdToFileInfo[rq->m_nFileId];

    int64_t res = fwrite(rq->m_szFileContent,1,/*rq->m_nBlockLen*/_DEF_CONTENT_SIZE,info->pFile);

    info->m_nPos += res;
    fflush(info->pFile);
    //cout<<"res: "<<res<<endl;
    if(/*rq->m_nBlockLen < _DEF_CONTENT_SIZE ||*/ info->m_nPos >= info->m_nFileSize)
    {
        string inputFile = "/home/ros/video/flv/" + string(info->m_UserName) +"/" + string(info->m_szFileName);
        //获取文件MD5
        string md5 = getFileMD5(inputFile);
        //创建目录
        string path = "/home/ros/tmp/hls/" + md5;
        //创建目录，设置权限为0777（可读写可执行）目录是要新创建 以前没有 需要自己代码实现创建并设置权限
        if(mkdir(path.c_str(),0777) == -1)
        {
            if(errno == EEXIST){
                cout<<"Directory already exists:"<<path.c_str()<<endl;
            }else{
                cout<<"Error creating directory:"<<strerror(errno)<<endl;
                return;
            }
        }else{
            cout<<"Directory created successfully:"<<path.c_str()<<endl;
        }
        string filename = "output.m3u8";
        string outputFile = "/home/ros/tmp/hls/" + md5 +"/" + filename;
        cout<<"outputFile"<<outputFile<<endl;
        //进行编码转换
        if(transcode(inputFile,outputFile)){
            cout<<"inputFile: "<<inputFile<<"转换完成"<<endl;
        }else{
            cerr<<"Handle the error."<<endl;
        }
        //写完了
        fclose(info->pFile);
        //判断 不是gif 写表记录 返回信息
       if(strcmp(info->m_szFileType,"gif") != 0)
       {
           //写表
//           create table t_VideoInfo( videoid bigint unsigned AUTO_INCREMENT primary key , userId bigint
//           unsigned , videoName nvarchar (300), picName nvarchar (300), videoPath nvarchar (300) , picPath
//           nvarchar (300) ,rtmp nvarchar (300) , food int, funny int ,ennegy int ,dance int , music int, video int,
//           outside int, edu int , hotdegree int);

           char sqlStr[_DEF_SQLLEN] = "";
           sprintf(sqlStr,"insert into t_VideoInfo ( userId , videoName , picName , videoPath , picPath , rtmp , food , funny  ,ennegy  ,dance  , music , video , outside , edu  , hotdegree  , hls) values(%d,'%s','%s','%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s' )",
                   info->m_nFileID,info->m_szFileName,info->m_szGifName,info->m_szFilePath,info->m_szGifPath,info->m_szRtmp,
                   info->m_Hobby[3],info->m_Hobby[4],info->m_Hobby[2],info->m_Hobby[0],info->m_Hobby[5],info->m_Hobby[7],info->m_Hobby[6],info->m_Hobby[1],0,(md5+"/"+filename).c_str());

           if(m_sql->UpdataMysql(sqlStr))
           {
               cout<<"UpdataMysql error!:"<<sqlStr<<endl;
           }
           //返回
           STRU_USER_UPLOAD_RS rs;
           rs.result = 1;
           m_tcp->SendData(clientfd,(char*)&rs,sizeof (rs));
       }
       m_mapFileIdToFileInfo.erase(rq->m_nFileId);
       delete info;
       info = nullptr;
    }
}

void CLogic::DownloadRq(sock_fd clientfd, char *szbuf, int nlen)
{
    _DEF_COUT_FUNC_
    //给这个用户推荐 发送动图 和视频对应的rtmp地址

    STRU_DOWNLOAD_RQ * rq = (STRU_DOWNLOAD_RQ*)szbuf;
    list<FileInfo*> fileList;
    //cout<<"oaisndoas"<<endl;
    GetFileList(fileList,rq->m_nUserId);

    //根据list发文件头 和  文件内容
    while(fileList.size()>0)
    {
        FileInfo* info = fileList.front();
        fileList.pop_front();

        STRU_DOWNLOAD_RS rs;
        strcpy(rs.m_rtmp,info->m_szRtmp);
        rs.m_nFileId = info->m_nFileID;
        rs.m_nVideoId = info->m_VideoID;
        rs.m_nFileSize = info->m_nFileSize;
        strcpy(rs.m_szFileName,info->m_szFileName);

        m_tcp->SendData(clientfd,(char*)&rs,sizeof(rs));

        info->pFile = fopen(info->m_szFilePath,"r");
        if(info->pFile)
        {
            while(1)
            {
                STRU_FILEBLOCK_RQ blockrq;
                int64_t res = fread(blockrq.m_szFileContent,1,_DEF_CONTENT_SIZE,info->pFile);
                blockrq.m_nBlockLen = res;
                info->m_nPos += res;
                blockrq.m_nFileId = info->m_VideoID;
                blockrq.m_nUserId = rq->m_nUserId;

                m_tcp->SendData(clientfd,(char*)&blockrq,sizeof(blockrq));

                if(info->m_nPos >= info->m_nFileSize)
                {
                    fclose(info->pFile);
                    delete info;
                    info = nullptr;
                    break;
                }
            }
        }
    }
}

//获取列表
void CLogic::GetFileList(list<FileInfo*>& fileList,int userid)
{
    //1.根据userid找用户没有接受过 in(t_UserRecv --> videoid)的视频的个数
//    select count(videoId) from t_VideoInfo where t_VideoInfo.videoId not in ( select t_UserRecv.videoId from
//    t_UserRecv where t_UserRecv.userId = 5 );
//    delete from t_UserRecv where userId = 5;
    cout<<__func__<<endl;
    char sqlStr[_DEF_SQLLEN] = "";
    sprintf(sqlStr,"select count(videoId) from t_VideoInfo where t_VideoInfo.videoId not in ( select t_UserRecv.videoId from t_UserRecv where t_UserRecv.userId = %d )",userid);
    int nCount = 0;
    list<string> resList;

    if(!m_sql->SelectMysql(sqlStr,1,resList))
    {
        cout<<"SelectMysql error: "<<sqlStr<<endl;
        return;
    }
    if(resList.size() == 0)return;
    nCount = atoi(resList.front().c_str());

    //2.个数 == 0 根据userid 清空 t_UserRecv 中
    if(nCount == 0)
    {
        sprintf(sqlStr,"delete from t_UserRecv where userId = %d",userid);
        if(!m_sql->UpdataMysql(sqlStr))
        {
            cout<<"UpdataMysql error: "<<sqlStr<<endl;
            return;
        }
    }

    //上面解决一定有视频可以推送
    //3.推送 用户没有接受过的 热度 hotdegree 前10的视频 order by hotdegree limit 0,10 的视频 写入--> fileList
    resList.clear();
    sprintf(sqlStr,"select videoId ,picName , picPath , hls from t_VideoInfo where t_VideoInfo.videoId not in ( select t_UserRecv.videoId from t_UserRecv where t_UserRecv.userId = %d ) order by hotdegree desc limit 0,10",userid);
    if(!m_sql->SelectMysql(sqlStr,4,resList))
    {
        cout<<"SelectMysql error: "<<sqlStr<<endl;
        return;
    }

    nCount = 1;
    int nSize = resList.size()/4;
    for(int i = 0;i<nSize;++i)
    {

        FileInfo* info = new FileInfo;
        //info 赋值
        info->m_nPos = 0;

        info->m_VideoID = atoi(resList.front().c_str());
        resList.pop_front();

        strcpy(info->m_szFileName,resList.front().c_str());
        resList.pop_front();

        strcpy(info->m_szFilePath,resList.front().c_str());
        resList.pop_front();
        cout<<"filepath: "<<info->m_szFilePath<<endl;
        strcpy(info->m_szRtmp,resList.front().c_str());
        resList.pop_front();

        info->m_nFileID = nCount++;

        info->pFile = fopen(info->m_szFilePath,"r");
        fseek(info->pFile,0,SEEK_END);

        info->m_nFileSize = ftell(info->pFile);
        fseek(info->pFile,0,SEEK_SET);
        fclose(info->pFile);

        info->pFile = nullptr;

        //list添加
        fileList.push_back(info);

        //4. t_UserRecv中插入信息
//        插入已推送数据
//        insert into t_UserRecv values(%d ,%d);
        sprintf(sqlStr,"insert into t_UserRecv values(%d ,%d)",userid,info->m_VideoID);

        if(!m_sql->UpdataMysql(sqlStr))
        {
            cout<<"UpdataMysql error: "<<sqlStr<<endl;
            return;
        }

    }


}

bool CLogic::transcode(const string &inputFile, const string &outputFile)
{
    // 构建 FFmpeg 命令
    /*需要转成 H.264 和 AAC 以满足客户端的需求使用这个
    std::string command = "ffmpeg -i " + inputFile + " -c:v libx264 -c:a aac -start_number 0 -hls_time 10
    -hls_list_size 0 -f hls " + outputFile;*/
    //直接复制编码, 可以快速实现文件切分 ts 文件
    std::string command = "ffmpeg -i " + inputFile + " -codec: copy -start_number 0 -hls_time 10\
            -hls_list_size 0 -f hls " + outputFile;
            // 调用 FFmpeg 进行转码
    int result = std::system(command.c_str());
    // 检查转码是否成功
    if (result == 0) {
        std::cout << "Transcoding completed successfully." << std::endl;
        return true;
    } else {
        std::cerr << "Transcoding failed with error code: " << result << std::endl;
        return false;
    }
}
