#include "virtual_inpue__widget.h"
#include "input_management_object.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QApplication>

#define background_input_is_transparence 1 // 背景透明

enum KEY_DEFINE_BD {
    KEY_DEFINE_BD_0,
    KEY_DEFINE_BD_1,
    KEY_DEFINE_BD_2,
    KEY_DEFINE_BD_3,
    KEY_DEFINE_BD_4,
    KEY_DEFINE_BD_5,
    KEY_DEFINE_BD_6,
    KEY_DEFINE_BD_7,
    KEY_DEFINE_BD_8,
    KEY_DEFINE_BD_9,
    KEY_DEFINE_BD_F1,
    KEY_DEFINE_BD_NAVIGATION,
    KEY_DEFINE_BD_UP,
    KEY_DEFINE_BD_DOWN,
    KEY_DEFINE_BD_LEFT,
    KEY_DEFINE_BD_RIGHT,
    KEY_DEFINE_BD_ENTER,
    KEY_DEFINE_BD_F2,
    KEY_DEFINE_BD_LOCK,
    KEY_DEFINE_BD_SWITCH
};

T9_Widget::T9_Widget(QWidget *parent, int width, int height) : QDialog(parent),
    input_state(1),
    input_state_last(1),
    ti_width(width),
    ti_height(height),
    ceng_mode(true),
    pycount(0),
    fuhao_rowcnt(1)
{
    twidget_more = NULL;
    background_color = QColor(255, 255, 255, 100);

    Input_Management_Object& input_obj = Input_Management_Object::instance();

    easyinput = input_obj.get_t26obj();
    mT9obj = input_obj.get_t9obj();

    // easyinput = new easyInput(this);

    // QPalette pe;
    // pe.setColor(QPalette::WindowText,QColor(Qt::black));
    QFont font("Microsoft Yahei", 8, QFont::Normal);

    // font.setPointSize(8);
    QPalette pl;

    if (background_input_is_transparence == 0) {
        pl.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255, 255)));
    } else {
        pl.setBrush(QPalette::Base, QBrush(QColor(0, 0, 0, 0)));
    }

    // this->setPalette(pl);

    strlisthead << "中文" << "英文" << "网络" << "数学" << "特殊" << "序号" << "部首" << "希腊" <<
        "拉丁";
    strlist = new QStringList *[strlisthead.count()];

    for (int i = 0; i < strlisthead.count(); i++) {
        strlist[i] = new QStringList;
    }
    *strlist[0] << "，" << "。" << "？" << "！" << "~" << "、" << "：" << "＇" << "；" <<
        "‘" << "’" << "——" << "（" << "）" << "！" << "【" << "】" << "《" << "》";
    *strlist[1] << "." << "@" << "~" << "-" << "," << ":" << "*" << "?" << "!" <<
        "_" << "#" << "/" << "=" << "+" << "^" << "&" << ";" << "%" << "$" <<
        "￥" <<
        "(" << ")" << "\\" << "<" << ">" << "|" << "[" << "]" << "\"" << "{" <<
        "}" <<
        "¥";
    *strlist[2] << "@" << "-" << "_" << "/" << "wap." << "bbs." << ".net" <<
        ".org" << ".com" << "www." << "http://" << ".cn" << "news." << "blog." <<
        "ftp://" << ".中国";
    *strlist[3] << "+" << "-" << "×" << "÷" << "﹤" << "﹥" << "≦" << "≧" << "≈" <<
        "≠" << "±" << "∴" << "∵" << "㏒" << "㏑" << "∑" << "∏" << "∅" << "₁" <<
        "₂" <<
        "₃" << "½" << "⅓" << "¼" << "¾" << "°" << "℃" << "℉" << "〒" << "∮" <<
        "∯" <<
        "∰" << "∫" << "∬" << "∭" << "∩" << "∪" << "º" << "¹" << "²" << "³" <<
        "ⁿ" <<
        "∂" << "∆";
    *strlist[4] << "♤" << "♧" << "♡" << "♢" << "♪" << "♬" << "♭" << "✔" << "✘" <<
        "♞" << "♟" << "☇" << "✌" << "☞" << "☟" << "☝" << "☜" << "♀" << "♂" <<
        "☺" <<
        "☹" << "◐" << "◑" << "…" << "‥" << "↖" << "↑" << "↗" << "→" << "↘" <<
        "↓" <<
        "↙" << "←" << "㊣" << "↕" << "↔" << "⊱" << "⋛" << "⋌" << "⋚" << "㊤" <<
        "㊦" <<
        "㊧" << "㊨" << "㊥" << "◤" << "◥" << "◢" << "◣" << "＊" << "❤";
    *strlist[5] << "①" << "②" << "③" << "④" << "⑤" << "⑥" << "⑦" << "⑧" << "⑨" <<
        "⑩" << "⑪" << "⑫" << "⑬" << "⑭" << "⑮" << "⑯" << "⑰" << "⑱" << "⑲" << "⑳"
                << "⑴" << "⑵" << "⑶" << "⑷" << "⑸" << "⑹" << "⑺" << "⑻" << "⑼" <<
        "⑽" << "⑾" << "⑿" << "⒀" << "⒁" << "⒂" << "⒃" << "⒄" << "⒅" << "⒆" << "⒇"
                << "⒈" << "⒉" << "⒊" << "⒋" << "⒌" << "⒍" << "⒎" << "⒏" << "⒐" <<
        "⒑" << "⒒" << "⒓" << "⒔" << "⒕" << "⒖" << "⒗" << "⒘" << "⒙" << "⒚" << "⒛"
                << "Ⅰ" << "Ⅱ" << "Ⅲ" << "Ⅳ" << "Ⅴ" << "Ⅵ" << "Ⅶ" << "Ⅷ" << "Ⅸ" <<
        "Ⅹ" << "Ⅺ" << "Ⅻ" << "ⅰ" << "ⅱ" << "ⅲ" << "ⅳ" << "ⅴ" << "ⅵ" << "ⅶ" <<
        "ⅷ" <<
        "ⅸ" << "ⅹ"
                << "❶" << "❷" << "❸" << "❹" << "❺" << "❻" << "❼" << "❽" << "❾" <<
        "❿"
                << "㈠" << "㈡" << "㈢" << "㈣" << "㈤" << "㈥" << "㈦" << "㈧" << "㈨" <<
        "㈩";
    *strlist[6] << "丶" << "冫" << "氵" << "灬" << "阝" << "卩" << "刂" << "忄" << "讠" <<
        "扌" << "亻" << "彳"
                << "厃" << "⺁" << "𠘨" << "⺆" << "⺄" << "广" << "疒" << "饣" << "钅" <<
        "礻" << "衤" << "辶"
                << "牜" << "釒" << "飠" << "⺪" << "⻊" << "糹" << "⺝" << "丩" << "丬" <<
        "犭" << "纟" << "廴"
                << "攵" << "夊" << "皿" << "臼" << "虍" << "勹" << "尢" << "廾" << "歺" <<
        "夬" << "氺" << "丂"
                << "爫" << "癶" << "耂" << "⺻" << "⺮" << "⺳" << "⺶" << "⺷" << "⺈" <<
        "龵" << "彐" << "⺋"
                << "罒" << "覀" << "⻗" << "亠" << "宀" << "冖" << "艹" << "⺌" << "丷" <<
        "⺧" << "亇" << "彡";
    *strlist[7] << "α" << "β" << "γ" << "δ" << "ε" << "ζ" << "η" << "θ" << "ι" <<
        "κ" << "λ" << "μ" << "ν" << "ξ" << "ο" << "π" << "ρ" << "σ" << "τ" <<
        "υ" <<
        "φ" << "χ" << "ψ" << "ω";
    *strlist[8] << "À" << "Á" << "Â" << "Ã" << "Ä" << "Å" << "Æ" << "Ç" << "È" <<
        "É" << "Ê" << "Ë" << "Ì" << "Í" << "Î" << "Ï" << "Ð" << "Ñ" << "Ò" <<
        "Ó" <<
        "Ô" << "Õ" << "Ö" << "Ø" << "Ù" << "Ú" << "Û" << "Ü" << "Ý" << "Þ" <<
        "Š" <<
        "Ÿ" << "Œ";

    pinyinlabel = new QLabel(this);

    // pinyinlabel->setAttribute(Qt::WA_TranslucentBackground,true);
    pinyinlabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // pinyinlabel->adjustSize();
    // pinyinlabel->setStyleSheet("QLabel{color:rgba(0,0,0,255);background-color:rgba(0,255,255,255);}");
    // pinyinlabel->setText("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    // pinyinlabel->adjustSize();
    // pinyinlabel->setPalette(pl);
    // pinyinlabel->setStyleSheet("QLabel{color:rgba(0,0,0,255);}");
    pinyinlabel->setFont(font);

    swidget = new SlipWidget(this);
    connect(swidget, SIGNAL(clicked(QModelIndex)), this,
            SLOT(swidget_click(QModelIndex)));
    swidget->setPalette(pl);
    swidget->setFont(font);

    twidget = new MTableWidget(this, 1);

    // twidget->setRowCount(1);
    twidget->set_move_state(1);
    connect(twidget, SIGNAL(clicked(QModelIndex)), this,
            SLOT(twidget_clicked(QModelIndex)));
    twidget->setPalette(pl);
    twidget->setFont(font);

    swidget_m = new SlipWidget(this, 10);
    swidget_m->hide();
    connect(swidget_m, SIGNAL(clicked(QModelIndex)), this,
            SLOT(Swidget_m_click(QModelIndex)));
    swidget_m->setPalette(pl);
    swidget_m->setFont(font);

    swidget_more = new SlipWidget(this);
    swidget_more->hide();
    connect(swidget_more, SIGNAL(clicked(QModelIndex)), this,
            SLOT(Swidget_m_click(QModelIndex)));
    swidget_more->setPalette(pl);
    swidget_more->setFont(font);

    swidget_b = new SlipWidget(this);
    swidget_b->set_listtext(strlisthead);
    swidget_b->hide();
    connect(swidget_b, SIGNAL(clicked(QModelIndex)), this,
            SLOT(Swidgetb_click(QModelIndex)));
    swidget_b->setPalette(pl);
    swidget_b->setFont(font);

    set_fuhao_table(); // 符号表数据

    button = new QPushButton *[18];

    for (int i = 0; i < 18; i++) {
        button[i] = new QPushButton(this);
        button[i]->installEventFilter(this);

        // 按钮透明
        if (background_input_is_transparence == 0) {
            button[i]->setFlat(false);
        } else {
            button[i]->setFlat(true);
        }

        // button[i]->setPalette(pl);
        // 背景色 圆角
        // button[i]->setStyleSheet("QPushButton{background-color:rgba(255,255,255,255);border-radius:
        // 10px;}");
        // 字颜色
        // button[i]->setStyleSheet("QPushButton{color:rgba(0,0,0,255);}");
        // button[i]->setFont(QFont("宋体",12));
        button[i]->setFont(font);
    }
    button[0]->setIcon(QIcon("easyInput/image/hid.png"));
    button[1]->setText("1");
    button[2]->setText("ABC");
    button[3]->setText("DEF");
    button[4]->setText("GHI");
    button[5]->setText("JKL");
    button[6]->setText("MNO");
    button[7]->setText("PQRS");
    button[8]->setText("TUV");
    button[9]->setText("WXYZ");
    button[10]->setText("符");
    button[11]->setText("123");
    button[12]->setText(" ");
    button[13]->setText("中");
    button[14]->setIcon(QIcon("easyInput/image/delete_white.png"));
    button[15]->setText("重输");

    // button[16]->setText("0");
    button[16]->setText("T26");
    button[17]->setIcon(QIcon("easyInput/image/enter.png"));

    returnbutton = new QPushButton(this);
    returnbutton->setText("返回");
    returnbutton->installEventFilter(this);
    returnbutton->hide();

    if (background_input_is_transparence == 0) {
        returnbutton->setFlat(false);
    } else {
        returnbutton->setFlat(true);
    }

    // returnbutton->setPalette(pl);
    returnbutton->setFont(font);

    buttont26 = new QPushButton *[34];

    for (int i = 0; i < 34; i++) {
        buttont26[i] = new QPushButton(this);
        buttont26[i]->hide();

        // buttont26[i]->installEventFilter(this);
        connect(buttont26[i], SIGNAL(pressed()), this, SLOT(buttont26press()));

        if (background_input_is_transparence == 0) {
            buttont26[i]->setFlat(false);
        } else {
            buttont26[i]->setFlat(true); // 按键透明
        }

        // buttont26[i]->setPalette(pl);
        // buttont26[i]->setStyleSheet("QPushButton{background:transparent;}");
        // buttont26[i]->setStyleSheet("background-color:rgba(0,0,0,0);");
        buttont26[i]->setFont(font);
    }
    buttont26[0]->setText("T9");
    buttont26[1]->setText("q"); buttont26[2]->setText("w"); buttont26[3]->setText(
        "e");
    buttont26[4]->setText("r"); buttont26[5]->setText("t"); buttont26[6]->setText(
        "y");
    buttont26[7]->setText("u"); buttont26[8]->setText("i"); buttont26[9]->setText(
        "o");
    buttont26[10]->setText("p"); buttont26[11]->setText("a");
    buttont26[12]->setText("s");
    buttont26[13]->setText("d"); buttont26[14]->setText("f");
    buttont26[15]->setText("g");
    buttont26[16]->setText("h"); buttont26[17]->setText("j");
    buttont26[18]->setText("k");
    buttont26[19]->setText("l"); buttont26[20]->setText("z");
    buttont26[21]->setText("x");
    buttont26[22]->setText("c"); buttont26[23]->setText("v");
    buttont26[24]->setText("b");
    buttont26[25]->setText("n"); buttont26[26]->setText("m");
    buttont26[27]->setIcon(QIcon("easyInput/image/upper_white.png"));
    buttont26[28]->setIcon(QIcon("easyInput/image/delete_white.png"));
    buttont26[29]->setText(",");
    buttont26[30]->setText(" ");
    buttont26[31]->setText(".");
    buttont26[32]->setText("中");
    buttont26[33]->setIcon(QIcon(":/image/enter.png"));

    this->setWindowFlags(Qt::FramelessWindowHint);

    // this->setFixedSize(width, height); // 设置固定大小
    // setAttribute(Qt::WA_TranslucentBackground,true);
}

T9_Widget::~T9_Widget()
{
    for (int i = 0; i < strlisthead.count(); i++) {
        delete strlist[i];
    }
    delete[] strlist;

    for (int i = 0; i < swidget_b->count(); i++) {
        delete twidget_more[i];
    }
    delete[] twidget_more;

    for (int i = 0; i < 34; i++) {
        delete buttont26[i];
    }
    delete[] buttont26;

    delete[] button;
}

void T9_Widget::set_show(int state)
{
    pinyinstr.clear();
    outstr.clear();
    pinyinlabel->clear();
    twidget->clear_list();
    swidget->clear_list();
    swidget_m->clear_list();
    swidget_more->clear_list();
    pinyinlist.clear();
    outlist.clear();
    set_windows(state);
    this->show();
}

int T9_Widget::start_exec(int state)
{
    pinyinstr.clear();
    outstr.clear();
    pinyinlabel->clear();
    twidget->clear_list();
    swidget->clear_list();
    swidget_m->clear_list();
    swidget_more->clear_list();
    pinyinlist.clear();
    outlist.clear();
    set_windows(state);

    return this->exec();
}

void T9_Widget::set_background(QColor color)
{
    background_color = color;
}

void T9_Widget::reset()
{}

void T9_Widget::paintEvent(QPaintEvent *event)
{
    //    Q_UNUSED(event);

    QPainter painter(this);

    painter.setPen(Qt::NoPen);

    if (input_state == 2) { // 符号
        painter.setBrush(background_color);
        painter.drawRect(rect());
    } else {
        // painter.setBrush(QColor(0,28, 60,1));
        painter.setBrush(QColor(0, 0, 0, 0));
        int w = QWidget::fontMetrics().horizontalAdvance(pinyinlabel->text()); //
                                                                               //
                                                                               //
                                                                               //
                                                                               // pinyinlabel->text().length()*8;
        // qDebug()<<QWidget::fontMetrics().width(pinyinlabel->text());
        int h = height() / 6;
        painter.drawRect(w, 0, width() - w, h);                                //
                                                                               //
                                                                               //
                                                                               //
                                                                               // pinyinlabel
        painter.setBrush(background_color);
        painter.drawRect(0, 0, w, h);                                          //
                                                                               //
                                                                               //
                                                                               //
                                                                               // pinyinlabel
        painter.drawRect(0, height() / 6, width(), h * 5);

        painter.setBrush(background_color);
        painter.drawRect(0, h, ti_width, ti_height - h);
        QPalette pl;
        pl.setBrush(QPalette::Base, QBrush(background_color));
        swidget->setPalette(pl);
        twidget->setPalette(pl);
        swidget_m->setPalette(pl);
        swidget_more->setPalette(pl);
        swidget_b->setPalette(pl);

        for (int i = 0; i < swidget_b->count(); i++) {
            twidget_more[i]->setPalette(pl);
        }

        for (int i = 0; i < 18; i++) {
            // button[i]->setPalette(pl);
            if (background_input_is_transparence == 0) {
                button[i]->setFlat(false);
            } else {
                button[i]->setFlat(true);

                // button[i]->setStyleSheet("QpushButton{background-color:rgba(255,255,255,100);}");
            }
        }

        // returnbutton->setPalette(pl);
        if (background_input_is_transparence == 0) {
            returnbutton->setFlat(false);
        } else {
            returnbutton->setFlat(true);

            // returnbutton->setStyleSheet("QpushButton{background-color:rgba(255,255,255,100);}");
        }

        for (int i = 0; i < 34; i++) {
            // buttont26[i]->setPalette(pl);
            if (background_input_is_transparence == 0) {
                buttont26[i]->setFlat(false);
            } else {
                buttont26[i]->setFlat(true);

                // buttont26[i]->setStyleSheet("QpushButton{background-color:rgba(255,255,255,100);}");
            }
        }
    }

    //    QPainter painter(this);
    //    painter.setBrush(QColor(0,28, 60,1));
    //
    //  painter.setPen(QPen(Qt::red,2,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
    //    painter.drawRect(rect());

    QWidget::paintEvent(event);
}

void T9_Widget::resizeEvent(QResizeEvent *event)
{
    ti_width = this->width();
    ti_height = this->height();


    if (ti_width / 60 != fuhao_rowcnt) {
        fuhao_rowcnt = ti_width / 60;

        // 重新构建符号table
        set_fuhao_table(); // 符号表数据
    }

    int w = width();
    int w1 = w / (float)5;
    int h = height() / 6;
    pinyinlabel->setGeometry(0, 0, w, h);
    twidget->setGeometry(0, pinyinlabel->geometry().bottom(), w - w1, h);
    button[0]->setGeometry(twidget->geometry().right(),
                           pinyinlabel->geometry().bottom(),
                           w1,
                           h);
    swidget->setGeometry(0, twidget->geometry().bottom(), w1, 3 * h);
    button[10]->setGeometry(0, swidget->geometry().bottom(), w1, h);
    button[1]->setGeometry(swidget->geometry().right(),
                           twidget->geometry().bottom(),
                           w1,
                           h);
    button[2]->setGeometry(button[1]->geometry().right(),
                           twidget->geometry().bottom(),
                           w1,
                           h);
    button[3]->setGeometry(button[2]->geometry().right(),
                           twidget->geometry().bottom(),
                           w1,
                           h);
    button[14]->setGeometry(button[3]->geometry().right(),
                            twidget->geometry().bottom(),
                            w1,
                            h);
    button[4]->setGeometry(swidget->geometry().right(),
                           button[1]->geometry().bottom(),
                           w1,
                           h);
    button[5]->setGeometry(button[4]->geometry().right(),
                           button[1]->geometry().bottom(),
                           w1,
                           h);
    button[6]->setGeometry(button[5]->geometry().right(),
                           button[1]->geometry().bottom(),
                           w1,
                           h);
    button[15]->setGeometry(button[6]->geometry().right(),
                            button[1]->geometry().bottom(),
                            w1,
                            h);
    button[7]->setGeometry(swidget->geometry().right(),
                           button[4]->geometry().bottom(),
                           w1,
                           h);
    button[8]->setGeometry(button[7]->geometry().right(),
                           button[4]->geometry().bottom(),
                           w1,
                           h);
    button[9]->setGeometry(button[8]->geometry().right(),
                           button[4]->geometry().bottom(),
                           w1,
                           h);
    button[16]->setGeometry(button[9]->geometry().right(),
                            button[4]->geometry().bottom(),
                            w1,
                            h);
    button[11]->setGeometry(button[10]->geometry().right(),
                            button[7]->geometry().bottom(),
                            w1,
                            h);
    button[12]->setGeometry(button[11]->geometry().right(),
                            button[7]->geometry().bottom(),
                            w1,
                            h);
    button[13]->setGeometry(button[12]->geometry().right(),
                            button[7]->geometry().bottom(),
                            w1,
                            h);
    button[17]->setGeometry(button[13]->geometry().right(),
                            button[7]->geometry().bottom(),
                            w1,
                            h);

    swidget_b->setGeometry(0, 0, w1, 5 * h);
    returnbutton->setGeometry(0, swidget_b->geometry().bottom(), w1, h);

    for (int i = 0; i < swidget_b->count(); i++) {
        twidget_more[i]->setGeometry(swidget_b->geometry().right(),
                                     0,
                                     w1 * 4,
                                     height());
    }

    swidget_m->setGeometry(0, pinyinlabel->geometry().bottom(), w1, 4 * h);
    swidget_more->setGeometry(swidget_m->geometry().right(),
                              pinyinlabel->geometry().bottom(),
                              w1 * 4,
                              5 * h);

    if (input_state == 4) {
        returnbutton->setGeometry(0, swidget_m->geometry().bottom(), w1, h);
    }

    // t26
    //    if(input_state==5){
    //        twidget->setGeometry(0,pinyinlabel->geometry().bottom(),w,h);
    //    }
    int width_space = 2;
    int w2 = w / (float)10 - width_space;
    buttont26[1]->setGeometry(0, twidget->geometry().bottom(), w2, h);
    buttont26[2]->setGeometry(buttont26[1]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[3]->setGeometry(buttont26[2]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[4]->setGeometry(buttont26[3]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[5]->setGeometry(buttont26[4]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[6]->setGeometry(buttont26[5]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[7]->setGeometry(buttont26[6]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[8]->setGeometry(buttont26[7]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[9]->setGeometry(buttont26[8]->geometry().right() + width_space,
                              buttont26[1]->geometry().top(),
                              w2,
                              h);
    buttont26[10]->setGeometry(buttont26[9]->geometry().right() + width_space,
                               buttont26[1]->geometry().top(),
                               w2,
                               h);

    buttont26[11]->setGeometry(w2 / 2, buttont26[1]->geometry().bottom(), w2, h);
    buttont26[12]->setGeometry(buttont26[11]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);
    buttont26[13]->setGeometry(buttont26[12]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);
    buttont26[14]->setGeometry(buttont26[13]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);
    buttont26[15]->setGeometry(buttont26[14]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);
    buttont26[16]->setGeometry(buttont26[15]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);
    buttont26[17]->setGeometry(buttont26[16]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);
    buttont26[18]->setGeometry(buttont26[17]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);
    buttont26[19]->setGeometry(buttont26[18]->geometry().right() + width_space,
                               buttont26[11]->geometry().top(),
                               w2,
                               h);

    int w3 = w2 * 1.5;
    buttont26[27]->setGeometry(0, buttont26[11]->geometry().bottom(), w3, h);
    buttont26[20]->setGeometry(buttont26[27]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w2,
                               h);
    buttont26[21]->setGeometry(buttont26[20]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w2,
                               h);
    buttont26[22]->setGeometry(buttont26[21]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w2,
                               h);
    buttont26[23]->setGeometry(buttont26[22]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w2,
                               h);
    buttont26[24]->setGeometry(buttont26[23]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w2,
                               h);
    buttont26[25]->setGeometry(buttont26[24]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w2,
                               h);
    buttont26[26]->setGeometry(buttont26[25]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w2,
                               h);
    buttont26[28]->setGeometry(buttont26[26]->geometry().right() + width_space,
                               buttont26[27]->geometry().top(),
                               w3,
                               h);

    int w4 = w / (float)6 - width_space;
    buttont26[0]->setGeometry(0, buttont26[27]->geometry().bottom(), w4, h);
    buttont26[29]->setGeometry(buttont26[0]->geometry().right() + width_space,
                               buttont26[0]->geometry().top(),
                               w4,
                               h);
    buttont26[30]->setGeometry(buttont26[29]->geometry().right() + width_space,
                               buttont26[0]->geometry().top(),
                               w4,
                               h);
    buttont26[31]->setGeometry(buttont26[30]->geometry().right() + width_space,
                               buttont26[0]->geometry().top(),
                               w4,
                               h);
    buttont26[32]->setGeometry(buttont26[31]->geometry().right() + width_space,
                               buttont26[0]->geometry().top(),
                               w4,
                               h);
    buttont26[33]->setGeometry(buttont26[32]->geometry().right() + width_space,
                               buttont26[0]->geometry().top(),
                               w4,
                               h);

    QWidget::resizeEvent(event);
}

bool T9_Widget::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->buttons() & Qt::LeftButton) {
            if (obj == button[0]) {
                if (outstr.count() > 0) {
                    if ((input_state == 1) && (ceng_mode == true)) { // t9
                        // 获取数据
                        swidget_m->set_listtext(pinyinlist);

                        if (pinyin_checkindex < outlist.count()) {
                            QStringList out = outlist.at(pinyin_checkindex);
                            swidget_more->set_listtext(out);
                        } else {
                            swidget_more->set_listtext(outstr);
                        }
                        set_windows(4);
                    } else if ((input_state == 5) && (ceng_mode == true)) { // t26
                        swidget_more->set_listtext(outstr);
                        set_windows(4);
                    }
                } else {
                    // this->close();
                    emit close_widget();
                    emit input_key(3);
                }
            } else if ((obj == button[1]) || (obj == button[2]) ||
                       (obj == button[3])
                       || (obj == button[4]) || (obj == button[5]) ||
                       (obj == button[6])
                       || (obj == button[7]) || (obj == button[8]) ||
                       (obj == button[9])) {
                if (input_state == 1) { // Pinyin
                    if (obj != button[1]) {
                        //
                        //
                        //
                        //
                        //
                        //
                        //       pinyinstr.append(((QPushButton*)obj)->text());
                        //
                        //                      pinyinlabel->setText(pinyinstr);
                        //                        //swidget;
                        //
                        //
                        //
                        //
                        //
                        //
                        //       easyinput->get_chinese_value(pinyinstr,outstr);
                        //
                        //
                        //
                        //                twidget->reset_row_listtext(outstr);
                        //
                        //                      //twidget->set_listtext(outstr);
                        //
                        //
                        //
                        //
                        //
                        //
                        //
                        //
                        // //qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<pinyinstr<<outstr;

                        int index = 2;

                        for (int i = 2; i < 10; i++) {
                            if (obj == button[i]) {
                                index = i;
                                break;
                            }
                        }

                        if ((ceng_mode == true) &&
                            (pinyinlist.count() <= MAX_SECTION_LINE)) { // pinyin
                            pinyinstr.append(QString::number(index));
                            get_chinese_w(pinyinstr);                   // 获取数据并显示到widget
                        } else if (ceng_mode == false) {
                            get_en_w(index);
                        }
                    }
                } else { // 数字
                    QString qvalue = ((QPushButton *)obj)->text();
                    emit    input_text(qvalue);
                }
            } else if (obj == button[10]) {
                set_windows(2);
            } else if (obj == button[11]) {
                if (input_state == 1) { // 切换为数字
                    set_windows(3);
                } else {                // 切换为pinyin
                    set_windows(1);
                }
            } else if (obj == button[12]) {
                if (input_state == 1) {
                    QString qvalue = " ";
                    emit    input_text(qvalue);
                } else {
                    QString qvalue = "0";
                    emit    input_text(qvalue);
                }
            } else if (obj == button[13]) { // 中/英
                if (input_state == 1) {     // 切换
                    pinyinstr.clear();
                    outstr.clear();
                    pinyinlabel->clear();
                    twidget->clear_list();
                    swidget->clear_list();
                    swidget_m->clear_list();
                    swidget_more->clear_list();
                    pinyinlist.clear();
                    outlist.clear();

                    if (button[13]->text() == "En") {
                        button[13]->setText("中");
                        ceng_mode = true;
                    } else {
                        button[13]->setText("En");
                        ceng_mode = false;
                    }

                    // ceng_mode = !ceng_mode;
                } else { // 空格
                    QString qvalue = " ";
                    emit    input_text(qvalue);
                }
            } else if (obj == button[14]) {
                if (pinyinstr.count() > 0) {
                    pinyinstr = pinyinstr.left(pinyinstr.count() - 1);
                    get_chinese_w(pinyinstr); // 获取数据并显示到widget
                } else {
                    emit input_key(2);
                }
            } else if (obj == button[15]) {
                if (input_state == 1) { // 重输
                    pinyinstr.clear();
                    outstr.clear();
                    pinyinlabel->clear();
                    twidget->clear_list();
                    swidget->clear_list();
                    swidget_m->clear_list();
                    swidget_more->clear_list();
                    pinyinlist.clear();
                    outlist.clear();

                    emit input_key(0);
                } else {
                    QString qvalue = "*";
                    emit    input_text(qvalue);
                }
            } else if (obj == button[16]) {
                if (input_state == 1) {
                    // QString qvalue = "0";emit input_text(qvalue);
                    set_windows(5);
                } else {
                    QString qvalue = "#";
                    emit    input_text(qvalue);
                }
            } else if (obj == button[17]) {
                emit input_key(1);
            } else if (obj == returnbutton) {
                set_windows(input_state_last);
            }

            // return true;
        } else {
            // return QWidget::eventFilter(obj, event);
        }
    } else {
        // return QWidget::eventFilter(obj, event);
    }
    return QWidget::eventFilter(obj, event);
}

void T9_Widget::set_fuhao_table()
{
    // QPalette pe;
    // pe.setColor(QPalette::WindowText,QColor(Qt::black));
    QFont font("Microsoft Yahei", 8, QFont::Normal);

    // font.setPointSize(8);
    QPalette pl;

    if (background_input_is_transparence == 0) {
        pl.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255, 255)));
    } else {
        pl.setBrush(QPalette::Base, QBrush(QColor(0, 0, 0, 0)));
    }

    if (twidget_more != NULL) {
        for (int i = 0; i < swidget_b->count(); i++) {
            delete twidget_more[i];
        }
        delete[] twidget_more;
    }
    twidget_more = new MTableWidget *[swidget_b->count()];
    int fuhao_w = (this->width() / 5 * 4) / fuhao_rowcnt;

    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<fuhao_rowcnt<<ti_width<<fuhao_w<<this->width();
    for (int i = 0; i < swidget_b->count(); i++) {
        twidget_more[i] = new MTableWidget(this, fuhao_rowcnt, fuhao_w);
        twidget_more[i]->set_move_state(2);
        twidget_more[i]->hide();
        twidget_more[i]->set_listtext(*strlist[i]);
        connect(twidget_more[i], SIGNAL(clicked(QModelIndex)), this,
                SLOT(twidget_m_clicked(QModelIndex)));
        twidget_more[i]->setPalette(pl);
        twidget_more[i]->setStyleSheet(
            "QTableWidget::item{border:0px solid rgb(0,0,0);border-bottom:1px solid rgb(0,0,0);}"); //
                                                                                                    //
                                                                                                    //
                                                                                                    //
                                                                                                    // item下绘制横线
        //        if(i != 2){
        //            twidget_more[i]->setFont(QFont("宋体",18));
        //        }
        twidget_more[i]->setFont(font);
    }
}

void T9_Widget::set_windows(int state)
{
    if ((state < 1) || (state > 6)) return;

    input_state_last = input_state;
    input_state = state;

    if (state == 1) { // pinyin
        pinyinlabel->show();
        swidget->show();
        twidget->show();
        button[0]->setIcon(QIcon("easyInput/image/hid.png"));
        button[1]->setText("1");
        button[2]->setText("ABC");
        button[3]->setText("DEF");
        button[4]->setText("GHI");
        button[5]->setText("JKL");
        button[6]->setText("MNO");
        button[7]->setText("PQRS");
        button[8]->setText("TUV");
        button[9]->setText("WXYZ");
        button[10]->setText("符");
        button[11]->setText("123");
        button[12]->setText(" ");
        button[13]->setText("中");
        button[14]->setIcon(QIcon("easyInput/image/delete_white.png"));
        button[15]->setText("重输");

        // button[16]->setText("0");
        button[16]->setText("T26");
        button[17]->setIcon(QIcon("easyInput/image/enter.png"));

        for (int i = 0; i < 18; i++) {
            button[i]->show();
        }

        swidget_m->hide();
        swidget_more->hide();

        swidget_b->hide();

        for (int i = 0; i < strlisthead.count(); i++) {
            twidget_more[i]->hide();
        }

        returnbutton->hide();

        for (int i = 0; i < 34; i++) {
            buttont26[i]->hide();
        }

        ceng_mode = true;
    } else if (state == 2) { // 符号
        returnbutton->show();
        swidget_b->show();

        if (swidget_b->count() > 0) {
            swidget_b->setCurrentRow(0);
        }
        twidget_more[0]->show();

        for (int i = 1; i < strlisthead.count(); i++) {
            twidget_more[i]->hide();
        }

        pinyinlabel->hide();

        swidget->hide();
        twidget->hide();

        swidget_m->hide();
        swidget_more->hide();

        for (int i = 0; i < 18; i++) {
            button[i]->hide();
        }

        for (int i = 0; i < 34; i++) {
            buttont26[i]->hide();
        }
    } else if (state == 3) { // 数字
        pinyinstr.clear();
        outstr.clear();
        pinyinlabel->clear();
        twidget->clear_list();
        swidget->clear_list();
        swidget_m->clear_list();
        swidget_more->clear_list();
        pinyinlist.clear();
        outlist.clear();

        pinyinlabel->show();
        twidget->show();
        swidget->show();
        button[0]->setIcon(QIcon("easyInput/image/hid.png"));
        button[1]->setText("1");
        button[2]->setText("2");
        button[3]->setText("3");
        button[4]->setText("4");
        button[5]->setText("5");
        button[6]->setText("6");
        button[7]->setText("7");
        button[8]->setText("8");
        button[9]->setText("9");
        button[10]->setText("符");
        button[11]->setText("返回");
        button[12]->setText("0");
        button[13]->setText("空格");
        button[14]->setIcon(QIcon("easyInput/image/delete_white.png"));
        button[15]->setText("*");
        button[16]->setText("#");
        button[17]->setIcon(QIcon("easyInput/image/enter.png"));

        for (int i = 0; i < 18; i++) {
            button[i]->show();
        }

        returnbutton->hide();
        swidget_m->hide();
        swidget_more->hide();
        swidget_b->hide();

        for (int i = 0; i < strlisthead.count(); i++) {
            twidget_more[i]->hide();
        }

        for (int i = 0; i < 34; i++) {
            buttont26[i]->hide();
        }
    } else if (state == 4) { // 更多pinyin输入
        swidget_m->show();
        swidget_more->show();

        if (swidget_more->count() > 0) {
            swidget_more->setCurrentRow(0);
        }
        returnbutton->show();
        pinyinlabel->show();

        twidget->hide();
        swidget->hide();
        swidget_b->hide();

        for (int i = 0; i < strlisthead.count(); i++) {
            twidget_more[i]->hide();
        }

        for (int i = 0; i < 18; i++) {
            button[i]->hide();
        }

        for (int i = 0; i < 34; i++) {
            buttont26[i]->hide();
        }
    } else if (state == 5) { // t26
        if (input_state_last == 1) {
            pinyinstr.clear();
            outstr.clear();
            pinyinlabel->clear();
            twidget->clear_list();
            swidget->clear_list();
            swidget_m->clear_list();
            swidget_more->clear_list();
            pinyinlist.clear();
            outlist.clear();
        }

        pinyinlabel->show();
        twidget->show();
        buttont26[1]->setText("q"); buttont26[2]->setText("w");
        buttont26[3]->setText("e");
        buttont26[4]->setText("r"); buttont26[5]->setText("t");
        buttont26[6]->setText("y");
        buttont26[7]->setText("u"); buttont26[8]->setText("i");
        buttont26[9]->setText("o");
        buttont26[10]->setText("p"); buttont26[11]->setText("a");
        buttont26[12]->setText("s");
        buttont26[13]->setText("d"); buttont26[14]->setText("f");
        buttont26[15]->setText("g");
        buttont26[16]->setText("h"); buttont26[17]->setText("j");
        buttont26[18]->setText("k");
        buttont26[19]->setText("l"); buttont26[20]->setText("z");
        buttont26[21]->setText("x");
        buttont26[22]->setText("c"); buttont26[23]->setText("v");
        buttont26[24]->setText("b");
        buttont26[25]->setText("n"); buttont26[26]->setText("m");

        for (int i = 0; i < 34; i++) {
            buttont26[i]->show();
        }

        // swidget_m->show();
        // swidget_more->show();

        swidget->hide();
        swidget_m->hide();
        swidget_more->hide();
        swidget_b->hide();

        for (int i = 0; i < strlisthead.count(); i++) {
            twidget_more[i]->hide();
        }

        for (int i = 0; i < 18; i++) {
            button[i]->hide();
        }
        returnbutton->hide();

        button[0]->show();

        // ceng_mode = true;
    }

    this->resizeEvent(NULL);
    this->repaint();

    //    pinyinstr.clear();
    //    outstr.clear();
    //    pinyinlabel->clear();
    //    twidget->clear_list();
    //    swidget->clear_list();
    //    swidget_m->clear_list();
    //    swidget_more->clear_list();
}

// T9
void T9_Widget::get_chinese_w(QString& pinyinstr)
{
    pinyin_checkindex = 0;

    if (pinyinstr.isEmpty()) {
        pinyinlabel->clear();
        pinyinlist.clear();
        outstr.clear();
        outlist.clear();
        swidget->set_listtext(pinyinlist);
        twidget->reset_row_listtext(outstr);
        return;
    }

    if (mT9obj == NULL) {
        qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << "mT9obj is NULL";
        return;
    }

    const table_entry_t *tabledata =
        mT9obj->func.get_py_cn_from_key(mT9obj,
                                        (char *)pinyinstr.toStdString().c_str());

    if (tabledata == NULL) {
        // pinyinstr = pinyinstr.left(pinyinstr.count()-1);
    } else {
        pycount = tabledata->count;
        pinyinlist.clear();
        outlist.clear();

        // printf("%s\n",tabledata->T9);
        for (int i = 0; i < tabledata->count; i++) {
            // printf("%s,%s\n",tabledata->data[i].PY,tabledata->data[i].MB);
            pinyinlist.append(QString(tabledata->data[i].PY));

            QString qstr;
            qstr.append(tabledata->data[i].MB);
            QStringList qlist = qstr.split(' ');

            if (i == 0) {
                // 第一组，显示
                outstr.clear();
                outstr.append(qlist);
            }
            outlist.append(qlist);
        }
        swidget->set_listtext(pinyinlist);
        twidget->reset_row_listtext(outstr);
    }

    pinyinlabel->setText(pinyinstr);
}

void T9_Widget::get_en_w(int id)
{
    outstr.clear();

    switch (id) {
    case 2:
        outstr << "a" << "b" << "c" << "A" << "B" << "C";
        break;

    case 3:
        outstr << "d" << "e" << "f" << "D" << "E" << "F";
        break;

    case 4:
        outstr << "g" << "h" << "i" << "G" << "H" << "I";
        break;

    case 5:
        outstr << "j" << "k" << "l" << "J" << "K" << "L";
        break;

    case 6:
        outstr << "m" << "n" << "o" << "M" << "N" << "O";
        break;

    case 7:
        outstr << "p" << "q" << "r" << "s" << "P" << "Q" << "R" << "S";
        break;

    case 8:
        outstr << "t" << "u" << "v" << "T" << "U" << "V";
        break;

    case 9:
        outstr << "w" << "x" << "y" << "z" << "W" << "X" << "Y" << "Z";
        break;

    default: break;
    }
    twidget->reset_row_listtext(outstr);
}

void T9_Widget::buttont26press()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    int index = 0;

    for (int i = 0; i < 34; i++) {
        if (btn == buttont26[i]) {
            index = i;
            break;
        }
    }

    if (index == 0) {
        pinyinstr.clear();
        outstr.clear();
        pinyinlabel->clear();
        twidget->clear_list();
        swidget->clear_list();
        swidget_m->clear_list();
        swidget_more->clear_list();
        pinyinlist.clear();
        outlist.clear();

        set_windows(1);
    } else if ((index >= 1) && (index <= 26)) { // 字母
        if (ceng_mode == true) {
            QString str = buttont26[index]->text();
            pinyinstr.append(str);
            pinyinlabel->setText(pinyinstr);
            easyinput->get_chinese_value(pinyinstr, outstr);
            twidget->reset_row_listtext(outstr);

            // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<pinyinstr<<outstr;
        } else {
            QString str = buttont26[index]->text();
            emit    input_text(str);
        }
    } else if (index == 27) { // 大小写
        if (ceng_mode == false) {
            if (buttont26[1]->text() == "q") {
                buttont26[1]->setText("Q"); buttont26[2]->setText("W");
                buttont26[3]->setText("E");
                buttont26[4]->setText("R"); buttont26[5]->setText("T");
                buttont26[6]->setText("Y");
                buttont26[7]->setText("U"); buttont26[8]->setText("I");
                buttont26[9]->setText("O");
                buttont26[10]->setText("P"); buttont26[11]->setText("A");
                buttont26[12]->setText("S");
                buttont26[13]->setText("D"); buttont26[14]->setText("F");
                buttont26[15]->setText("G");
                buttont26[16]->setText("H"); buttont26[17]->setText("J");
                buttont26[18]->setText("K");
                buttont26[19]->setText("L"); buttont26[20]->setText("Z");
                buttont26[21]->setText("X");
                buttont26[22]->setText("C"); buttont26[23]->setText("V");
                buttont26[24]->setText("B");
                buttont26[25]->setText("N"); buttont26[26]->setText("M");
            } else {
                buttont26[1]->setText("q"); buttont26[2]->setText("w");
                buttont26[3]->setText("e");
                buttont26[4]->setText("r"); buttont26[5]->setText("t");
                buttont26[6]->setText("y");
                buttont26[7]->setText("u"); buttont26[8]->setText("i");
                buttont26[9]->setText("o");
                buttont26[10]->setText("p"); buttont26[11]->setText("a");
                buttont26[12]->setText("s");
                buttont26[13]->setText("d"); buttont26[14]->setText("f");
                buttont26[15]->setText("g");
                buttont26[16]->setText("h"); buttont26[17]->setText("j");
                buttont26[18]->setText("k");
                buttont26[19]->setText("l"); buttont26[20]->setText("z");
                buttont26[21]->setText("x");
                buttont26[22]->setText("c"); buttont26[23]->setText("v");
                buttont26[24]->setText("b");
                buttont26[25]->setText("n"); buttont26[26]->setText("m");
            }
        }
    } else if (index == 28) { // 删除
        if (pinyinstr.length() > 0) {
            pinyinstr = pinyinstr.left(pinyinstr.length() - 1);
            pinyinlabel->setText(pinyinstr);
            easyinput->get_chinese_value(pinyinstr, outstr);
            twidget->reset_row_listtext(outstr);

            // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<pinyinstr<<outstr;
        } else {
            emit input_key(2);
        }
    } else if (index == 29) { // ","
        QString str = buttont26[index]->text();
        emit    input_text(str);
    } else if (index == 30) { // " "
        QString str = buttont26[index]->text();
        emit    input_text(str);
    } else if (index == 31) { // "."
        QString str = buttont26[index]->text();
        emit    input_text(str);
    } else if (index == 32) { // 切换中英
        pinyinstr.clear();    // 清空输入
        pinyinlabel->clear();
        twidget->clear_list();

        if (buttont26[index]->text() == "En") {
            buttont26[index]->setText("中");
            ceng_mode = true;
        } else {
            buttont26[index]->setText("En");
            ceng_mode = false;
        }
        buttont26[1]->setText("q"); buttont26[2]->setText("w");
        buttont26[3]->setText("e");
        buttont26[4]->setText("r"); buttont26[5]->setText("t");
        buttont26[6]->setText("y");
        buttont26[7]->setText("u"); buttont26[8]->setText("i");
        buttont26[9]->setText("o");
        buttont26[10]->setText("p"); buttont26[11]->setText("a");
        buttont26[12]->setText("s");
        buttont26[13]->setText("d"); buttont26[14]->setText("f");
        buttont26[15]->setText("g");
        buttont26[16]->setText("h"); buttont26[17]->setText("j");
        buttont26[18]->setText("k");
        buttont26[19]->setText("l"); buttont26[20]->setText("z");
        buttont26[21]->setText("x");
        buttont26[22]->setText("c"); buttont26[23]->setText("v");
        buttont26[24]->setText("b");
        buttont26[25]->setText("n"); buttont26[26]->setText("m");
    } else if (index == 33) { // 确定
        emit input_key(1);
    }
}

void T9_Widget::Swidgetb_click(QModelIndex index)
{
    for (int i = 0; i < swidget_b->count(); i++) {
        if (i == index.row()) {
            twidget_more[i]->show();
        } else {
            twidget_more[i]->hide();
        }
    }
}

void T9_Widget::twidget_m_clicked(QModelIndex index)
{
    MTableWidget *mtw = qobject_cast<MTableWidget *>(sender());
    bool find = false;

    for (int i = 0; i < swidget_b->count(); i++) {
        if (mtw == twidget_more[i]) {
            find = true;
            break;
        }
    }

    if (find == true) {
        QTableWidgetItem *item = mtw->item(index.row(), index.column());

        if (item == NULL) {
            return;
        }

        // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<item->text();
        QString qvalue = item->text();
        emit    input_text(qvalue);
    }
}

void T9_Widget::twidget_clicked(QModelIndex index)
{
    QTableWidgetItem *item = twidget->item(index.row(), index.column());

    if (item == NULL) {
        return;
    }

    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<item->text();
    QString qvalue = item->text();
    emit    input_text(qvalue);
    pinyinlabel->clear();
    swidget->clear_list();
    twidget->clear_list();
    pinyinstr.clear();
    pinyinlist.clear();
    outstr.clear();
    outlist.clear();
}

void T9_Widget::swidget_click(QModelIndex index)
{
    pinyin_checkindex = index.row();
    QListWidgetItem *item = swidget->item(pinyin_checkindex);

    if (item == NULL) {
        return;
    }

    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<item->text();
    if (pinyin_checkindex < outlist.count()) {
        QStringList out = outlist.at(pinyin_checkindex);
        twidget->reset_row_listtext(out);
    }
}

// 选择上一个拼音
void T9_Widget::swidget_last_check_pinyin()
{
    if (pinyin_checkindex < 0) return;

    QListWidgetItem *item = swidget->item(pinyin_checkindex - 1);

    if (item == NULL) {
        return;
    }
    pinyin_checkindex--;

    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<item->text();
    if (pinyin_checkindex < outlist.count()) {
        QStringList out = outlist.at(pinyin_checkindex);
        twidget->reset_row_listtext(out);
    }
}

// 选择下一个拼音
void T9_Widget::swidget_next_check_pinyin()
{
    QListWidgetItem *item = swidget->item(pinyin_checkindex + 1);

    if (item == NULL) {
        return;
    }
    pinyin_checkindex++;

    // qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<item->text();
    if (pinyin_checkindex < outlist.count()) {
        QStringList out = outlist.at(pinyin_checkindex);
        twidget->reset_row_listtext(out);
    }
}

void T9_Widget::Swidget_m_click(QModelIndex index) {
    int row = index.row();
    SlipWidget *slw = qobject_cast<SlipWidget *>(sender());

    if (slw == swidget_m) {
        if (row < outlist.count()) {
            QStringList out = outlist.at(row);
            swidget_more->set_listtext(out);
        }
    } else if (slw == swidget_more) {
        QListWidgetItem *item = swidget_more->item(row);
        QString qvalue = item->text();
        emit    input_text(qvalue);

        pinyinstr.clear();
        outstr.clear();
        pinyinlabel->clear();
        twidget->clear_list();
        swidget->clear_list();
        swidget_m->clear_list();
        swidget_more->clear_list();
        pinyinlist.clear();
        outlist.clear();

        set_windows(input_state_last);
    }
}

void T9_Widget::Inpue_Key_Slot(int key)
{
    switch (key) {
    case KEY_DEFINE_BD_F1: // 确定
    case KEY_DEFINE_BD_ENTER:
    {
        if (ceng_mode == true) {
            switch (input_state) {
            case 1: // t9pinyin
            case 5: // t26pinyin
            {
                twidget_clicked(twidget->currentIndex());
                break;
            }

            case 4: // 更多输入选择
            {
                QListWidgetItem *item = swidget_more->currentItem();

                if (item != NULL) {
                    QString qvalue = item->text();
                    emit    input_text(qvalue);

                    pinyinstr.clear();
                    outstr.clear();
                    pinyinlabel->clear();
                    twidget->clear_list();
                    swidget->clear_list();
                    swidget_m->clear_list();
                    swidget_more->clear_list();
                    pinyinlist.clear();
                    outlist.clear();

                    set_windows(input_state_last);
                }

                break;
            }

            default: break;
            }
        }
        break;
    }

    case KEY_DEFINE_BD_F2: // 取消返回键
    {
        if (input_state == 4) {
            set_windows(input_state_last);
        } else {
            if ((input_state == 1) || (input_state == 5)) {
                if (pinyinstr.count() > 0) {
                    pinyinstr = pinyinstr.left(pinyinstr.count() - 1);

                    if ((input_state == 1) || (input_state == 5) &&
                        (ceng_mode == true)) {
                        get_chinese_w(pinyinstr); // 获取数据并显示到widget
                    }
                } else {
                    // this->close();
                    emit close_widget();

                    // emit input_key(2); // 删除键
                }
            } else {
                // this->close();
                emit close_widget();

                // emit input_key(2); // 删除键
            }
        }
        break;
    }

    case KEY_DEFINE_BD_UP:
    {
        if (ceng_mode == true) {
            switch (input_state) {
            case 1:
            {
                // t9pinyin,选择拼音
                swidget_last_check_pinyin();
                break;
            }

            case 4: // 更多输入选择
            {
                if (swidget_more->count() < 1) {
                    break;
                }
                QModelIndex index = swidget_more->currentIndex();

                if ((index.column() < 0) || (index.row() < 0)) {
                    swidget_more->setCurrentRow(0);
                } else {
                    if (index.row() > 0) {
                        swidget_more->setCurrentRow(index.row() - 1);
                    }
                }
                break;
            }

            default: break;
            }
        }
        break;
    }

    case KEY_DEFINE_BD_DOWN:
    {
        if (ceng_mode == true) {
            switch (input_state) {
            case 1:
            {
                // t9pinyin,选择拼音
                swidget_next_check_pinyin();
                break;
            }

            case 4: // 更多输入选择
            {
                if (swidget_more->count() < 1) {
                    break;
                }
                QModelIndex index = swidget_more->currentIndex();

                if ((index.column() < 0) || (index.row() < 0)) {
                    swidget_more->setCurrentRow(0);
                } else {
                    if (index.row() + 1 < swidget_more->count()) {
                        swidget_more->setCurrentRow(index.row() + 1);
                    }
                }
                break;
            }

            default: break;
            }
        }
        break;
    }

    case KEY_DEFINE_BD_LEFT:
    {
        if (ceng_mode == true) {
            switch (input_state) {
            case 1: // t9pinyin
            case 5: // t26pinyin
            {
                if ((twidget->columnCount() < 1) || (twidget->rowCount() < 1)) {
                    break;
                }
                QModelIndex index = twidget->currentIndex();

                if ((index.column() < 0) || (index.row() < 0)) {
                    twidget->setCurrentCell(0, 0);
                } else {
                    if (index.column() > 0) {
                        twidget->setCurrentCell(index.row(), index.column() - 1);
                    }
                }
                break;
            }

            default: break;
            }
        }
        break;
    }

    case KEY_DEFINE_BD_RIGHT:
    {
        if (ceng_mode == true) {
            switch (input_state) {
            case 1: // t9pinyin
            case 5: // t26pinyin
            {
                if ((twidget->columnCount() < 1) || (twidget->rowCount() < 1)) {
                    break;
                }
                QModelIndex index = twidget->currentIndex();

                if ((index.column() < 0) || (index.row() < 0)) {
                    twidget->setCurrentCell(0, 0);
                } else {
                    if (index.column() + 1 < twidget->columnCount()) {
                        twidget->setCurrentCell(index.row(), index.column() + 1);
                    }
                }
                break;
            }

            default: break;
            }
        }
        break;
    }

    case KEY_DEFINE_BD_NAVIGATION: // 导航键
    {
        break;
    }

    case KEY_DEFINE_BD_0:
    case KEY_DEFINE_BD_1:
    case KEY_DEFINE_BD_2:
    case KEY_DEFINE_BD_3:
    case KEY_DEFINE_BD_4:
    case KEY_DEFINE_BD_5:
    case KEY_DEFINE_BD_6:
    case KEY_DEFINE_BD_7:
    case KEY_DEFINE_BD_8:
    case KEY_DEFINE_BD_9:
    {
        if ((input_state == 1) && (key != KEY_DEFINE_BD_0) &&
            (key != KEY_DEFINE_BD_1)) {                            // t9pinyin
            if ((ceng_mode == true) && (pinyinlist.count() < 6)) { // pinyin
                pinyinstr.append(QString::number(key));
                get_chinese_w(pinyinstr);                          // 获取数据并显示到widget
            } else {                                               // 字母
                get_en_w(key);
            }
        } else if (input_state == 3) {                             // 数字
            QString qvalue = QString::number(key);
            emit    input_text(qvalue);
        }
        break;
    }

    case KEY_DEFINE_BD_LOCK:           // *
    {
        if (input_state == 1) {        // 切换为数字
            set_windows(3);
        } else if (input_state == 3) { // 切换为pinyin
            set_windows(1);
        }
        break;
    }

    case KEY_DEFINE_BD_SWITCH:  // #
    {
        if (input_state == 1) { // 切换
            pinyinstr.clear();
            outstr.clear();
            pinyinlabel->clear();
            twidget->clear_list();
            swidget->clear_list();
            swidget_m->clear_list();
            swidget_more->clear_list();
            pinyinlist.clear();
            outlist.clear();

            if (button[13]->text() == "En") {
                button[13]->setText("中");
                ceng_mode = true;
            } else {
                button[13]->setText("En");
                ceng_mode = false;
            }

            // ceng_mode = !ceng_mode;
        } else if (input_state == 3) { // 空格
            QString qvalue = " ";
            emit    input_text(qvalue);
        }
        break;
    }

    default: break;
    }
}
