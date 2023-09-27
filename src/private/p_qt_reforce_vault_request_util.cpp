#include "./p_qt_reforce_vault_request_util.h"
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QScopedPointerDeleteLater>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaEnum>

namespace QtReforce{

static const auto __list="list";
static const auto __statusCode="__statusCode";
static const auto __reasonPhrase="reasonPhrase";
static const auto __responseBody="responseBody";

struct ResponsePvt
{
public:
    QVariantHash headers;
    QNetworkReply::NetworkError error=QNetworkReply::NoError;
    int statusCode;
    QByteArray reasonPhrase;
    QByteArray body;
    RequestUtil::VoidMethod onStarted=nullptr;
    RequestUtil::VoidMethod onSent=nullptr;
    RequestUtil::ResponseMethod onSuccessful=nullptr;
    RequestUtil::ResponseMethod onFail=nullptr;
    RequestUtil::RequestMethod onFinished=nullptr;
    bool isRunning=false;

    explicit ResponsePvt(){}

    bool isOK()
    {
        return statusCode==200 ||statusCode==201 ||statusCode==202;
    }

    void clear()
    {
        this->isRunning=false;
        this->error=QNetworkReply::NoError;
        this->headers={};
        this->statusCode={};
        this->reasonPhrase={};
        this->body={};
    }
};

class RequestUtilPvt:public QObject
{
public:
    RequestUtil *parent;
    RequestUtil::Method method=RequestUtil::Get;
    QUrl url;
    QVariantHash headers;
    QVariant body;
    ResponsePvt response;

    QNetworkAccessManager qnam;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> replyScope;
    explicit RequestUtilPvt(RequestUtil *parent):QObject{parent},parent{parent}{

    }

    QUrl makeUrlArgs()
    {
        return this->url;
    }

    QByteArray makeBody()
    {
        switch (this->body.typeId()) {
        case QMetaType::QVariantPair:
        case QMetaType::QVariantHash:
        case QMetaType::QVariantMap:
        case QMetaType::QVariantList:
        case QMetaType::QStringList:
            return QJsonDocument::fromVariant(this->body).toJson(QJsonDocument::Compact);
        default:
            return this->body.toByteArray();
        }
    }

    QNetworkReply *makeReply()
    {
        this->response.clear();
        this->response.isRunning=true;
        auto networkRequest=QNetworkRequest(url);

        if(!this->headers.isEmpty()){
            QHashIterator<QString, QVariant> i(this->headers);
            while (i.hasNext()){
                i.next();
                auto key=i.key().toUtf8();
                auto value=i.value().toByteArray();
                networkRequest.setRawHeader(key,value);
            }
        }
        switch (this->method) {
        case RequestUtil::Head:
            replyScope.reset(qnam.head(networkRequest));
            break;
        case RequestUtil::Get:
            replyScope.reset(qnam.get(networkRequest));
            break;
        case RequestUtil::Post:
            replyScope.reset(qnam.post(networkRequest,this->makeBody()));
            break;
        case RequestUtil::Put:
            replyScope.reset(qnam.put(networkRequest,this->makeBody()));
            break;
        case RequestUtil::Delete:
            replyScope.reset(qnam.deleteResource(networkRequest));
            break;
        case RequestUtil::List:
            replyScope.reset(qnam.sendCustomRequest(networkRequest,__list,this->makeBody()));
            break;
        default:
            replyScope->reset();
        }
        replyScope->ignoreSslErrors();
        return replyScope.get();
    }

    void abort()
    {
        if(this->replyScope.get()!=nullptr)
            this->replyScope->abort();
    }

    void call()
    {
        auto reply=this->makeReply();
        if(reply==nullptr){
            this->httpStarted();
            this->httpFinished();
            return;
        }
        connect(reply, &QNetworkReply::socketStartedConnecting, this, &RequestUtilPvt::httpStarted);
        connect(reply, &QNetworkReply::requestSent, this, &RequestUtilPvt::httpSent);
        connect(reply, &QNetworkReply::finished, this, &RequestUtilPvt::httpFinished);
#if QT_CONFIG(ssl)
        connect(reply, &QNetworkReply::sslErrors, this, &RequestUtilPvt::sslErrors);
#endif
    }
private slots:
    void httpStarted()
    {
        if(this->response.onStarted)
            this->response.onStarted();
    }
    void httpSent()
    {
        if(this->response.onSent)
            this->response.onSent();
    }
    void httpFinished()
    {
        static const auto eNetworkError=QMetaEnum::fromType<QNetworkReply::NetworkError>();
        auto reply=this->replyScope.get();
        if(reply==nullptr){
            this->response.clear();
            this->response.error=QNetworkReply::UnknownNetworkError;
            this->response.reasonPhrase=eNetworkError.key(this->response.error);
            if(this->response.onFail)
                this->response.onFail(RequestUtil::Response(this->parent->response()));
        }
        else{
            this->response.error = reply->error();
            if(this->response.error==QNetworkReply::NoError){
                this->response.statusCode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                this->response.reasonPhrase=reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
                this->response.body=reply->readAll();
            }
            else{
                this->response.statusCode=reply->error();
                this->response.reasonPhrase=eNetworkError.key(this->response.error);
                this->response.body=reply->readAll();
            }
            if(this->response.isOK())
                this->response.onSuccessful(RequestUtil::Response(this->parent->response()));
            else
                this->response.onFail(RequestUtil::Response(this->parent->response()));
        }
        this->replyScope.reset();
        if(response.onFinished!=nullptr)
            response.onFinished(this->parent);
        this->response.isRunning=false;
    }
#if QT_CONFIG(ssl)
    void sslErrors(const QList<QSslError> &errors)
    {
        Q_UNUSED(errors)
        replyScope->ignoreSslErrors();
    }
#endif
};

const QVariantHash RequestUtil::Response::toMap() const
{
    return{
             {__statusCode,this->statusCode()}
            ,{__reasonPhrase,this->reasonPhrase()}
            ,{__responseBody, this->body()}

    };
}

bool RequestUtil::Response::isOK() const
{
    return p->response.isOK();
}

bool RequestUtil::Response::isRunning() const
{
    return p->response.isRunning;
}

const QVariantHash &RequestUtil::Response::headers() const
{
    return p->response.headers;
}

int RequestUtil::Response::statusCode() const
{
    return p->response.statusCode;
}

QString RequestUtil::Response::reasonPhrase()const
{
    return p->response.reasonPhrase;
}

const QByteArray &RequestUtil::Response::body() const
{
    return p->response.body;
}

const QVariantHash RequestUtil::Response::bodyAsMap() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toHash();
}

const QVariantList RequestUtil::Response::bodyAsList() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toList();
}

const QStringList RequestUtil::Response::bodyAsStringList() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toStringList();
}

RequestUtil::RequestUtil(QObject *parent):QObject{parent},p{new RequestUtilPvt{this}}
{
}

RequestUtil &RequestUtil::builder(QObject *parent)
{
    return *(new RequestUtil(parent));
}

const RequestUtil &RequestUtil::onStarted(VoidMethod method) const
{
    p->response.onStarted=method;
    return *this;
}

const RequestUtil &RequestUtil::onSent(VoidMethod method) const
{
    p->response.onSent=method;
    return *this;
}

const RequestUtil &RequestUtil::onSuccessful(ResponseMethod method) const
{
    p->response.onSuccessful=method;
    return *this;
}

const RequestUtil &RequestUtil::onFail(ResponseMethod method) const
{
    p->response.onFail=method;
    return *this;
}

const RequestUtil &RequestUtil::onFinished(RequestMethod method) const
{
    p->response.onFinished=method;
    return *this;
}

const RequestUtil &RequestUtil::call() const
{
    p->call();
    return *this;
}

const RequestUtil &RequestUtil::abort()
{
    p->abort();
    return *this;
}

bool RequestUtil::isOK() const
{
    return p->response.isOK();
}

const RequestUtil::Response RequestUtil::response()const
{
    return RequestUtil::Response(p);
}

const RequestUtil &RequestUtil::HEAD() const
{
    return this->method(Head);
}

const RequestUtil &RequestUtil::GET() const
{
    return this->method(Get);
}

const RequestUtil &RequestUtil::POST() const
{
    return this->method(Post);
}

const RequestUtil &RequestUtil::PUT() const
{
    return this->method(Put);
}

const RequestUtil &RequestUtil::DELETE() const
{
    return this->method(Delete);
}

const RequestUtil &RequestUtil::LIST() const
{
    return this->method(List);
}

RequestUtil::Method RequestUtil::method()
{
    return p->method;
}

const RequestUtil &RequestUtil::method(Method newMethod) const
{
    p->method=newMethod;
    return *this;
}

const QVariantHash &RequestUtil::headers() const
{
    return p->headers;
}

const RequestUtil &RequestUtil::headers(const QVariantHash &newHeaders) const
{
    p->headers=newHeaders;
    return *this;
}

const RequestUtil &RequestUtil::headers(const QString &key, const QString &value) const
{
    p->headers.insert(key, value);
    return *this;
}

const RequestUtil &RequestUtil::headerApplicationJson() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_JSON());
}

const RequestUtil &RequestUtil::headerFormUrlencoded() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_FORM_URLENCODED());
}

const RequestUtil &RequestUtil::headerApplicationOctecStream() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_OCTET_STREAM());
}

const RequestUtil &RequestUtil::headerApplicationCBor() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_CBOR());
}

const RequestUtil &RequestUtil::headerTextPlain() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::TEXT_PLAIN());
}

const RequestUtil &RequestUtil::headerApplicationPdf() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_PDF());
}

const RequestUtil &RequestUtil::headerApplicationXml() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_XML());
}

const QUrl &RequestUtil::url() const
{
    return p->url;
}

const RequestUtil &RequestUtil::url(const QUrl &newUrl) const
{
    p->url=newUrl;
    return *this;
}

const QVariant &RequestUtil::body() const
{
    return p->body;
}

const RequestUtil &RequestUtil::body(const QVariant &newBody) const
{
    p->body=newBody;
    return *this;
}

}

