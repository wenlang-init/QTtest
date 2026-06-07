#ifndef INPUT_MANAGEMENT_OBJECT_H
#define INPUT_MANAGEMENT_OBJECT_H

#include <QObject>
#include "../googleinput/easyinput.h"
#include "../t9pinyin/py_t9_core.h"

class Input_Management_Object : public QObject {
    Q_OBJECT

private:

    explicit Input_Management_Object(QObject *parent = NULL);

public:

    static Input_Management_Object& instance();

    ~Input_Management_Object();

    PY_OBJ   * get_t9obj();
    easyInput* get_t26obj();

signals:

public slots:

private:

    easyInput *easyinput;
    PY_OBJ *mT9obj;
};

#endif // INPUT_MANAGEMENT_OBJECT_H
