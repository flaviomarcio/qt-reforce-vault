#pragma once

#include <QObject>
#include <QDebug>
#include <QThread>

#define ___party_1 QString::number(qlonglong(QThread::currentThreadId()))+QStringLiteral(":")

#define ___party_2 QString(__PRETTY_FUNCTION__).replace(QStringLiteral("virtual"),QLatin1String("")).split(QStringLiteral("(")).first().trimmed()+QStringLiteral(":")

#define sDebug() qDebug()<<___party_1<<___party_2

#define sCritical() qCritical()<<___party_1<<___party_2

#define sInfo() qInfo()<<___party_1<<___party_2

#define sWarning() qWarning()<<___party_1<<___party_2

#define sDebugMethodStart()qDebug()<<___party_1<<___party_2<<QStringLiteral(":start")

#define sDebugMethodFinish()qDebug()<<___party_1<<___party_2<<QStringLiteral(":finish")

#define v_vl QVariantList()

#define v_sl QStringList()

#define qv QVariant

#define qvm QVariantMap

#define qvh QVariantHash

#define qvl QVariantList

#define qvm_null QVariantMap{}

#define qvsl_null QStringList{}

#define qsl_pipe QStringLiteral("|")

#define qsl_percent QByteArrayLiteral("%")

#define qsl_underline QStringLiteral("_")

#define qbl_percent QByteArrayLiteral("%")

#define qbl_underline QStringLiteral("_")

#define qsl_number(v) QString::number(v,'f',0)

#define qbl_number(v) QString::number(v,'f',0).toUtf8()

#define qsl_double(v) QString::number(v,'f',6)

#define qbl_double(v) QString::number(v,'f',6).toUtf8()

#define qsl_key_return QByteArrayLiteral("\n")

#define qsl_key_enter QByteArrayLiteral("\n")

#define Q_V_MAP_ITERATOR(v)QMapIterator<QString, QVariant> i(v);while (i.hasNext())

#define Q_V_HASH_ITERATOR(v) QHashIterator<QString, QVariant> i(v); while (i.hasNext())

#define Q_V_MULTI_HASH_ITERATOR(v)QMultiHashIterator<QString, QVariant> i(v); while (i.hasNext())

#define Q_V_PROPERTY_ITERATOR(v)QHashIterator<QString, QMetaProperty> i(v); while (i.hasNext())

#define Q_V_METHOD_ITERATOR(v)QHashIterator<QString, QMetaMethod> i(v); while (i.hasNext())

#define Q_V_METAOBJECT_ITERATOR(v)QHashIterator<QByteArray, QMetaObject> i(v); while (i.hasNext())

#define Q_V_HASH_ITERATOR_STR_DOUBLE(v) QHashIterator<QString, double> i(v); while (i.hasNext())

#define Q_V_HASH_ITERATOR_STRING(v) QHashIterator<QString, QString> i(v); while (i.hasNext())

#define Q_V_VVM_ITERATOR(v)QHashIterator<QString, QVariant> i(v); while (i.hasNext())

#define Q_V_DATABASE_ITERATOR(v)QHashIterator<QString, QSqlDatabase> i(v); while (i.hasNext())

#define Q_LOOP_LIMIT(counter,loops) auto counter=0; while(loops>=counter++)

#define Q_SORT(TARGET, LAMBDA) std::sort(TARGET->begin(), TARGET->end(), LAMBDA)

#define qtr QObject::tr

#define __FUNCTION_UUID__ QCryptographicHash::hash(__PRETTY_FUNCTION__, QCryptographicHash::Md5).toHex()
