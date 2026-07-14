#ifndef WINDOWKITWIDGET_H
#define WINDOWKITWIDGET_H

#include <QWidget>
#include <QWKWidgets/widgetwindowagent.h>
#include <widgetframe/windowbar.h>

class WindowKitWidget : public QWidget {
    Q_OBJECT

public:

    enum Theme {
        Dark,
        Light,
    };
    Q_ENUM(Theme)

    Q_INVOKABLE explicit WindowKitWidget(QWidget *parent = nullptr);

private:

    void loadStyleSheet(Theme theme);
    void resizeEvent(QResizeEvent *event) override;

private:

    Theme currentTheme = Dark;
    QWK::WidgetWindowAgent *windowAgent;
    QWK::WindowBar *windowBar;

signals:
};

#endif // WINDOWKITWIDGET_H
