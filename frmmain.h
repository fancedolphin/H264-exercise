#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include <qDebug>
#include "avffmpegnetcamera.h"
#include "avffmpegcamerathread.h"

#include "dialogsetting.h"

#define MAX_CHANNEL_COUNT 16

namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected slots:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private slots:
    void SetImage1(const QImage &image);
    void SetImage2(const QImage &image);
    void SetImage3(const QImage &image);
    void SetImage4(const QImage &image);

    void on_btnOpen_clicked();

    void on_btn1_clicked();

    void on_btn4_clicked();

    void on_btn9_clicked();

    void on_btn16_clicked();

    void on_btn_min_clicked();

    void on_btn_max_clicked();

    void on_btn_close_clicked();

    void on_btnStop_clicked();

    void on_btn_menu_clicked();

private:
    Ui::frmMain *ui;

    QPixmap m_pixmapBg;
    QPoint m_pointStart;
    QPoint m_pointPress;

    void removelayout1();
    void removelayout2();
    void removelayout3();
    void removelayout4();

    AVFFmpegNetCamera * m_pTheFmpg[MAX_CHANNEL_COUNT]; //私有成员变量
    AVFFmpegCameraThread * m_pFmpgThread[MAX_CHANNEL_COUNT];
    int m_ChannelLiving[MAX_CHANNEL_COUNT]; //标识当前通道是否占用

    int  m_bIsPlayingbacked;
};

#endif // FRMMAIN_H
