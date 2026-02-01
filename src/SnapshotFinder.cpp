#include <QDir>
#include <QFileInfo>

#include "SnapshotFinder.h"

const QStringList SnapshotFinder::snapshotDirectoryNames = {
    QStringLiteral(".snap"),
    QStringLiteral(".snapshot"),
    QStringLiteral(".snapshots"),
    QStringLiteral(".zfs/snapshot"),
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
            info.lastModified = snapshotFileInfo.lastModified();
            snapshots.append(info);
        }
    }

    return snapshots;
}
