#pragma once

#include <QStringList>
#include <QDateTime>

struct SnapshotInfo {
    QString name;
    QString path;
    QDateTime timestamp;
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
