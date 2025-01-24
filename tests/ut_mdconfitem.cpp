#include <QtCore/QProcess>
#include <QtTest/QSignalSpy>
#include <QTest>

#include "mdconfitem.h"

#include "testbase.h"

namespace Tests {

class UtMDConfItem : public TestBase
{
    Q_OBJECT

public:
    UtMDConfItem();

private slots:
    void initTestCase();
    void cleanup();
    void basicTest_data();
    void basicTest();
    void defaultValue();
    void listDirs();

private:
    static const QString KEY;
};

} // namespace Tests

using namespace Tests;

const QString UtMDConfItem::KEY = "/mlite-tests/ut_mdconfitem/foo";

UtMDConfItem::UtMDConfItem()
{
}

void UtMDConfItem::initTestCase()
{
    MDConfItem(KEY).unset();
}

void UtMDConfItem::cleanup()
{
    if (!QTest::currentTestFailed()) {
        MDConfItem(KEY).unset();
    }
}

void UtMDConfItem::basicTest_data()
{
    QTest::addColumn<QVariant>("value1");
    QTest::addColumn<QVariant>("value2");

    QTest::newRow("string")  << QVariant("bar") << QVariant("baz");
    QTest::newRow("bool") << QVariant(true) << QVariant(false);
    QTest::newRow("int") << QVariant(42) << QVariant(24);
    QTest::newRow("float") << QVariant((double)42.5) << QVariant((double)24.5);
    QTest::newRow("string-list")
        << QVariant((QStringList() << "bar" << "baz"))
        << QVariant((QStringList() << "baz" << "bar"));
    QTest::newRow("variant-list-string")
        << QVariant((QVariantList() << "bar" << "baz"))
        << QVariant((QVariantList() << "baz" << "bar"));
    QTest::newRow("variant-list-bool")
        << QVariant((QVariantList() << true << false))
        << QVariant((QVariantList() << false << true));
    QTest::newRow("variant-list-int")
        << QVariant((QVariantList() << 42 << 24))
        << QVariant((QVariantList() << 24 << 42));
    QTest::newRow("variant-list-float")
        << QVariant((QVariantList() << (double)42.5 << (double)24.5))
        << QVariant((QVariantList() << (double)24.5 << (double)42.5));
}

void UtMDConfItem::basicTest()
{
    QFETCH(QVariant, value1);
    QFETCH(QVariant, value2);

    MDConfItem item1(KEY);

    MDConfItem item2(KEY);
    QSignalSpy spy2(&item2, SIGNAL(valueChanged()));
    item2.set(value1);
    QCOMPARE(item2.value(), value1);
    QCOMPARE(spy2.count(), 1);

    waitForSignal(&item1, SIGNAL(valueChanged()));
    QTRY_COMPARE(item1.value(), value1);

    QCOMPARE(MDConfItem(KEY).value(), value1);

    QSignalSpy spy1(&item1, SIGNAL(valueChanged()));
    item1.set(value2);
    QCOMPARE(item1.value(), value2);
    QCOMPARE(spy1.count(), 1);

    waitForSignal(&item2, SIGNAL(valueChanged()));
    QTRY_COMPARE(item2.value(), value2);

    QCOMPARE(MDConfItem(KEY).value(), value2);
}

void UtMDConfItem::defaultValue()
{
    MDConfItem(KEY).unset();
    QTRY_COMPARE(MDConfItem(KEY).value("default").toString(), QString("default"));
}

void UtMDConfItem::listDirs()
{
  MDConfItem key1("/mlite-tests/ut_mdconfitem/foo1/foo1");
  key1.set("foo1");
  MDConfItem key2("/mlite-tests/ut_mdconfitem/foo2/foo2");
  key2.set("foo2");

  QTRY_VERIFY(MDConfItem("/mlite-tests/ut_mdconfitem").listDirs().size() == 2);

  QStringList keys = MDConfItem("/mlite-tests/ut_mdconfitem").listDirs();

  QVERIFY(keys.indexOf("/mlite-tests/ut_mdconfitem/foo1") != -1);
  QVERIFY(keys.indexOf("/mlite-tests/ut_mdconfitem/foo2") != -1);

  key1.unset();
  key2.unset();

  QTRY_VERIFY(MDConfItem("/mlite-tests/ut_mdconfitem").listDirs().size() == 0);
}

QTEST_MAIN(Tests::UtMDConfItem)

#include "ut_mdconfitem.moc"
