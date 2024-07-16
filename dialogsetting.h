#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>
#include <qDebug>
#include <QListWidgetItem>

namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetting(QWidget *parent = nullptr);
    ~DialogSetting();

    QString getCurFileName(){ return m_CurFileName; }

private slots:
    void on_calendarWidget_date_selectionChanged();

    void on_listWidget_Videos_itemSelectionChanged();

    void on_listWidget_Videos_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::DialogSetting *ui;

    QString m_CurFileName;
};

#endif // DIALOGSETTING_H
