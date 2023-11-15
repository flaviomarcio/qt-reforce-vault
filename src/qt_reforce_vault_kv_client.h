#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantHash>
#include <QPair>
#include <QVariantMap>
#include "./qt_reforce_vault_global.h"
#include "./qt_reforce_vault_setting.h"

namespace QtVault {
class KvClientPvt;

typedef QPair<QString,QVariant> VariantPair;

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
    //! \brief KvClient
    //! \param settings
    //! \param parent
    //!
    explicit KvClient(const QVariant &settings, QObject *parent=nullptr);

    //!
    //! \brief setting
    //! \return
    //!
    Setting &setting();
    const KvClient &setting(const QVariant &newValues)const;

    //!
    //! \brief clear
    //! \return
    //!
    const KvClient &clear()const;

    //!
    //! \brief clean
    //! \return
    //!
    const KvClient &clean()const;

    //!
    //! \brief values
    //! \param newValues
    //! \return
    //!
    const KvClient &values(const QString &newValues)const;

    //!
    //! \brief values
    //! \param newValues
    //! \return
    //!
    const KvClient &values(const QVariantHash &newValues)const;

    //!
    //! \brief values
    //! \param newValues
    //! \return
    //!
    const KvClient &values(const QVariantMap &newValues)const;

    //!
    //! \brief values
    //! \param newValues
    //! \return
    //!
    const KvClient &values(const VariantPair &newValues)const;

    //!
    //! \brief values
    //! \param key
    //! \param value
    //! \return
    //!
    const KvClient &values(const QString &key,const QString &value)const;

    //!
    //! \brief put
    //! \param newValues
    //! \return
    //!
    const KvClient &put(const QString &newValues)const;

    //!
    //! \brief put
    //! \param newValues
    //! \return
    //!
    const KvClient &put(const QVariantHash &newValues)const;

    //!
    //! \brief put
    //! \param newValues
    //! \return
    //!
    const KvClient &put(const VariantPair &newValues)const;

    //!
    //! \brief put
    //! \param key
    //! \param value
    //! \return
    //!
    const KvClient &put(const QString &key,const QString &value)const;

    //!
    //! \brief remove
    //! \param key
    //! \return
    //!
    const KvClient &rm(const QString &key)const;

    //!
    //! \brief rm
    //! \param keys
    //! \return
    //!
    const KvClient &rm(const QVariantHash &keys)const;

    //!
    //! \brief rm
    //! \param keys
    //! \return
    //!
    const KvClient &rm(const QVariantMap &keys)const;

    //!
    //! \brief rm
    //! \param keys
    //! \return
    //!
    const KvClient &rm(const QVariantList &keys)const;

    //!
    //! \brief rm
    //! \param keys
    //! \return
    //!
    const KvClient &rm(const QStringList &keys)const;

    //!
    //! \brief get
    //! \return
    //!
    const QVariantHash &get()const;

    //!
    //! \brief get
    //! \param key
    //! \return
    //!
    const QString get(const QString &key)const;

    //!
    //! \brief get
    //! \param keys
    //! \return
    //!
    const QVariantHash get(const QVariantList &keys)const;

    //!
    //! \brief get
    //! \param keys
    //! \return
    //!
    const QVariantHash get(const QStringList &keys)const;

    //!
    //! \brief get
    //! \param keys
    //! \return
    //!
    const QVariantHash get(const QVariantHash &keys)const;

    //!
    //! \brief get
    //! \param keys
    //! \return
    //!
    const QVariantHash get(const QVariantMap &keys)const;

    //!
    //! \brief isLoading
    //! \return
    //!
    bool isLoading() const;

    //!
    //! \brief isLoaded
    //! \return
    //!
    bool isLoaded() const;

    //!
    //! \brief isSuccessful
    //! \return
    //!
    bool isSuccessful() const;

    //!
    //! \brief isAuthenticated
    //! \return
    //!
    bool isAuthenticated()const;

    //!
    //! \brief pull
    //! \return
    //!
    const KvClient &pull()const;

    //!
    //! \brief push
    //! \return
    //!
    const KvClient &push()const;

    //!
    //! \brief systemEnvironmentSet
    //! \return
    //!
    const KvClient &systemEnvironmentSet()const;

    //!
    //! \brief systemEnvironmentUnSet
    //! \return
    //!
    const KvClient &systemEnvironmentUnSet()const;

    //!
    //! \brief onStarted
    //! \param method
    //! \return
    //!
    const KvClient &onStarted(VoidKvMethod method)const;

    //!
    //! \brief onLoaded
    //! \param method
    //! \return
    //!
    const KvClient &onLoaded(VoidMapMethod method)const;

    //!
    //! \brief onFail
    //! \param method
    //! \return
    //!
    const KvClient &onFail(VoidMapMethod method)const;

    //!
    //! \brief onFinished
    //! \param method
    //! \return
    //!
    const KvClient &onFinished(VoidKvMethod method)const;

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

