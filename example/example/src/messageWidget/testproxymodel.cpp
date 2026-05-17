#include "testproxymodel.h"
#include "messagemodel.h"
#include <QDebug>
#include <QDateTime>
#include <QImage>

#define qdebug qDebug().noquote() << "[" << __FILE__ << ":" << __LINE__ << "] " << \
        __FUNCTION__ << "() "

TestProxyModel::TestProxyModel(QObject *parent)
    : QIdentityProxyModel{parent}
{}

QVariant TestProxyModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    // 这里使用setSourceModel的处理,如果设置为MessageModel,则使用MessageModel::data
    if ((role >= (int)MessageModel::Roles::firstRoles) &&
        (role <= (int)MessageModel::Roles::lastRoles)) {
        return sourceModel()->data(index, role);
    }

    // QVariant vdata;
    // 这里实现此处需要的自定义处理
    // return vdata;

    return QIdentityProxyModel::data(index, role);
}

void TestProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QIdentityProxyModel::setSourceModel(sourceModel);

    if (!sourceModel) {
        return;
    }
    connect(sourceModel, &QAbstractItemModel::modelReset, [this] {
        Q_UNUSED(this)

        // static_cast<MessageModel *>(sourceModel())->reset();
    });
    connect(sourceModel, &QAbstractItemModel::rowsInserted,
            [this](const QModelIndex& /*parent*/, int first, int last) {
        Q_UNUSED(this)
        Q_UNUSED(first)
        Q_UNUSED(last)

        // static_cast<MessageModel *>(sourceModel())->inserted();
    });
}

bool TestProxyModel::addData(QDateTime& messageTime,
                             bool& isSend,
                             QString& messageData, QString& imagename)
{
    MessageInfo *mdata = new MessageInfo;

    mdata->set_isSend(isSend);
    mdata->set_messageTime(messageTime.toMSecsSinceEpoch());
    mdata->set_messageData(messageData);
    mdata->set_headPortrait(imagename);
    return static_cast<MessageModel *>(sourceModel())->addData(mdata);
}

bool TestProxyModel::updateData(const int& index,
                                QDateTime& messageTime,
                                bool& isSend,
                                QString& messageData, QString& imagename)
{
    MessageInfo *mdata = new MessageInfo;

    mdata->set_isSend(isSend);
    mdata->set_messageTime(messageTime.toMSecsSinceEpoch());
    mdata->set_messageData(messageData);
    mdata->set_headPortrait(imagename);
    return static_cast<MessageModel *>(sourceModel())->updateData(index, mdata);
}

bool TestProxyModel::deleteData(const int& row)
{
    return static_cast<MessageModel *>(sourceModel())->deleteData(row);
}

void TestProxyModel::clearData()
{
    static_cast<MessageModel *>(sourceModel())->clearData();
}

int TestProxyModel::rowCount()
{
    return static_cast<MessageModel *>(sourceModel())->rowCount();
}

int TestProxyModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<MessageModel *>(sourceModel())->rowCount(parent);
}
