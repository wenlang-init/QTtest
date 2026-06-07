#include "input_management_object.h"
#include <QApplication>

Input_Management_Object& Input_Management_Object::instance()
{
    static Input_Management_Object m_pManager;

    return m_pManager;
}

Input_Management_Object::Input_Management_Object(QObject *parent) : QObject(parent)
{
    QString appstr = qApp->applicationDirPath(); // 程序路径

    appstr += "/easyInput/chinese_words.txt";
    mT9obj = init_pyobj((char *)appstr.toStdString().c_str());

    easyinput = new easyInput();
}

Input_Management_Object::~Input_Management_Object()
{
    if (mT9obj != NULL) {
        mT9obj->func.delete_pyobj(mT9obj);
    }
    easyinput->deleteLater();
}

PY_OBJ * Input_Management_Object::get_t9obj()
{
    return mT9obj;
}

easyInput * Input_Management_Object::get_t26obj()
{
    return easyinput;
}
