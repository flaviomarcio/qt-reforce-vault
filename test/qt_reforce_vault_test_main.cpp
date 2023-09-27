#include "qt_reforce_vault_test_main.h"
#include <QTest>

#include "./qt_reforce_vault_test_main.h"
#include "./qt_reforce_vault_test_kv_client.h"

#define Q_VAULT_TEST_RUNNER(CLASS){CLASS var; var.execute();}

namespace QtReforce{

void TestMain::execute()
{
    Q_VAULT_TEST_RUNNER(UT_KvClient);
}

}
QTEST_MAIN(QtReforce::TestMain)
#include "moc_qt_reforce_vault_test_main.cpp"


