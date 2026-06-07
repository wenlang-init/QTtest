#include "virtual_input_widget.h"
#include "../key_define.h"
#include <QFile>
#include <QDebug>

enum _VIRTUAL_INPUT_MODE {
    T9_MODE,
    T26_MODE,
    NUMBER,
    SYMBOL
};

#define MAX_FIND_CHAR 16

Virtual_Input_Widget::Virtual_Input_Widget(QWidget *parent) : QWidget(parent)
{
    Input_Management_Object& input_obj = Input_Management_Object::instance();

    easyinput = input_obj.get_t26obj();
    mT9obj = input_obj.get_t9obj();

    pycount = 0;
    input_mode = 0;

    // QFont font(this->font());
    QFont font("Microsoft Yahei", 8, QFont::Normal);

    // font.setPointSize(12);
    QPalette pe(this->palette());
    pe.setColor(QPalette::ButtonText, QColor(0, 0, 0, 255));

    for (quint32 i = 0; i < sizeof(pushbutton) / sizeof(pushbutton[0]); i++) {
        pushbutton[i] = new QPushButton(this);
        pushbutton[i]->setFont(font);
        pushbutton[i]->setPalette(pe);
        connect(pushbutton[i], SIGNAL(clicked(bool)), this,
                SLOT(button_clicked(bool)));
    }
    pe.setColor(QPalette::ButtonText, QColor(12, 111, 255, 255));
    pushbutton[0]->setPalette(pe);
    pe.setColor(QPalette::ButtonText, QColor(0, 0, 0, 255));
    pushbutton[0]->setText(QString::fromUtf8("9键"));
    pushbutton[1]->setText(QString::fromUtf8("26键"));
    pushbutton[2]->setText(QString::fromUtf8("数字"));
    pushbutton[3]->setText(QString::fromUtf8("符号"));
    pushbutton[4]->setText(QString::fromUtf8("关闭"));

    t9widget = new T9_Input_Widget(this);
    connect(t9widget, SIGNAL(clicked_key(int)), this,
            SLOT(clicked_key(int)));
    connect(t9widget, SIGNAL(clicked_chinese(QString)), this,
            SLOT(clicked_text(QString)));
    t26widget = new T26_Input_Widget(this);
    connect(t26widget, SIGNAL(clicked_key(int)), this,
            SLOT(clicked_key(int)));
    connect(t26widget, SIGNAL(clicked_chinese(QString)), this,
            SLOT(clicked_text(QString)));
    numberwidget = new Number_Input_Widget(this);
    connect(numberwidget, SIGNAL(clicked_chinese(QString)), this,
            SLOT(clicked_text(QString)));
    symbolwidget = new Symbol_Input_Widget(this);
    connect(symbolwidget, SIGNAL(clicked_chinese(QString)), this,
            SLOT(clicked_text(QString)));
    set_show(input_mode);
}

void Virtual_Input_Widget::show_mode(int index)
{
    int count = sizeof(pushbutton) / sizeof(pushbutton[0]);

    if (index == count - 1) {
        // 关闭输入法
        input_str.clear();
        pycount = 0;
        pinyinlist.clear();
        T9chineselist.clear();
        T26chineselist.clear();

        if (mT9obj != NULL) {
            mT9obj->func.reset_search(mT9obj);
        }
        easyinput->reset_search();
        emit close_widget();

        // this->close();
        return;
    }

    if ((index < 0) || (input_mode == index)) return;

    input_mode = index;
    QPalette pe(this->palette());
    pe.setColor(QPalette::ButtonText, QColor(0, 0, 0, 255));

    for (int i = 0; i < count; i++) {
        if (input_mode == i) {
            pe.setColor(QPalette::ButtonText, QColor(12, 111, 255, 255));
            pushbutton[i]->setPalette(pe);
            pe.setColor(QPalette::ButtonText, QColor(0, 0, 0, 255));
        } else {
            pushbutton[i]->setPalette(pe);
        }
    }
    input_str.clear();
    pycount = 0;
    pinyinlist.clear();
    T9chineselist.clear();
    T26chineselist.clear();

    if (mT9obj != NULL) {
        mT9obj->func.reset_search(mT9obj);
    }
    easyinput->reset_search();

    t9widget->reset();
    t26widget->reset();
    numberwidget->reset();
    symbolwidget->reset();

    set_show(input_mode);
}

void Virtual_Input_Widget::reset()
{
    t9widget->reset();
    t26widget->reset();
    numberwidget->reset();
    symbolwidget->reset();
    set_show(input_mode);
}

void Virtual_Input_Widget::resizeEvent(QResizeEvent *event)
{
    int w = this->width() / (sizeof(pushbutton) / sizeof(pushbutton[0]));
    int h = 30;

    for (quint32 i = 0; i < sizeof(pushbutton) / sizeof(pushbutton[0]); i++) {
        pushbutton[i]->setGeometry(w * i, 0, w, h);
    }

    t9widget->setGeometry(0, h, this->width(), this->height() - h);
    t26widget->setGeometry(0, h, this->width(), this->height() - h);
    numberwidget->setGeometry(0, h, this->width(), this->height() - h);
    symbolwidget->setGeometry(0, h, this->width(), this->height() - h);
    QWidget::resizeEvent(event);
}

void Virtual_Input_Widget::set_show(int value)
{
    t9widget->hide();
    t26widget->hide();
    numberwidget->hide();
    symbolwidget->hide();

    switch (value) {
    case 0:
        t9widget->show();
        break;

    case 1:
        t26widget->show();
        break;

    case 2:
        numberwidget->show();
        break;

    case 3:
        symbolwidget->show();
        break;

    default:
        t9widget->show();
        break;
    }
}

bool Virtual_Input_Widget::find_chinese()
{
    if (input_str.size() <= 0) {
        input_str.clear();
        pycount = 0;
        pinyinlist.clear();
        T9chineselist.clear();
        T26chineselist.clear();

        if (mT9obj != NULL) {
            mT9obj->func.reset_search(mT9obj);
        }
        easyinput->reset_search();

        if (input_mode == 0) {
            t9widget->reset();
        } else if (input_mode == 1) {
            t26widget->reset();
        }
        return false;
    }

    if (input_mode == 0) {
        if (mT9obj == NULL) {
            return false;
        }
        const table_entry_t *tabledata =
            mT9obj->func.get_py_cn_from_key(mT9obj,
                                            (char *)input_str.toStdString().c_str());

        if (tabledata == NULL) {
            // pinyinstr = pinyinstr.left(pinyinstr.count()-1);
            return false;
        } else {
            pycount = tabledata->count;
            pinyinlist.clear();
            T9chineselist.clear();

            for (int i = 0; i < tabledata->count; i++) {
                // printf("%s,%d:%s,%s\n",__FILE__,__LINE__,tabledata->data[i].PY,tabledata->data[i].MB);fflush(stdout);
                pinyinlist.append(QString(tabledata->data[i].PY));
                QString qstr;
                qstr.append(tabledata->data[i].MB);
                QStringList qlist = qstr.split(' ');
                T9chineselist.append(qlist);
            }
            t9widget->set_pinyin_list(pinyinlist);
            t9widget->set_chinese_list(T9chineselist);
        }
    } else if (input_mode == 1) {
        easyinput->get_chinese_value(input_str, T26chineselist);
        t26widget->set_pinyin_str(input_str);
        t26widget->set_chinese_list(T26chineselist);
    }

    return true;
}

void Virtual_Input_Widget::button_clicked(bool)
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    int index = -1;
    int count = sizeof(pushbutton) / sizeof(pushbutton[0]);

    for (int i = 0; i < count; i++) {
        if (pushbutton[i] == btn) {
            index = i;
            break;
        }
    }
    show_mode(index);
}

void Virtual_Input_Widget::clicked_key(int key)
{
    void *p = (void *)sender();

    if (p == t9widget) {
        if (key == KEY_DEFINE_T9_DEL) {
            if (input_str.size() == 0) {
                emit clicked_delete();
            } else {
                input_str.remove(input_str.size() - 1, 1);
                find_chinese();
            }
        } else if (key == KEY_DEFINE_T9_RELOSE) {
            input_str.clear();
            find_chinese();
        } else if (key == KEY_DEFINE_T9_ENTER) {
            emit clicked_enter();
        } else if ((key > 1) && (key < 10)) {
            if (input_str.size() <= MAX_FIND_CHAR) {
                int number = key - KEY_DEFINE_T9_2 + 2;
                input_str.append(QString::number(number));
                find_chinese();
            }
        }
    } else if (p == t26widget) {
        if (key == KEY_DEFINE_T26_DEL) {
            if (input_str.size() == 0) {
                emit clicked_delete();
            } else {
                input_str.remove(input_str.size() - 1, 1);
                find_chinese();
            }
        } else if (key == KEY_DEFINE_T26_RELOSE) {
            input_str.clear();
            find_chinese();
        } else if (key == KEY_DEFINE_T26_ENTER) {
            emit clicked_enter();
        } else if ((key >= KEY_DEFINE_T26_A) && (key <= KEY_DEFINE_T26_Z)) {
            if (input_str.size() <= MAX_FIND_CHAR) {
                char cc = key - KEY_DEFINE_T26_A + 'a';
                input_str.append(cc);
                find_chinese();
            }
        }
    }
}

void Virtual_Input_Widget::clicked_text(QString str)
{
    input_str.clear();
    t9widget->reset();
    t26widget->reset();
    numberwidget->reset();
    symbolwidget->reset();

    // qDebug() << str;
    emit clicked_str(str);
}

void Virtual_Input_Widget::Key_Input_Code(int m_key)
{
    if (this->isVisible() == false) return;

    if (m_key == KEY_DEFINE_INPUT_STAR) {
        int inputmode = input_mode;

        if (inputmode < 3) inputmode++;
        else inputmode = 0;

        //  切换输入模式
        show_mode(inputmode);
        return;
    }

    if (input_mode == 0) { // t9
        if ((m_key >= KEY_DEFINE_INPUT_2) && (m_key <= KEY_DEFINE_INPUT_9)) {
            if (input_str.size() <= MAX_FIND_CHAR) {
                int number = m_key - KEY_DEFINE_INPUT_2 + 2;
                input_str.append(QString::number(number));
                find_chinese();
            }
        } else if (m_key == KEY_DEFINE_INPUT_BACK) {
            if (input_str.size() == 0) {
                emit clicked_delete();
            } else {
                input_str.remove(input_str.size() - 1, 1);
                find_chinese();
            }
        } else if (m_key == KEY_DEFINE_INPUT_1) { // KEY_DEFINE_INPUT_ESC){
            input_str.clear();
            find_chinese();
        } else if (m_key == KEY_DEFINE_INPUT_ENTER) {
            QString str = t9widget->current_text();

            if (str.isEmpty() == true) {
                emit clicked_enter();
            } else {
                clicked_text(str);
            }
        } else if (m_key == KEY_DEFINE_INPUT_UP) {
            t9widget->set_prepinyin();
        } else if (m_key == KEY_DEFINE_INPUT_DOWN) {
            t9widget->set_nextpinyin();
        } else if (m_key == KEY_DEFINE_INPUT_LEFT) {
            t9widget->set_pretext();
        } else if (m_key == KEY_DEFINE_INPUT_RIGHT) {
            t9widget->set_nexttext();
        }
    } else if (input_mode == 1) { // t26
        if ((m_key >= KEY_DEFINE_INPUT_A) && (m_key <= KEY_DEFINE_INPUT_Z)) {
            if (input_str.size() <= MAX_FIND_CHAR) {
                char cc = m_key - KEY_DEFINE_INPUT_A + 'a';
                input_str.append(cc);
                find_chinese();
            }
        } else if (m_key == KEY_DEFINE_INPUT_BACK) {
            if (input_str.size() == 0) {
                emit clicked_delete();
            } else {
                input_str.remove(input_str.size() - 1, 1);
                find_chinese();
            }
        } else if (m_key == KEY_DEFINE_INPUT_1) { // KEY_DEFINE_INPUT_ESC){
            input_str.clear();
            find_chinese();
        } else if (m_key == KEY_DEFINE_INPUT_ENTER) {
            // emit clicked_enter();
            QString str = t26widget->current_text();

            if (str.isEmpty() == true) {
                emit clicked_enter();
            } else {
                clicked_text(str);
            }
        } else if (m_key == KEY_DEFINE_INPUT_UP) {} else if (m_key ==
                                                             KEY_DEFINE_INPUT_DOWN)
        {} else if (m_key == KEY_DEFINE_INPUT_LEFT) {
            t26widget->set_pretext();
        } else if (m_key == KEY_DEFINE_INPUT_RIGHT) {
            t26widget->set_nexttext();
        }
    } else if (input_mode == 2) { // 数字
        if ((m_key >= KEY_DEFINE_INPUT_0) && (m_key <= KEY_DEFINE_INPUT_9)) {
            if (input_str.size() <= MAX_FIND_CHAR) {
                char cc = m_key - KEY_DEFINE_INPUT_0 + '0';
                QString str;
                str.append(cc);
                emit clicked_str(str);
            }
        } else if (m_key == KEY_DEFINE_INPUT_BACK) {
            emit clicked_delete();
        }
    } else if (input_mode == 3) { // 符号
        if (m_key == KEY_DEFINE_INPUT_ENTER) {
            // emit clicked_enter();
            QString str = symbolwidget->current_text();

            if (str.isEmpty() == true) {
                emit clicked_enter();
            } else {
                clicked_text(str);
            }
        } else if (m_key == KEY_DEFINE_INPUT_UP) {
            symbolwidget->set_pretab();
        } else if (m_key == KEY_DEFINE_INPUT_DOWN) {
            symbolwidget->set_nexttab();
        } else if (m_key == KEY_DEFINE_INPUT_LEFT) {
            symbolwidget->set_pretext();
        } else if (m_key == KEY_DEFINE_INPUT_RIGHT) {
            symbolwidget->set_nexttext();
        } else if (m_key == KEY_DEFINE_INPUT_BACK) {
            emit clicked_delete();
        }
    }
}
