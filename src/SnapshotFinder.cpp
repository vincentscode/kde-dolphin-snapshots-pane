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
    QDir dir(path);
    for (const QString &snapshotDirName : snapshotDirectoryNames) {
        QString potentialPath = dir.absoluteFilePath(snapshotDirName);
        QDir potentialDir(potentialPath);
        if (potentialDir.exists()) {
            return potentialPath;
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
    QString fileName = QFileInfo(path).fileName();
    QList<SnapshotInfo> snapshots;

    QString snapshotDirectoryPath = getSnapshotDirectoryPath(path);
    if (snapshotDirectoryPath.isEmpty()) {
        return snapshots;
    }

    QDir snapshotDirectory(snapshotDirectoryPath);
    QStringList snapshotEntries = snapshotDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &snapshotName : snapshotEntries) {
        QString snapshotPath = snapshotDirectoryPath + QStringLiteral("/") + snapshotName;
        if (!fileName.isEmpty()) {
            snapshotPath += QStringLiteral("/") + fileName;
        }

        QFileInfo snapshotFileInfo(snapshotPath);
        if (snapshotFileInfo.exists()) {
            SnapshotInfo info;
            info.name = snapshotName;
            info.path = snapshotPath;
            info.timestamp = snapshotFileInfo.lastModified();
            snapshots.append(info);
        }
    }

    return snapshots;
}
