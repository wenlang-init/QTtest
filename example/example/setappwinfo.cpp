#include "setappwinfo.h"
#include "ui_setappwinfo.h"
#include <QFont>
#include <QFontDatabase>
#include <QTimer>
#include <QFontDialog>
#include <QColorDialog>
#include <QPalette>

setAppWInfo::setAppWInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setAppWInfo)
{
    ui->setupUi(this);
    QSharedPointer<QFontDialog> sp(new QFontDialog, [](QFontDialog *p) {
                                   p->deleteLater();
        });

    m_FontDialog = new QFontDialog;

    // m_FontDialog->setOption(QFontDialog::DontUseNativeDialog);
    connect(m_FontDialog, &QFontDialog::fontSelected, this,
            [ = ](const QFont& font) {
        qApp->setFont(font);
    });

    m_SPColorDialog =
        QSharedPointer<QColorDialog>(new QColorDialog, [ = ](QColorDialog *p) {
        qDebug() << p;
        p->deleteLater();
    });

    connect(m_SPColorDialog.data(), &QColorDialog::colorSelected, this,
            [ = ](const QColor& color) {
        QPalette pe(qApp->palette());
        pe.setColor(QPalette::WindowText, color);
        qApp->setPalette(pe);
    });

    // m_fontlist = QFontDatabase::families();
    // QStringList listtmp = QFontDatabase::families();
    // bool ok;
    // QFontDialog::FontDialogOptions options;
    // options |= QFontDialog::ScalableFonts;
    // options |= QFontDialog::MonospacedFonts;
    // options |= QFontDialog::DontUseNativeDialog;
    // QFont font = QFontDialog::getFont(&ok, qApp->font(), parent,
    //                                   "选择字体", options);
    // qDebug() << ok << font;

    // QFont font(ui->comboBox->currentText());
    // qApp->setFont(font);
}

setAppWInfo::~setAppWInfo()
{
    delete ui;
    delete m_FontDialog;
}

void setAppWInfo::on_pushButton_clicked()
{
    m_FontDialog->show();
}

void setAppWInfo::on_pushButton_2_clicked()
{
    m_SPColorDialog->show();
}
