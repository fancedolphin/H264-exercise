#include "dialogsetting.h"
#include "ui_dialogsetting.h"
#include <QDir>



static QStringList getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.h264" << "*.h265";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

    return files;
}


DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting)
{
    ui->setupUi(this);
}

DialogSetting::~DialogSetting()
{
    delete ui;
}

void DialogSetting::on_calendarWidget_date_selectionChanged()
{
    /// 1. get now date
    /// 2. get files: matching "now date"
    qDebug() << ui->calendarWidget_date->selectedDate().toString() << " ";
    QString strSelectedDate = ui->calendarWidget_date->selectedDate().toString("yyyy-MM-dd");
    qDebug() << strSelectedDate;

    /// get files
    ui->listWidget_Videos->clear();
    QStringList files = getFileNames("./");
    for (int i = 0; i < files.size() ; i++) {
        qDebug() << "::" << files[i] << "\n";

        if(files[i].indexOf( strSelectedDate ) >= 0 ){
            ui->listWidget_Videos->addItem(files[i]);
        }
    }
}


void DialogSetting::on_listWidget_Videos_itemSelectionChanged()
{
    ///qDebug() << ui->listWidget_Videos->currentItem()->text();
}



void DialogSetting::on_listWidget_Videos_itemDoubleClicked(QListWidgetItem *item)
{
    qDebug() << item->text();
    m_CurFileName = item->text();
    this->done(1);
}
