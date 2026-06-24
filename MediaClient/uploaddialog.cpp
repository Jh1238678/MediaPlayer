#include "uploaddialog.h"
#include "ui_uploaddialog.h"
#include <QFileDialog>
#include<QDebug>
#include<QProcess>
#include<QTime>
#include<QDir>
#include<QMovie>
UpLoadDialog::UpLoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpLoadDialog)
{
    ui->setupUi(this);
    qRegisterMetaType<Hobby>("Hobby");
    ui->pb_beginupload->setEnabled(false);
    //m_tcpclient = new TcpClientMediator;
    m_player = new VideoPlayer;

//    connect(this,&UpLoadDialog::SIG_GetOnePicture,
//            ui->wdg_jpg,&ImageWidget::slot_setImage);
    connect(this,&UpLoadDialog::SIG_updataProcess,
            this,&UpLoadDialog::slot_updataProcess);
}

UpLoadDialog::~UpLoadDialog()
{
    delete ui;

    if(m_player)
        delete  m_player;
}
//点击浏览
void UpLoadDialog::on_pb_view_clicked()
{

    //打开游览选择文件
    QString path = QFileDialog::getOpenFileName(this,"打开文件","./"
        ,"视频文件 (*.flv *.rmvb *.avi *.MP4 *.mkv);; 所有文件(*.*);;");

    //判断
    if(path.isEmpty())return;
    ui->le_path->setText(path);

    //保证有temp路径
    QDir dir;
    if(!dir.exists(QDir::currentPath() + "/temp/"))
        dir.mkpath(QDir::currentPath() + "/temp/");

    //显示gif
    QString imgPath = /*"E:/dongtu.gif"*/SaveVideoJpg(path);

    //加载到控件
    QMovie * movie = new QMovie(imgPath);
    ui->lb_gif->setMovie(movie);
    movie->start();

    filePath = path;
    this->imgPath = imgPath;

    ui->pb_beginupload->setEnabled(true);
}

//开始上传
void UpLoadDialog::on_pb_beginupload_clicked()
{
    Hobby hy;
    hy.dance      = ui->cb_dance   ->isChecked() ? 50 : 0;
    hy.edu        = ui->cb_educate ->isChecked() ? 50 : 0;
    hy.film       = ui->cb_film    ->isChecked() ? 50 : 0;
    hy.food       = ui->cb_food    ->isChecked() ? 50 : 0;
    hy.funny      = ui->cb_funny   ->isChecked() ? 50 : 0;
    hy.good       = ui->cb_posenergy->isChecked() ? 50 : 0;
    hy.music      = ui->cb_music   ->isChecked() ? 50 : 0;
    hy.outdoor    = ui->cb_outdoors ->isChecked() ? 50 : 0;
    Q_EMIT SIG_UploadFile(filePath,imgPath,hy);
}

//清空
void UpLoadDialog::clear(){
    ui->le_path->setText("");
    ui->pb_beginupload->setEnabled(false);
    //Q_EMIT SIG_GetOnePicture(QImage());
    ui->lb_gif->setText("");
    ui->pgb_progress->setValue(0);
}

QString UpLoadDialog::SaveVideoJpg(QString FilePath)
{
    std::string tmp = FilePath.toStdString();
    char* file_path = (char*)tmp.c_str();

    AVFormatContext* pFormatCtx;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;
    AVFrame* pFrame,*pFrameRGB;
    AVPacket* packet;
    uint8_t *out_buffer;
    static struct SwsContext* img_convert_ctx;
    int videoStream,i,numBytes;
    int ret,got_picture;
    av_register_all();
    //初始化 FFMPEG 调用了这个才能正常适用编码器和解码器

    pFormatCtx = avformat_alloc_context();
    if(avformat_open_input(&pFormatCtx,file_path,NULL,NULL) != 0)
    {
        qDebug()<<"can't open the file.";
        return QString();
    }
    if(avformat_find_stream_info(pFormatCtx,NULL) < 0)
    {
        qDebug()<<"Could't find stream infomation.";
        return QString();
    }
    videoStream = -1;
    for(int i = 0;i<pFormatCtx->nb_streams;++i)
    {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStream = i;
        }
    }

    //如果videoStream为-1说明没有找到视频流
    if(videoStream == -1)
    {
        qDebug()<<"Didn't find a video stream.";
        return QString();
    }
    //查找解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL)
    {
        qDebug()<<"Codec not found.";
        return QString();
    }
    //打开解码器
    if(avcodec_open2(pCodecCtx,pCodec,NULL)<0)
    {
        qDebug()<<"Could not open codec.";
        return QString();
    }
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    img_convert_ctx = sws_getContext(pCodecCtx->width,pCodecCtx->height,
                                     pCodecCtx->pix_fmt,pCodecCtx->width,pCodecCtx->height,
                                     AV_PIX_FMT_RGB32,SWS_BICUBIC,NULL,NULL,NULL);
    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32,
                                  pCodecCtx->width,pCodecCtx->height);
    out_buffer = (uint8_t*)av_malloc(numBytes*sizeof (uint8_t));
    avpicture_fill((AVPicture*)pFrameRGB,out_buffer,AV_PIX_FMT_RGB32,
                   pCodecCtx->width,pCodecCtx->height);
    int y_size = pCodecCtx->width * pCodecCtx->height;
    packet = (AVPacket* )malloc(sizeof(AVPacket)); // 分配一个packet
    av_new_packet(packet,y_size); //分配packet的数据
    int index = 0;
    //用于生成gif图片的数量
    int nCount = 0;
    while(1)
    {
        if(av_read_frame(pFormatCtx,packet) < 0)
        {
            break; //这里认为视频读取完了
        }
        if(packet->stream_index == videoStream)
        {
            ret = avcodec_decode_video2(pCodecCtx,pFrame,&got_picture,packet);
            if(ret < 0)
            {
                qDebug()<<"decode error.";
                return QString();
            }
            if(got_picture && pFrame->key_frame == 1 && pFrame->pict_type == AV_PICTURE_TYPE_I)
            {
                sws_scale(img_convert_ctx,
                          (uint8_t const* const*)pFrame->data,
                          pFrame->linesize,0,pCodecCtx->height,pFrameRGB->data,
                          pFrameRGB->linesize);
                QImage tmpImg((uchar*)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                QImage image = tmpImg.scaled(631,361,Qt::KeepAspectRatio);//把图像复制一份 传递给界面显示

                image.save(QString("./temp/%1.jpg").arg(nCount,2,10,QChar('0')));

                nCount++;
                if(nCount == 6)
                {
                    av_free_packet(packet);
                    break;
                }
//                emit SIG_GetOnePicture(image);
//                av_free_packet(packet);
//                av_free(out_buffer);
//                av_free(pFrameRGB);
//                avcodec_close(pCodecCtx);
//                avformat_close_input(&pFormatCtx);
//                return image;
            }
        }
        av_free_packet(packet);
    }
    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);


    QProcess p(0);
    p.start("cmd" );//开启 cmd 窗口
    p.waitForStarted();
    QString curPath = QDir::currentPath();
    curPath.replace("/","\\\\");
    QString strcd = QString("%1/ \n").arg(QCoreApplication::applicationDirPath());
    strcd.replace("/","\\\\");
    strcd = QString("cd /d ")+strcd;
    p.write( strcd.toLocal8Bit() );
    QString imgName = QString("%1\\\\temp\\\\%2.gif")
            .arg(curPath).arg(QTime::currentTime().toString("hhmmsszzz"));
    QString cmd = QString("ffmpeg -r 2 -i %1\\\\temp\\\\").arg(curPath);
    cmd += "%02d.jpg ";
    cmd += imgName;
    cmd += " \n";
    qDebug() << "cmd "<< cmd;
    p.write( cmd.toLocal8Bit() );
    p.closeWriteChannel();
    p.waitForFinished();
    qDebug()<< QString::fromLocal8Bit( p.readAllStandardOutput() );
    p.close();

    qDebug()<<"imgName:"<<imgName;
    return imgName;

}

void UpLoadDialog::slot_updataProcess(qint64 cur, qint64 max)
{
    ui->pgb_progress->setMaximum(max);
    ui->pgb_progress->setValue(cur);
}

