#ifndef WINDOWKITWIDGET_H
#define WINDOWKITWIDGET_H

#include <QWidget>
#include <QWKWidgets/widgetwindowagent.h>

class WindowKitWidget : public QWidget {
    Q_OBJECT

public:

    enum Theme {
        Dark,
        Light,
    };
    Q_ENUM(Theme)

    explicit WindowKitWidget(QWidget *parent = nullptr);

private:

    void loadStyleSheet(Theme theme);

private:

    Theme currentTheme = Dark;
    QWK::WidgetWindowAgent *windowAgent;

signals:
};

#endif // WINDOWKITWIDGET_H
