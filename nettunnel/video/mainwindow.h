#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    void seturl(const QUrl& url);

private:

    Ui::MainWindow *ui;
    QMediaPlayer *m_mediaPlayer;
};
#endif // MAINWINDOW_H
