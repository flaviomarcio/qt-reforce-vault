#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QVariantHash>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include <QFile>
#include "./qt_reforce_request_client_global.h"
#include "./qt_reforce_request_media_types.h"
#include "./qt_reforce_request_response.h"

namespace QtReforce {
class QRequestClientPvt;

//!
//! \brief The QRequestClient class
//!
class Q_REFORCE_REQUEST_CLIENT_EXPORT QRequestClient: public QObject
{
    Q_OBJECT
public:

    typedef std::function<void()> VoidMethod;
    typedef std::function<void(QRequestClient *request)> RequestMethod;
    typedef std::function<void(QRequestResponse response)> ResponseMethod;

    class HttpEncoding {
    public:
        static const QByteArray CONTENT_LENGTH()            { static const auto value=QByteArray("Content-Length"); return value;}
        static const QByteArray CONTENT_TYPE()              { static const auto value=QByteArray("Content-Type"); return value;}
        static const QByteArray ACCEPT_ENCODING_HEADER()    { static const auto value=QByteArray("Accept-Encoding"); return value;}
        static const QByteArray CONTENT_ENCODING_HEADER()   { static const auto value=QByteArray("Content-Encoding"); return value;}
        static const QByteArray GZIP_ENCODING()             { static const auto value=QByteArray("gzip"); return value;}
        static const QByteArray DEFLATE_ENCODING()          { static const auto value=QByteArray("deflate"); return value;}
    };


    enum Method{
        Head, Get, Post, Put, Delete, List, Upload, Download
    };
    Q_ENUM(Method)

    //!
    //! \brief RequestUtil
    //! \param parent
    //!
    Q_INVOKABLE explicit QRequestClient(QObject *parent=nullptr);

    //!
    //! \brief builder
    //! \return
    //!
    static QRequestClient &builder(QObject *parent=nullptr);

    //!
    //! \brief onStarted
    //! \param method
    //!
    const QRequestClient &onStarted(QRequestClient::VoidMethod method)const;

    //!
    //! \brief onSent
    //! \param method
    //!
    const QRequestClient &onSent(QRequestClient::VoidMethod method)const;

    //!
    //! \brief onSuccessful
    //! \param method
    //!
    const QRequestClient &onSuccessful(QRequestClient::ResponseMethod method)const;

    //!
    //! \brief onFail
    //! \param method
    //!
    const QRequestClient &onFail(QRequestClient::ResponseMethod method)const;

    //!
    //! \brief onFinished
    //! \param method
    //!
    const QRequestClient &onFinished(RequestMethod method)const;

    //!
    //! \brief call
    //! \return
    //!
    const QRequestClient &call()const;

    //!
    //! \brief abort
    //! \return
    //!
    const QRequestClient &abort();

    //!
    //! \brief isOK
    //! \return
    //!
    bool isOK()const;

    //!
    //! \brief HEAD
    //! \return
    //!
    const QRequestClient &HEAD()const;

    //!
    //! \brief GET
    //! \return
    //!
    const QRequestClient &GET()const;

    //!
    //! \brief POST
    //! \return
    //!
    const QRequestClient &POST()const;

    //!
    //! \brief PUT
    //! \return
    //!
    const QRequestClient &PUT()const;

    //!
    //! \brief DELETE
    //! \return
    //!
    const QRequestClient &DELETE()const;

    //!
    //! \brief LIST
    //! \return
    //!
    const QRequestClient &LIST()const;

    //!
    //! \brief UPLOAD
    //! \return
    //!
    const QRequestClient &UPLOAD()const;

    //!
    //! \brief DOWNLOAD
    //! \return
    //!
    const QRequestClient &DOWNLOAD()const;

    //!
    //! \brief print
    //! \return
    //!
    const QRequestClient &print()const;

    //!
    //! \brief printOnFail
    //! \return
    //!
    const QRequestClient &printOnFail() const;
    const QRequestClient &printOnFail(bool newValue) const;

    //!
    //! \brief waitForFinished
    //! \return
    //!
    bool waitForFinished();
    const QRequestClient &waitForFinished(bool newWaitForFinished)const;

    //!
    //! \brief method
    //! \return
    //!
    Method method();
    const QRequestClient &method(Method newMethod)const;
    const QRequestClient &method(const QString &newMethod)const;

    //!
    //! \brief headers
    //! \return
    //!
    const QVariantHash &headers()const;
    const QRequestClient &headers(const QVariantHash &newHeaders)const;

    //!
    //! \brief header
    //! \param key
    //! \param value
    //! \return
    //!
    const QRequestClient &header(const QString&key, const QString &value)const;

    //!
    //! \brief headerApplicationJson
    //! \return
    //!
    const QRequestClient &headerApplicationJson()const;

    //!
    //! \brief headerFormUrlencoded
    //! \return
    //!
    const QRequestClient &headerFormUrlencoded()const;

    //!
    //! \brief headerApplicationOctecStream
    //! \return
    //!
    const QRequestClient &headerApplicationOctecStream()const;

    //!
    //! \brief headerApplicationCBor
    //! \return
    //!
    const QRequestClient &headerApplicationCBor()const;

    //!
    //! \brief headerTextPlain
    //! \return
    //!
    const QRequestClient &headerTextPlain()const;

    //!
    //! \brief headerApplicationXml
    //! \return
    //!
    const QRequestClient &headerApplicationXml()const;

    //!
    //! \brief headerApplicationPdf
    //! \return
    //!
    const QRequestClient &headerApplicationPdf()const;

    //!
    //! \brief url
    //! \return
    //!
    const QUrl &url()const;
    const QRequestClient &url(const QUrl &newUrl)const;
    const QRequestClient &url(const QString &newUrl)const;

    //!
    //! \brief args
    //! \return
    //!
    const QVariantHash &args()const;
    const QRequestClient &args(const QString &key, const QVariant &value)const;
    const QRequestClient &args(const QPair<QVariant, QVariant> &newArg)const;
    const QRequestClient &args(const QVariantHash &newArgs)const;
    const QRequestClient &args(const QVariantMap &newArgs)const;
    const QRequestClient &args(QObject *newBody)const;
    const QRequestClient &args(const QFile &file)const;

    //!
    //! \brief body
    //! \return
    //!
    const QByteArray &body()const;
    const QRequestClient &body(const QVariant &newBody)const;
    const QRequestClient &body(QObject *newBody)const;
    const QRequestClient &body(const QFile &file)const;

    //!
    //! \brief response
    //! \return
    //!
    const QRequestResponse response()const;

private:
    //!
    //! \brief p
    //!
    QRequestClientPvt *p=nullptr;
};

}

