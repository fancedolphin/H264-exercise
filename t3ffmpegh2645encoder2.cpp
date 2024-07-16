#include "t3ffmpegh2645encoder2.h"

T3FFmpegH2645Encoder2::T3FFmpegH2645Encoder2(QObject *parent) : QObject(parent)
{
    /// init the member variables to zero
    pFormatCtx = NULL;
    pOutputFmt = NULL;
    pStream = NULL;
    pCodecCtx = NULL;
    pCodec = NULL;
    pkt = NULL;
    pFrame = NULL;

    out_file = NULL;
    pFrameBuf = NULL;
    m_frameIndex = 0;

    in_w = 640;
    in_h = 480;
}


int T3FFmpegH2645Encoder2::_encode(
            AVCodecContext *pCodecCtx,
            AVPacket * pPkt,
            AVFrame  * pFrame,
            FILE* out_file /* = NULL*/){//pole
    /// step 1: avcodec_send_frame
    /// step 2: avcodec_receive_packet

    int got_packet = 0;
    int ret = avcodec_send_frame(pCodecCtx, pFrame);
    if (ret < 0) {
        //failed to send frame for encoding
        return -1;
    }
    while (!ret) {
        ret = avcodec_receive_packet(pCodecCtx, pPkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        }else if (ret < 0) {
            //error during encoding
            return -1;
        }

        printf("Write frame %d, size=%d\n", pPkt->pts, pPkt->size);
        /// write the pack data to local file
        fwrite(pPkt->data, 1, pPkt->size, out_file);
        av_packet_unref(pPkt);
    }

    return 0;
}


int T3FFmpegH2645Encoder2::initLibx2645(){
    /// 1: define  variables: structures, .....
    /// 2: openfile,
    /// 3: ffmpeg: workflow,
    /// init, open_input, find_stream_info
    /// codec:
    /// sws_context


    /// read local file: binary
    /// yuvtest1-352x288-yuv420p.yuv

    /// output file:

    int nFrameNum = 100;
    char * pstrOutfile = this->m_outfile.toLocal8Bit().data();
    out_file = fopen(this->m_outfile.toLocal8Bit().data(), "wb");
    if (out_file == NULL) {
        printf("cannot create out file\n");
        return -1;
    }


    /// prepare codec
    uint8_t* pFrameBuf = NULL;
    int frame_buf_size = 0;
    int y_size = 0;
    int nEncodedFrameCount = 0;

    /////av_register_all();
    pFormatCtx = avformat_alloc_context();
    pOutputFmt = av_guess_format(NULL, this->m_outfile.toLocal8Bit().data(), NULL);
    pFormatCtx->oformat = pOutputFmt;

    //除了以下方法，另外还可以使用avcodec_find_encoder_by_name()来获取AVCodec
    pCodec = avcodec_find_encoder(pOutputFmt->video_codec);
    if (!pCodec) {
        //cannot find encoder
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx) {
        //failed get AVCodecContext
        return -1;
    }
    pkt = av_packet_alloc();
    if (!pkt){
        return -1;
    }

    //pCodecCtx = pStream->codec;
    pCodecCtx->codec_id = pOutputFmt->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecCtx->width = in_w;
    pCodecCtx->height = in_h;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;
    //pCodecCtx->time_base = (AVRational){ 1, 25 };
    pCodecCtx->bit_rate = 400000;   //
    pCodecCtx->gop_size = 250;      //
    //pCodecCtx->framerate = (AVRational){ 25, 1 };
    //H264
    //pCodecCtx->me_range = 16;     //
    //pCodecCtx->max_qdiff = 4;     //
    //pCodecCtx->qcompress = 0.6;   //
    pCodecCtx->qmin = 10;           //
    pCodecCtx->qmax = 51;           //
    //Optional Param
    pCodecCtx->max_b_frames = 3;



    /// encode :
    //Set Option
    AVDictionary *param = NULL;
    //H.264
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        ///av_dict_set(&param, "profile", "main", 0);
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    //H.265
    if (pCodecCtx->codec_id == AV_CODEC_ID_HEVC) {//AV_CODEC_ID_HEVC,AV_CODEC_ID_H265
        av_dict_set(&param, "profile", "main", 0);
        ///av_dict_set(&param, "preset", "ultrafast", 0);
        ///// note uncompatilable://av_dict_set(&param, "tune", "zero-latency", 0);
    }

    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
        //failed to open codec
        return -1;
    }

    pFrame = av_frame_alloc();
    if (!pFrame) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        return -1;
    }
    pFrame->format = pCodecCtx->pix_fmt;
    pFrame->width = pCodecCtx->width;
    pFrame->height = pCodecCtx->height;

    int ret = av_frame_get_buffer(pFrame, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        return -1;
    }

    frame_buf_size = av_image_get_buffer_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);
    pFrameBuf = (uint8_t*)av_malloc(frame_buf_size);
    av_image_fill_arrays(pFrame->data, pFrame->linesize,
        pFrameBuf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 1);

    y_size = pCodecCtx->width * pCodecCtx->height;


    return 0;
}


int T3FFmpegH2645Encoder2::quitLibx2645() // inpara: camera one frame (yuv420p)
{
    //flush the encoder
    uint8_t endcode[] = { 0, 0, 1, 0xb7 }; // end of sequence
    _encode(pCodecCtx, pkt,  NULL, out_file);

    //add sequence end code to have a real MPEG file
    fwrite(endcode, 1, sizeof(endcode), out_file);

    avcodec_free_context(&pCodecCtx);
    avformat_free_context(pFormatCtx);
    av_frame_free(&pFrame);
    av_packet_free(&pkt);
    av_free(pFrameBuf);


    if (out_file)
        fclose(out_file);

    return 0;
}

/// encode per frame for the local camera: libx264,5
int T3FFmpegH2645Encoder2::encodeLibx2645OneFrame(AVFrame * pFrameYUV420p){

    //Read local camera: yuv420p
    int  ret = av_frame_make_writable(pFrame);
    if (ret < 0) {
        return -1;
    }

    /// pointer to local frame
    pFrame->data[0] = pFrameYUV420p->data[0];   //Y
    pFrame->data[1] = pFrameYUV420p->data[1];   //U
    pFrame->data[2] = pFrameYUV420p->data[2];   //V

    //PTS
    pFrame->pts = m_frameIndex++;

    //encode
    _encode(pCodecCtx, pkt, pFrame,   out_file);

    return 0;
}
