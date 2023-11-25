#include "./p_qt_reforce_request_client.h"
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QScopedPointerDeleteLater>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMetaEnum>
#include <QEventLoop>
#include <QDebug>
#include <QVariant>

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
    RequestClient::VoidMethod onStarted=nullptr;
    RequestClient::VoidMethod onSent=nullptr;
    RequestClient::ResponseMethod onSuccessful=nullptr;
    RequestClient::ResponseMethod onFail=nullptr;
    RequestClient::RequestMethod onFinished=nullptr;
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

class RequestClientPvt:public QObject
{
public:
    RequestClient *parent;
    bool aSync=false;
    bool printOnFail=false;
    RequestClient::Method method=RequestClient::Get;
    QUrl url;
    QVariantHash headers, args;
    QByteArray body;
    ResponsePvt response;
    QNetworkAccessManager qnam;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> replyScope;
    explicit RequestClientPvt(RequestClient *parent):QObject{parent},parent{parent}{

    }

    void print() {
        auto e=QMetaEnum::fromType<RequestClient::Method>();
        auto method=(this->method==RequestClient::Method::Upload)
            ?RequestClient::Method::Post
                          :this->method;
        auto methodName=QByteArray(e.valueToKey(method)).toUpper();
        QStringList str;
        str.append(QString("curl -i -X %1").arg(methodName));
        if(this->method==RequestClient::Upload)
            str.append(QString("-F \"file=@%1\"").arg(this->body));

        str.append(QString("--location \"%1\"").arg(this->url.toString()));
        {
            QHashIterator<QString,QVariant> i(this->headers);
            while(i.hasNext()){
                i.next();
                str.append(QString("--header '%1: %2'").arg(i.key(),i.value().toString()));
            }
        }
        {
            QHashIterator<QString,QVariant> i(this->args);
            while(i.hasNext()){
                i.next();
                str.append(QString("--data-urlencode \"%1=%2\"").arg(i.key(), i.value().toString()));
            }
        }
        if(!this->body.isEmpty()){
            switch (this->method) {
            case RequestClient::Post:
            case RequestClient::Put:
                str.append(QString("--data '%1'").arg(this->body));
                break;
            default:
                break;
            }
        }

        qDebug()<<"Request:";
        qDebug()<<"   "+str.join(' ').toUtf8();
        qDebug()<<"response:";
        qDebug()<<"   statusCode: "<<response.statusCode;
        qDebug()<<"   code: "<<response.body;
    }

    QUrl makeUrlArgs()
    {
        return this->url;
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
        case RequestClient::Head:
            replyScope.reset(qnam.head(networkRequest));
            break;
        case RequestClient::Get:
            replyScope.reset(qnam.get(networkRequest));
            break;
        case RequestClient::Post:
            replyScope.reset(qnam.post(networkRequest,this->body));
            break;
        case RequestClient::Put:
            replyScope.reset(qnam.put(networkRequest,this->body));
            break;
        case RequestClient::Delete:
            replyScope.reset(qnam.deleteResource(networkRequest));
            break;
        case RequestClient::List:
            replyScope.reset(qnam.sendCustomRequest(networkRequest,__list,this->body));
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
        QEventLoop loop;
        auto reply=this->makeReply();
        if(reply==nullptr){
            this->httpStarted();
            this->httpFinished();
            return;
        }
        connect(reply, &QNetworkReply::socketStartedConnecting, this, &RequestClientPvt::httpStarted);
        connect(reply, &QNetworkReply::requestSent, this, &RequestClientPvt::httpSent);
        connect(reply, &QNetworkReply::finished, this, &RequestClientPvt::httpFinished);
#if QT_CONFIG(ssl)
        connect(reply, &QNetworkReply::sslErrors, this, &RequestClientPvt::sslErrors);
#endif
        if(!this->aSync){
            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }
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
                this->response.onFail(RequestClient::Response(this->parent->response()));
            if(this->printOnFail)
                this->print();
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
            if(this->response.isOK()){
                this->response.onSuccessful(RequestClient::Response(this->parent->response()));
            }
            else{
                this->response.onFail(RequestClient::Response(this->parent->response()));
                if(this->printOnFail)
                    this->print();
            }
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

const QVariantHash RequestClient::Response::toMap() const
{
    return{
        {__statusCode,this->statusCode()}
        ,{__reasonPhrase,this->reasonPhrase()}
        ,{__responseBody, this->body()}

    };
}

bool RequestClient::Response::isOK() const
{
    return p->response.isOK();
}

bool RequestClient::Response::isRunning() const
{
    return p->response.isRunning;
}

const QVariantHash &RequestClient::Response::headers() const
{
    return p->response.headers;
}

int RequestClient::Response::statusCode() const
{
    return p->response.statusCode;
}

QString RequestClient::Response::reasonPhrase()const
{
    return p->response.reasonPhrase;
}

const QByteArray &RequestClient::Response::body() const
{
    return p->response.body;
}

const QVariantHash RequestClient::Response::bodyAsMap() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toHash();
}

const QVariantList RequestClient::Response::bodyAsList() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toList();
}

const QStringList RequestClient::Response::bodyAsStringList() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toStringList();
}

RequestClient::RequestClient(QObject *parent):QObject{parent},p{new RequestClientPvt{this}}
{
}

RequestClient &RequestClient::builder(QObject *parent)
{
    return *(new RequestClient(parent));
}

const RequestClient &RequestClient::onStarted(VoidMethod method) const
{
    p->response.onStarted=method;
    return *this;
}

const RequestClient &RequestClient::onSent(VoidMethod method) const
{
    p->response.onSent=method;
    return *this;
}

const RequestClient &RequestClient::onSuccessful(ResponseMethod method) const
{
    p->response.onSuccessful=method;
    return *this;
}

const RequestClient &RequestClient::onFail(ResponseMethod method) const
{
    p->response.onFail=method;
    return *this;
}

const RequestClient &RequestClient::onFinished(RequestMethod method) const
{
    p->response.onFinished=method;
    return *this;
}

const RequestClient &RequestClient::call() const
{
    p->call();
    return *this;
}

const RequestClient &RequestClient::abort()
{
    p->abort();
    return *this;
}

bool RequestClient::isOK() const
{
    return p->response.isOK();
}

const RequestClient::Response RequestClient::response()const
{
    return RequestClient::Response(p);
}

const RequestClient &RequestClient::HEAD() const
{
    return this->method(Head);
}

const RequestClient &RequestClient::GET() const
{
    return this->method(Get);
}

const RequestClient &RequestClient::POST() const
{
    return this->method(Post);
}

const RequestClient &RequestClient::PUT() const
{
    return this->method(Put);
}

const RequestClient &RequestClient::DELETE() const
{
    return this->method(Delete);
}

const RequestClient &RequestClient::LIST() const
{
    return this->method(List);
}

const RequestClient &RequestClient::print() const
{
    p->print();
    return *this;
}

const RequestClient &RequestClient::printOnFail()const
{
    p->printOnFail=true;
    return *this;
}

const RequestClient &RequestClient::printOnFail(bool newValue)const
{
    p->printOnFail=newValue;
    return *this;
}

bool RequestClient::aSync()
{
    return p->aSync;
}

const RequestClient &RequestClient::aSync(bool newASync) const
{
    p->aSync=newASync;
    return *this;
}

RequestClient::Method RequestClient::method()
{
    return p->method;
}

const RequestClient &RequestClient::method(Method newMethod) const
{
    p->method=newMethod;
    return *this;
}

const QVariantHash &RequestClient::headers() const
{
    return p->headers;
}

const RequestClient &RequestClient::headers(const QVariantHash &newHeaders) const
{
    p->headers=newHeaders;
    return *this;
}

const RequestClient &RequestClient::headers(const QString &key, const QString &value) const
{
    p->headers.insert(key, value);
    return *this;
}

const RequestClient &RequestClient::headerApplicationJson() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_JSON());
}

const RequestClient &RequestClient::headerFormUrlencoded() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_FORM_URLENCODED());
}

const RequestClient &RequestClient::headerApplicationOctecStream() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_OCTET_STREAM());
}

const RequestClient &RequestClient::headerApplicationCBor() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_CBOR());
}

const RequestClient &RequestClient::headerTextPlain() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::TEXT_PLAIN());
}

const RequestClient &RequestClient::headerApplicationPdf() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_PDF());
}

const RequestClient &RequestClient::headerApplicationXml() const
{
    return this->headers(HttpEncoding::CONTENT_TYPE(), MediaType::APPLICATION_XML());
}

const QUrl &RequestClient::url() const
{
    return p->url;
}

const RequestClient &RequestClient::url(const QUrl &newUrl) const
{
    p->url=newUrl;
    return *this;
}

const QVariantHash &RequestClient::args() const
{
    return p->args;
}

const RequestClient &RequestClient::args(const QString &key, const QVariant &value) const
{
    QByteArray valueBytes;
    switch(value.typeId()){
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantPair:
        valueBytes=QJsonDocument::fromVariant(value).toJson(QJsonDocument::Compact);
        break;
    default:
        valueBytes=value.toByteArray().trimmed();
    }
    p->args.insert(key, valueBytes);
    return *this;
}

const RequestClient &RequestClient::args(const QPair<QVariant, QVariant> &newArg) const
{
    return this->args(newArg.first.toString(), newArg.second);
}

const RequestClient &RequestClient::args(const QVariantHash &newArgs) const
{
    p->args.clear();
    QHashIterator<QString,QVariant> i(newArgs);
    while(i.hasNext()){
        i.next();
        this->args(i.key(), i.value());
    }
    return *this;
}

const RequestClient &RequestClient::args(const QVariantMap &newArgs) const
{
    return this->args(QVariant(newArgs).toHash());
}

const QByteArray &RequestClient::body() const
{
    return p->body;
}

const RequestClient &RequestClient::body(const QVariant &newBody) const
{
    switch(newBody.typeId()){
    case QMetaType::QStringList:
    case QMetaType::QVariantList:
    case QMetaType::QVariantHash:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantPair:
        p->body=QJsonDocument::fromVariant(newBody).toJson(QJsonDocument::Compact);
        break;
    default:
        p->body=newBody.toByteArray().trimmed();
    }
    return *this;
}

}

