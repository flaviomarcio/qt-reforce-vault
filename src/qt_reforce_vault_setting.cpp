#include "./qt_reforce_vault_setting.h"
#include <QDebug>
#include <QMetaEnum>
#include <QCoreApplication>

namespace QtReforce{

static const auto __vault_v1="v1";

class SettingPvt:public QObject
{
public:
    Setting *parent;
    Setting::Method method=Setting::Token;
    QUrl url;
    QString import;
    QString version;
    QString token, roleId, secretId;
    explicit SettingPvt(Setting *parent):QObject{parent},parent{parent}{
    }

};

Setting::Setting(QObject *parent):QObject{parent},p{new SettingPvt{this}}
{
}

Setting::Method Setting::method() const
{
    return p->method;
}

Setting &Setting::method(const Method &newMethod)
{
    if (p->method == newMethod)
        return *this;
    p->method = newMethod;
    emit methodChanged();
    return *this;
}

Setting &Setting::resetMethod()
{
    return method(Setting::Token);
}

const QString &Setting::version() const
{
    if(p->version.isEmpty())
        p->version=__vault_v1;
    return p->version;
}

Setting &Setting::version(const QString &newVersion)
{
    if (p->version == newVersion.trimmed())
        return *this;
    p->version = newVersion.trimmed();
    emit versionChanged();
    return *this;
}

Setting &Setting::resetVersion()
{
    return this->version({});
}

const QString &Setting::token() const
{
    return p->token;
}

Setting &Setting::token(const QString &newToken)
{
    if (p->token == newToken)
        return *this;
    p->token = newToken;
    emit tokenChanged();
    return *this;
}

Setting &Setting::resetToken()
{
    return this->token({});
}

const QString &Setting::roleId() const
{
    return p->roleId;
}

Setting &Setting::roleId(const QString &newRoleId)
{
    if (p->token == newRoleId)
        return *this;
    p->token = newRoleId;
    emit roleIdChanged();
    return *this;
}

Setting &Setting::resetRoleId()
{
    return this->roleId({});
}

const QString &Setting::secretId() const
{
    return p->secretId;
}

Setting &Setting::secretId(const QString &newSecretId)
{
    if (p->secretId == newSecretId)
        return *this;
    p->secretId = newSecretId.trimmed();
    emit secretIdChanged();
    return *this;
}

Setting &Setting::resetSecretId()
{
    return this->secretId({});
}

const QUrl &Setting::url() const
{
    return p->url;
}

Setting &Setting::url(const QVariant &newUrl)
{
    QUrl __url;
    switch (newUrl.typeId()) {
    case QMetaType::QUrl:
        __url=newUrl.toUrl();
        break;
    case QMetaType::QString:
    case QMetaType::QByteArray:
        __url=QUrl(newUrl.toString());
        break;
    default:
        break;
    }
    if (p->url == __url)
        return *this;
    p->url = __url;
    emit urlChanged();
    return *this;
}

Setting &Setting::resetUrl()
{
    return this->url(QUrl{});
}

const QString &Setting::import() const
{
    return p->import;
}

Setting &Setting::import(const QString &newImport)
{
    if(p->import==newImport)
        return *this;
    p->import=newImport.trimmed();
    emit importChanged();
    return *this;
}

Setting &Setting::resetImport()
{
    return this->import({});
}


}

