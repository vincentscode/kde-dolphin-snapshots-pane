#include <QTest>
#include <QFile>
#include <QTextStream>
#include <QTemporaryDir>
#include <QLocale>
#include <KLocalizedString>
#include <QDebug>

class LocalizationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Translation file tests
    void testPoFilesExist();
    void testGermanPoFileValid();
    void testEnglishPoFileValid();
    
    // Translation string tests
    void testPluralFormTranslations();
    void testContextTranslations();
    void testSnapshotFoundString();
    void testColumnHeaders();
    
    // Locale handling tests
    void testLocaleFormatting();

private:
    QString repoRoot;
    
    bool checkPoFileStructure(const QString &filePath);
    QMap<QString, QString> parsePoFile(const QString &filePath);
};

void LocalizationTest::initTestCase()
{
    // Get the repository root
    repoRoot = QDir::currentPath();
    while (!QFile::exists(repoRoot + "/CMakeLists.txt") && repoRoot != "/") {
        QDir dir(repoRoot);
        dir.cdUp();
        repoRoot = dir.absolutePath();
    }
    
    QVERIFY(QFile::exists(repoRoot + "/CMakeLists.txt"));
}

void LocalizationTest::cleanupTestCase()
{
    // Cleanup after all tests
}

void LocalizationTest::testPoFilesExist()
{
    QString poDir = repoRoot + "/po";
    QVERIFY(QDir(poDir).exists());
    
    // Check that translation files exist
    QVERIFY(QFile::exists(poDir + "/de.po"));
    QVERIFY(QFile::exists(poDir + "/en_US.po"));
}

bool LocalizationTest::checkPoFileStructure(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    // Check for required PO file elements
    return content.contains("msgid") && 
           content.contains("msgstr") &&
           content.contains("Content-Type: text/plain; charset=UTF-8");
}

QMap<QString, QString> LocalizationTest::parsePoFile(const QString &filePath)
{
    QMap<QString, QString> translations;
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return translations;
    }
    
    QTextStream in(&file);
    QString currentMsgId;
    QString currentMsgStr;
    bool inMsgId = false;
    bool inMsgStr = false;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.startsWith("msgid ")) {
            if (!currentMsgId.isEmpty() && !currentMsgStr.isEmpty()) {
                translations[currentMsgId] = currentMsgStr;
            }
            currentMsgId = line.mid(6).trimmed().remove('"');
            inMsgId = true;
            inMsgStr = false;
        } else if (line.startsWith("msgstr ")) {
            currentMsgStr = line.mid(7).trimmed().remove('"');
            inMsgId = false;
            inMsgStr = true;
        } else if (line.startsWith("msgstr[")) {
            // Handle plural forms
            inMsgId = false;
            inMsgStr = true;
        } else if (line.isEmpty() || line.startsWith("#")) {
            if (!currentMsgId.isEmpty() && !currentMsgStr.isEmpty()) {
                translations[currentMsgId] = currentMsgStr;
            }
            currentMsgId.clear();
            currentMsgStr.clear();
            inMsgId = false;
            inMsgStr = false;
        }
    }
    
    if (!currentMsgId.isEmpty() && !currentMsgStr.isEmpty()) {
        translations[currentMsgId] = currentMsgStr;
    }
    
    file.close();
    return translations;
}

void LocalizationTest::testGermanPoFileValid()
{
    QString poFile = repoRoot + "/po/de.po";
    QVERIFY(checkPoFileStructure(poFile));
    
    QFile file(poFile);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    
    QString content = file.readAll();
    file.close();
    
    // Check that German translations exist
    QVERIFY(content.contains("Snapshots"));
    QVERIFY(content.contains("gefunden")); // German word for "found"
}

void LocalizationTest::testEnglishPoFileValid()
{
    QString poFile = repoRoot + "/po/en_US.po";
    QVERIFY(checkPoFileStructure(poFile));
    
    QFile file(poFile);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    
    QString content = file.readAll();
    file.close();
    
    // Check that English translations exist
    QVERIFY(content.contains("snapshot found"));
    QVERIFY(content.contains("Name"));
    QVERIFY(content.contains("Date"));
}

void LocalizationTest::testPluralFormTranslations()
{
    // Test that plural forms are properly defined
    QString germanPo = repoRoot + "/po/de.po";
    QFile file(germanPo);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    
    QString content = file.readAll();
    file.close();
    
    // Check for plural forms
    QVERIFY(content.contains("msgid_plural"));
    QVERIFY(content.contains("1 snapshot found:"));
    QVERIFY(content.contains("%1 snapshots found:"));
    
    // Check German plural forms
    QVERIFY(content.contains("msgstr[0]"));
    QVERIFY(content.contains("msgstr[1]"));
    QVERIFY(content.contains("1 Snapshot gefunden:"));
    QVERIFY(content.contains("%1 Snapshots gefunden:"));
}

void LocalizationTest::testContextTranslations()
{
    QString germanPo = repoRoot + "/po/de.po";
    QFile file(germanPo);
    QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
    
    QString content = file.readAll();
    file.close();
    
    // Check for context-based translations
    QVERIFY(content.contains("msgctxt \"@title:column\""));
    QVERIFY(content.contains("msgctxt \"SnapshotsPane\""));
}

void LocalizationTest::testSnapshotFoundString()
{
    // Test that the main string exists in both translations
    QStringList languages = {"de.po", "en_US.po"};
    
    for (const QString &lang : languages) {
        QString poFile = repoRoot + "/po/" + lang;
        QFile file(poFile);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        
        QString content = file.readAll();
        file.close();
        
        QVERIFY(content.contains("1 snapshot found:"));
        QVERIFY(content.contains("%1 snapshots found:"));
    }
}

void LocalizationTest::testColumnHeaders()
{
    // Test that column header translations exist
    QStringList languages = {"de.po", "en_US.po"};
    
    for (const QString &lang : languages) {
        QString poFile = repoRoot + "/po/" + lang;
        QFile file(poFile);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
        
        QString content = file.readAll();
        file.close();
        
        // Both files should have Name and Date
        QVERIFY(content.contains("msgid \"Name\""));
        QVERIFY(content.contains("msgid \"Date\""));
        
        if (lang == "de.po") {
            QVERIFY(content.contains("msgstr \"Name\""));
            QVERIFY(content.contains("msgstr \"Datum\""));
        }
    }
}

void LocalizationTest::testLocaleFormatting()
{
    // Test that QLocale is available for date formatting
    QLocale locale = QLocale::system();
    QVERIFY(!locale.name().isEmpty());
    
    // Test date formatting with different locales
    QDateTime testDate = QDateTime::fromString("2024-01-15 14:30:00", "yyyy-MM-dd HH:mm:ss");
    QVERIFY(testDate.isValid());
    
    QString formatted = locale.toString(testDate, QLocale::ShortFormat);
    QVERIFY(!formatted.isEmpty());
    
    // Test with German locale
    QLocale germanLocale(QLocale::German);
    QString germanFormatted = germanLocale.toString(testDate, QLocale::ShortFormat);
    QVERIFY(!germanFormatted.isEmpty());
    
    // Test with US locale
    QLocale usLocale(QLocale::English, QLocale::UnitedStates);
    QString usFormatted = usLocale.toString(testDate, QLocale::ShortFormat);
    QVERIFY(!usFormatted.isEmpty());
}

QTEST_MAIN(LocalizationTest)
#include "LocalizationTest.moc"
