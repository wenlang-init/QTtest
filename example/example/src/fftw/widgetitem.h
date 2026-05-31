#ifndef WIDGETSITEM_H
#define WIDGETSITEM_H

// 注册 mian()
// qmlRegisterType<WidgetItem>("custto.qwidget.item", 1, 0, "ThirdWidgetItem");

// 示例要嵌入的widget
// sceneWidget = new TestWidget(":/images/2.jpg",1020,596);
// bomWidget = new TestWidget(":/images/4.jpg",1280,596);
// rootWidget->engine()->rootContext()->
//     setContextProperty("thScentWidget",sceneWidget);
// rootWidget->engine()->rootContext()->
//     setContextProperty("thBomWidget",bomWidget);

// 在qml中使用
// ThirdWidgetItem{
//     id:scencbg
//     anchors.fill: parent
//     thirdWidget: thScentWidget
// }

#include <QQuickPaintedItem>
#include <QWidget>
#include <QPainter>
#include <QVariant>

class WidgetItem : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(QVariant thirdWidget READ getThirdWidget WRITE setThirdWidget)

public:

    explicit WidgetItem(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent)
    {
        setFlag(ItemHasContents, true);
    }

    virtual ~WidgetItem();

    QVariant& getThirdWidget() {
        // m_thirdWidget = QVariant::fromValue(m_widget);
        return m_thirdWidget;
    }

    void setThirdWidget(QVariant widget) {
        m_thirdWidget = widget;

        // m_widget = widget.value<QWidget *>();
    }

protected:

    void paint(QPainter *painter) override {}

    void geometryChange(const QRectF& newGeometry,
                        const QRectF& oldGeometry) override {
        QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);

        QWidget *m_widget = static_cast<QWidget *>(m_thirdWidget.data());

        if (m_widget) {
            m_widget->setGeometry(newGeometry.toRect());
        }
    }

    bool eventFilter(QObject *obj, QEvent *e) override {
        QWidget *m_widget = (QWidget *)m_thirdWidget.data();

        if ((obj == m_widget) && (e->type() == QEvent::Paint)) {
            update();
        }
        return QQuickPaintedItem::eventFilter(obj, e);
    }

private:

    QVariant m_thirdWidget;
};

#endif // WIDGETSITEM_H
