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

private:
    static const QStringList snapshotDirectoryNames;
    QString findSnapshotDirectory(const QString &parentPath);
    QString getSearchPathForSnapshots(const QString &filePath);
    QList<SnapshotInfo> findSnapshots(const QString &filePath);
};
