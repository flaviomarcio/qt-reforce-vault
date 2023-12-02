#include "./qt_reforce_request_client.h"
#include "./qt_reforce_request_media_types.h"
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

struct ResponsePvt
{
public:
    QVariantHash headers;
    QNetworkReply::NetworkError error=QNetworkReply::NoError;
    int statusCode;
    QByteArray reasonPhrase;
    QByteArray body;
    QRequestClient::VoidMethod onStarted=nullptr;
    QRequestClient::VoidMethod onSent=nullptr;
    QRequestClient::ResponseMethod onSuccessful=nullptr;
    QRequestClient::ResponseMethod onFail=nullptr;
    QRequestClient::RequestMethod onFinished=nullptr;
    bool isRunning=false;
    QUrl bodyUrl;

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


class QRequestClientPvt:public QObject
{
public:
    QRequestClient *parent;
    bool waitForFinished=false;
    bool printOnFail=false;
    QRequestClient::Method method=QRequestClient::Get;
    QByteArray methodName;
    QUrl url;
    QVariantHash headers, args;
    QByteArray body;
    ResponsePvt response;
    QNetworkAccessManager qnam;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> replyScope;
    explicit QRequestClientPvt(QRequestClient *parent):QObject{parent},parent{parent}{
        qnam.setAutoDeleteReplies(true);
    }

    void print() {
        auto e=QMetaEnum::fromType<QRequestClient::Method>();
        auto method=(this->method==QRequestClient::Method::Upload)
            ?QRequestClient::Method::Post
                          :this->method;
        auto methodName=QByteArray(e.valueToKey(method)).toUpper();
        QStringList str;
        str.append(QString("curl -i -X %1").arg(methodName));
        if(this->method==QRequestClient::Upload)
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
            case QRequestClient::Post:
            case QRequestClient::Put:
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

        if(!this->methodName.isEmpty()){
            replyScope.reset(qnam.sendCustomRequest(networkRequest,this->methodName,this->body));
        }
        else{
            switch (this->method) {
            case QRequestClient::Head:
                replyScope.reset(qnam.head(networkRequest));
                break;
            case QRequestClient::Get:
                replyScope.reset(qnam.get(networkRequest));
                break;
            case QRequestClient::Post:
                replyScope.reset(qnam.post(networkRequest,this->body));
                break;
            case QRequestClient::Put:
                replyScope.reset(qnam.put(networkRequest,this->body));
                break;
            case QRequestClient::Delete:
                replyScope.reset(qnam.deleteResource(networkRequest));
                break;
            case QRequestClient::List:
                replyScope.reset(qnam.sendCustomRequest(networkRequest,__list,this->body));
                break;
            default:
                replyScope->reset();
            }
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
        connect(reply, &QNetworkReply::socketStartedConnecting, this, &QRequestClientPvt::httpStarted);
        connect(reply, &QNetworkReply::requestSent, this, &QRequestClientPvt::httpSent);
        connect(reply, &QNetworkReply::finished, this, &QRequestClientPvt::httpFinished);
#if QT_CONFIG(ssl)
        connect(reply, &QNetworkReply::sslErrors, this, &QRequestClientPvt::sslErrors);
#endif
        if(!this->waitForFinished){
            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }
    }

    QVariantHash objectToHash(QObject *o){
        if(o==nullptr)
            return {};
        QVariantHash vHash;
        static const QList<QByteArray> __ignoreProperty={"destroyed", "objectName", "objectNameChanged", "deleteLater", "_q_reregisterTimers"};
        auto metaObject=QObject::staticMetaObject;
        for (int index = 0; index < metaObject.propertyCount(); ++index){
            auto property=metaObject.property(index);
            if(!property.isReadable())
                continue;
            if(__ignoreProperty.contains(property.name()))
                continue;
            vHash.insert(property.name(),property.read(o));
        }
        return vHash;
    }

    QByteArray readFile(const QFile &f){
        if(!f.exists()){
            qCritical()<<QStringLiteral("file not found: ")<<f.fileName();
            return {};
        }
        QFile file(f.fileName());

        if(!file.open(QFile::Unbuffered | QFile::ReadOnly)){
            qCritical()<<file.errorString()<<": "<<file.fileName();
            return {};
        }
        auto __return=file.readAll();
        file.close();
        return __return;
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
                this->response.onFail(QRequestResponse(this->parent->response()));
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
                this->response.onSuccessful(QRequestResponse(this->parent->response()));
            }
            else{
                this->response.onFail(QRequestResponse(this->parent->response()));
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

QRequestClient::QRequestClient(QObject *parent):QObject{parent},p{new QRequestClientPvt{this}}
{
}

QRequestClient &QRequestClient::builder(QObject *parent)
{
    return *(new QRequestClient(parent));
}

const QRequestClient &QRequestClient::onStarted(VoidMethod method) const
{
    p->response.onStarted=method;
    return *this;
}

const QRequestClient &QRequestClient::onSent(VoidMethod method) const
{
    p->response.onSent=method;
    return *this;
}

const QRequestClient &QRequestClient::onSuccessful(ResponseMethod method) const
{
    p->response.onSuccessful=method;
    return *this;
}

const QRequestClient &QRequestClient::onFail(ResponseMethod method) const
{
    p->response.onFail=method;
    return *this;
}

const QRequestClient &QRequestClient::onFinished(RequestMethod method) const
{
    p->response.onFinished=method;
    return *this;
}

const QRequestClient &QRequestClient::call() const
{
    p->call();
    return *this;
}

const QRequestClient &QRequestClient::abort()
{
    p->abort();
    return *this;
}

bool QRequestClient::isOK() const
{
    return p->response.isOK();
}

const QRequestClient &QRequestClient::HEAD() const
{
    return this->method(Head);
}

const QRequestClient &QRequestClient::GET() const
{
    return this->method(Get);
}

const QRequestClient &QRequestClient::POST() const
{
    return this->method(Post);
}

const QRequestClient &QRequestClient::PUT() const
{
    return this->method(Put);
}

const QRequestClient &QRequestClient::DELETE() const
{
    return this->method(Delete);
}

const QRequestClient &QRequestClient::LIST() const
{
    return this->method(List);
}

const QRequestClient &QRequestClient::UPLOAD() const
{
    return this->method(Upload);
}

const QRequestClient &QRequestClient::DOWNLOAD() const
{
    return this->method(Download);
}

const QRequestClient &QRequestClient::print() const
{
    p->print();
    return *this;
}

const QRequestClient &QRequestClient::printOnFail()const
{
    p->printOnFail=true;
    return *this;
}

const QRequestClient &QRequestClient::printOnFail(bool newValue)const
{
    p->printOnFail=newValue;
    return *this;
}

bool QRequestClient::waitForFinished()
{
    return p->waitForFinished;
}

const QRequestClient &QRequestClient::waitForFinished(bool newWaitForFinished) const
{
    p->waitForFinished=newWaitForFinished;
    return *this;
}

QRequestClient::Method QRequestClient::method()
{
    return p->method;
}

const QRequestClient &QRequestClient::method(Method newMethod) const
{
    p->methodName.clear();
    p->method=newMethod;
    return *this;
}

const QRequestClient &QRequestClient::method(const QString &newMethod) const
{
    p->methodName=newMethod.trimmed().toUpper().toUtf8();
    return *this;
}

const QVariantHash &QRequestClient::headers() const
{
    return p->headers;
}

const QRequestClient &QRequestClient::headers(const QVariantHash &newHeaders) const
{
    p->headers=newHeaders;
    return *this;
}

const QRequestClient &QRequestClient::header(const QString &key, const QString &value) const
{
    p->headers.insert(key, value);
    return *this;
}

const QRequestClient &QRequestClient::headerApplicationJson() const
{
    return this->header(HttpEncoding::CONTENT_TYPE(), QMediaType::APPLICATION_JSON());
}

const QRequestClient &QRequestClient::headerFormUrlencoded() const
{
    return this->header(HttpEncoding::CONTENT_TYPE(), QMediaType::APPLICATION_FORM_URLENCODED());
}

const QRequestClient &QRequestClient::headerApplicationOctecStream() const
{
    return this->header(HttpEncoding::CONTENT_TYPE(), QMediaType::APPLICATION_OCTET_STREAM());
}

const QRequestClient &QRequestClient::headerApplicationCBor() const
{
    return this->header(HttpEncoding::CONTENT_TYPE(), QMediaType::APPLICATION_CBOR());
}

const QRequestClient &QRequestClient::headerTextPlain() const
{
    return this->header(HttpEncoding::CONTENT_TYPE(), QMediaType::TEXT_PLAIN());
}

const QRequestClient &QRequestClient::headerApplicationPdf() const
{
    return this->header(HttpEncoding::CONTENT_TYPE(), QMediaType::APPLICATION_PDF());
}

const QRequestClient &QRequestClient::headerApplicationXml() const
{
    return this->header(HttpEncoding::CONTENT_TYPE(), QMediaType::APPLICATION_XML());
}

const QUrl &QRequestClient::url() const
{
    return p->url;
}

const QRequestClient &QRequestClient::url(const QUrl &newUrl) const
{
    p->url=newUrl;
    return *this;
}

const QRequestClient &QRequestClient::url(const QString &newUrl) const
{
    p->url=QUrl{newUrl};
    return *this;
}

const QVariantHash &QRequestClient::args() const
{
    return p->args;
}

const QRequestClient &QRequestClient::args(const QString &key, const QVariant &value) const
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

const QRequestClient &QRequestClient::args(const QPair<QVariant, QVariant> &newArg) const
{
    return this->args(newArg.first.toString(), newArg.second);
}

const QRequestClient &QRequestClient::args(const QVariantHash &newArgs) const
{
    p->args.clear();
    QHashIterator<QString,QVariant> i(newArgs);
    while(i.hasNext()){
        i.next();
        this->args(i.key(), i.value());
    }
    return *this;
}

const QRequestClient &QRequestClient::args(const QVariantMap &newArgs) const
{
    return this->args(QVariant(newArgs).toHash());
}

const QRequestClient &QRequestClient::args(QObject *newBody) const
{
    return this->args(p->objectToHash(newBody));
}

const QRequestClient &QRequestClient::args(const QFile &file) const
{
    auto args=p->readFile(file);
    return this->args(QJsonDocument::fromJson(args).toVariant().toHash());
}

const QByteArray &QRequestClient::body() const
{
    return p->body;
}

const QRequestClient &QRequestClient::body(const QVariant &newBody) const
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

const QRequestClient &QRequestClient::body(QObject *newBody) const
{
    return this->body(p->objectToHash(newBody));
}

const QRequestClient &QRequestClient::body(const QFile &file) const
{
    p->body=p->readFile(file);
    return *this;
}

const QRequestResponse QRequestClient::response() const
{
    return QRequestResponse(p);
}

}

