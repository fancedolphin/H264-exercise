#ifndef T3FFMPEGH2645ENCODER2_H
#define T3FFMPEGH2645ENCODER2_H

#include <QObject>
extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
};
#include <iostream>
using namespace  std;


class T3FFmpegH2645Encoder2 : public QObject
{
    Q_OBJECT
public:
    explicit T3FFmpegH2645Encoder2(QObject *parent = nullptr);

    ///// init, encode,  quit
    int initLibx2645();
    int quitLibx2645();
    int encodeLibx2645OneFrame(AVFrame * pFrameYUV420p); // inpara: camera one frame (yuv420p)
    void setOutfile(QString strfilename){this->m_outfile = strfilename;}
    void setVideoWidth(int ww){in_w =  ww;}
    void setVideoHeight(int hh) {in_h = hh; }

private:
    int _encode(AVCodecContext *avCodecCtx,
                AVPacket * pack,
                AVFrame  * frame,
                FILE* fp = NULL);

private:
    AVFormatContext *pFormatCtx;
    AVOutputFormat *pOutputFmt;
    AVStream *pStream;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVPacket *pkt;
    AVFrame *pFrame;

    FILE *out_file;
    QString m_outfile;
    uint8_t* pFrameBuf;
    int m_frameIndex;
    int in_w, in_h;


signals:

public slots:
};

#endif // T3FFMPEGH2645ENCODER2_H
