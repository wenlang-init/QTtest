#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    struct tnode {
        int index;
        int cnt;
    };

    Q_INVOKABLE explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void addWindow(QString     processName,
                   const char *lpClassName,
                   const char *lpWindowName);
    int  lottery(int           count,
                 int           cnt1,
                 int           cnt2,
                 double        prob_early,
                 double        prob_late,
                 int         & underway,
                 QList<tnode>& vlist,
                 QList<tnode>& vlistbase);

    void _init();

protected:

    // void resizeEvent(QResizeEvent *event) override;

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_checkBox_checkStateChanged(const Qt::CheckState& arg1);

private:

    Ui::MainWindow *ui;
    int m_underway = 0;
    QVector<qint64>m_vlistbase;
    qint64 m_count = 0;
};
#endif // MAINWINDOW_H
