#ifndef LISTMESSAGEVIEW_H
#define LISTMESSAGEVIEW_H

#include <QWidget>

class ListMessageView : public QWidget {
    Q_OBJECT

public:

    Q_INVOKABLE explicit ListMessageView(QWidget *parent = nullptr);

signals:
};

#endif // LISTMESSAGEVIEW_H
