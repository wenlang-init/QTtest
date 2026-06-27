#include "mapviewwidget.h"
#include "ui_mapviewwidget.h"
#include <qfiledialog.h>
#include <QApplication>
#include <QDebug>

MapViewWidget::MapViewWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MapViewWidget)
{
    ui->setupUi(this);
    this->resize(800, 600);
    qApp->setStyleSheet("*{font: 9pt '宋体';}");
    this->setWindowTitle(QString("QT加载显示离线瓦片地图示例--V%1").arg(1.0));
}

MapViewWidget::~MapViewWidget()
{
    delete ui;
}

/**
 * @brief 获取瓦片路径并显示
 */
void MapViewWidget::on_pushButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                     "瓦片路径",
                                                     qApp->applicationDirPath(),
                                                     QFileDialog::ShowDirsOnly |
                                                     QFileDialog::DontResolveSymlinks);

    if (path.isEmpty()) return;

    ui->lineEdit->setText(path);
    ui->graphicsView->setPath(path);
}
