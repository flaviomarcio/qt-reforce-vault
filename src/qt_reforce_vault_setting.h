#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QUrl>
#include "./qt_reforce_vault_global.h"

namespace QtVault {
class SettingPvt;
class KvClient;

//!
//! \brief The Setting class
//!
class Q_REFORCE_VAULT_EXPORT Setting: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant printOnFail READ printOnFail WRITE printOnFail RESET resetPrintOnFail NOTIFY printOnFailChanged FINAL)
    Q_PROPERTY(QVariant method READ method WRITE method RESET resetMethod NOTIFY methodChanged FINAL)
    Q_PROPERTY(QVariant url READ url WRITE url RESET resetUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(QVariant version READ version WRITE version RESET resetVersion NOTIFY versionChanged FINAL)
    Q_PROPERTY(QVariant nameSpace READ nameSpace WRITE nameSpace RESET resetNameSpace NOTIFY nameSpaceChanged FINAL)
    Q_PROPERTY(QVariant token READ token WRITE token RESET resetToken NOTIFY tokenChanged FINAL)
    Q_PROPERTY(QVariant roleId READ roleId WRITE roleId RESET resetRoleId NOTIFY roleIdChanged FINAL)
    Q_PROPERTY(QVariant secretId READ secretId WRITE secretId RESET resetSecretId NOTIFY secretIdChanged FINAL)
    Q_PROPERTY(QVariant secretsPath READ secretsPath WRITE secretsPath RESET resetSecretsPath NOTIFY secretsPathChanged FINAL)
    Q_PROPERTY(QVariant secretsName READ secretsName WRITE secretsName RESET resetSecretsName NOTIFY secretsNameChanged FINAL)
    Q_PROPERTY(QVariant secretsRevision READ secretsRevision WRITE secretsRevision RESET resetSecretsRevision NOTIFY secretsRevisionChanged FINAL)

public:
    enum Method{
        Token, AppRole
    };
    Q_ENUM(Method)

    //!
    //! \brief Setting
    //! \param parent
    //!
    Q_INVOKABLE explicit Setting(QObject *parent=nullptr);
    explicit Setting(const QVariant &values, QObject *parent=nullptr);

    //!
    //! \brief kv
    //! \return
    //!
    KvClient *kv();

    //!
    //! \brief clear
    //! \return
    //!
    Setting &clear();

    //!
    //! \brief setValues
    //! \param newValues
    //! \return
    //!
    const Setting &setValues(const QVariant &newValues);

    //!
    //! \brief printOnFail
    //! \return
    //!
    bool printOnFail() const;
    Setting &printOnFail(const QVariant &newPrintOnFail);
    Setting &resetPrintOnFail();

    //!
    //! \brief url
    //! \return
    //!
    const QUrl &url() const;
    Setting &url(const QVariant &newValues);
    Setting &resetUrl();

    //!
    //! \brief method
    //! \return
    //!
    Method method() const;
    Setting &method(const QVariant &newValues);
    Setting &resetMethod();

    //!
    //! \brief version
    //! \return
    //!
    const QByteArray &version() const;
    Setting &version(const QVariant &newValues);
    Setting &resetVersion();

    //!
    //! \brief nameSpace
    //! \return
    //!
    const QByteArray &nameSpace() const;
    Setting &nameSpace(const QVariant &newValues);
    Setting &resetNameSpace();

    //!
    //! \brief token
    //! \return
    //!
    const QByteArray &token() const;
    Setting &token(const QVariant &newValues);
    Setting &resetToken();

    //!
    //! \brief roleId
    //! \return
    //!
    const QByteArray &roleId() const;
    Setting &roleId(const QVariant &newValues);
    Setting &resetRoleId();

    //!
    //! \brief secretId
    //! \return
    //!
    const QByteArray &secretId() const;
    Setting &secretId(const QVariant &newValues);
    Setting &resetSecretId();

    //!
    //! \brief secretPath
    //! \return
    //!
    const QUrl &secretsPath() const;
    Setting &secretsPath(const QVariant &newValues);
    Setting &resetSecretsPath();

    //!
    //! \brief secretsName
    //! \return
    //!
    const QByteArray &secretsName() const;
    Setting &secretsName(const QVariant &newValues);
    Setting &resetSecretsName();

    //!
    //! \brief secretsRevision
    //! \return
    //!
    int secretsRevision() const;
    Setting &secretsRevision(const QVariant &newValues);
    Setting &resetSecretsRevision();


private:

    //!
    //! \brief p
    //!
    SettingPvt *p=nullptr;

signals:
    void printOnFailChanged();
    void urlChanged();
    void methodChanged();
    void versionChanged();
    void nameSpaceChanged();
    void tokenChanged();
    void roleIdChanged();
    void secretIdChanged();
    void secretsPathChanged();
    void secretsNameChanged();
    void secretsRevisionChanged();
};

}

