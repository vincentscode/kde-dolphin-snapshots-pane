#include <QVBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QDir>
#include <QFileInfo>
#include <QTreeWidget>
#include <QHeaderView>
#include <QDesktopServices>
#include <QLocale>

#include <KPropertiesDialog>

#include "SnapshotsPane.h"

const QStringList SnapshotsPane::snapshotDirectoryNames = {
    QStringLiteral(".snap"),
    QStringLiteral(".snapshot"),
    QStringLiteral(".snapshots"),
    QStringLiteral(".zfs/snapshot"),
};

QString SnapshotsPane::findSnapshotDirectory(const QString &parentPath)
{
    for (const QString &name : snapshotDirectoryNames) {
        QString path = parentPath + QStringLiteral("/") + name;
        if (QDir(path).exists()) {
            return path;
        }
    }
    return QString();
}

QList<SnapshotInfo> SnapshotsPane::findSnapshots(const QString &filePath)
{
    QList<SnapshotInfo> snapshots;
    QFileInfo fileInfo(filePath);

    QString parentDirectoryPath = fileInfo.absolutePath();
    QString fileName = fileInfo.fileName();

    QString snapDirectoryPath = findSnapshotDirectory(parentDirectoryPath);
    if (snapDirectoryPath.isEmpty()) {
        return snapshots;
    }

    QDir snapDirectory(snapDirectoryPath);
    QStringList snapshotEntries = snapDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    for (const QString &snapshotName : snapshotEntries) {
        QString snapshotDirPath = snapDirectoryPath + QStringLiteral("/") + snapshotName;
        QString snapshotPath = snapshotDirPath + QStringLiteral("/") + fileName;
        QFileInfo snapshotFileInfo(snapshotPath);

        if (snapshotFileInfo.exists()) {
            // Get the snapshot directory's modification time, which represents when the snapshot was created
            QFileInfo snapshotDirInfo(snapshotDirPath);
            
            SnapshotInfo info;
            info.name = snapshotName;
            info.path = snapshotPath;
            info.timestamp = snapshotDirInfo.lastModified();
            snapshots.append(info);
        }
    }

    return snapshots;
}

SnapshotsPane::SnapshotsPane(const QString &filePath, KPropertiesDialog *props)
    : QWidget(props)
{
    setMinimumWidth(500);
    QVBoxLayout *layout = new QVBoxLayout(this);

    QFileInfo fileInfo(filePath);
    QString parentDirectoryPath = fileInfo.absolutePath();

    if (findSnapshotDirectory(parentDirectoryPath).isEmpty()) {
        QLabel *messageLabel = new QLabel(QStringLiteral("Snapshots are not supported at this location."), this);
        layout->addWidget(messageLabel);
        return;
    }

    QList<SnapshotInfo> snapshots = findSnapshots(filePath);

    QLabel *headerLabel = new QLabel(QString::number(snapshots.size()) + QStringLiteral(" snapshot(s) found:"), this);
    layout->addWidget(headerLabel);

    QTreeWidget *treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels({QStringLiteral("Name"), QStringLiteral("Snapshot Date")});
    treeWidget->setRootIsDecorated(false);
    treeWidget->header()->setStretchLastSection(true);

    for (const SnapshotInfo &snapshot : snapshots) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, snapshot.name);
        item->setText(1, QLocale::system().toString(snapshot.timestamp, QLocale::ShortFormat));
        item->setData(0, Qt::UserRole, snapshot.path);
        treeWidget->addTopLevelItem(item);
    }

    treeWidget->resizeColumnToContents(0);
    layout->addWidget(treeWidget);

    connect(treeWidget, &QTreeWidget::itemDoubleClicked, this, [](QTreeWidgetItem *item) {
        QString path = item->data(0, Qt::UserRole).toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });
}
