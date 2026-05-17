#include "shouyinw.h"
#include "ui_shouyinw.h"
#include "decode/beastSound.h"
#include <QMessageBox>
#include "public/funchelper.h"
#include "decode/autf8.h"
#include "usecode.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QThread>

// #include "decode/base64.h"

// 检查变体选择符
bool isVariationSelector(char32_t codePoint) {
    return (codePoint >= 0xFE00 && codePoint <= 0xFE0F) || // 标准VS1~VS16
           (codePoint >= 0xE0100 && codePoint <= 0xE01EF); // 补充 变体选择器补充字符
}

// 遍历QString检测
void checkVariationSelectors(const QString& str) {
    for (int i = 0; i < str.length();) {
        QChar ch = str[i];
        char32_t codePoint;

        if (ch.isHighSurrogate() && (i + 1 < str.length()) &&
            str[i + 1].isLowSurrogate()) {
            codePoint = QChar::surrogateToUcs4(ch, str[i + 1]);

            if (isVariationSelector(codePoint)) {
                qDebug() << "Found supplementary VS at position" << i;
                i += 2; // 跳过代理对
                continue;
            }
        } else {
            codePoint = ch.unicode();

            if (isVariationSelector(codePoint)) {
                qDebug() << "Found standard VS at position" << i;
            }
        }
        i++;
    }
}

struct utf8node
{
    QString  word;
    char32_t variant;
    utf8node() {
        word = "";
        variant = 0;
    }
};

QList<utf8node>getVariationSelectors(const QString& input) {
    QList<utf8node> vsList;

    // qDebug() << input.size();

    for (int i = 0; i < input.length();) {
        QChar current = input[i];

        if (current.isHighSurrogate() && (i + 1 < input.length()) &&
            input[i + 1].isLowSurrogate()) {
            char32_t codePoint = QChar::surrogateToUcs4(current, input[i + 1]);

            if (!isVariationSelector(codePoint)) {
                utf8node vsEntry;
                vsEntry.word.append(current);
                vsEntry.word.append(input[i + 1]);
                vsList.append(vsEntry);

                // qDebug() << __FILE__ << __LINE__ << i << current <<input[i +
                // 1] << vsEntry.word;
            } else {
                vsList.last().variant = codePoint;

                // qDebug() << __FILE__ << __LINE__ << i << current << input[i +
                // 1];
            }
            i += 2;
            continue;
        }

        if (!isVariationSelector(current.unicode())) {
            utf8node vsEntry;
            vsEntry.word.append(current);
            vsList.append(vsEntry);

            // qDebug() << __FILE__ << __LINE__ << i << vsEntry.word
            // <<current.unicode();
        } else {
            // qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << i << current;

            if (vsList.size() > 0) vsList.last().variant = current.unicode();
        }
        i++;
    }
    return vsList;
}

// 移除变体选择符
QString removeVariationSelectors(const QString& input) {
    QString result;

    for (int i = 0; i < input.length();) {
        QChar current = input[i];

        if (current.isHighSurrogate() && (i + 1 < input.length())) {
            QChar next = input[i + 1];

            if (next.isLowSurrogate()) {
                char32_t codePoint = QChar::surrogateToUcs4(current, next);

                if (!isVariationSelector(codePoint)) {
                    result.append(current);
                    result.append(next);
                }
                i += 2;
                continue;
            }
        }

        if (!isVariationSelector(current.unicode())) {
            result.append(current);
        }
        i++;
    }
    return result;
}

// 添加标准VS (U+FE0F 示例)
QString addEmojiVariant(const QString& baseChar) {
    return baseChar + QChar(0xFE0F);
}

// 添加标准VS 0xFE00~0xFE0F
QString addEmojiVariant(const QString& baseChar, char32_t variant) {
    if ((variant < 0xFE00) || (variant > 0xFE0F)) {
        return baseChar; // 非法范围，返回原字符
    }
    return baseChar + QChar(variant);
}

// 添加补充VS (U+E0100 示例)
QString addSupplementaryVariant(const QString& baseChar) {
    return baseChar + QChar(0xDB40) + QChar(0xDD00); // 代理对表示 U+E0100
}

// 添加补充VS 0xE0100~0xE01EF
QString addSupplementaryVariant(const QString& baseChar, char32_t variant) {
    if ((variant < 0xE0100) || (variant > 0xE01EF)) {
        return baseChar; // 非法范围，返回原字符
    }
    char32_t codePoint = variant - 0x10000;
    QChar    highSurrogate = QChar((codePoint >> 10) + 0xD800);
    QChar    lowSurrogate = QChar((codePoint & 0x3FF) + 0xDC00);
    return baseChar + highSurrogate + lowSurrogate;
}

#include <QStyleHints>
void setTheme(QWidget *w)
{
    w->isFullScreen();
#ifdef Q_OS_ANDROID
    QFile file;

# if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto scheme = QGuiApplication::styleHints()->colorScheme();

    if (scheme == Qt::ColorScheme::Dark) {
        file.setFileName(":/qss/darkstyle.qss");
    } else {
        file.setFileName(":/qss/lightstyle.qss");
    }
# else // if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    file.setFileName(":/qss/MacOS.qss");
# endif // if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)

    if (file.open(QIODevice::ReadOnly)) {
        w->setStyleSheet(file.readAll());
        file.close();
    }

#endif // ifdef Q_OS_ANDROID
}

ShouYinW::ShouYinW(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShouYinW)
    , checkindex(1)
{
    ui->setupUi(this);
    QFont font = ui->spinBox->font();
    font.setCapitalization(QFont::AllUppercase);
    ui->spinBox->setFont(font);
    ui->spinBox->setDisplayIntegerBase(16);

    ui->textEditIn->setFocus();

    // setAttribute(Qt::WA_DeleteOnClose, true);

    QStringList strlist;
    strlist << "MD4" << "MD5" << "SHA-1" << "SHA-224"
            << "SHA-256" << "SHA-384" << "SHA-512" << "SHA3-224"
            << "SHA3-256" << "SHA3-384" << "SHA3-512" << "Keccak-224"
            << "Keccak-256" << "Keccak-384" << "Keccak-512" << "BLAKE2b-160"
            << "BLAKE2b-25" << "BLAKE2b-384" << "BLAKE2b-512" << "BLAKE2s-128"
            << "BLAKE2s-160" << "BLAKE2s-224" << "BLAKE2s-25";

    ui->comboBox->addItems(strlist);
    ui->comboBox->setEditable(false);
    ui->comboBox->setCurrentIndex(1);

    connect(this,
            &ShouYinW::getFileHash,
            &FuncHelper::getInstance(),
            &FuncHelper::slotGetFileHash);
    connect(&FuncHelper::getInstance(), &FuncHelper::sig_hashinfo, this,
            [ = ](int value) {
        QString info = QString("计算中...%1%").arg(value);
        ui->textEditOut->setPlainText(info);
    });
    connect(&FuncHelper::getInstance(), &FuncHelper::sig_hashfinished, this,
            [ = ](const QString& hash) {
        ui->textEditOut->setPlainText(hash);
        ui->pushButtonmd52->setEnabled(true);
    });
    setTheme(this);
}

ShouYinW::~ShouYinW()
{
    delete ui;

    FuncHelper::getInstance().stopGetFileHASH();
}

void ShouYinW::on_pushButton1_clicked()
{
    QString qsdict = ui->lineEdit->text();

#if 0
    QList<utf8node> utf8wordtab = getVariationSelectors(qsdict);

    qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << utf8wordtab.size();

    for (int i = 0; i < utf8wordtab.size(); i++) {
        qDebug() << __FILE__ << __LINE__ << __FUNCTION__ <<
            utf8wordtab[i].word.size() << utf8wordtab[i].word <<
            utf8wordtab[i].variant;

        for (int j = 0; j < utf8wordtab[i].word.size(); j++) {
            if (utf8wordtab[i].word[j].isHighSurrogate()) {
                qDebug() << i << j << utf8wordtab[i].word;
            }
        }
    }
    {
        QByteArray utf8d = qsdict.toUtf8();
        int cnt = 0, ret, unicodecnt = 0, len = utf8d.size();
        unsigned long *unicode = (unsigned long *)malloc(
            sizeof(unsigned long) * len);

        while (cnt < len) {
            ret = UTF8_getc((const unsigned char *)utf8d.data() + cnt,
                            len - cnt,
                            &unicode[unicodecnt]);

            if (ret < 0) {
                printf("UTF8_getc error\n"); fflush(stdout);
                break;
            }
            cnt += ret;
            unicodecnt++;
        }
        qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << unicodecnt;

        for (int i = 0; i < unicodecnt; i++) {
            qDebug() << __FILE__ << __LINE__ << __FUNCTION__ << i <<
                QString::number(unicode[i], 16);
        }
        free(unicode);
    }
#endif // if 0
    qsdict = removeVariationSelectors(qsdict);

    // for(int i=0;i<qsdict.length();i++){
    //     QChar high = qsdict[i];
    //     qDebug()<<"11111111111" << high;
    //     // 字符❤️(\u2764\ufe0f组合字符)解析出问题
    //     if (high.isHighSurrogate() && i + 1 < qsdict.length()) {
    //         QChar low = qsdict[i + 1];
    //         qDebug()<<"222222222222" << high << low;
    //         if (low.isLowSurrogate()) {
    //             uint32_t codePoint = QChar::surrogateToUcs4(high, low);
    //             // 处理码点
    //             i++; // 跳过低代理
    //             qDebug()<<"333333333333" << high << low <<
    // QString::number(codePoint,16);
    //             continue;
    //         }
    //     }
    // }
    // 字符❤️(\u2764\ufe0f组合字符)解析出问题
    auto ucs4 = qsdict.toUcs4(); // 字符的Unicode如果超过16bit,QString.size 返回2

    // ，根据这个函数可得到正确个数

    if (ucs4.size() != 4) {
        QMessageBox::information(this, "提示",
                                 QString("秘钥为4个不同的字(注:不能是❤️这样的组合字符),%1").arg(
                                     qsdict));
        qDebug() << ucs4 << ucs4.size() << qsdict << qsdict.size() <<
            qsdict.unicode() << qsdict.split("");

        QString s = removeVariationSelectors(qsdict);
        auto    _ucs4 = s.toUcs4();
        qDebug() << _ucs4 << _ucs4.size() << s << s.size() << s.unicode();

        // QString text = u8"▶";  // U+25B6
        // text += QChar(0xFE0F); // 添加VS-16（Emoji样式）
        // qDebug() << text;      // 显示为Emoji风格的▶
        // QString clean = removeVariationSelectors(text);
        // qDebug() << clean;     // 显示为文本风格的▶
        return;
    }

    for (int i = 0; i < ucs4.size(); i++) {
        for (int j = i + 1; j < ucs4.size(); j++) {
            if (ucs4.at(i) == ucs4.at(j)) {
                QMessageBox::information(this,
                                         "提示",
                                         QString(
                                             "秘钥为4个不同的字(注:不能是❤️这样的组合字符),%1").arg(
                                             qsdict));
                qDebug() << ucs4 << ucs4.size() << qsdict << qsdict.size();
                return;
            }
        }
    }
    QByteArray  dictqb[4];
    const char *dict[4];

    for (int i = 0; i < 4; i++) {
#if 0
        dictqb[i] = QString::fromUcs4((const uint *)&ucs4[i]).toUtf8(); // QChar::fromUcs4(ucs4[i]);
#else // if 0
        char utf8[7];
        int  cnt = UTF8_putc((unsigned char *)utf8, 6, ucs4[i]);

        if (cnt < 1) {
            QMessageBox::information(this, "提示", QString("秘钥错误,%1").arg(qsdict));
            return;
        }
        dictqb[i].append(utf8, cnt);
        dictqb[i].append("\0");
#endif // if 0
        dict[i] = dictqb[i].constData();

        // DEBUG_PRINT_LOG("%s\n",dict[i]);
        // DEBUG_PRINT_LOG("%s\n",utf8);
        // qDebug()<<QString::fromUtf8(utf8);
    }
    QByteArray indata = ui->textEditIn->toPlainText().toUtf8();

    if (indata.size() < 1) return;

    switch (checkindex) {
    case 1:
    {
        char *p =
            toBeastSound_2byte((const unsigned char *)indata.constData(),
                               (int)indata.size(),
                               dict);

        if (p) {
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditOut->setPlainText(dest);
        } else {
            QMessageBox::information(this, "提示", "编码错误");
        }
        break;
    }

    case 2:
    {
        char *p =
            toBeastSound_4byte((const unsigned char *)indata.constData(),
                               (int)indata.size(),
                               dict);

        if (p) {
            QString dest;
            dest.append(p);
            free(p);
            QString dd;

            // for (int i = 0; i < dest.size(); i++) {
            //     qDebug() << i << dest[i] << QChar(0x2764);
            //     dd.append(dest[i]);

            //     if (QChar(0x2764) == dest[i]) dd.append(QChar(0xFE0F));

            //     // dd=addEmojiVariant(dd,0xFE0F);
            //     ui->textEditOut->setPlainText(dd);
            // }

            ui->textEditOut->setPlainText(dest);
        } else {
            QMessageBox::information(this, "提示", "编码错误");
        }
        break;
    }

    case 3:
    {
        char *p = toBeastSound((const char *)indata.constData(),
                               (int)indata.size(),
                               dict);

        if (p) {
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditOut->setPlainText(dest);
        } else {
            QMessageBox::information(this, "提示", "编码错误");
        }
        break;
    }

    default:
        break;
    }
}

void ShouYinW::on_pushButton2_clicked()
{
    QByteArray indata = ui->textEditOut->toPlainText().toUtf8();

    if (indata.size() < 1) return;

    switch (checkindex) {
    case 1:
    {
        char *p = fromBeastSound_2byte((const unsigned char *)indata.constData(),
                                       (int)indata.size());

        if (p) {
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditIn->setPlainText(dest);
        } else {
            QMessageBox::information(this, "提示", "解码错误");
        }
        break;
    }

    case 2:
    {
        char *p = fromBeastSound_4byte((const unsigned char *)indata.constData(),
                                       (int)indata.size());

        if (p) {
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditIn->setPlainText(dest);
        } else {
            QMessageBox::information(this, "提示", "解码错误");
        }
        break;
    }

    case 3:
    {
        int   destsize;
        char *p = fromBeastSound((const char *)indata.constData(),
                                 (int)indata.size(),
                                 &destsize);

        if (p) {
            QByteArray destb;
            destb.append(p, destsize);
            free(p);
            QString dest = destb;
            ui->textEditIn->setPlainText(dest);
        } else {
            QMessageBox::information(this, "提示", "解码错误");
        }
        break;
    }

    default:
        break;
    }
}

void ShouYinW::on_checkBox1_clicked()
{
    checkindex = 1;
    ui->checkBox1->setChecked(true);
    ui->checkBox1_2->setChecked(false);
    ui->checkBox3->setChecked(false);
}

void ShouYinW::on_checkBox1_2_clicked()
{
    checkindex = 2;
    ui->checkBox1->setChecked(false);
    ui->checkBox1_2->setChecked(true);
    ui->checkBox3->setChecked(false);
}

void ShouYinW::on_checkBox3_clicked()
{
    checkindex = 3;
    ui->checkBox1->setChecked(false);
    ui->checkBox1_2->setChecked(false);
    ui->checkBox3->setChecked(true);
}

void ShouYinW::on_pushButtonb1_clicked()
{
    if (ui->textEditIn->toPlainText().size() < 1) return;

    QByteArray data = ui->textEditIn->toPlainText().toUtf8().toBase64();
    ui->textEditOut->setPlainText(data);
}

void ShouYinW::on_pushButtonb2_clicked()
{
    if (ui->textEditOut->toPlainText().size() < 1) return;

    QByteArray data = QByteArray::fromBase64(
        ui->textEditOut->toPlainText().toUtf8());
    ui->textEditIn->setPlainText(QString::fromUtf8(data));
}

void ShouYinW::on_pushButton_clicked()
{
    useCode ucode;

    if (ui->textEditIn->toPlainText().size() < 1) return;

    QByteArray data = ucode.getCode(
        ui->textEditIn->toPlainText().toStdString().c_str(),
        ui->spinBox->value());

    // qDebug().noquote() << data;
    ui->textEditOut->setPlainText(data);

    // QByteArray aa =
    //
    //
    // "sadsapkansak;lfsa\nadsgasdfagdskhkjbfdhsdfhsljfdgsuahsjlkjfdbgsalsjashgashdaushiguisahkjgiashgdiuashdigaiushhgdisusad1阿法狗萨芬";
    // QByteArray bb = ucode.GzipCompress(aa);
    // QByteArray cc = ucode.GZipUnCompress(bb);
    // qDebug() << aa.size() << bb.size() << cc.size() << aa.compare(cc);
    // qDebug() << ucode.GzipCompress("D:\\杂项\\下载\\OP_720.wmv",
    //                                "D:\\杂项\\下载\\OP_720.wmv.zip");
    // qDebug() << ucode.GZipUnCompress("D:\\杂项\\下载\\OP_720.wmv.zip",
    //                                  "D:\\杂项\\下载\\OP_720-.wmv");
}

void ShouYinW::on_pushButtonu_clicked()
{
    useCode ucode;

    if (ui->textEditOut->toPlainText().size() < 1) return;

    QByteArray data = ucode.getDecode(
        ui->textEditOut->toPlainText().toStdString().c_str());
    ui->textEditIn->setPlainText(data);
}

void ShouYinW::on_pushButtonmd51_clicked()
{
    QByteArray data = ui->textEditIn->toPlainText().toUtf8();
    int value = getHashValue(ui->comboBox->currentIndex());
    QString md5 =
        FuncHelper::getInstance().getDataHash(data,
                                              (QCryptographicHash::Algorithm)value);

    ui->textEditOut->setPlainText(md5);
}

void ShouYinW::on_pushButtonmd52_clicked()
{
    QFileDialog fileDialog(this);

    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("打开文件"));
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::
                                                              MoviesLocation)
                            .value(0, QDir::homePath()));

    if (fileDialog.exec() == QDialog::Accepted) {
        if (fileDialog.selectedFiles().isEmpty()) return;

        QString file = fileDialog.selectedFiles().constFirst();

        ui->textEditIn->setPlainText(file);

        FuncHelper::getInstance().stopGetFileHASH();
        int value = ui->comboBox->currentIndex();
        ui->pushButtonmd52->setEnabled(false);
        emit getFileHash(file, getHashValue(value));

        // QString md5 = FuncHelper::getInstance().getFileMD5(file);
        // ui->textEditOut->setPlainText(md5);
    }
}

void ShouYinW::on_pushButtonmd53_clicked()
{
    FuncHelper::getInstance().stopGetFileHASH();
}

int ShouYinW::getHashValue(int value) {
    int control = QCryptographicHash::Algorithm::Md5;

    switch (value) {
    case 0:
        control = QCryptographicHash::Md4;
        break;

    case 1:
        control = QCryptographicHash::Md5;
        break;

    case 2:
        control = QCryptographicHash::Sha1;
        break;

    case 3:
        control = QCryptographicHash::Sha224;
        break;

    case 4:
        control = QCryptographicHash::Sha256;
        break;

    case 5:
        control = QCryptographicHash::Sha384;
        break;

    case 6:
        control = QCryptographicHash::Sha512;
        break;

    case 7:
        control = QCryptographicHash::Sha3_224;
        break;

    case 8:
        control = QCryptographicHash::Sha3_256;
        break;

    case 9:
        control = QCryptographicHash::Sha3_384;
        break;

    case 10:
        control = QCryptographicHash::Sha3_512;
        break;

    case 11:
        control = QCryptographicHash::Keccak_224;
        break;

    case 12:
        control = QCryptographicHash::Keccak_256;
        break;

    case 13:
        control = QCryptographicHash::Keccak_384;
        break;

    case 14:
        control = QCryptographicHash::Keccak_512;
        break;

    case 15:
        control = QCryptographicHash::Blake2b_160;
        break;

    case 16:
        control = QCryptographicHash::Blake2b_256;
        break;

    case 17:
        control = QCryptographicHash::Blake2b_384;
        break;

    case 18:
        control = QCryptographicHash::Blake2b_512;
        break;

    case 19:
        control = QCryptographicHash::Blake2s_128;
        break;

    case 20:
        control = QCryptographicHash::Blake2s_160;
        break;

    case 21:
        control = QCryptographicHash::Blake2s_224;
        break;

    case 22:
        control = QCryptographicHash::Blake2s_256;
        break;

    default:
        break;
    }
    return control;
}
