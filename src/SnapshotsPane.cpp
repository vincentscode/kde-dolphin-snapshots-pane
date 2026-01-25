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
        QString snapshotPath = snapDirectoryPath + QStringLiteral("/") + snapshotName + QStringLiteral("/") + fileName;
        QFileInfo snapshotFileInfo(snapshotPath);

        if (snapshotFileInfo.exists()) {
            SnapshotInfo info;
            info.name = snapshotName;
            info.path = snapshotPath;
            info.timestamp = snapshotFileInfo.lastModified();
            info.fileSize = snapshotFileInfo.size();
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
    treeWidget->setHeaderLabels({QStringLiteral("Name"), QStringLiteral("Date modified")});
    treeWidget->setRootIsDecorated(true);
    treeWidget->header()->setStretchLastSection(true);

    // Group consecutive snapshots with no changes
    int i = 0;
    while (i < snapshots.size()) {
        const SnapshotInfo &currentSnapshot = snapshots[i];
        
        // Find consecutive snapshots with same size and timestamp (unchanged)
        int groupEnd = i;
        while (groupEnd + 1 < snapshots.size() &&
               snapshots[groupEnd + 1].fileSize == currentSnapshot.fileSize &&
               snapshots[groupEnd + 1].timestamp == currentSnapshot.timestamp) {
            groupEnd++;
        }
        
        int groupSize = groupEnd - i + 1;
        
        if (groupSize > 1) {
            // Create a parent item for the group
            QTreeWidgetItem *groupItem = new QTreeWidgetItem();
            QString groupLabel = QString::number(groupSize) + QStringLiteral(" snapshots (no changes)");
            groupItem->setText(0, groupLabel);
            groupItem->setText(1, QLocale::system().toString(currentSnapshot.timestamp, QLocale::ShortFormat));
            groupItem->setForeground(0, treeWidget->palette().color(QPalette::Disabled, QPalette::Text));
            groupItem->setForeground(1, treeWidget->palette().color(QPalette::Disabled, QPalette::Text));
            treeWidget->addTopLevelItem(groupItem);
            
            // Add individual snapshots as children
            for (int j = i; j <= groupEnd; j++) {
                QTreeWidgetItem *childItem = new QTreeWidgetItem(groupItem);
                childItem->setText(0, snapshots[j].name);
                childItem->setText(1, QLocale::system().toString(snapshots[j].timestamp, QLocale::ShortFormat));
                childItem->setData(0, Qt::UserRole, snapshots[j].path);
            }
            
            // Start collapsed
            groupItem->setExpanded(false);
        } else {
            // Single snapshot or changed snapshot - add normally
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, currentSnapshot.name);
            item->setText(1, QLocale::system().toString(currentSnapshot.timestamp, QLocale::ShortFormat));
            item->setData(0, Qt::UserRole, currentSnapshot.path);
            treeWidget->addTopLevelItem(item);
        }
        
        i = groupEnd + 1;
    }

    treeWidget->resizeColumnToContents(0);
    layout->addWidget(treeWidget);

    connect(treeWidget, &QTreeWidget::itemDoubleClicked, this, [](QTreeWidgetItem *item) {
        QString path = item->data(0, Qt::UserRole).toString();
        if (!path.isEmpty()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    });
}
