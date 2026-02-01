#include <QTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QTimeZone>

#include "SnapshotFinder.h"

class SnapshotFinderTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testNoSnapshotDirectory();
    void testFileNoSnapshotDirectory();
    void testEmptySnapshotDirectory();
    void testFileEmptySnapshotDirectory();
    void testSnapshotDirectory();
    void testFileSnapshotDirectoryWithoutFile();
    void testFileSnapshotDirectory();
    void testFileSnapshotMultipleDirectories();
    void testFileSnapshotMultiplePartialDirectories();
    void testTimestamp();
    void testTimestampWithPrefixAndSuffix();
    void testTimestampUnknownFormat();
};

void SnapshotFinderTest::testNoSnapshotDirectory()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path();

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(!hasSnapshots);
    QCOMPARE(snapshots.size(), 0);
}

void SnapshotFinderTest::testFileNoSnapshotDirectory()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path() + QStringLiteral("/file.txt");

    QFile file(targetPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(!hasSnapshots);
    QCOMPARE(snapshots.size(), 0);
}

void SnapshotFinderTest::testEmptySnapshotDirectory()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path();

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(hasSnapshots);
    QCOMPARE(snapshots.size(), 0);
}

void SnapshotFinderTest::testFileEmptySnapshotDirectory()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path() + QStringLiteral("/file.txt");

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QFile file(targetPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(hasSnapshots);
    QCOMPARE(snapshots.size(), 0);
}

void SnapshotFinderTest::testSnapshotDirectory()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path();

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("snapshot1"));

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(hasSnapshots);
    QCOMPARE(snapshots.size(), 1);
}

void SnapshotFinderTest::testFileSnapshotDirectoryWithoutFile()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path() + QStringLiteral("/file.txt");

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("snapshot1"));

    QFile file(targetPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(hasSnapshots);
    QCOMPARE(snapshots.size(), 0);
}

void SnapshotFinderTest::testFileSnapshotDirectory()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path() + QStringLiteral("/file.txt");

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("snapshot1"));

    QFile snapshotFile(tmp.path() + QStringLiteral("/.snap/snapshot1/file.txt"));
    QVERIFY(snapshotFile.open(QIODevice::WriteOnly));
    snapshotFile.close();

    QFile file(targetPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(hasSnapshots);
    QCOMPARE(snapshots.size(), 1);
}

void SnapshotFinderTest::testFileSnapshotMultipleDirectories()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path() + QStringLiteral("/file.txt");

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("snapshot1"));
    snapDir.mkdir(QStringLiteral("snapshot2"));
    snapDir.mkdir(QStringLiteral("snapshot3"));

    QFile snapshotFile1(tmp.path() + QStringLiteral("/.snap/snapshot1/file.txt"));
    QVERIFY(snapshotFile1.open(QIODevice::WriteOnly));
    snapshotFile1.close();

    QFile snapshotFile2(tmp.path() + QStringLiteral("/.snap/snapshot2/file.txt"));
    QVERIFY(snapshotFile2.open(QIODevice::WriteOnly));
    snapshotFile2.close();

    QFile snapshotFile3(tmp.path() + QStringLiteral("/.snap/snapshot3/file.txt"));
    QVERIFY(snapshotFile3.open(QIODevice::WriteOnly));
    snapshotFile3.close();

    QFile file(targetPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(hasSnapshots);
    QCOMPARE(snapshots.size(), 3);
}

void SnapshotFinderTest::testFileSnapshotMultiplePartialDirectories()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path() + QStringLiteral("/file.txt");

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("snapshot1"));
    snapDir.mkdir(QStringLiteral("snapshot2"));
    snapDir.mkdir(QStringLiteral("snapshot3"));

    QFile snapshotFile2(tmp.path() + QStringLiteral("/.snap/snapshot2/file.txt"));
    QVERIFY(snapshotFile2.open(QIODevice::WriteOnly));
    snapshotFile2.close();

    QFile snapshotFile3(tmp.path() + QStringLiteral("/.snap/snapshot3/file.txt"));
    QVERIFY(snapshotFile3.open(QIODevice::WriteOnly));
    snapshotFile3.close();

    QFile file(targetPath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();

    // act
    bool hasSnapshots = SnapshotFinder::hasSnapshots(targetPath);
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(hasSnapshots);
    QCOMPARE(snapshots.size(), 2);
}

void SnapshotFinderTest::testTimestamp()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path();

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("scheduled-2026-01-23-04_00_40_UTC"));

    // act
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(snapshots.size() == 1);
    QVERIFY(snapshots.first().snapshotTimestamp.isValid());
    QCOMPARE(snapshots.first().snapshotTimestamp.date(), QDate(2026, 1, 23));
    QCOMPARE(snapshots.first().snapshotTimestamp.time(), QTime(4, 0, 40));
    QCOMPARE(snapshots.first().snapshotTimestamp.timeZone(), QTimeZone::utc());
}

void SnapshotFinderTest::testTimestampWithPrefixAndSuffix()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path();

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("_scheduled-2026-01-24-04_00_40_UTC_1"));

    // act
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(snapshots.size() == 1);
    QVERIFY(snapshots.first().snapshotTimestamp.isValid());
    QCOMPARE(snapshots.first().snapshotTimestamp.date(), QDate(2026, 1, 24));
    QCOMPARE(snapshots.first().snapshotTimestamp.time(), QTime(4, 0, 40));
    QCOMPARE(snapshots.first().snapshotTimestamp.timeZone(), QTimeZone::utc());
}

void SnapshotFinderTest::testTimestampUnknownFormat()
{
    // arrange
    QTemporaryDir tmp;
    QString targetPath = tmp.path();

    QDir dir(tmp.path());
    dir.mkdir(QStringLiteral(".snap"));

    QDir snapDir(tmp.path() + QStringLiteral("/.snap"));
    snapDir.mkdir(QStringLiteral("20260124T040040Z"));

    // act
    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(targetPath);

    // assert
    QVERIFY(snapshots.size() == 1);
    QVERIFY(!snapshots.first().snapshotTimestamp.isValid());
}

QTEST_MAIN(SnapshotFinderTest)
#include "SnapshotFinderTest.moc"
