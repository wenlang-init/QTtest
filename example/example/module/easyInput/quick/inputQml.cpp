#include "inputQml.h"
#include <QQuickWidget>
#include <QQmlEngine>
#include <QQmlContext>
#include <QBoxLayout>

InputQML::InputQML(QWidget *parent) : QDialog(parent)
{
    QQuickWidget *qw = new QQuickWidget(this);

    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);

    // qw->engine()->addImportPath("qrc:/");
    qw->rootContext()->setContextProperty("InputQML", this);

    // 初始化指定的资源,子项目中添加的qrc可能不能自动初始化
    Q_INIT_RESOURCE(qml);
    qw->setSource(QUrl("qrc:/virtualboardqml/VirtualKeyboard.qml"));

    // qw->setSource(QUrl("qrc:/src/souyin/edittxt.qml"));
    connect(qw, &QQuickWidget::sceneGraphError, this,
            [ = ](QQuickWindow::SceneGraphError error, const QString& message) {
        qDebug() << error << message;
    });
    connect(qw, &QQuickWidget::statusChanged, this,
            [ = ](QQuickWidget::Status status) {
        qDebug() << status;
    });

    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    hboxlayout->setContentsMargins(0, 0, 0, 0);
    hboxlayout->addWidget(qw);
}

QVariantList InputQML::set_input_value(QString value)
{
    QString pinying;
    QStringList dict;

    int count = easyinput.get_chinese_value(value, dict);
    QVariantList topDict;

    m_outStringList.clear();

    for (int i = 0; i < dict.size(); i++) {
        m_outStringList.append(dict[i]);

        if (i < 5) {
            topDict.append(dict[i]);
        }
    }
    return topDict;
}

QVariantList InputQML::get_all_icodedata()
{
    return m_outStringList;
}
