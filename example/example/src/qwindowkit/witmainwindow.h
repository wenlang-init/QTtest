#ifndef WITMAINWINDOW_H
#define WITMAINWINDOW_H

#include <QMainWindow>

namespace QWK {
class WidgetWindowAgent;
class StyleAgent;
} // namespace QWK

class WitMainWindow : public QMainWindow {
    Q_OBJECT

public:

    Q_INVOKABLE WitMainWindow(QWidget *parent = nullptr);
    ~WitMainWindow();

    enum Theme {
        Dark,
        Light,
    };
    Q_ENUM(Theme)

Q_SIGNALS:

    void themeChanged();

protected:

    bool event(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:

    void installWindowAgent();
    void loadStyleSheet(Theme theme);

private:

    Theme currentTheme{};
    QWK::WidgetWindowAgent *windowAgent;
};
#endif // WITMAINWINDOW_H
