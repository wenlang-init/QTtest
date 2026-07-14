#include "homewidget.h"
#include <QDebug>
#include "src/desktop/desktopwidget.h"
#include <QHBoxLayout>
#include <QKeyEvent>
#include "src/public/funchelper.h"

// #include "input_method_widget.h"

#include "mainwindow.h"
#include "src/messageWidget/listmessageview.h"
#include "src/video/videowidget.h"
#include "src/graphics/graphicswidget.h"
#include "src/qmlList/listw.h"
#include "src/audio/widget.h"
#include "src/layout/wlayout.h"
#include "src/souyin/shouyinw.h"
#if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
# include "src/fftw/widegtfft.h"
# include "src/screen/screenwidget.h"
# include "src/screen/qmlwidgetloader.h"
#endif // if defined(Q_OS_WINDOWS) || defined(Q_OS_LINUX)
#include "src/fftw/customplotqmlwidget.h"
#include "src/map/mapwidget.h"
#include "src/map/MapDownload/mapdownloadwidget.h"
#include "src/map/MapView/mapviewwidget.h"
#include "src/map/MapView2/mapview2widget.h"
#include "src/map/MapView3/mapview3widget.h"
#include "src/qwindowkit/windowkitwidget.h"
#include "src/qwindowkit/witmainwindow.h"
#include "src/screen/testwidgetgl.h"
#include "setappwinfo.h"

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
        showW(index);
    });

    init();
    initDesktopFile();
    srand(QDateTime::currentMSecsSinceEpoch());

    for (int i = 0; i < desktopwidget->get_itemcount(); i++) {
        desktopwidget->setTextColor(i, QColor(rand() % 256, rand() % 256,
                                              rand() % 256));
    }

    // desktopwidget->setTextColor(QColor(222, 122, 211));

    // installEventFilter(this);
}

homewidget::~homewidget()
{
    foreach(auto node, m_widgetList) {
        if (node.widget) node.widget->deleteLater();
    }
}

void homewidget::init()
{
    widgetItem witem;
    int index = 0;

    witem.imageurl = ":/QtTheme/icon/check_box_checked/#4caf50.svg";
    witem.name = "FFT";
    witem.metaObject = &widegtFFT::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#8bc34a.svg";
    witem.name = "video";
    witem.metaObject = &videoWidget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#9e9e9e.svg";
    witem.name = "ListMessage";
    witem.metaObject = &ListMessageView::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#388e3c.svg";
    witem.name = "Graphics";
    witem.metaObject = &GraphicsWidget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#689f38.svg";
    witem.name = "List";
    witem.metaObject = &ListW::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#1976d2.svg";
    witem.name = "audio";
    witem.metaObject = &Widget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#2196f3.svg";
    witem.name = "layout";
    witem.metaObject = &wLayout::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#d32f2f.svg";
    witem.name = "兽音";
    witem.metaObject = &ShouYinW::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#f5f5f5.svg";
    witem.name = "异环抽卡";
    witem.metaObject = &MainWindow::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#f57c00.svg";
    witem.name = "截屏1";
    witem.metaObject = &screenWidgetShow::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#f44336.svg";
    witem.name = "截屏2";
    witem.metaObject = &qmlWidgetLoader::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#ff9800.svg";
    witem.name = "截屏3";
    witem.metaObject = &TestWidgetGL::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/check_box_checked/#ff9800.svg";
    witem.name = "customplotqml";
    witem.metaObject = &CustomPlotQMLWidget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#4caf50.svg";
    witem.name = "地图";
    witem.metaObject = &MapWidget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#ff9800.svg";
    witem.name = "地图下载";
    witem.metaObject = &MapDownLoadWidget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#4caf50.svg";
    witem.name = "地图显示1";
    witem.metaObject = &MapViewWidget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#ff9800.svg";
    witem.name = "地图显示2";
    witem.metaObject = &MapView2Widget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#4caf50.svg";
    witem.name = "地图显示3";
    witem.metaObject = &MapView3Widget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#ff9800.svg";
    witem.name = "QWindowKit";
    witem.metaObject = &WindowKitWidget::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#4caf50.svg";
    witem.name = "QWindowKitMainW";
    witem.metaObject = &WitMainWindow::staticMetaObject;
    m_widgetList.append(witem);
    witem.imageurl = ":/QtTheme/icon/radio_button_checked/#4caf50.svg";
    witem.name = "设置窗口";
    witem.metaObject = &setAppWInfo::staticMetaObject;
    m_widgetList.append(witem);

    foreach(auto node, m_widgetList) {
        desktopwidget->additem(node.imageurl, node.name);
    }
    desktopwidget->set_btn_current(0);
    qDebug() << m_widgetList.count();
}

void homewidget::initDesktopFile()
{
    QString filename;
    QPixmap pixmap;
    QString desktopPath =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QStringList fileList;
    QDir dir(desktopPath);

    foreach(auto file, dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (file.endsWith(".lnk")) {
            fileList.append(desktopPath + "/" + file);
        }
    }

    for (int i = 0; i < fileList.size(); i++) {
        if (false == FuncHelper::getInstance().getLinkInfo(fileList[i],
                                                           filename,
                                                           pixmap)) continue;
        desktopItem node;
        node.lnkName = fileList[i];
        node.fname = filename;

        // node.name = filename.split("/", Qt::SkipEmptyParts).last();
        // QStringList slist = node.name.split(".exe", Qt::SkipEmptyParts);
        // node.name = slist[slist.size() - 1];
        node.name = node.lnkName.split("/", Qt::SkipEmptyParts).last();
        node.name = node.name.split(".lnk", Qt::SkipEmptyParts).last();
        node.pixmap = pixmap;
        m_desktopList.append(node);
    }

    foreach(auto node, m_desktopList) {
        desktopwidget->additem(node.pixmap, node.name);
    }
    desktopwidget->set_btn_current(0);
    qDebug() << m_desktopList.count();
}

void homewidget::showWidget(int index)
{
    if ((index < 0) || (index >= m_widgetList.size())) {
        qDebug() << "Invalid widget index:" << index;
        return;
    }
    QWidget *w = qobject_cast<QWidget *>(
        m_widgetList[index].metaObject->newInstance());

    if (w) {
        m_widgetList[index].widget = w;

        QObject::connect(w, &QWidget::destroyed,
                         this, &homewidget::widgetDestroyed);
        w->setAttribute(Qt::WA_DeleteOnClose, true);
        w->resize(600, 600);
        w->show();
    }
}

void homewidget::showW(int index)
{
    if (index < m_widgetList.size()) {
        showWidget(index);
    } else {
        int i = index - m_widgetList.size();

        if (i < m_desktopList.size()) {
            QStringList sl = m_desktopList[i].fname.split("/");
            QString     workDir = m_desktopList[i].fname;

            if (sl.size() > 1) {
                workDir.remove("/" + sl[sl.size() - 1]);
            } else {
                workDir = QCoreApplication::applicationFilePath();
            }
            qDebug() << workDir;
            FuncHelper::getInstance().execCmd(m_desktopList[i].fname, workDir,
                                              {});
        }
    }
}

void homewidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up) {
        desktopwidget->set_switch_row(false);
    } else if (event->key() == Qt::Key_Down) {
        desktopwidget->set_switch_row(true);
    } else if (event->key() == Qt::Key_Left) {
        desktopwidget->set_switch_one(false);
    } else if (event->key() == Qt::Key_Right) {
        desktopwidget->set_switch_one(true);
    } else if ((event->key() == Qt::Key_Return) ||
               (event->key() == Qt::Key_Enter)) {
        int index = desktopwidget->get_current_index();
        showW(index);
    }
    QWidget::keyPressEvent(event);
}

void homewidget::widgetDestroyed() {
    for (int i = 0; i < m_widgetList.size(); i++) {
        if (m_widgetList[i].widget == sender()) {
            qDebug() << "widget destroyed:" << m_widgetList[i].name;
            m_widgetList[i].widget = nullptr;
            break;
        }
    }
}
