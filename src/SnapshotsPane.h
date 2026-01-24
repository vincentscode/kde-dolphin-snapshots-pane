#pragma once

#include <QWidget>
#include <QStringList>
#include <QDateTime>

class KPropertiesDialog;

struct SnapshotInfo {
    QString name;
    QString path;
    QDateTime timestamp;
};

class SnapshotsPane : public QWidget
{
Q_OBJECT

public:
    SnapshotsPane(const QString &filePath, KPropertiesDialog *props);
    static QString findSnapshotDirectory(const QString &parentPath);

private:
    static const QStringList snapshotDirectoryNames;
    QList<SnapshotInfo> findSnapshots(const QString &filePath);
};
