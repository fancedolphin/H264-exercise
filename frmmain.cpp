#include "frmmain.h"
#include "ui_frmmain.h"
#include <QDebug>
#include <qpainter.h>
#include <QMouseEvent>
#include <qpropertyanimation.h>
#include <QMessageBox>

#define BORDER_TOP 55
#define BORDER_RIGHT 26
#define BORDER_BOTTOM 23
#define BORDER_LEFT 23

frmMain::frmMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);

    m_bIsPlayingbacked = 0;

    for(int i=0; i<MAX_CHANNEL_COUNT; i++){
        m_pTheFmpg[i] = NULL; //私有成员变量
        m_pFmpgThread[i] = NULL;

        m_ChannelLiving[i] = 0;
    }


    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();

    //设置为无边框窗体
    this->setWindowFlags(Qt::FramelessWindowHint);

    //设置为背景透明
    this->setAttribute(Qt::WA_TranslucentBackground);

    //设置界面背景图
    m_pixmapBg.load(":/image/bg_main.png");

}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::paintEvent(QPaintEvent *e)
{    
    QPainter painter(this);
    //九宫格
    //画左上角
    painter.drawPixmap(QPoint(0, 0), m_pixmapBg,
                       QRect(0, 0, BORDER_LEFT, BORDER_TOP));

    //画顶部
    painter.drawPixmap(QRect(BORDER_LEFT, 0, width() - BORDER_RIGHT - BORDER_LEFT, BORDER_TOP),
                       m_pixmapBg,
                       QRect(BORDER_LEFT, 0, m_pixmapBg.width() - BORDER_RIGHT - BORDER_LEFT, BORDER_TOP));
    //画右上部分
    painter.drawPixmap(QPoint(width() - BORDER_RIGHT, 0), m_pixmapBg,
                       QRect(m_pixmapBg.width() - BORDER_RIGHT, 0, BORDER_RIGHT, BORDER_TOP));
    //画左下部分
    painter.drawPixmap(QPoint(0, height() - BORDER_BOTTOM), m_pixmapBg,
                       QRect(0, m_pixmapBg.height() - BORDER_BOTTOM, BORDER_LEFT, BORDER_BOTTOM));
    //画底部分
    painter.drawPixmap(QRect(BORDER_LEFT, height() - BORDER_BOTTOM, width() - BORDER_RIGHT - BORDER_LEFT, BORDER_BOTTOM),
                       m_pixmapBg,
                       QRect(BORDER_LEFT, m_pixmapBg.height() - BORDER_BOTTOM, m_pixmapBg.width() - BORDER_RIGHT - BORDER_LEFT, BORDER_BOTTOM));
    //画右下部分
    painter.drawPixmap(QPoint(width() - BORDER_RIGHT, height() - BORDER_BOTTOM),
                       m_pixmapBg,
                       QRect(m_pixmapBg.width() - BORDER_RIGHT, m_pixmapBg.height() - BORDER_BOTTOM, BORDER_RIGHT, BORDER_BOTTOM));
    //画左部分
    painter.drawPixmap(QRect(0, BORDER_TOP, BORDER_LEFT, height() - BORDER_BOTTOM - BORDER_TOP),
                       m_pixmapBg,
                       QRect(0, BORDER_TOP, BORDER_LEFT, m_pixmapBg.height() - BORDER_BOTTOM - BORDER_TOP));
    //画右部分
    painter.drawPixmap(QRect(width() - BORDER_RIGHT, BORDER_TOP, BORDER_LEFT, height() - BORDER_BOTTOM - BORDER_TOP),
                       m_pixmapBg,
                       QRect(m_pixmapBg.width() - BORDER_RIGHT, BORDER_TOP, BORDER_LEFT, m_pixmapBg.height() - BORDER_BOTTOM - BORDER_TOP));
    //画中间部分
    painter.drawPixmap(QRect(BORDER_LEFT, BORDER_TOP, width() - BORDER_RIGHT - BORDER_LEFT, height() - BORDER_BOTTOM - BORDER_TOP),
                       m_pixmapBg,
                       QRect(BORDER_LEFT, BORDER_TOP, m_pixmapBg.width() - BORDER_RIGHT - BORDER_LEFT, m_pixmapBg.height() - BORDER_BOTTOM - BORDER_TOP));

}

void frmMain::mouseMoveEvent(QMouseEvent *e)
{
    this->move(e->globalPos() - m_pointStart);
}

void frmMain::mousePressEvent(QMouseEvent *e)
{
    m_pointPress = e->globalPos();
    m_pointStart = m_pointPress - this->pos();
}

void frmMain::on_btnOpen_clicked()
{
    // 获取通道索引
    int nChannelIndex = ui->comboBox_channels->currentIndex();
    /// 如果所选通道已经占用，则直接退出
    if( m_ChannelLiving[nChannelIndex] ){
        QMessageBox::information(NULL,  "Warning",  "Current Channel is busy.", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    /// 将所选通道设置为 “忙碌”
    m_ChannelLiving[nChannelIndex] = 1;


    QString strUrl = ui->txtUrl->text();
    if(strUrl.indexOf("rtsp", 0) >= 0 ){
        m_bIsPlayingbacked = 0;
    }
    else{
        m_bIsPlayingbacked = 1;
    }

    m_pTheFmpg[nChannelIndex] = new AVFFmpegNetCamera(this);

    /// 胆子要够大：：等着栽更头
    if(m_pTheFmpg[nChannelIndex]){
        // init, play, deinit
        m_pTheFmpg[nChannelIndex]->SetUrl( strUrl );
        m_pTheFmpg[nChannelIndex]->setChannelIndex(ui->comboBox_channels->currentIndex());
        ///设置：是否回放模式
        m_pTheFmpg[nChannelIndex]->setIsPlayingbacked(m_bIsPlayingbacked);
        ///"rtsp://127.0.0.1:8554/aabb"
        m_pTheFmpg[nChannelIndex]->Init();


        /*
        坑5：
        解码成功了，但是界面上没有图像
        原因：没有绑定 信号对应的 槽函数
        connect(.....)
        */
        if(nChannelIndex == 0){ // channel:1
            connect(m_pTheFmpg[nChannelIndex], SIGNAL(GetImage(QImage)),
                    this, SLOT(SetImage1(QImage)));
        }
        else if(nChannelIndex == 1){ // channel:2
            connect(m_pTheFmpg[nChannelIndex], SIGNAL(GetImage(QImage)),
                    this, SLOT(SetImage2(QImage)));
        }
        else if(nChannelIndex == 2){ // channel:3
            connect(m_pTheFmpg[nChannelIndex], SIGNAL(GetImage(QImage)),
                    this, SLOT(SetImage3(QImage)));
        }
        else if(nChannelIndex == 3){ // channel:4
            connect(m_pTheFmpg[nChannelIndex], SIGNAL(GetImage(QImage)),
                    this, SLOT(SetImage4(QImage)));
        }


        ///// start a new thread , to run decoding(rtsp)
        m_pFmpgThread[nChannelIndex] = new AVFFmpegCameraThread(this);
        if(m_pFmpgThread[nChannelIndex]){
            m_pFmpgThread[nChannelIndex]->setFFmpeg(m_pTheFmpg[nChannelIndex]);
            m_pFmpgThread[nChannelIndex]->start();
        }

    }
}


void frmMain::SetImage1(const QImage &image)
{
    int tempWidth = ui->labVideo1->geometry().width();
    int tempHeight = ui->labVideo1->geometry().height();
    if (image.height() > 0) {
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth, tempHeight));
        ui->labVideo1->setPixmap(pix);
    }
}

void frmMain::SetImage2(const QImage &image)
{
    int tempWidth = ui->labVideo2->geometry().width();
    int tempHeight = ui->labVideo2->geometry().height();
    if (image.height() > 0) {
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth, tempHeight));
        ui->labVideo2->setPixmap(pix);
    }
}


void frmMain::SetImage3(const QImage &image)
{
    int tempWidth = ui->labVideo3->geometry().width();
    int tempHeight = ui->labVideo3->geometry().height();
    if (image.height() > 0) {
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth, tempHeight));
        ui->labVideo3->setPixmap(pix);
    }
}

void frmMain::SetImage4(const QImage &image)
{
    int tempWidth = ui->labVideo4->geometry().width();
    int tempHeight = ui->labVideo4->geometry().height();
    if (image.height() > 0) {
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth, tempHeight));
        ui->labVideo4->setPixmap(pix);
    }
}


void frmMain::removelayout1()
{
    ui->lay1->removeWidget(ui->labVideo1);
    ui->lay1->removeWidget(ui->labVideo2);
    ui->lay1->removeWidget(ui->labVideo3);
    ui->lay1->removeWidget(ui->labVideo4);

    ui->labVideo1->setVisible(false);
    ui->labVideo2->setVisible(false);
    ui->labVideo3->setVisible(false);
    ui->labVideo4->setVisible(false);
}

void frmMain::removelayout2()
{
    ui->lay2->removeWidget(ui->labVideo5);
    ui->lay2->removeWidget(ui->labVideo6);
    ui->lay2->removeWidget(ui->labVideo7);
    ui->lay2->removeWidget(ui->labVideo8);

    ui->labVideo5->setVisible(false);
    ui->labVideo6->setVisible(false);
    ui->labVideo7->setVisible(false);
    ui->labVideo8->setVisible(false);
}

void frmMain::removelayout3()
{
    ui->lay3->removeWidget(ui->labVideo9);
    ui->lay3->removeWidget(ui->labVideo10);
    ui->lay3->removeWidget(ui->labVideo11);
    ui->lay3->removeWidget(ui->labVideo12);

    ui->labVideo9->setVisible(false);
    ui->labVideo10->setVisible(false);
    ui->labVideo11->setVisible(false);
    ui->labVideo12->setVisible(false);
}

void frmMain::removelayout4()
{
    ui->lay4->removeWidget(ui->labVideo13);
    ui->lay4->removeWidget(ui->labVideo14);
    ui->lay4->removeWidget(ui->labVideo15);
    ui->lay4->removeWidget(ui->labVideo16);

    ui->labVideo13->setVisible(false);
    ui->labVideo14->setVisible(false);
    ui->labVideo15->setVisible(false);
    ui->labVideo16->setVisible(false);
}

void frmMain::on_btn1_clicked()
{
    removelayout1();
    removelayout2();
    removelayout3();
    removelayout4();
    qDebug() << "width:" << this->frameGeometry().width();
    qDebug() << "hight:" << this->frameGeometry().height();

    ui->layMain->setStretch(0, 0);
    ui->layMain->setStretch(1, 0);
    ui->layMain->setStretch(2, 0);
    ui->layMain->setStretch(3, 0);
    ui->lay1->addWidget(ui->labVideo1);
    ui->labVideo1->setVisible(true);
}

void frmMain::on_btn4_clicked()
{
    removelayout1();
    removelayout2();
    removelayout3();
    removelayout4();
    int nWidth = this->frameGeometry().width();
    int nHeight = this->frameGeometry().height();
    qDebug() << "width:" << nWidth;
    qDebug() << "hight:" << nHeight;

    ui->layMain->setStretch(0, 0);
    ui->layMain->setStretch(1, 0);
    ui->layMain->setStretch(2, 0);
    ui->layMain->setStretch(3, 0);

    ui->layMain->setStretch(0, 1);
    ui->layMain->setStretch(1, 1);


    ui->lay1->addWidget(ui->labVideo1);
    ui->labVideo1->setVisible(true);

    ui->lay1->addWidget(ui->labVideo2);
    ui->labVideo2->setVisible(true);
    ui->lay1->setStretch(0,0);
    ui->lay1->setStretch(1,0);
    ui->lay1->setStretch(2,0);
    ui->lay1->setStretch(3,0);
    ui->lay1->setStretch(0,1);
    ui->lay1->setStretch(1,1);


    ui->lay2->addWidget(ui->labVideo3);
    ui->labVideo3->setVisible(true);

    ui->lay2->addWidget(ui->labVideo4);
    ui->labVideo4->setVisible(true);

    ui->lay2->setStretch(0,0);
    ui->lay2->setStretch(1,0);
    ui->lay2->setStretch(2,0);
    ui->lay2->setStretch(3,0);
    ui->lay2->setStretch(0,1);
    ui->lay2->setStretch(1,1);
}

void frmMain::on_btn9_clicked()
{
    removelayout1();
    removelayout2();
    removelayout3();
    removelayout4();

    ui->layMain->setStretch(0, 0);
    ui->layMain->setStretch(1, 0);
    ui->layMain->setStretch(2, 0);
    ui->layMain->setStretch(3, 0);

    ui->layMain->setStretch(0, 1);
    ui->layMain->setStretch(1, 1);
    ui->layMain->setStretch(2, 1);

    ui->lay1->addWidget(ui->labVideo1);
    ui->labVideo1->setVisible(true);
    ui->lay1->addWidget(ui->labVideo2);
    ui->labVideo2->setVisible(true);
    ui->lay1->addWidget(ui->labVideo3);
    ui->labVideo3->setVisible(true);

    ui->lay2->addWidget(ui->labVideo4);
    ui->labVideo4->setVisible(true);
    ui->lay2->addWidget(ui->labVideo5);
    ui->labVideo5->setVisible(true);
    ui->lay2->addWidget(ui->labVideo6);
    ui->labVideo6->setVisible(true);

    ui->lay3->addWidget(ui->labVideo7);
    ui->labVideo7->setVisible(true);
    ui->lay3->addWidget(ui->labVideo8);
    ui->labVideo8->setVisible(true);
    ui->lay3->addWidget(ui->labVideo9);
    ui->labVideo9->setVisible(true);
}

void frmMain::on_btn16_clicked()
{
    removelayout1();
    removelayout2();
    removelayout3();
    removelayout4();

    ui->layMain->setStretch(0, 0);
    ui->layMain->setStretch(1, 0);
    ui->layMain->setStretch(2, 0);
    ui->layMain->setStretch(3, 0);

    ui->layMain->setStretch(0, 1);
    ui->layMain->setStretch(1, 1);
    ui->layMain->setStretch(2, 1);
    ui->layMain->setStretch(3, 1);

    ui->lay1->addWidget(ui->labVideo1);
    ui->labVideo1->setVisible(true);
    ui->lay1->addWidget(ui->labVideo2);
    ui->labVideo2->setVisible(true);
    ui->lay1->addWidget(ui->labVideo3);
    ui->labVideo3->setVisible(true);
    ui->lay1->addWidget(ui->labVideo4);
    ui->labVideo4->setVisible(true);

    ui->lay2->addWidget(ui->labVideo5);
    ui->labVideo5->setVisible(true);
    ui->lay2->addWidget(ui->labVideo6);
    ui->labVideo6->setVisible(true);
    ui->lay2->addWidget(ui->labVideo7);
    ui->labVideo7->setVisible(true);
    ui->lay2->addWidget(ui->labVideo8);
    ui->labVideo8->setVisible(true);

    ui->lay3->addWidget(ui->labVideo9);
    ui->labVideo9->setVisible(true);
    ui->lay3->addWidget(ui->labVideo10);
    ui->labVideo10->setVisible(true);
    ui->lay3->addWidget(ui->labVideo11);
    ui->labVideo11->setVisible(true);
    ui->lay3->addWidget(ui->labVideo12);
    ui->labVideo12->setVisible(true);

    ui->lay4->addWidget(ui->labVideo13);
    ui->labVideo13->setVisible(true);
    ui->lay4->addWidget(ui->labVideo14);
    ui->labVideo14->setVisible(true);
    ui->lay4->addWidget(ui->labVideo15);
    ui->labVideo15->setVisible(true);
    ui->lay4->addWidget(ui->labVideo16);
    ui->labVideo16->setVisible(true);
}

void frmMain::on_btn_min_clicked()
{
    if (!this->isMinimized()) {
        this->showMinimized();
    }
}

void frmMain::on_btn_max_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();
    } else {
        this->showMaximized();
    }
}

void frmMain::on_btn_close_clicked()
{
    qApp->exit();
}

/// stop camera
void frmMain::on_btnStop_clicked()
{
    int nChannelIndex = ui->comboBox_channels->currentIndex();
    m_ChannelLiving[nChannelIndex] = 0;

    if(m_pTheFmpg[nChannelIndex]){
        m_pTheFmpg[nChannelIndex]->setStopped(1);
        QThread::msleep(500);
    }

    if(m_pFmpgThread[nChannelIndex]){
        delete m_pFmpgThread[nChannelIndex];
        m_pFmpgThread[nChannelIndex] = NULL;
    }

    if(m_pTheFmpg[nChannelIndex]){
        delete m_pTheFmpg[nChannelIndex];
        m_pTheFmpg[nChannelIndex] = NULL;
    }
}


void frmMain::on_btn_menu_clicked()
{
    DialogSetting dlgSettings;// 自添加的对话框
    if (dlgSettings.exec() == QDialog::Accepted) {
        qDebug() << "main::" << dlgSettings.getCurFileName();

        /// 回放模式：默认用1画面，通道用1
        m_bIsPlayingbacked = 1;
        on_btn1_clicked();
        ui->comboBox_channels->setCurrentIndex(0);
        ui->txtUrl->setText( dlgSettings.getCurFileName() );
        on_btnOpen_clicked();
    }
}
