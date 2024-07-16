#ifndef AVFFMPEGNETCAMERA_H
#define AVFFMPEGNETCAMERA_H

#include <QObject>
#include <QMutex>
#include <QImage>
#include <QThread>

extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libswscale/swscale.h>
    #include <libavutil/frame.h>
};

#include "t3ffmpegh2645encoder2.h"



class AVFFmpegNetCamera : public QObject
{
    Q_OBJECT
public:
    explicit AVFFmpegNetCamera(QObject *parent = nullptr);

    bool Init();
    void Play();
    void Deinit();

    void SetUrl(QString url){this->url=url;}
    QString Url()const{return url;}
    int VideoWidth()const{ return videoWidth; }
    int VideoHeight()const{return videoHeight;}
    void setStopped(int st){ this->m_stopped = st; }
    void setChannelIndex(int ci) {this->m_channelIndex = ci;}
    void setIsPlayingbacked(int np){this->m_bIsPlayingbacked = np; }

    /// declare member variables:
private:
    QMutex mutex;
    AVFormatContext *pAVFormatContext;
    AVCodecContext *pAVCodecContext;
    AVFrame *pAVFrame;
    SwsContext * pSwsContext;
    AVPacket pAVPacket;
    AVPicture  pAVPicture;

    QString url;
    int videoWidth;
    int videoHeight;
    int videoStreamIndex;
    int m_channelIndex;

    int m_stopped;
    int m_bIsPlayingbacked;

signals:
    void GetImage(const QImage &image);

public slots:

};

#endif // AVFFMPEGNETCAMERA_H
