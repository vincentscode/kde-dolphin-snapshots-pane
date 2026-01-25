#include <QTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include "SnapshotsPane.h"

class SnapshotDiscoveryTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Snapshot directory discovery tests
    void testFindSnapshotDirectory_snap();
    void testFindSnapshotDirectory_snapshot();
    void testFindSnapshotDirectory_snapshots();
    void testFindSnapshotDirectory_zfsSnapshot();
    void testFindSnapshotDirectory_noSnapshots();
    void testFindSnapshotDirectory_multipleTypes();

    // Snapshot finding tests
    void testFindSnapshots_forFile();
    void testFindSnapshots_forDirectory();
    void testFindSnapshots_noSnapshots();
    void testFindSnapshots_multipleSnapshots();
    void testFindSnapshots_snapshotWithTimestamp();
    void testFindSnapshotDirectory_nestedStructure();

private:
    QTemporaryDir *tempDir;
    QString testPath;

    void createDirectory(const QString &path);
    void createFile(const QString &path);
    void createSnapshotStructure(const QString &snapshotType, const QStringList &snapshotNames);
};

void SnapshotDiscoveryTest::initTestCase()
{
    // This runs once before all tests
}

void SnapshotDiscoveryTest::cleanupTestCase()
{
    // This runs once after all tests
}

void SnapshotDiscoveryTest::init()
{
    // This runs before each test
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());
    testPath = tempDir->path();
}

void SnapshotDiscoveryTest::cleanup()
{
    // This runs after each test
    delete tempDir;
    tempDir = nullptr;
}

void SnapshotDiscoveryTest::createDirectory(const QString &path)
{
    QDir dir;
    QVERIFY(dir.mkpath(path));
}

void SnapshotDiscoveryTest::createFile(const QString &path)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("test content");
    file.close();
}

void SnapshotDiscoveryTest::createSnapshotStructure(const QString &snapshotType, const QStringList &snapshotNames)
{
    QString snapshotDir = testPath + "/" + snapshotType;
    createDirectory(snapshotDir);
    
    for (const QString &name : snapshotNames) {
        QString snapshotPath = snapshotDir + "/" + name;
        createDirectory(snapshotPath);
    }
}

void SnapshotDiscoveryTest::testFindSnapshotDirectory_snap()
{
    createDirectory(testPath + "/.snap");
    
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QCOMPARE(result, testPath + "/.snap");
}

void SnapshotDiscoveryTest::testFindSnapshotDirectory_snapshot()
{
    createDirectory(testPath + "/.snapshot");
    
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QCOMPARE(result, testPath + "/.snapshot");
}

void SnapshotDiscoveryTest::testFindSnapshotDirectory_snapshots()
{
    createDirectory(testPath + "/.snapshots");
    
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QCOMPARE(result, testPath + "/.snapshots");
}

void SnapshotDiscoveryTest::testFindSnapshotDirectory_zfsSnapshot()
{
    createDirectory(testPath + "/.zfs");
    createDirectory(testPath + "/.zfs/snapshot");
    
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QCOMPARE(result, testPath + "/.zfs/snapshot");
}

void SnapshotDiscoveryTest::testFindSnapshotDirectory_noSnapshots()
{
    // Don't create any snapshot directories
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QVERIFY(result.isEmpty());
}

void SnapshotDiscoveryTest::testFindSnapshotDirectory_multipleTypes()
{
    // Test priority: .snap should be found first
    createDirectory(testPath + "/.snap");
    createDirectory(testPath + "/.snapshots");
    
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QCOMPARE(result, testPath + "/.snap");
}

void SnapshotDiscoveryTest::testFindSnapshots_forFile()
{
    // Create a file and snapshots for it
    QString testFile = testPath + "/testfile.txt";
    createFile(testFile);
    
    createSnapshotStructure(".snapshots", {"snapshot1", "snapshot2"});
    
    // Create the file in each snapshot
    createFile(testPath + "/.snapshots/snapshot1/testfile.txt");
    createFile(testPath + "/.snapshots/snapshot2/testfile.txt");
    
    // This test would need access to private findSnapshots method
    // or we would need to make it accessible for testing
    // For now, we test the static methods only
}

void SnapshotDiscoveryTest::testFindSnapshots_forDirectory()
{
    // Create a directory structure
    QString testSubDir = testPath + "/subdir";
    createDirectory(testSubDir);
    
    // Create snapshots
    createDirectory(testSubDir + "/.snapshots");
    createDirectory(testSubDir + "/.snapshots/snap1");
    createDirectory(testSubDir + "/.snapshots/snap2");
}

void SnapshotDiscoveryTest::testFindSnapshots_noSnapshots()
{
    QString testFile = testPath + "/testfile.txt";
    createFile(testFile);
    
    // No snapshot directory created
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QVERIFY(result.isEmpty());
}

void SnapshotDiscoveryTest::testFindSnapshots_multipleSnapshots()
{
    createSnapshotStructure(".snap", {"2024-01-01", "2024-01-02", "2024-01-03"});
    
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QVERIFY(!result.isEmpty());
    
    QDir snapDir(result);
    QStringList snapshots = snapDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    QCOMPARE(snapshots.size(), 3);
}

void SnapshotDiscoveryTest::testFindSnapshots_snapshotWithTimestamp()
{
    // Create snapshots with timestamp-like names
    QStringList timestampNames = {
        "2024-01-01_10-00-00",
        "2024-01-02_15-30-00",
        "2024-01-03_20-45-00"
    };
    
    createSnapshotStructure(".snapshots", timestampNames);
    
    QString result = SnapshotsPane::findSnapshotDirectory(testPath);
    QVERIFY(!result.isEmpty());
    
    QDir snapDir(result);
    QStringList snapshots = snapDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    QCOMPARE(snapshots.size(), 3);
    QCOMPARE(snapshots[0], timestampNames[0]);
}

void SnapshotDiscoveryTest::testFindSnapshotDirectory_nestedStructure()
{
    // Test case from user comment: Ensure snapshot discovery only looks in immediate directory
    // Given folder structure /a/b/c where:
    // - /a does not contain .snap
    // - /a/b/.snap exists
    // - /a/b/c/.snap exists
    
    // Create the directory structure
    QString dirA = testPath + "/a";
    QString dirB = dirA + "/b";
    QString dirC = dirB + "/c";
    
    createDirectory(dirA);
    createDirectory(dirB);
    createDirectory(dirC);
    
    // Create snapshot directories in b and c, but NOT in a
    createDirectory(dirB + "/.snap");
    createDirectory(dirC + "/.snap");
    
    // Test 1: Querying /a should find NO snapshots (even though child dirs have .snap)
    QString resultA = SnapshotsPane::findSnapshotDirectory(dirA);
    QVERIFY(resultA.isEmpty());
    
    // Test 2: Querying /a/b should find /a/b/.snap
    QString resultB = SnapshotsPane::findSnapshotDirectory(dirB);
    QCOMPARE(resultB, dirB + "/.snap");
    
    // Test 3: Querying for file /a/b/test.txt should find /a/b/.snap (parent directory)
    QString testFile = dirB + "/test.txt";
    createFile(testFile);
    
    // For a file, we need to get its parent directory and check there
    QFileInfo fileInfo(testFile);
    QString parentDir = fileInfo.absolutePath();
    QString resultFile = SnapshotsPane::findSnapshotDirectory(parentDir);
    QCOMPARE(resultFile, dirB + "/.snap");
}

QTEST_MAIN(SnapshotDiscoveryTest)
#include "SnapshotDiscoveryTest.moc"
