#include "./qt_reforce_vault_kv_client.h"
#include "./qt_reforce_vault_setting.h"
#include "./private/p_qt_reforce_vault_request_util.h"
#include "./private/p_qt_reforce_vault_logs.h"
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonDocument>


//ref
//  https://developer.hashicorp.com/vault/tutorials/getting-started/getting-started-intro
//  https://developer.hashicorp.com/vault/tutorials/getting-started/getting-started-apis
//  https://developer.hashicorp.com/vault/tutorials/getting-started/getting-started-authentication

namespace QtReforce{

static const auto __data="data";
static const auto __splitEnv="=";
static const auto __X_Vault_Token="X-Vault-Token";
static const auto __role_id="role_id";
static const auto __secret_id="secret_id";

Q_GLOBAL_STATIC(QVariantHash,systemEnvironment)

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
    KvClient::VoidKvMethod onStarted=nullptr;
    KvClient::VoidMapMethod onLoaded=nullptr;
    KvClient::VoidMapMethod onFail=nullptr;
    KvClient::VoidKvMethod onFinished=nullptr;

    QVariantHash envOriginal;
    QVariantHash envLoaded;
    QVariantHash vaultMetaData;
    bool isLoading=false;

    explicit KvClientPvt(KvClient *parent):QObject{parent},parent{parent},setting{parent}
    {
    }

    QUrl makeToLogin()
    {
        auto url=this->setting.url().toString();
        auto version=this->setting.version().trimmed().toLower();
        auto import=this->setting.import().trimmed();
        import=import.isEmpty()?"":("/"+import);

        static const auto __format=QString("%1/%2/auth/approle/login");
        url=__format.arg(url,version);
        return QUrl(url);
    }

    QUrl makeToGetSet()
    {
        auto url=this->setting.url().toString();
        auto version=this->setting.version().trimmed().toLower();
        auto import=this->setting.import().trimmed();
        import=import.isEmpty()?"":("/"+import);

        static const auto __format=QString("%1/%2/secret/data%3");
        url=__format.arg(url,version,import);
        return QUrl(url);
    }

    auto makeRequest()
    {
        return &RequestUtil::builder()
            .onStarted([](){})
            .onFail(
                [this](RequestUtil::Response response)
                {
                    this->isLoading=false;
                    auto error=response.toMap();
                    if(this->onFail)
                        this->onFail(error);
                    emit this->parent->fail(error);
                }
                )
            .onFinished(
                [this](RequestUtil *r)
                {
                    r->deleteLater();
                    this->isLoading=false;
                    if(this->onFinished)
                        this->onFinished(*this->parent);
                });
    }


    void auth(RequestUtil::VoidMethod callbackSuccess)
    {
        if(this->setting.method()==Setting::Token){
            callbackSuccess();
            return;
        }

        if(this->setting.method()==Setting::RoleId){
            /*
request
curl --request POST \
       --data '{"role_id": "3c301960-8a02-d776-f025-c3443d513a18", "secret_id": "22d1e0d6-a70b-f91f-f918-a0ee8902666b"}' \
       http://127.0.0.1:8200/v1/auth/approle/login | jq -r ".auth"

response
{
  "auth": {
    "renewable": true,
    "lease_duration": 2764800,
    "metadata": {},
    "policies": ["default", "dev-policy", "test-policy"],
    "accessor": "5d7fb475-07cb-4060-c2de-1ca3fcbf0c56",
    "client_token": "98a4c7ab-b1fe-361b-ba0b-e307aacfd587"
  }
}
*/
            auto &req=*makeRequest();
            req
                .onSuccessful(
                    [this, &callbackSuccess](RequestUtil::Response response)
                    {
                        static const auto __auth="auth";
                        static const auto __client_token="client_token";
                        auto map=response.bodyAsMap();
                        auto token=map.value(__auth).toHash().value(__client_token).toString();
                        this->setting.token(token);
                        callbackSuccess();
                    })
                .onFail(
                    [this](RequestUtil::Response response)
                    {
                        auto error=response.toMap();
                        if(this->onFail)
                            this->onFail(error);
                        emit this->parent->fail(error);
                        if(this->onFinished)
                            this->onFinished(*this->parent);
                        this->isLoading=true;
                    }
                    )
                .onFinished([](RequestUtil *r){r->deleteLater();})
                .POST()
                .headers(__X_Vault_Token, this->setting.token())
                .url(this->makeToLogin())
                .body(QVariantHash{{__role_id, this->setting.roleId()},{__secret_id, this->setting.secretId()}})
                .call();
            return;
        }


    }

    void load()
    {
        this->isLoading=true;
        this->auth(
            [this]()
            {
                auto &req=*makeRequest();
                req
                    .onSuccessful(
                        [this](RequestUtil::Response response)
                        {
                            this->envsSet(response.bodyAsMap());
                            if(this->onLoaded)
                                this->onLoaded(this->envLoaded);
                            emit this->parent->loaded(this->envLoaded);
                        })
                    .GET()
                    .headers(__X_Vault_Token, this->setting.token())
                    .url(this->makeToGetSet())
                    .call();
            });
/*
curl \
    -H "X-Vault-Token: 00000000-0000-0000-0000-000000000000" \
    -X GET \
    http://127.0.0.1:8200/v1/secret/data/app

{
  "request_id": "40389033-5626-be19-cd05-cebb963bbd46",
  "lease_id": "",
  "renewable": false,
  "lease_duration": 0,
  "data": {
    "data": {
      "a": "bbbbb",
      "c": "dddddd"
    },
    "metadata": {
      "created_time": "2023-09-27T04:45:10.018536565Z",
      "custom_metadata": null,
      "deletion_time": "",
      "destroyed": false,
      "version": 9
    }
  },
  "wrap_info": null,
  "warnings": null,
  "auth": null
}

*/
    }

    void update()
    {
        this->isLoading=true;
        this->auth(
            [this]()
            {
                auto &req=*makeRequest();
                req
                    .onSuccessful(
                        [this](RequestUtil::Response response)
                        {
                            this->vaultMetaData=response.bodyAsMap();
                        })
                    .POST()
                    .headers(__X_Vault_Token, this->setting.token())
                    .url(this->makeToGetSet())
                    .body(this->envLoaded)
                    .call();
            });
/*
curl \
    --header "X-Vault-Token: 00000000-0000-0000-0000-000000000000" \
    --request POST \
    --data '{ "data": {"password": "my-long-password"} }' \
    http://127.0.0.1:8200/v1/secret/data/app | jq -r ".data"

{
  "created_time": "2020-02-05T16:51:34.0887877Z",
  "deletion_time": "",
  "destroyed": false,
  "version": 1
}

*/
    }

    void envsUnSet()
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

    void envsSet(const QVariantHash &envs)
    {
        if(!this->vaultMetaData.isEmpty())
            this->envsUnSet();
        this->vaultMetaData=envs;
        this->envLoaded=envs.value(__data).toHash().value(__data).toHash();

        QHashIterator<QString, QVariant> i(this->envLoaded);
        while (i.hasNext()){
            i.next();
            auto key=i.key().toUtf8();
            auto value=i.value().toByteArray();
            qunsetenv(key);
            if(!qputenv(key, value))
                sWarning()<<QStringLiteral("Fail on qputenv(%1, %2)").arg(key, value);
        }
        envsUnSet();
    }

};

KvClient::KvClient(QObject *parent):QObject{parent},p{new KvClientPvt{this}}
{
}

const QVariantHash &KvClient::values() const
{
    return p->envLoaded;
}

const KvClient &KvClient::values(const QVariantHash &newValues) const
{
    p->envLoaded=newValues;
    return *this;
}

const KvClient &KvClient::values(const QString &key, const QString &value) const
{
    p->envLoaded.insert(key,value);
    return *this;
}

const QString KvClient::value(const QString &key) const
{
    return p->envLoaded.value(key).toString();
}

bool KvClient::isLoading()
{
    return p->isLoading;
}

Setting &KvClient::setting()
{
    return p->setting;
}

KvClient &KvClient::load()
{
    p->load();
    return *this;
}

KvClient &KvClient::update()
{
    p->update();
    return *this;
}

KvClient &KvClient::revert()
{
    p->envsUnSet();
    return *this;
}

KvClient &KvClient::onStarted(VoidKvMethod method)
{
    p->onStarted=method;
    return *this;
}

KvClient &KvClient::onLoaded(VoidMapMethod method)
{
    p->onLoaded=method;
    return *this;
}

KvClient &KvClient::onFail(VoidMapMethod method)
{
    p->onFail=method;
    return *this;
}

KvClient &KvClient::onFinished(VoidKvMethod method)
{
    p->onFinished=method;
    return *this;
}

}

