#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantHash>
#include "./qt_reforce_vault_global.h"
#include "./qt_reforce_vault_setting.h"

namespace QtReforce {
class KvClientPvt;

//!
//! \brief The KvClient class
//!
class Q_REFORCE_VAULT_EXPORT KvClient: public QObject
{
    Q_OBJECT
public:

    typedef std::function<void(const KvClient &values)> VoidKvMethod;
    typedef std::function<void(const QVariantHash &values)> VoidMapMethod;

    //!
    //! \brief KvClient
    //! \param parent
    //!
    Q_INVOKABLE explicit KvClient(QObject *parent=nullptr);

    //!
    //! \brief values
    //! \return
    //!
    const QVariantHash &values()const;
    const KvClient &values(const QVariantHash &newValues)const;
    const KvClient &values(const QString &key,const QString &value)const;

    //!
    //! \brief value
    //! \param key
    //! \return
    //!
    const QString value(const QString &key)const;

    //!
    //! \brief isLoading
    //! \return
    //!
    bool isLoading();

    //!
    //! \brief setting
    //! \return
    //!
    Setting &setting();

    //!
    //! \brief load
    //! \return
    //!
    KvClient &load();

    //!
    //! \brief update
    //! \return
    //!
    KvClient &update();

    //!
    //! \brief revert
    //! \return
    //!
    KvClient &revert();

    //!
    //! \brief onStarted
    //! \param method
    //! \return
    //!
    KvClient &onStarted(VoidKvMethod method);

    //!
    //! \brief onLoaded
    //! \param method
    //! \return
    //!
    KvClient &onLoaded(VoidMapMethod method);

    //!
    //! \brief onFail
    //! \param method
    //! \return
    //!
    KvClient &onFail(VoidMapMethod method);

    //!
    //! \brief onFinished
    //! \param method
    //! \return
    //!
    KvClient &onFinished(VoidKvMethod method);

signals:
    //!
    //! \brief started
    //!
    void started();

    //!
    //! \brief loaded
    //! \param kv
    //!
    void loaded(QVariantHash &kv);

    //!
    //! \brief fail
    //! \param fail
    //!
    void fail(const QVariantHash &fail);

    //!
    //! \brief finished
    //!
    void finished();

private:

    //!
    //! \brief p
    //!
    KvClientPvt *p=nullptr;
};

}

