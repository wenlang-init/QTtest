#include "waitdialog.h"
#include <QColor>
WaitDialog::WaitDialog(QWidget *parent)
    : QDialog(parent)
{
    // 如果需要显示任务栏对话框则删除Qt::Tool
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

    // setWindowFlags(Qt::FramelessWindowHint | Qt::Tool |
    // Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    control_init();
}

WaitDialog::~WaitDialog()
{
    delete movie;
    delete labelMovie;
    delete labelText;
    delete btnCancel;
    delete centerFrame;
}

void WaitDialog::control_init()
{
    this->setFixedSize(250, 260);
    centerFrame = new QFrame(this);
    centerFrame->setGeometry(10, 10, this->width() - 20, this->height() - 20);


    // 加载Loading动画
    labelMovie = new QLabel();
    movie = new QMovie(":/image/wait.gif");

    int size = qMin(labelMovie->width(), labelMovie->height());
    movie->setScaledSize(QSize(size, size));

    labelMovie->setScaledContents(true);
    labelMovie->setMovie(movie);
    movie->start();

    // 提示文本
    labelText = new QLabel();
    labelText->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    labelText->setText("");
    labelText->setMinimumHeight(35);
    labelText->setObjectName("labelText");
    labelText->setFont(QFont("Microsoft YaHei", 10));
    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor(0, 0, 0));
    labelText->setPalette(pe);

    // labelText->setStyleSheet("color:#ff6600;");
    // labelText->setStyleSheet("color:red;");
    // QLabel("<h2><i>Hello</i><font color=red>Qt!</font></h2>");

    // 取消按钮
    btnCancel = new QPushButton();
    btnCancel->setObjectName("btnCancel");
    btnCancel->setText("取消等待");
    btnCancel->setStyleSheet("QPushButton#btnCancel{"
                             "background-color: rgba(255,127,39,130);"
                             "border-radius: 4px;"
                             "font-family: \"Microsoft YaHei\";"
                             "font-size: 14px;"
                             "color: #333333;"
                             "}"
                             "QPushButton#btnCancel::hover{"
                             "background:rgba(255,127,39, 170)"
                             "}"
                             "QPushButton#btnCancel::pressed{"
                             "background:rgba(255,127,39, 255)"
                             "}");
    btnCancel->setMinimumHeight(35);
    btnCancel->setEnabled(true);
    connect(btnCancel, &QPushButton::clicked, this, &WaitDialog::btn_click_slot);

    btnCancel->hide();

    set_btn_cancel(false);


    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setContentsMargins(2, 2, 2, 2);
    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));


    verticalLayout->addWidget(labelMovie);
    verticalLayout->addWidget(labelText);
    verticalLayout->addWidget(btnCancel);

    verticalLayout->setStretch(0, 4);
    verticalLayout->setStretch(1, 3);
    verticalLayout->setStretch(2, 1);

    gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

    centerFrame->setLayout(gridLayout);


    // 实例阴影shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(32, 101, 165));
    shadow->setBlurRadius(10);
    this->setGraphicsEffect(shadow);
}

void WaitDialog::set_text(QString strTipsText)
{
    labelText->setText(strTipsText);
    labelText->repaint();
}

void WaitDialog::set_btn_cancel(bool bCanCancel)
{
    btnCancel->setEnabled(bCanCancel);
}

void WaitDialog::set_btn_cancel_show(bool isshow)
{
    if (isshow) btnCancel->show();
    else btnCancel->hide();
}

void WaitDialog::move_to_center(QWidget *pParent)
{
    if ((pParent != nullptr) && (pParent != NULL))
    {
        int nParentWidth = pParent->width();
        int nParentHeigth = pParent->height();

        int nWidth = this->width();
        int nHeight = this->height();

        int nParentX = pParent->x();
        int nParentY = pParent->y();

        int x = (nParentX + (nParentWidth - nWidth) / 2);
        int y = (nParentY + (nParentHeigth - nHeight) / 2);

        this->move(x, y);
    }
}

void WaitDialog::set_progress(quint8 progress)
{
    if (progress > 100) progress = 100;
    float a = progress / 100.0;

    if (a >= 1) a = 0.999;

    labelText->setStyleSheet(QString(
                                 "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, "
                                 "stop:0 rgba(0, 168,243, 100), "
                                 "stop:%1 rgba(0, 168, 243, 100),"
                                 "stop:%2 rgba(255, 255, 255, 255), "
                                 "stop:1 rgba(255, 255, 255, 255));").arg(
                                 a).arg(a + 0.000004));
}

void WaitDialog::btn_click_slot()
{
    emit cancel_waiting_signal();
}

void WaitDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing); // 反锯齿
    painter.setBrush(QBrush(Qt::white));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setLeft(9);
    rect.setTop(9);
    rect.setWidth(rect.width() - 9);
    rect.setHeight(rect.height() - 9);
    painter.drawRoundedRect(rect, 8, 8);
    QWidget::paintEvent(event);
}
