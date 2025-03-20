#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

// 实现高亮显示
class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:

    /* 使用私有结构存储突出显示规则：规则由QRegularExpression和
     * QTextCharFormat的实例组成。
     * 并使用QVector存储各种规则。*/
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    // 跨行的表达式需要特殊处理
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    /* QTextCharFormat类提供QTextDocument中字符的格式设置信息，
     * 以指定文本的视觉属性，以及有关其在超文本文档中的作用的信息。
     * QTextCharFormat::setFontWeight()定义字体粗细
     * QTextCharFormat::setForeground()函数定义颜色。*/
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // HIGHLIGHTER_H
