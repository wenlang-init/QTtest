#include "custombtn.h"
#include <QPainter>
#include <QColor>
#include <QFont>

// #include <QDesktopServices>
// #include <QFontMetrics>
// #include <QByteArray>
// #include <QTextCodec>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

CustomBtn::CustomBtn(QWidget *parent, QString img_url, QString text)
    : BaseWidget(parent)
    , status(false)
    , _status(false)
{
    img_str = img_url;
    show_text = text;

    // this->setStyleSheet("background-color:rgba(0,0,0,0)");

    color_checked.setRgb(128, 128, 128, 255);

    // color_checked.setNamedColor(QString("#af00ff00"));
    color_hover.setRgb(0, 0, 0, 0);

    color_pr.setRgb(34, 233, 51, 255);

    icon.load(img_str);

    label_image = new QLabel(this);
    label_text = new QLabel(this);

    label_image->setPixmap(icon);
    label_text->setText(text);

    label_image->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label_text->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // QPalette pe;
    // pe.setColor(QPalette::WindowText,QColor(Qt::black));
    label_text->setPalette(pe);
    label_text->setFont(font);

    // label_text->setFont(QFont("Microsoft Yahei", 8, QFont::Normal));

    this->resize(60, 80);

    connect(this, SIGNAL(Key_Pass_Signal(int,int)), this,
            SLOT(Key_Pass_Slot(int,int)));
}

void CustomBtn::setimage(QString img_url)
{
    img_str = img_url;
    icon.load(img_str);
    label_image->setPixmap(icon);
    this->repaint();
}

void CustomBtn::setpixmap(const QPixmap& pixmap)
{
    label_image->setPixmap(pixmap);
    this->repaint();
}

void CustomBtn::settext(QString text)
{
    show_text = text;
    label_text->setText(show_text);
    this->repaint();
}

void CustomBtn::settext_fontcolor(QFont textfont, QColor textColor)
{
    QPalette pe;

    pe.setColor(QPalette::WindowText, textColor);
    label_text->setPalette(pe);
    label_text->setFont(textfont);
    label_text->repaint();
}

void CustomBtn::set_checked(QColor color)
{
    color_checked = color;
}

void CustomBtn::set_hover(QColor color)
{
    color_hover = color;
}

void CustomBtn::set_focus(bool state)
{
    status = state;
}

bool CustomBtn::get_focus()
{
    return status;
}

void CustomBtn::resizeEvent(QResizeEvent *event)
{
    label_image->setGeometry(0, 0, this->width(), this->height() - 20);
    label_text->setGeometry(0, this->height() - 20, this->width(), 20);

    QWidget::resizeEvent(event);
}

void CustomBtn::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QPen     pen;

    if (_status == true) {
        painter.setBrush(QBrush(color_pr));

        // painter.setPen(QColor(0,0,0,255)); // 边框色
    } else {
        painter.setBrush(QBrush(color_hover));

        // painter.setPen(QColor(0,0,0,255)); // 边框色
    }

    if (status == true) { // 选中
        pen.setColor(color_checked);
    } else {
        pen.setColor(QColor(128, 128, 128, 0));
    }
    painter.setPen(pen);                           // 边框色

    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿;
    painter.drawRoundedRect(this->rect(), this->width() / 5, this->height() / 5);

    QWidget::paintEvent(event);
}

void CustomBtn::mousePressEvent(QMouseEvent *event)
{
    _status = true;
    this->repaint();
    QWidget::mousePressEvent(event);
    emit mousePressed();
    mpoint = cursor().pos();
}

void CustomBtn::mouseReleaseEvent(QMouseEvent *event)
{
    _status = false;
    this->repaint();
    QWidget::mouseReleaseEvent(event);

    if ((qAbs(mpoint.x() - cursor().pos().x()) < 10) &&
        (qAbs(mpoint.y() - cursor().pos().y()) < 10)) {
        emit clicked();
        emit clicked(this);
    }
    emit mouseRelease();
}

void CustomBtn::Key_Pass_Slot(int key, int index)
{
    switch (key) {
    case 0:

        break;

    default:
        break;
    }
}
