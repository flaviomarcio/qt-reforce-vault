#include "./qt_reforce_vault_setting.h"
#include "./qt_reforce_vault_kv_client.h"
#include "./private/p_qt_reforce_vault_utils.h"
#include <QDebug>
#include <QMetaEnum>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>

namespace QtVault{
static const auto __vault_v1="v1";
static const auto __vault_protocol="vault://";
static const auto __vault_default_url="http://localhost:1234";
static const auto __vault_default_path="vault://secret/secret";
static const auto __vault_default_method=Setting::Method::Token;

static const auto __method="method";
static const auto __url="url";
static const auto __version="version";
static const auto __token="token";
static const auto __roleId="roleId";
static const auto __secretId="secretId";
static const auto __secretsPath="secretsPath";
static const auto __secretsName="secretsName";
static const auto __secretsRevision="secretsRevision";

class SettingPvt:public QObject
{
public:
    const QStringList ignore_methods={"destroyed", "objectName", "objectNameChanged", "deleteLater", "_q_reregisterTimers"};
    Setting *parent;
    Setting::Method method=Setting::Token;
    QUrl url=QUrl(__vault_default_url);
    QByteArray version;
    QByteArray token, roleId, secretId;
    QUrl secretsPath=QUrl(__vault_default_path);
    QByteArray secretsName;
    int secretsRevision=0;
    explicit SettingPvt(Setting *parent):QObject{parent},parent{parent}
    {
    }

    explicit SettingPvt(const QVariant &values, Setting *parent):QObject{parent},parent{parent}
    {
        this->setValues(values);
    }

    Setting::Method toMethod(const QVariant &v)
    {
        auto e=QMetaEnum::fromType<Setting::Method>();
        auto eValue=Utils::toEnumType(e,v);
        return eValue<0
                   ?__vault_default_method
                   :Setting::Method(eValue);
    }

    void setValues(const QVariant &newValues)
    {
        auto vHash=Utils::toHash(newValues);

        this->method=toMethod(vHash.value(__method));
        this->url=QUrl(vHash.value(__url).toByteArray());
        this->version=vHash.value(__version).toByteArray();
        this->token=vHash.value(__token).toByteArray();
        this->roleId=vHash.value(__roleId).toByteArray();
        this->secretId=vHash.value(__secretId).toByteArray();
        this->secretsPath=QUrl(vHash.value(__secretsPath).toByteArray());
        this->secretsName=vHash.value(__secretsName).toByteArray();
        this->secretsRevision=vHash.value(__secretsRevision).toInt();

        if(this->url.isEmpty())
            this->url=__vault_default_url;

        if(this->secretsPath.isEmpty())
            this->secretsPath=__vault_default_path;
    }
};

Setting::Setting(QObject *parent):QObject{parent},p{new SettingPvt{this}}
{
}

Setting::Setting(const QVariant &values, QObject *parent):QObject{parent}, p{new SettingPvt{values, this}}
{
}

KvClient *Setting::kv()
{
    return dynamic_cast<KvClient*>(this->parent());
}

Setting &Setting::clear()
{
    auto metaObject = this->metaObject();
    for (int col = 0; col < metaObject->propertyCount(); ++col) {
        auto property = metaObject->property(col);
        if(!property.isResettable())
            continue;
        if(p->ignore_methods.contains(property.name()))
            continue;
        property.reset(this);
    }
    return *this;
}

Setting &Setting::setValues(const QVariant &newValues)
{
    this->clear();
    p->setValues(newValues);
    return *this;
}

const QUrl &Setting::url() const
{
    return p->url;
}

Setting &Setting::url(const QVariant &newValues)
{
    QUrl __url;
    switch (newValues.typeId()) {
    case QMetaType::QUrl:
        __url=newValues.toUrl();
        break;
    case QMetaType::QString:
    case QMetaType::QByteArray:
        __url=QUrl(newValues.toString());
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
    return this->url(QUrl(__vault_default_url));
}

Setting::Method Setting::method() const
{
    return p->method;
}

Setting &Setting::method(const QVariant &newValues)
{
    auto newMethod=p->toMethod(newValues);
    if(newMethod==p->method)
        return *this;

    emit methodChanged();
    return *this;
}

Setting &Setting::resetMethod()
{
    return method(__vault_default_method);
}

const QByteArray &Setting::version() const
{
    if(p->version.isEmpty())
        p->version=__vault_v1;
    return p->version;
}

Setting &Setting::version(const QVariant &newValues)
{
    if (p->version == newValues.toByteArray().trimmed())
        return *this;
    p->version = newValues.toByteArray().trimmed();
    emit versionChanged();
    return *this;
}

Setting &Setting::resetVersion()
{
    return this->version({});
}

const QByteArray &Setting::token() const
{
    return p->token;
}

Setting &Setting::token(const QVariant &newValues)
{
    if (p->token == newValues.toByteArray().trimmed())
        return *this;
    p->token = newValues.toByteArray().trimmed();
    emit tokenChanged();
    return *this;
}

Setting &Setting::resetToken()
{
    return this->token({});
}

const QByteArray &Setting::roleId() const
{
    return p->roleId;
}

Setting &Setting::roleId(const QVariant &newValues)
{
    if (p->token == newValues.toString().trimmed())
        return *this;
    p->token = newValues.toByteArray().trimmed();
    emit roleIdChanged();
    return *this;
}

Setting &Setting::resetRoleId()
{
    return this->roleId({});
}

const QByteArray &Setting::secretId() const
{
    return p->secretId;
}

Setting &Setting::secretId(const QVariant &newValues)
{
    if (p->secretId == newValues.toString())
        return *this;
    p->secretId = newValues.toByteArray().trimmed();
    emit secretIdChanged();
    return *this;
}

Setting &Setting::resetSecretId()
{
    return this->secretId({});
}

const QUrl &Setting::secretsPath() const
{
    return p->secretsPath;
}

Setting &Setting::secretsPath(const QVariant &newValues)
{
    switch (newValues.typeId()) {
    case QMetaType::QUrl:
        p->secretsPath=newValues.toUrl();
        break;
    case QMetaType::QString:
    case QMetaType::QByteArray:
    {
        auto str=newValues.toString().trimmed();
        QUrl url;
        if(!str.isEmpty()){
            url=(str.startsWith(__vault_protocol))
                      ?QUrl(str)
                      :QUrl(__vault_protocol+str);
        }
        p->secretsPath=url;
        break;
    }
    default:
        p->secretsPath=QUrl{};
        break;
    }
    emit secretsPathChanged();
    return *this;
}

Setting &Setting::resetSecretsPath()
{
    return this->secretsPath(QUrl(__vault_default_path));
}

const QByteArray &Setting::secretsName() const
{
    return p->secretsName;
}

Setting &Setting::secretsName(const QVariant &newValues)
{
    if(p->secretsName==newValues.toByteArray().trimmed())
        return *this;
    p->secretsName=newValues.toByteArray().trimmed();
    emit secretsNameChanged();
    return *this;
}

Setting &Setting::resetSecretsName()
{
    return this->secretsName({});
}

int Setting::secretsRevision() const
{
    return p->secretsRevision;
}

Setting &Setting::secretsRevision(const QVariant &newValues)
{
    int newRevision=newValues.toInt();
    if(p->secretsRevision==newRevision)
        return *this;
    p->secretsRevision=newRevision;
    emit secretsRevisionChanged();
    return *this;
}

Setting &Setting::resetSecretsRevision()
{
    return this->secretsRevision(0);
}


}

