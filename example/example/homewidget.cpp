#include "homewidget.h"
#include <QDebug>
#include "src/desktop/desktopwidget.h"
#include <QHBoxLayout>

// #include "input_method_widget.h"

#include "mainwindow.h"
#include "src/fftw/widegtfft.h"
#include "src/messageWidget/listmessageview.h"
#include "src/video/videowidget.h"
#include "src/graphics/graphicswidget.h"
#include "src/qmlList/listw.h"
#include "src/audio/widget.h"
#include "src/layout/wlayout.h"
#include "src/souyin/shouyinw.h"

homewidget::homewidget(QWidget *parent)
{
    // Input_Method_Widget::instance().set_mode(INPUT_MODE_VIRTUAL1);
    // Input_Method_Widget::instance().show();
    // connect(&Input_Method_Widget::instance(),
    //         &Input_Method_Widget::clicked_text,
    //         this, [ = ](QString text) {
    //     qDebug() << text;
    // });
    // connect(&Input_Method_Widget::instance(),
    //         &Input_Method_Widget::clicked_value,
    //         this, [ = ](int value) {
    //     qDebug() << value;
    // });

    // connect(this, &homewidget::keyClicked,
    //         &Input_Method_Widget::instance(),
    //         &Input_Method_Widget::Key_System_Code);

    desktopwidget = new DeskTopWidget(this);
    QHBoxLayout *hlayout = new QHBoxLayout(this);

    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(desktopwidget);

    int rowCount = 3;
    desktopwidget->set_page_count(rowCount * 2);
    desktopwidget->set_row_count(rowCount);

    // desktopwidget->set_switch_page(true);
    // desktopwidget->set_switch_row(true);
    // desktopwidget->set_switch_one(true);
    connect(desktopwidget, &DeskTopWidget::itemclicked, this, [ = ](int index) {
        // qDebug() << "item clicked:" << index;

        if (index < m_widgetList.count()) {
            if (m_widgetList[index].widget->isHidden()) {
                m_widgetList[index].widget->show();
            }
        }
    });

    init();
}

homewidget::~homewidget()
{
    foreach(auto node, m_widgetList) {
        delete node.widget;
    }
}

void homewidget::init()
{
    widgetItem witem;

    witem.imageurl = ":/MacOS/actions/object-select-symbolic.symbolic.png";
    {
        widegtFFT *w = new widegtFFT;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &widegtFFT::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "FFT";
        m_widgetList.append(witem);
    }
    {
        videoWidget *w = new videoWidget;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &videoWidget::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "video";
        m_widgetList.append(witem);
    }
    {
        ListMessageView *w = new ListMessageView;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &ListMessageView::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "ListMessage";
        m_widgetList.append(witem);
    }
    {
        GraphicsWidget *w = new GraphicsWidget;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &GraphicsWidget::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "Graphics";
        m_widgetList.append(witem);
    }
    {
        ListW *w = new ListW;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &ListW::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "List";
        m_widgetList.append(witem);
    }
    {
        Widget *w = new Widget;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &Widget::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "audio";
        m_widgetList.append(witem);
    }
    {
        wLayout *w = new wLayout;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &wLayout::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "layout";
        m_widgetList.append(witem);
    }
    {
        ShouYinW *w = new ShouYinW;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &ShouYinW::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "兽音";
        m_widgetList.append(witem);
    }
    {
        MainWindow *w = new MainWindow;
        w->resize(600, 600);
        w->hide();

        // w->setAttribute(Qt::WA_DeleteOnClose, true);
        // QObject::connect(w, &MainWindow::destroyed,
        //                  this, &homewidget::widgetDestroyed);
        witem.widget = w;
        witem.name = "异环抽卡";
        m_widgetList.append(witem);
    }

    foreach(auto node, m_widgetList) {
        desktopwidget->additem(node.imageurl, node.name);
    }
    qDebug() << m_widgetList.count();
}

void homewidget::widgetDestroyed() {
    qDebug() << "widegtFFT -------" << sender(); // ->metaObject()->metaType();
}
