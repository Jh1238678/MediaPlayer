#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
#include <QTimer>
#include <QThread>
#include <QImage>
#include <QWidget>
#include "PacketQueue.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include"libswresample/swresample.h"
#include"libavutil/time.h"
#include "SDL.h"
}
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 //1 second of 48khz 32bit audio
#define SDL_AUDIO_BUFFER_SIZE 1024 //
#define FLUSH_DATA "FLUSH"
//AVFrame wanted_frame;
//PacketQueue audio_queue;
//int quit = 0;

class VideoPlayer ;
typedef struct VideoState {
    AVFormatContext *pFormatCtx;//相当于视频”文件指针”
    ///////////////音频///////////////////////////////////
    AVStream *audio_st; //音频流
    PacketQueue *audioq;//音频缓冲队列
    AVCodecContext *pAudioCodecCtx ;//音频解码器信息指针
    int audioStream;//视频音频流索引
    double audio_clock; ///<pts of last decoded frame 音频时钟
    SDL_AudioDeviceID audioID; //音频 ID
    AVFrame out_frame; //设置参数，供音频解码后的 swr_alloc_set_opts 使用。
    /// 音频回调函数使用的量
    uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    unsigned int audio_buf_size = 0;
    unsigned int audio_buf_index = 0;
    AVFrame *audioFrame;
    //////////////////////////////////////////////////////
    ///////////////视频///////////////////////////////////
    AVStream *video_st; //视频流
    PacketQueue *videoq;//视频队列
    AVCodecContext *pCodecCtx ;//音频解码器信息指针
    int videoStream;//视频音频流索引
    double video_clock; ///<pts of last decoded frame 视频时钟
    SDL_Thread *video_tid; //视频线程 id
    /////////////////////////////////////////////////////

    /// 播放控制的变量
     bool isPause = false;//暂停标志
     bool quit = false; //停止
     bool readFinished; //读线程文件读取完毕
     bool readThreadFinished; //读取线程是否结束
     bool videoThreadFinished; // 视频线程是否结束
     /////////////////////////////////////////////////////
     //// 跳转相关的变量
     int seek_req; //跳转标志 -- 读线程
     int64_t seek_pos; //跳转的位置 -- 微秒
     int seek_flag_audio;//跳转标志 -- 用于音频线程中
     int seek_flag_video;//跳转标志 -- 用于视频线程中
     double seek_time; //跳转的时间(秒) 值和 seek_pos 是一样的
     //////////////////////////////////////////////////////
     int64_t start_time; //单位 微秒
    VideoState()
    {
        audio_clock = video_clock = start_time = 0;
    }
    VideoPlayer* m_player;//用于调用函数
    //~VideoState();
} VideoState;

class VideoPlayer : public QThread
{
    Q_OBJECT
    //qt通过信号和槽 实现qt 多线程通信
signals:
    void SIG_getOneImage(QImage img);
    void SIG_PlayerStateChanged(int state);
    void SIG_TotalTime(qint64 uSec);
public:
    VideoPlayer();
    //explicit VideoPlayer(QWidget *parent = nullptr);
    void run();
    //void setFileName(const QString &newFileName);
public:
    enum PlayerState
    {
     Playing = 0,
     Pause,
     Stop
    };
public slots:
    void SendGetOneImage(QImage &img);
private:
    QString  m_fileName;
    VideoState m_videoState;
    PlayerState m_playerState;
public:
    void play();
    void pause();
    void stop( bool isWait);
    void setFileName(const QString &fileName);

//    为了完成播放的控制, 需要在 VideoPlayer 中添加成员 PlayerState m_playerState;播放状态是一个枚举,
//    代表着播放状态.


    PlayerState playerState() const;
    void seek(int64_t pos);
    double getCurrentTime();
    qint64 getTotalTime();
};

#endif // VIDEOPLAYER_H
