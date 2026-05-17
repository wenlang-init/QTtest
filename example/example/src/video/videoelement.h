#ifndef VIDEOELEMENT_H
#define VIDEOELEMENT_H

#include <QObject>
#include <QtQml>

/*
 * cpp添加QML_ELEMENT

   cmake:
   qt_add_qml_module(appMyProject
    URI com.mycompany.qmlcomponents
    VERSION 1.0
    SOURCES
        videoelement.h videoelement.cpp
   )
 *
   qmake:
   HEADERS += videoelement.h
   SOURCES += videoelement.cpp
   CONFIG += qmltypes
   QML_IMPORT_NAME = com.mycompany.qmlcomponents
   QML_IMPORT_MAJOR_VERSION = 1

   qml:
   import com.mycompany.qmlcomponents 1.0
    VideoElement {
        id: videoElement
        Component.onCompleted: videoElement.sayHello()
    }

   也可以在命名空间中定义元素，例如：
   namespace MyNamespace {
       Q_NAMESPACE
       QML_ELEMENT

       enum MyEnum {
           Key1,
           Key2,
       };
       Q_ENUM_NS(MyEnum)
   }
    然后qml中：console.log(MyNamespace.Key2)

   限制范围使用：
   namespace MyNamespace {
     Q_NAMESPACE
     enum MyEnum { MyEnumerator = 10 };
     Q_ENUM_NS(MyEnum)
   }
   class QmlType : public QObject
   {
     Q_OBJECT
     QML_ELEMENT
     QML_EXTENDED_NAMESPACE(MyNamespace)
   }
   qml中：
   QmlType {
     property int i: QmlType.MyEnumerator // i will be 10
   }

 * */
class videoElement : public QObject {
    Q_OBJECT
    QML_ELEMENT // 声明为qml元素，默认qml元素名称为类名
    // QML_NAMED_ELEMENT(testelementname) // 自定义qml元素名称

    // QML_SINGLETON //单例
    // private:
    //     explicit videoElement(QObject *parent = nullptr);
    // public:
    //      static videoElement *create(QQmlEngine *qmlEngine, QJSEngine
    // *jsEngine)
    //      {
    //          static videoElement result;
    //          return &result;
    //      }

public:

    explicit videoElement(QObject *parent = nullptr);
    Q_INVOKABLE void sayHello() {
        qDebug() << "Hello from C++!";
    }

signals:
};

#endif // VIDEOELEMENT_H
