#ifndef AVFFMPEGCAMERATHREAD_H
#define AVFFMPEGCAMERATHREAD_H

#include <QObject>
#include <QThread>
#include "AVFFmpegNetCamera.h"

class AVFFmpegCameraThread : public QThread
{
    Q_OBJECT
public:
    explicit AVFFmpegCameraThread(QObject *parent = nullptr);
    void run();
    void setFFmpeg(AVFFmpegNetCamera * pfmpg){ m_pFmpgNetCamera = pfmpg; }

private:
    AVFFmpegNetCamera *m_pFmpgNetCamera;

signals:
public slots:
};

#endif // AVFFMPEGCAMERATHREAD_H
