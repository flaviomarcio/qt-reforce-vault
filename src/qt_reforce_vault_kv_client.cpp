#include "./qt_reforce_vault_kv_client.h"
#include "./qt_reforce_vault_setting.h"
#ifdef QTREFORCE_QREQUEST_CLIENT
#include "../../qrequestclient/qt_reforce_request_client.h"
#else
#include "./private/qrequestclient/qt_reforce_request_client.h"
#endif
#include "./private/p_qt_reforce_vault_utils.h"
#include "./private/p_qt_reforce_vault_logs.h"
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QJsonDocument>
#include <QCoreApplication>


//ref
//  https://developer.hashicorp.com/vault/tutorials/getting-started/getting-started-intro
//  https://developer.hashicorp.com/vault/tutorials/getting-started/getting-started-apis
//  https://developer.hashicorp.com/vault/tutorials/getting-started/getting-started-authentication

using namespace QtReforce;

namespace QtVault{

static const auto __auth="auth";
static const auto __client_token="client_token";
static const auto __data="data";
static const auto __splitEnv="=";
static const auto __X_Vault_Token="X-Vault-Token";
static const auto __role_id="role_id";
static const auto __secret_id="secret_id";
static const auto __content_type="Content-Type";
static const auto __application_json="application/json";

Q_GLOBAL_STATIC(QVariantHash, systemEnvironment)

QVariantHash makeEnvs()
{
    QVariantHash __return;
    QProcess process;
    auto lst = process.systemEnvironment();
    for (auto &v : lst) {
        auto s = v.split(__splitEnv);
        if(s.size()>2){
            auto env = s.takeFirst().trimmed();
            auto value = s.join(__splitEnv);
            __return.insert(env, value);
        }
        else{
            auto env = s.first().trimmed();
            auto value = s.last().trimmed();
            __return.insert(env, value);
        }
    }
    return __return;
}


static void init()
{
    *systemEnvironment=makeEnvs();
}

Q_COREAPP_STARTUP_FUNCTION(init)


class KvClientPvt:public QObject
{
public:
    KvClient *parent;
    Setting setting;
    QByteArray currentToken;
    KvClient::VoidKvMethod onStarted=nullptr;
    KvClient::VoidMapMethod onLoaded=nullptr;
    KvClient::VoidMapMethod onFail=nullptr;
    KvClient::VoidKvMethod onFinished=nullptr;

    QVariantHash envOriginal;
    QVariantHash data;
    QVariantHash vaultMetaData;
    bool isAuthenticated=false;
    bool isLoading=false;
    bool isLoaded=false;
    bool isSuccessful=false;

    explicit KvClientPvt(KvClient *parent):QObject{parent},parent{parent},setting{parent}
    {
    }

    explicit KvClientPvt(const QVariant &values, KvClient *parent):QObject{parent},parent{parent},setting{values, parent}
    {
    }

    void clear()
    {
        this->vaultMetaData.clear();
        this->isAuthenticated=false;
        this->clean();
    }

    void clean()
    {
        this->data.clear();
        this->vaultMetaData.clear();
        this->isLoading=false;
        this->isLoaded=false;
        this->isSuccessful=false;
    }

    QUrl makeUrlVaultLogin()
    {
        auto url=this->setting.url().toString();
        auto version=this->setting.version().trimmed().toLower();
        auto import=this->setting.secretsName().trimmed();
        import=import.isEmpty()?"":("/"+import);

        static const auto __format=QString("%1/%2/auth/approle/login");
        url=__format.arg(url,version);
        return QUrl(url);
    }

    QUrl makeUrlVaultData()
    {
        auto nameSpace=this->setting.nameSpace().isEmpty()
                             ?""
                             :"/"+this->setting.nameSpace();
        auto url=this->setting.url().toString()+nameSpace;
        auto version=this->setting.version().trimmed().toLower();
        auto import=this->setting.secretsName().trimmed();
        import=import.isEmpty()?"":("/"+import);

        static const auto __format=QString("%1/%2/secret/data%3");
        url=__format.arg(url,version,import);
        return QUrl(url);
    }

    QUrl makeUrlVaultMetaData()
    {
        auto nameSpace=this->setting.nameSpace().isEmpty()
                             ?""
                             :"/"+this->setting.nameSpace();
        auto url=this->setting.url().toString()+nameSpace;
        auto version=this->setting.version().trimmed().toLower();
        auto import=this->setting.secretsName().trimmed();
        import=import.isEmpty()?"":("/"+import);

        static const auto __format=QString("%1/%2/secret/metadata%3");
        url=__format.arg(url,version,import);
        return QUrl(url);
    }

    void emitFail(const QVariantHash &error){

        emit this->parent->fail(error);
        if(this->onFinished)
            this->onFinished(*this->parent);
        this->isSuccessful=false;
    }
    
    void emitFail(const QRequestResponse &response)
    {
        qCritical()<<response.body();
        emitFail(response.bodyAsMap());
    }

    QString getCurrentToken(){
        if(!this->currentToken.isEmpty())
            return this->currentToken;
        return this->setting.token();
    }

    auto makeRequest()
    {
        this->isLoading=true;
        return &QRequestClient::builder(this)
                    .onStarted([](){})
                    .onFail(
                        [this](QRequestResponse response)
                        {
                            emitFail(response);
                        }
                        )
                    .onFinished(
                        [this](QRequestClient *r)
                        {
                            r->deleteLater();
                            this->isLoading=false;
                            if(this->onFinished)
                                this->onFinished(*this->parent);
                            this->isLoading=false;
                        })
                    .printOnFail(this->setting.printOnFail())
                    .header(__X_Vault_Token, getCurrentToken());
    }

    void auth(QRequestClient::VoidMethod callbackSuccess)
    {
        if(this->isAuthenticated){
            callbackSuccess();
            return;
        }

        if(!this->setting.token().isEmpty()){
            this->isAuthenticated=true;
            this->currentToken=this->setting.token();
            callbackSuccess();
            return;
        }

        auto &req=*makeRequest();
        req
            .onSuccessful(
                [this, &callbackSuccess](QRequestResponse response)
                {
                    auto map=response.bodyAsMap();
                    this->currentToken=map.value(__auth).toHash().value(__client_token).toByteArray().trimmed();
                    if(this->currentToken.isEmpty()){
                        emitFail({{"error","Invalid token, no token found"}});
                    }
                    else{
                        this->isSuccessful=true;
                        callbackSuccess();
                    }
                })
            .POST()
            .headers({})
            .url(this->makeUrlVaultLogin())
            .body(QVariantHash{{__role_id, this->setting.roleId()},{__secret_id, this->setting.secretId()}})
            .call();
        return;
    }

    void pull()
    {
        this->clean();
        this->auth(
            [this]()
            {
                auto &req=*makeRequest();
                req
                    .onSuccessful(
                        [this](QRequestResponse response)
                        {
                            this->setValues(response.bodyAsMap());
                            if(this->onLoaded)
                                this->onLoaded(this->data);
                            emit this->parent->loaded(this->data);
                            this->isSuccessful=true;
                        })
                    .GET()
                    .header(__content_type, __application_json)
                    .url(this->makeUrlVaultData())
                    .call();
            });
    }

    void push()
    {
        this->isLoading=true;
        this->auth(
            [this]()
            {
                auto &req=*makeRequest();
                req
                    .onSuccessful(
                        [this](QRequestResponse response)
                        {
                            this->vaultMetaData=response.bodyAsMap();
                            this->isSuccessful=true;
                        })
                    .POST()
                    .header(__content_type, __application_json)
                    .url(this->makeUrlVaultData())
                    .body(QVariantHash{{__data,this->data}})
                    .call();
            });
    }

    void addValues(const QVariant &data)
    {
        setData(data,false);
    }

    void setValues(const QVariant &data)
    {
        setData(data,true);
    }

    void systemEnvironmentUnSet()
    {
        auto envCurrent=makeEnvs();
        //unset envs
        QHashIterator<QString, QVariant> i(envCurrent);
        while (i.hasNext()){
            i.next();
            auto key=i.key().toUtf8();
            if(!systemEnvironment->contains(key))
                qunsetenv(key);
            else{
                auto value=i.value().toByteArray();
                qputenv(key, value);
            }
        }
    }

    void systemEnvironmentSet()
    {
        this->systemEnvironmentSet(this->data);
    }

    static void systemEnvironmentSet(const QVariantHash &data)
    {
        QHashIterator<QString, QVariant> i(data);
        while (i.hasNext()){
            i.next();
            auto key=i.key().toUtf8();
            auto value=i.value().toByteArray();
            qunsetenv(key);
            if(!qputenv(key, value))
                sWarning()<<QStringLiteral("Fail on qputenv(%1, %2)").arg(key, value);
        }
    }
    void rm(const QVariantList &keys)
    {
        for(auto&v:keys){
            auto key=v.toString().trimmed().toLower();
            this->data.remove(key);
        }
    }
    const QVariant get(const QString &key) const
    {
        auto keyName=key.trimmed();
        if(keyName.isEmpty())
            return {};

        if(this->data.contains(key))
            return this->data.value(key).toString();

        keyName=keyName.toLower();
        QHashIterator<QString,QVariant> i(this->data);
        while(i.hasNext()){
            i.next();
            auto k=i.key().trimmed();
            if(keyName==k)
                return this->data.value(keyName).toString();
        }

        return {};
    }

    const QVariantHash get(const QVariant &keys) const
    {
        if(keys.isNull() || !keys.isValid())
            return {};

        QStringList keyNames;
        switch (keys.typeId()) {
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
        {
            auto vList=keys.toList();
            for(auto&v: vList)
                keyNames.append(v.toString().trimmed());
            break;
        }
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantPair:
        {
            auto vList=keys.toHash().keys();
            for(auto&v: vList)
                keyNames.append(v.trimmed());
            break;
        }
        default:
            keyNames.append(keys.toString().trimmed());
            break;
        }
        QVariantHash __values;
        for(auto&key:keyNames){
            auto value=get(key);
            if(!value.isNull() && value.isValid())
                __values.insert(key,value);
        }

        return __values;
    }
private:
    void setData(const QVariant &data, bool replaceAll)
    {
        if(replaceAll){
            if(!this->vaultMetaData.isEmpty())
                this->systemEnvironmentUnSet();
            this->clear();
        }

        if(data.isNull() || !data.isValid())
            return;

        QVariantHash environments=Utils::toHash(data);
        if(environments.contains(__data)){
            auto vHash=environments.value(__data).toHash();
            if(vHash.contains(__data)){
                if(replaceAll)
                    this->vaultMetaData=data.toHash();
                environments=vHash.value(__data).toHash();
                this->isLoaded=true;
            }
        }

        QHashIterator<QString,QVariant> i(environments);
        while(i.hasNext()){
            i.next();
            auto k=i.key().trimmed();
            if(!k.isEmpty()){
                const auto &v=i.value();
                this->data.insert(k, v);
            }
        }
    }
};

KvClient::KvClient(QObject *parent):QObject{parent},p{new KvClientPvt{this}}
{
}

KvClient::KvClient(const QVariant &settings, QObject *parent):QObject{parent},p{new KvClientPvt{settings, this}}
{
}

Setting &KvClient::setting()
{
    return p->setting;
}

const KvClient &KvClient::setting(const QVariant &newValues) const
{
    p->setting.setValues(newValues);
    return *this;
}

const KvClient &KvClient::clear() const
{
    p->clear();
    return *this;
}

const KvClient &KvClient::clean() const
{
    p->clean();
    return *this;
}

const KvClient &KvClient::values(const QString &newValues)const
{
    p->setValues(newValues);
    return *this;
}

const KvClient &KvClient::values(const QVariantHash &newValues)const
{
    p->setValues(newValues);
    return *this;
}

const KvClient &KvClient::values(const QVariantMap &newValues)const
{
    p->setValues(newValues);
    return *this;
}

const KvClient &KvClient::values(const VariantPair &newValues)const
{
    p->setValues(QVariantHash{{newValues.first, newValues.second}});
    return *this;
}

const KvClient &KvClient::values(const QString &key, const QString &value)const
{
    p->setValues(QVariantHash{{key, value}});
    return *this;
}

const KvClient &KvClient::put(const QString &newValues) const
{
    p->addValues(newValues);
    return *this;
}

const KvClient &KvClient::put(const QVariantHash &newValues) const
{
    p->addValues(newValues);
    return *this;
}

const KvClient &KvClient::put(const VariantPair &newValues) const
{
    p->addValues(QVariantHash{{newValues.first,newValues.second}});
    return *this;
}

const KvClient &KvClient::put(const QString &key, const QString &value) const
{
    p->addValues(QVariantHash{{key,value}});
    return *this;
}

const KvClient &KvClient::rm(const QString &key) const
{
    p->rm({key});
    return *this;
}

const KvClient &KvClient::rm(const QVariantHash &keys) const
{
    p->rm({keys.keys()});
    return *this;
}

const KvClient &KvClient::rm(const QVariantMap &keys) const
{
    p->rm({keys.keys()});
    return *this;
}

const KvClient &KvClient::rm(const QVariantList &keys) const
{
    p->rm(keys);
    return *this;
}

const KvClient &KvClient::rm(const QStringList &keys) const
{
    p->rm(QVariantList{keys});
    return *this;
}

const QVariantHash &KvClient::get() const
{
    return p->data;
}

const QString KvClient::get(const QString &key) const
{
    return p->get(key).toString();
}

const QVariantHash KvClient::get(const QVariantList &keys) const
{
    return p->get(keys);
}

const QVariantHash KvClient::get(const QStringList &keys) const
{
    return p->get(keys);
}

const QVariantHash KvClient::get(const QVariantHash &keys) const
{
    return p->get(keys);
}

const QVariantHash KvClient::get(const QVariantMap &keys) const
{
    return p->get(keys);
}

bool KvClient::isLoading() const
{
    return p->isLoading;
}

bool KvClient::isLoaded() const
{
    return p->isLoaded;
}

bool KvClient::isSuccessful() const
{
    return p->isSuccessful;
}

bool KvClient::isAuthenticated() const
{
    return p->isAuthenticated;
}

const KvClient &KvClient::login(const QByteArray &token)
{
    p->setting.method(Setting::Token);
    p->setting.token(token.trimmed());
    p->currentToken=token.trimmed();
    emit authorized();
    return *this;
}

const KvClient &KvClient::login(const QByteArray &roleId, const QByteArray &secretId)
{
    p->setting.method(Setting::AppRole);
    p->setting.roleId(roleId);
    p->setting.secretId(secretId);
    p->auth(
        [this]()
        {
            emit this->authorized();
        }
        );
    return *this;
}

const KvClient &KvClient::pull() const
{
    p->pull();
    return *this;
}

const QVariantHash &KvClient::metaData() const
{
    return p->vaultMetaData;
}

const KvClient &KvClient::push() const
{
    p->push();
    return *this;
}

const KvClient &KvClient::systemEnvironmentSet() const
{
    p->systemEnvironmentSet();
    return *this;
}

const KvClient &KvClient::systemEnvironmentUnSet() const
{
    p->systemEnvironmentUnSet();
    return *this;
}

const KvClient &KvClient::onStarted(VoidKvMethod method) const
{
    p->onStarted=method;
    return *this;
}

const KvClient &KvClient::onLoaded(VoidMapMethod method) const
{
    p->onLoaded=method;
    return *this;
}

const KvClient &KvClient::onFail(VoidMapMethod method) const
{
    p->onFail=method;
    return *this;
}

const KvClient &KvClient::onFinished(VoidKvMethod method) const
{
    p->onFinished=method;
    return *this;
}

}

