#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>
#include <QVariantHash>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include "../qt_reforce_vault_global.h"

namespace QtVault {
class RequestClientPvt;

//!
//! \brief The RequestUtil class
//!

class Q_REFORCE_VAULT_EXPORT RequestUtil: public QObject
{
    Q_OBJECT
public:
    struct Response
    {
        friend class RequestClientPvt;
        friend class RequestUtil;
    private:
        explicit Response(RequestClientPvt *p):p{p}{}
    public:

        //!
        //! \brief toMap
        //! \return
        //!
        const QVariantHash toMap()const;

        //!
        //! \brief isOK
        //! \return
        //!
        bool isOK()const;

        //!
        //! \brief isRunning
        //! \return
        //!
        bool isRunning()const;

        //!
        //! \brief headers
        //! \return
        //!
        const QVariantHash &headers()const;

        //!
        //! \brief statusCode
        //! \return
        //!
        int statusCode()const;

        //!
        //! \brief reasonPhrase
        //! \return
        //!
        QString reasonPhrase() const;

        //!
        //! \brief bodyAsBytes
        //! \return
        //!
        const QByteArray &body()const;

        //!
        //! \brief bodyAsMap
        //! \return
        //!
        const QVariantHash bodyAsMap()const;

        //!
        //! \brief bodyAsList
        //! \return
        //!
        const QVariantList bodyAsList()const;

        //!
        //! \brief bodyAsStringList
        //! \return
        //!
        const QStringList bodyAsStringList()const;

        //!
        //! \brief bodyAs
        //! \param parent
        //! \return
        //!
        template <class T>
        T *bodyAs(QObject *parent)const
        {
            return new T(this->body(), parent);
        }
    private:
        RequestClientPvt *p=nullptr;
    };

    typedef std::function<void()> VoidMethod;
    typedef std::function<void(RequestUtil *request)> RequestMethod;
    typedef std::function<void(Response response)> ResponseMethod;

    class HttpEncoding {
    public:
        static const QByteArray CONTENT_LENGTH()            { static const auto value=QByteArray("Content-Length"); return value;}
        static const QByteArray CONTENT_TYPE()              { static const auto value=QByteArray("Content-Type"); return value;}
        static const QByteArray ACCEPT_ENCODING_HEADER()    { static const auto value=QByteArray("Accept-Encoding"); return value;}
        static const QByteArray CONTENT_ENCODING_HEADER()   { static const auto value=QByteArray("Content-Encoding"); return value;}
        static const QByteArray GZIP_ENCODING()             { static const auto value=QByteArray("gzip"); return value;}
        static const QByteArray DEFLATE_ENCODING()          { static const auto value=QByteArray("deflate"); return value;}
    };

    class MediaType{
    public:
        static const QByteArray ALL()                           { static const auto value=QByteArray("*/*");                                return value; }
        static const QByteArray APPLICATION_ATOM_XML()          { static const auto value=QByteArray("application/atom+xml");               return value; }
        static const QByteArray APPLICATION_CBOR()              { static const auto value=QByteArray("application/cbor");                   return value; }
        static const QByteArray APPLICATION_FORM_URLENCODED()   { static const auto value=QByteArray("application/x-www-form-urlencoded");  return value; }
        static const QByteArray APPLICATION_GRAPHQL()           { static const auto value=QByteArray("application/graphql+json");           return value; }
        static const QByteArray APPLICATION_JSON()              { static const auto value=QByteArray("application/json");                   return value; }
        static const QByteArray APPLICATION_NDJSON()            { static const auto value=QByteArray("application/x-ndjson");               return value; }
        static const QByteArray APPLICATION_OCTET_STREAM()      { static const auto value=QByteArray("application/octet-stream");           return value; }
        static const QByteArray APPLICATION_PDF()               { static const auto value=QByteArray("application/pdf");                    return value; }
        static const QByteArray APPLICATION_PROBLEM_JSON()      { static const auto value=QByteArray("application/problem+json");           return value; }
        static const QByteArray APPLICATION_PROBLEM_XML()       { static const auto value=QByteArray("application/problem+xml");            return value; }
        static const QByteArray APPLICATION_RSS_XML()           { static const auto value=QByteArray("application/rss+xml");                return value; }
        static const QByteArray APPLICATION_STREAM_JSON()       { static const auto value=QByteArray("application/stream+json");            return value; }
        static const QByteArray APPLICATION_XHTML_XML()         { static const auto value=QByteArray("application/xhtml+xml");              return value; }
        static const QByteArray APPLICATION_XML()               { static const auto value=QByteArray("application/xml");                    return value; }
        static const QByteArray IMAGE_GIF()                     { static const auto value=QByteArray("image/gif");                          return value; }
        static const QByteArray IMAGE_JPEG()                    { static const auto value=QByteArray("image/jpeg");                         return value; }
        static const QByteArray IMAGE_PNG()                     { static const auto value=QByteArray("image/png");                          return value; }
        static const QByteArray MULTIPART_FORM_DATA()           { static const auto value=QByteArray("multipart/form-data");                return value; }
        static const QByteArray MULTIPART_MIXED()               { static const auto value=QByteArray("multipart/mixed");                    return value; }
        static const QByteArray MULTIPART_RELATED()             { static const auto value=QByteArray("multipart/related");                  return value; }
        static const QByteArray TEXT_EVENT_STREAM()             { static const auto value=QByteArray("text/event-stream");                  return value; }
        static const QByteArray TEXT_HTML()                     { static const auto value=QByteArray("text/html");                          return value; }
        static const QByteArray TEXT_MARKDOWN()                 { static const auto value=QByteArray("text/markdown");                      return value; }
        static const QByteArray TEXT_PLAIN()                    { static const auto value=QByteArray("text/plain");                         return value; }
        static const QByteArray TEXT_XML()                      { static const auto value=QByteArray("text/xml");                           return value; }
    };

    enum Method{
        Head, Get, Post, Put, Delete, List, Upload, Download
    };
    Q_ENUM(Method)

    //!
    //! \brief RequestUtil
    //! \param parent
    //!
    Q_INVOKABLE explicit RequestUtil(QObject *parent=nullptr);

    //!
    //! \brief builder
    //! \return
    //!
    static RequestUtil &builder(QObject *parent=nullptr);

    //!
    //! \brief onStarted
    //! \param method
    //!
    const RequestUtil &onStarted(RequestUtil::VoidMethod method)const;

    //!
    //! \brief onSent
    //! \param method
    //!
    const RequestUtil &onSent(RequestUtil::VoidMethod method)const;

    //!
    //! \brief onSuccessful
    //! \param method
    //!
    const RequestUtil &onSuccessful(RequestUtil::ResponseMethod method)const;

    //!
    //! \brief onFail
    //! \param method
    //!
    const RequestUtil &onFail(RequestUtil::ResponseMethod method)const;

    //!
    //! \brief onFinished
    //! \param method
    //!
    const RequestUtil &onFinished(RequestMethod method)const;

    //!
    //! \brief call
    //! \return
    //!
    const RequestUtil &call()const;

    //!
    //! \brief abort
    //! \return
    //!
    const RequestUtil &abort();

    //!
    //! \brief isOK
    //! \return
    //!
    bool isOK()const;

    //!
    //! \brief response
    //! \return
    //!
    const Response response()const;

    //!
    //! \brief HEAD
    //! \return
    //!
    const RequestUtil &HEAD()const;

    //!
    //! \brief GET
    //! \return
    //!
    const RequestUtil &GET()const;

    //!
    //! \brief POST
    //! \return
    //!
    const RequestUtil &POST()const;

    //!
    //! \brief PUT
    //! \return
    //!
    const RequestUtil &PUT()const;

    //!
    //! \brief DELETE
    //! \return
    //!
    const RequestUtil &DELETE()const;

    //!
    //! \brief LIST
    //! \return
    //!
    const RequestUtil &LIST()const;

    //!
    //! \brief print
    //! \return
    //!
    const RequestUtil &print()const;

    //!
    //! \brief printOnFail
    //! \return
    //!
    const RequestUtil &printOnFail() const;
    const RequestUtil &printOnFail(bool newValue) const;

    //!
    //! \brief aSync
    //! \return
    //!
    bool aSync();
    const RequestUtil &aSync(bool newASync)const;

    //!
    //! \brief method
    //! \return
    //!
    Method method();
    const RequestUtil &method(Method newMethod)const;

    //!
    //! \brief headers
    //! \return
    //!
    const QVariantHash &headers()const;
    const RequestUtil &headers(const QVariantHash &newHeaders)const;
    const RequestUtil &headers(const QString&key, const QString &value)const;

    //!
    //! \brief headerApplicationJson
    //! \return
    //!
    const RequestUtil &headerApplicationJson()const;

    //!
    //! \brief headerFormUrlencoded
    //! \return
    //!
    const RequestUtil &headerFormUrlencoded()const;

    //!
    //! \brief headerApplicationOctecStream
    //! \return
    //!
    const RequestUtil &headerApplicationOctecStream()const;

    //!
    //! \brief headerApplicationCBor
    //! \return
    //!
    const RequestUtil &headerApplicationCBor()const;

    //!
    //! \brief headerTextPlain
    //! \return
    //!
    const RequestUtil &headerTextPlain()const;

    //!
    //! \brief headerApplicationXml
    //! \return
    //!
    const RequestUtil &headerApplicationXml()const;

    //!
    //! \brief headerApplicationPdf
    //! \return
    //!
    const RequestUtil &headerApplicationPdf()const;

    //!
    //! \brief url
    //! \return
    //!
    const QUrl &url()const;
    const RequestUtil &url(const QUrl &newUrl)const;

    //!
    //! \brief args
    //! \return
    //!
    const QVariantHash &args()const;
    const RequestUtil &args(const QString &key, const QVariant &value)const;
    const RequestUtil &args(const QPair<QVariant, QVariant> &newArg)const;
    const RequestUtil &args(const QVariantHash &newArgs)const;
    const RequestUtil &args(const QVariantMap &newArgs)const;

    //!
    //! \brief body
    //! \return
    //!
    const QByteArray &body()const;
    const RequestUtil &body(const QVariant &newBody)const;

private:
    //!
    //! \brief p
    //!
    RequestClientPvt *p=nullptr;
};

}

