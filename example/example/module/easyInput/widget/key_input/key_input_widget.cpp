#include "key_input_widget.h"
#include "text_list_widget.h"
#include <QLabel>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>

#define MAX_FIND_CHAR 16

enum KEY_INPUT_DEFINE {
    KEY_INPUT_DEFINE_0,
    KEY_INPUT_DEFINE_1,
    KEY_INPUT_DEFINE_2,
    KEY_INPUT_DEFINE_3,
    KEY_INPUT_DEFINE_4,
    KEY_INPUT_DEFINE_5,
    KEY_INPUT_DEFINE_6,
    KEY_INPUT_DEFINE_7,
    KEY_INPUT_DEFINE_8,
    KEY_INPUT_DEFINE_9,
    KEY_INPUT_DEFINE_A,
    KEY_INPUT_DEFINE_B,
    KEY_INPUT_DEFINE_C,
    KEY_INPUT_DEFINE_D,
    KEY_INPUT_DEFINE_E,
    KEY_INPUT_DEFINE_F,
    KEY_INPUT_DEFINE_G,
    KEY_INPUT_DEFINE_H,
    KEY_INPUT_DEFINE_I,
    KEY_INPUT_DEFINE_J,
    KEY_INPUT_DEFINE_K,
    KEY_INPUT_DEFINE_L,
    KEY_INPUT_DEFINE_M,
    KEY_INPUT_DEFINE_N,
    KEY_INPUT_DEFINE_O,
    KEY_INPUT_DEFINE_P,
    KEY_INPUT_DEFINE_Q,
    KEY_INPUT_DEFINE_R,
    KEY_INPUT_DEFINE_S,
    KEY_INPUT_DEFINE_T,
    KEY_INPUT_DEFINE_U,
    KEY_INPUT_DEFINE_V,
    KEY_INPUT_DEFINE_W,
    KEY_INPUT_DEFINE_X,
    KEY_INPUT_DEFINE_Y,
    KEY_INPUT_DEFINE_Z,
    KEY_INPUT_DEFINE_UP,
    KEY_INPUT_DEFINE_DOWN,
    KEY_INPUT_DEFINE_LEFT,
    KEY_INPUT_DEFINE_RIGHT,
    KEY_INPUT_DEFINE_ENTER,  // 选择
    KEY_INPUT_DEFINE_DELETE, // 删除一个输入
    KEY_INPUT_DEFINE_CLEAN,  // 清空输入
    KEY_INPUT_DEFINE_SWITCH, // 切换选择拼音或汉字
    KEY_INPUT_DEFINE_PATTERN // 切换输入法模式
};

Key_Input_Widget::Key_Input_Widget(QWidget *parent) : QWidget(parent)
{
    this->resize(240, 40);

    // setFocusPolicy(Qt::StrongFocus);

    Input_Management_Object& input_obj = Input_Management_Object::instance();

    easyinput = input_obj.get_t26obj();
    mT9obj = input_obj.get_t9obj();

    t26_mode = true;
    check_pinyin = true;

    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor(12, 111, 255, 255));
    QFont font(this->font());

    // font.setPointSize(12);
    font.setBold(true);
    label = new QLabel(this);
    label->setFont(font);
    label->setPalette(pe);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label->setText(QString::fromUtf8("26键"));
    label_chinese_info = new QLabel(this);
    label_chinese_info->setFont(font);
    label_chinese_info->setPalette(pe);
    label_chinese_info->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label_chinese_info->setText(QString::fromUtf8("0/0"));

    pinyin_widget = new Text_List_Widget(this);
    chinese_widget =  new Text_List_Widget(this);
    pinyin_widget->set_check(true);

    key_input_oper(KEY_INPUT_DEFINE_PATTERN);
}

void Key_Input_Widget::reset()
{
    info_reset();
}

void Key_Input_Widget::resizeEvent(QResizeEvent *event)
{
    QFontMetrics fm(label->font());
    int w = fm.horizontalAdvance(label->text()) + 10;
    QFontMetrics fm1(label_chinese_info->font());
    int w1 = fm1.horizontalAdvance(label_chinese_info->text()) + 10;

    if (w < w1) {
        w = w1;
    }

    label->setGeometry(0, 0, w, this->height() / 2);
    label_chinese_info->setGeometry(0, this->height() / 2, w, this->height() / 2);

    pinyin_widget->setGeometry(w, 0, this->width() - w, this->height() / 2);
    chinese_widget->setGeometry(w,
                                this->height() / 2,
                                this->width() - w,
                                this->height() / 2);

    QWidget::resizeEvent(event);
}

void Key_Input_Widget::keyPressEvent(QKeyEvent *event)
{
    if (event != NULL) {
        Key_System_Code(event->key());
    }
    QWidget::keyPressEvent(event);
}

void Key_Input_Widget::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);
}

void Key_Input_Widget::key_input_oper(int key)
{
#define NUMBER_CLICKED_CHECK 0

    if (key == KEY_INPUT_DEFINE_1) {
        info_reset();
#if NUMBER_CLICKED_CHECK

        if (check_pinyin == false) {
            QString str = chinese_widget->get_index_text(0);

            if (!str.isEmpty()) {
                emit current_clicked_chinese(str);
                info_reset();
            }
        }
#endif // if NUMBER_CLICKED_CHECK
    } else if ((key > KEY_INPUT_DEFINE_1) && (key <= KEY_INPUT_DEFINE_9)) {
#if NUMBER_CLICKED_CHECK

        if (check_pinyin == true) {
            if (t26_mode == false) {
                if (input_str.size() <= MAX_FIND_CHAR) {
                    input_str.append(QString::number(key - KEY_INPUT_DEFINE_0));
                    find_chinese();
                }
            }
        } else {
            int index = key - KEY_INPUT_DEFINE_0 - 1;
            QString str = chinese_widget->get_index_text(index);

            if (!str.isEmpty()) {
                emit current_clicked_chinese(str);
                info_reset();
            }
        }
#else // if NUMBER_CLICKED_CHECK

        if (t26_mode == false) {
            if (input_str.size() <= MAX_FIND_CHAR) {
                input_str.append(QString::number(key - KEY_INPUT_DEFINE_0));
                find_chinese();
            }
        }
#endif // if NUMBER_CLICKED_CHECK
    } else if ((key >= KEY_INPUT_DEFINE_A) && (key <= KEY_INPUT_DEFINE_Z)) {
        if (t26_mode == true) {
            if (input_str.size() <= MAX_FIND_CHAR) {
                char chr = key - KEY_INPUT_DEFINE_A + 'a';
                input_str.append(chr);
                find_chinese();
            }
        }
    } else {
        switch (key) {
        case KEY_INPUT_DEFINE_UP:

            if (check_pinyin == true) {
                pinyin_widget->select_prev_page();
            } else {
                chinese_widget->select_prev_page();
            }
            set_label_chinese_info();
            break;

        case KEY_INPUT_DEFINE_DOWN:

            if (check_pinyin == true) {
                pinyin_widget->select_next_page();
            } else {
                chinese_widget->select_next_page();
            }
            set_label_chinese_info();
            break;

        case KEY_INPUT_DEFINE_LEFT:

            if (check_pinyin == true) {
                pinyin_widget->select_prev();
            } else {
                chinese_widget->select_prev();
            }
            set_label_chinese_info();
            break;

        case KEY_INPUT_DEFINE_RIGHT:

            if (check_pinyin == true) {
                pinyin_widget->select_next();
            } else {
                chinese_widget->select_next();
            }
            set_label_chinese_info();
            break;

        case KEY_INPUT_DEFINE_ENTER:

            if (check_pinyin == true) {
                // 当前选中pinyin
                if (false == t26_mode) {
                    int index = pinyin_widget->get_current_index();

                    if ((index < T9chineselist.count()) && (index >= 0)) {
                        chinese_widget->set_text(T9chineselist[index]);
                    }
                } else {
                    emit clicked_enter();

                    // qDebug() << pinyin_widget->get_current_index();
                }
            } else {
                QString str = chinese_widget->get_current_text();

                if (!str.isEmpty()) {
                    emit current_clicked_chinese(str);
                } else {
                    emit clicked_enter();
                }

                info_reset();
            }
            break;

        case KEY_INPUT_DEFINE_DELETE:

            if (input_str.size() == 0) {
                emit clicked_delete();
            }
            input_str.remove(input_str.size() - 1, 1);
            find_chinese();
            break;

        case KEY_INPUT_DEFINE_CLEAN:
            info_reset();
            break;

        case KEY_INPUT_DEFINE_SWITCH:
            check_pinyin = !check_pinyin;

            if (check_pinyin == true) {
                pinyin_widget->set_check(true);
                chinese_widget->set_check(false);
            } else {
                chinese_widget->set_check(true);
                pinyin_widget->set_check(false);
            }
            set_label_chinese_info();
            break;

        case KEY_INPUT_DEFINE_PATTERN:
            t26_mode = !t26_mode;

            if (t26_mode == true) {
                label->setText(QString::fromUtf8("26键"));
            } else {
                label->setText(QString::fromUtf8("9键"));

                if (mT9obj != NULL) {
                    mT9obj->func.reset_search(mT9obj);
                }
            }
            info_reset();
            break;

        default:
            break;
        }
    }
}

void Key_Input_Widget::info_reset()
{
    // 清除本次记录
    input_str.clear();
    pycount = 0;
    pinyinlist.clear();
    T9chineselist.clear();
    T26chineselist.clear();
    pinyin_widget->reset();
    chinese_widget->reset();

    if (mT9obj != NULL) {
        mT9obj->func.reset_search(mT9obj);
    }
    set_label_chinese_info();
}

bool Key_Input_Widget::find_chinese()
{
    if (input_str.size() <= 0) {
        pinyin_widget->reset();
        chinese_widget->reset();
        return false;
    }

    if (t26_mode == true) {
        easyinput->get_chinese_value(input_str, T26chineselist);
        QStringList strlist;
        strlist.append(input_str);
        pinyin_widget->set_text(strlist);
        chinese_widget->set_text(T26chineselist);
    } else {
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
                // pinyinlist.append(QString(tabledata->data[i].PY));
                pinyinlist.append(QString(tabledata->data[i].PY) + " |");
                QString qstr;
                qstr.append(tabledata->data[i].MB);
                QStringList qlist = qstr.split(' ');
                T9chineselist.append(qlist);
            }

            if (pycount > 0) {
                pinyin_widget->set_text(pinyinlist);
                chinese_widget->set_text(T9chineselist[0]);
            }
        }
    }
    set_label_chinese_info();
    return true;
}

void Key_Input_Widget::set_label_chinese_info()
{
    if (true == check_pinyin) {
        int allcnt = pinyin_widget->get_row_cnt();
        int current_i = pinyin_widget->get_current_row();
        label_chinese_info->setText(QString("%1/%2").arg(current_i).arg(allcnt));
    } else {
        int allcnt = chinese_widget->get_row_cnt();
        int current_i = chinese_widget->get_current_row();
        label_chinese_info->setText(QString("%1/%2").arg(current_i).arg(allcnt));
    }
}

void Key_Input_Widget::Key_Input_Code(int m_key)
{
    int key = 0;

    if ((m_key >= KEY_DEFINE_INPUT_0) && (m_key <= KEY_DEFINE_INPUT_9)) {
        key = KEY_INPUT_DEFINE_0 + (m_key - KEY_DEFINE_INPUT_0);
    } else if ((m_key >= KEY_DEFINE_INPUT_A) && (m_key <= KEY_DEFINE_INPUT_Z)) {
        key = KEY_INPUT_DEFINE_A + (m_key - KEY_DEFINE_INPUT_A);
    } else {
        switch (m_key) {
        case KEY_DEFINE_INPUT_LEFT:
            key = KEY_INPUT_DEFINE_LEFT;
            break;

        case KEY_DEFINE_INPUT_RIGHT:
            key = KEY_INPUT_DEFINE_RIGHT;
            break;

        case KEY_DEFINE_INPUT_UP:
            key = KEY_INPUT_DEFINE_UP;
            break;

        case KEY_DEFINE_INPUT_DOWN:
            key = KEY_INPUT_DEFINE_DOWN;
            break;

        case KEY_DEFINE_INPUT_ENTER:
            key = KEY_INPUT_DEFINE_ENTER;
            break;

        case KEY_DEFINE_INPUT_ESC:
            key = KEY_INPUT_DEFINE_CLEAN;
            break;

        case KEY_DEFINE_INPUT_BACK:
            key = KEY_INPUT_DEFINE_DELETE;
            break;

        case KEY_DEFINE_INPUT_SPACE:
            key = KEY_INPUT_DEFINE_SWITCH;
            break;

        case KEY_DEFINE_INPUT_STAR:
            key = KEY_INPUT_DEFINE_PATTERN;
            break;

        default:
            break;
        }
    }
    key_input_oper(key);
}

void Key_Input_Widget::Key_System_Code(int m_key)
{
    int key = 0;

    if ((m_key >= Qt::Key_0) && (m_key <= Qt::Key_9)) {
        key = KEY_INPUT_DEFINE_0 + (m_key - Qt::Key_0);
    } else if ((m_key >= Qt::Key_A) && (m_key <= Qt::Key_Z)) {
        key = KEY_INPUT_DEFINE_A + (m_key - Qt::Key_A);
    } else {
        switch (m_key) {
        case Qt::Key_Left:
            key = KEY_INPUT_DEFINE_LEFT;
            break;

        case Qt::Key_Right:
            key = KEY_INPUT_DEFINE_RIGHT;
            break;

        case Qt::Key_Up:
            key = KEY_INPUT_DEFINE_UP;
            break;

        case Qt::Key_Down:
            key = KEY_INPUT_DEFINE_DOWN;
            break;

        case Qt::Key_Enter: // 小键盘
        case Qt::Key_Return:
            key = KEY_INPUT_DEFINE_ENTER;
            break;

        case Qt::Key_Escape:
            key = KEY_INPUT_DEFINE_CLEAN;
            break;

        case Qt::Key_Backspace:
            key = KEY_INPUT_DEFINE_DELETE;
            break;

        case Qt::Key_Tab:
            key = KEY_INPUT_DEFINE_SWITCH;
            break;

        case Qt::Key_Shift:
            key = KEY_INPUT_DEFINE_PATTERN;
            break;

        default:
            break;
        }
    }
    key_input_oper(key);
}
