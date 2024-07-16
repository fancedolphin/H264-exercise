#include "avffmpegcamerathread.h"

AVFFmpegCameraThread::AVFFmpegCameraThread(QObject *parent) : QThread(parent)
{
    m_pFmpgNetCamera = NULL;
}


void AVFFmpegCameraThread::run(){
    if(m_pFmpgNetCamera){
        m_pFmpgNetCamera->Play();
    }
}
