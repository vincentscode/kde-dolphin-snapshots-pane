#include <QDir>
#include <QFileInfo>
#include <QTimeZone>

#include "SnapshotFinder.h"

const QStringList SnapshotFinder::snapshotDirectoryNames = {
    QStringLiteral(".snap"),
    QStringLiteral(".snapshot"),
    QStringLiteral(".snapshots"),
    QStringLiteral(".zfs/snapshot"),
};

const QStringList SnapshotFinder::snapshotTimestampTemplates = {
    QStringLiteral("*'scheduled-'yyyy-MM-dd-HH_mm_ss'_UTC'*"),
};

QString SnapshotFinder::getSnapshotDirectoryPath(const QString &path)
{
    QFileInfo pathInfo(path);
    QString snapshotSearchPath = pathInfo.isDir()
        ? pathInfo.absoluteFilePath()
        : pathInfo.absolutePath();

    QDir snapshotSearchDirectory(snapshotSearchPath);
    for (const QString &snapshotDirectoryName : snapshotDirectoryNames) {
        QString snapshotDirectoryPath = QDir::cleanPath(snapshotSearchDirectory.absoluteFilePath(snapshotDirectoryName));
        QDir snapshotDirectory(snapshotDirectoryPath);

        if (snapshotDirectory.exists()) {
            return snapshotDirectoryPath;
        }
    }

    return QString();
}

QDateTime SnapshotFinder::parseSnapshotTimestamp(const QString &snapshotName)
{
    for (const QString &timestampTemplate : snapshotTimestampTemplates) {
        QString format = timestampTemplate;

        bool ignorePrefix = format.startsWith(QLatin1Char('*'));
        if (ignorePrefix) {
            format = format.mid(1);
        }

        bool ignoreSuffix = format.endsWith(QLatin1Char('*'));
        if (ignoreSuffix) {
            format.chop(1);
        }

        int maxPrefixLength = ignorePrefix ? snapshotName.length() : 0;
        int maxSuffixLength = ignoreSuffix ? snapshotName.length() : 0;
        for (int prefixLength = 0; prefixLength <= maxPrefixLength; prefixLength++) {
            for (int suffixLength = 0; suffixLength <= maxSuffixLength; suffixLength++) {
                int remainingLength = snapshotName.length() - prefixLength - suffixLength;
                if (remainingLength <= 0) break;

                QDateTime timestamp = QDateTime::fromString(snapshotName.mid(prefixLength, remainingLength), format);
                if (timestamp.isValid()) {
                    timestamp.setTimeZone(QTimeZone::UTC);
                    return timestamp;
                }
            }
        }
    }

    return QDateTime();
}

bool SnapshotFinder::hasSnapshots(const QString &path)
{
    return !getSnapshotDirectoryPath(path).isEmpty();
}

QList<SnapshotInfo> SnapshotFinder::getSnapshots(const QString &path)
{
    QFileInfo pathInfo(path);
    QString fileName = pathInfo.fileName();
    QList<SnapshotInfo> snapshots;

    QString snapshotDirectoryPath = getSnapshotDirectoryPath(path);
    if (snapshotDirectoryPath.isEmpty()) {
        return snapshots;
    }

    QDir snapshotDirectory(snapshotDirectoryPath);
    QStringList snapshotEntries = snapshotDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &snapshotName : snapshotEntries) {
        QString snapshotPath = QDir::cleanPath(snapshotDirectory.absoluteFilePath(snapshotName));
        if (pathInfo.isFile()) {
            snapshotPath = QDir::cleanPath(QDir(snapshotPath).absoluteFilePath(fileName));
        }

        QFileInfo snapshotFileInfo(snapshotPath);
        if (snapshotFileInfo.exists()) {
            SnapshotInfo info;
            info.name = snapshotName;
            info.path = snapshotPath;
            info.lastModifiedTimestamp = snapshotFileInfo.lastModified();
            info.snapshotTimestamp = parseSnapshotTimestamp(snapshotName);
            snapshots.append(info);
        }
    }

    return snapshots;
}
