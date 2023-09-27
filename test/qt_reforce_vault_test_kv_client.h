#pragma once

#include <QThread>
#include "./qt_reforce_vault_test.h"
#include "../src/qt_reforce_vault_kv_client.h"

namespace QtReforce {

class UT_KvClient : public QtReforce::ObjectTest
{
    Q_OBJECT
    Q_VAULT_OBJECT_TEST(UT_KvClient)
public:
    KvClient u;

    Q_INVOKABLE void test_loaded()
    {
        QString vault_url=qgetenv("vault_url");
        QString vault_token=qgetenv("vault_token");
        QString vault_import=qgetenv("vault_import");

        u
            .setting()
            .token(vault_token)
            .url(vault_url)
            .import(vault_import);

        u.load();

        while(u.isLoading()){
            QThread::msleep(1);
            qApp->processEvents();
        }
        auto values=u.values();

        QCOMPARE_EQ(values.size(),2);
        QVERIFY(values.contains("a"));
        QVERIFY(values.contains("c"));
        QCOMPARE_EQ(values.value("a"),"bbbbb");
        QCOMPARE_EQ(values.value("c"),"dddddd");
    }

};

}

