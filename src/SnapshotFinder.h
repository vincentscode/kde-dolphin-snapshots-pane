#pragma once

#include <QStringList>
#include <QDateTime>

struct SnapshotInfo {
    QString name;
    QString path;
    QDateTime lastModified;
};

class SnapshotFinder
{
public:
    static bool hasSnapshots(const QString &path);
    static QList<SnapshotInfo> getSnapshots(const QString &path);

private:
    static const QStringList snapshotDirectoryNames;
    static QString getSnapshotDirectoryPath(const QString &path);
};
