#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>
#include "./qt_reforce_vault_global.h"

namespace QtReforce {
class SettingPvt;

//!
//! \brief The Setting class
//!
class Q_REFORCE_VAULT_EXPORT Setting: public QObject
{
    Q_OBJECT
    Q_PROPERTY(Method method READ method WRITE method RESET resetMethod NOTIFY methodChanged FINAL)
    Q_PROPERTY(QVariant url READ url WRITE url RESET resetUrl NOTIFY urlChanged FINAL)
    Q_PROPERTY(QString version READ version WRITE version RESET resetVersion NOTIFY versionChanged FINAL)
    Q_PROPERTY(QString token READ token WRITE token RESET resetToken NOTIFY tokenChanged FINAL)
    Q_PROPERTY(QString roleId READ roleId WRITE roleId RESET resetRoleId NOTIFY roleIdChanged FINAL)
    Q_PROPERTY(QString secretId READ secretId WRITE secretId RESET resetSecretId NOTIFY secretIdChanged FINAL)
    Q_PROPERTY(QString import READ import WRITE import RESET resetImport NOTIFY importChanged FINAL)
public:
    enum Method{
        Token, RoleId
    };
    Q_ENUM(Method)
    //!
    //! \brief Setting
    //! \param parent
    //!
    Q_INVOKABLE explicit Setting(QObject *parent=nullptr);

    //!
    //! \brief method
    //! \return
    //!
    Method method() const;
    Setting &method(const Method &newMethod);
    Setting &resetMethod();

    //!
    //! \brief version
    //! \return
    //!
    const QString &version() const;
    Setting &version(const QString &newVersion);
    Setting &resetVersion();

    //!
    //! \brief token
    //! \return
    //!
    const QString &token() const;
    Setting &token(const QString &newToken);
    Setting &resetToken();

    //!
    //! \brief roleId
    //! \return
    //!
    const QString &roleId() const;
    Setting &roleId(const QString &newRoleId);
    Setting &resetRoleId();

    //!
    //! \brief secretId
    //! \return
    //!
    const QString &secretId() const;
    Setting &secretId(const QString &SecretId);
    Setting &resetSecretId();

    //!
    //! \brief url
    //! \return
    //!
    const QUrl &url() const;
    Setting &url(const QVariant &newUrl);
    Setting &resetUrl();

    //!
    //! \brief import
    //! \return
    //!
    const QString &import() const;
    Setting &import(const QString &newImport);
    Setting &resetImport();


private:

    //!
    //! \brief p
    //!
    SettingPvt *p=nullptr;

signals:

    void methodChanged();
    void versionChanged();
    void tokenChanged();
    void roleIdChanged();
    void secretIdChanged();
    void urlChanged();
    void importChanged();
};

}

