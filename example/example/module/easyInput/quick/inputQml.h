#ifndef INPUTQML_H
#define INPUTQML_H

#include <QDialog>
#include "../googleinput/easyinput.h"

class InputQML : public QDialog {
    Q_OBJECT

public:

    explicit InputQML(QWidget *parent = nullptr);

    Q_INVOKABLE QVariantList set_input_value(QString value);
    Q_INVOKABLE QVariantList get_all_icodedata();

private:

    easyInput easyinput;
    QVariantList m_outStringList;
};

#endif // ifndef INPUTQML_H
