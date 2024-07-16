#include "avffmpegnetcamera.h"
#include <QDebug>
#include <QDateTime>


AVFFmpegNetCamera::AVFFmpegNetCamera(QObject *parent)
    : QObject(parent)
{
    pAVFormatContext = NULL;
    pAVCodecContext = NULL;
    pAVFrame = NULL;
    pSwsContext = NULL;

    videoWidth = 0;
    videoHeight = 0;
    videoStreamIndex = -1;

    m_stopped = 0;
}


/// 初始化
bool AVFFmpegNetCamera::Init(){
    /// rtsp:
    //初始化网络流格式,使用RTSP网络流时必须先执行
    avformat_network_init();

    //申请一个AVFormatContext结构的内存,并进行简单初始化
    pAVFormatContext = avformat_alloc_context();
    pAVFrame = av_frame_alloc();

    //打开视频流
    int result = avformat_open_input(
                &pAVFormatContext, url.toStdString().c_str(), NULL, NULL);
    if (result<0){
        qDebug() << "打开视频流失败 ";
        return false;
    }

    //获取视频流信息
    result = avformat_find_stream_info(pAVFormatContext, NULL);
    if (result<0){
        qDebug() << "获取视频流信息失败 ";
        return false;
    }

    //获取视频流索引
    videoStreamIndex = -1;
    for (int i = 0; i < pAVFormatContext->nb_streams; i++) {
        if (pAVFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1){
        qDebug() << "获取视频流索引失败 ";
        return false;
    }

    /// 分析编解码器的详细信息
    pAVCodecContext = pAVFormatContext->streams[videoStreamIndex]->codec;
    videoWidth = pAVCodecContext->width;
    videoHeight = pAVCodecContext->height;

    /// 分配AVPicture：RGB24
    avpicture_alloc(&pAVPicture,AV_PIX_FMT_RGB24,videoWidth,videoHeight);

    /// decoder:
    AVCodec *pAVCodec = NULL;

    //获取视频流解码器
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);// eg: h264,h265

    /// color space convert:  SwsContext
    /// sws_getContext, sws_freeContext, sws_scale(...)
    pSwsContext = sws_getContext(
                videoWidth,videoHeight,AV_PIX_FMT_YUV420P,
                videoWidth,videoHeight,AV_PIX_FMT_RGB24,
                SWS_BICUBIC,0,0,0);

    //打开对应解码器
    result = avcodec_open2(pAVCodecContext,pAVCodec,NULL);
    if (result<0){
        qDebug()<<"打开解码器失败 ";
        return false;
    }
    qDebug()<<"初始化视频流成功 ";
    return true;

    return true;
}


void AVFFmpegNetCamera::Deinit(){
    avformat_network_deinit();

    av_frame_free(&pAVFrame);
    avformat_free_context(pAVFormatContext);
    avpicture_free(&pAVPicture);
    sws_freeContext(pSwsContext);

}


/// 开始读取网络摄像头，解封装、解码，预览
void AVFFmpegNetCamera::Play(){
    /// 1. av_read_frame : demuxing
    /// 2. avcodec_send_packet, avcodec_receive_frame: decoding
    /// 3. sws_scale: yuv420p-->rgb24,
    /// 4. QImage, emit signal
    ///
    ///// prepare libx2645 encoder
    ///
    ///判断：如果是rtsp直播摄像头，则录制；如果是回放、则不用录制
    T3FFmpegH2645Encoder2 objFmpgH2645Encoder;
    if( ! this->m_bIsPlayingbacked ){
        /// filename: QDateTime::currentDateTime().toString("yyyy-MM-dd=hhmmss");
        QString strFileName = QDateTime::currentDateTime().toString("yyyy-MM-dd=hhmmss")
                 + "-channel" + QString::number( m_channelIndex)  + ".h264" ;
        objFmpgH2645Encoder.setOutfile( strFileName );
        objFmpgH2645Encoder.setVideoWidth(videoWidth);
        objFmpgH2645Encoder.setVideoHeight(videoHeight);
        objFmpgH2645Encoder.initLibx2645();
    }





    int ret = -1;
    while(1){
        if(m_stopped){
            break;
        }

        if (av_read_frame(pAVFormatContext, &pAVPacket) >= 0){
            if(pAVPacket.stream_index == videoStreamIndex){
                qDebug()<<"开始解码 "<<QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss ");
                /////avcodec_decode_video2(pAVCodecContext, pAVFrame, &frameFinished, &pAVPacket);


                /* send the packet with the compressed data to the decoder */
                ret = avcodec_send_packet(pAVCodecContext, &pAVPacket);
                if (ret < 0)
                {
                    fprintf(stderr, "Error submitting the packet to the decoder\n");
                    continue;
                }

                ret = avcodec_receive_frame(pAVCodecContext, pAVFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    continue;

                /// note: encode the FrameYUV420p
                if( ! this->m_bIsPlayingbacked ){
                    objFmpgH2645Encoder.encodeLibx2645OneFrame( pAVFrame  );
                }


                if (ret >= 0){///frameFinished
                    mutex.lock();
                    sws_scale(pSwsContext,(const uint8_t* const *)pAVFrame->data,pAVFrame->linesize,0,videoHeight,pAVPicture.data,pAVPicture.linesize);
                    //发送获取一帧图像信号
                    QImage image(pAVPicture.data[0],videoWidth,videoHeight,QImage::Format_RGB888);
                    emit GetImage(image);
                    mutex.unlock();
                }
                /// -re  ： 帧率
                QThread::msleep(33);
            }
        }

        //释放资源,否则内存会一直上升
        av_free_packet(&pAVPacket);
    }

    qDebug() << "OK to exit, bye...\n" ;
    if( ! this->m_bIsPlayingbacked ){
        objFmpgH2645Encoder.quitLibx2645();
    }



    return;
}


