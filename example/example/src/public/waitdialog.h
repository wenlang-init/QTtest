#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QWidget>
#include <QMovie>
#include <QLabel>
#include <QDialog>
#include <QPainter>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPainterPath>
#include <QFontMetrics>
class WaitDialog : public QDialog {
    Q_OBJECT

Q_SIGNALS:

    void cancel_waiting_signal();

public:

    explicit WaitDialog(QWidget *parent = nullptr);
    ~WaitDialog();

    // 设置提示文本
    void set_text(QString strTipsText);

    // 设置是否显示取消等待按钮
    void set_btn_cancel(bool bCanCancel);
    void set_btn_cancel_show(bool isshow = true);

    // 移动到指定窗口中间显示
    void move_to_center(QWidget *pParent);

    // 百分比显示(0~100)
    void set_progress(quint8 progress);

protected:

    void paintEvent(QPaintEvent *event) override;

private:

    void control_init();

private slots:

    void btn_click_slot();

private:

    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QFrame *centerFrame;
    QLabel *labelMovie;
    QMovie *movie;
    QLabel *labelText;
    QPushButton *btnCancel;
};

#endif // WAITDIALOG_H
