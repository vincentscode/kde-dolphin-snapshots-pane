#include <QVBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QTreeWidget>
#include <QHeaderView>
#include <QDesktopServices>
#include <QLocale>

#include <KPropertiesDialog>
#include <KLocalizedString>

#include "SnapshotsPane.h"
#include "SnapshotFinder.h"

SnapshotsPane::SnapshotsPane(const QString &filePath, KPropertiesDialog *props) : QWidget(props)
{
    setMinimumWidth(500);
    QVBoxLayout *layout = new QVBoxLayout(this);

    QList<SnapshotInfo> snapshots = SnapshotFinder::getSnapshots(filePath);

    QLabel *headerLabel = new QLabel(i18np("1 snapshot found:", "%1 snapshots found:", snapshots.size()), this);
    layout->addWidget(headerLabel);

    QTreeWidget *treeWidget = new QTreeWidget(this);
    treeWidget->setHeaderLabels({ i18nc("@title:column", "Name"), i18nc("@title:column", "Date") });
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
