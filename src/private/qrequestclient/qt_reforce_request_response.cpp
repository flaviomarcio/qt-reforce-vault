#include "./qt_reforce_request_response.h"
#include "./qt_reforce_request_client.cpp"
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

bool QRequestResponse::isOK() const
{
    return p->response.isOK();
}

bool QRequestResponse::isRunning() const
{
    return p->response.isRunning;
}

const QVariantHash &QRequestResponse::headers() const
{
    return p->response.headers;
}

int QRequestResponse::statusCode() const
{
    return p->response.statusCode;
}

QString QRequestResponse::reasonPhrase()const
{
    return p->response.reasonPhrase;
}

const QByteArray &QRequestResponse::body() const
{
    if(this->isOK() && p->response.bodyUrl.isLocalFile()){
        if(p->response.body.isEmpty()){
            QFile file(p->response.bodyUrl.toString());
            if(file.exists())
                p->response.body=p->readFile(file);
        }
    }
    return p->response.body;
}

const QVariantHash QRequestResponse::bodyAsMap() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toHash();
}

const QVariantList QRequestResponse::bodyAsList() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toList();
}

const QStringList QRequestResponse::bodyAsStringList() const
{
    return QJsonDocument::fromJson(p->response.body).toVariant().toStringList();
}

const QVariant QRequestResponse::bodyAsVariant() const
{
    auto v=QJsonDocument::fromVariant(p->response.body).toVariant();
    if(v.isValid())
        return v;
    return p->response.body;
}

QFile QRequestResponse::bodyAsFile()
{
    if(!this->isOK() || !p->response.bodyUrl.isEmpty())
        return {};

    return QFile(p->response.bodyUrl.toLocalFile());
}

}

