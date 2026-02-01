#include <KPluginFactory>
#include <KFileItem>
#include <KPropertiesDialog>
#include <KLocalizedString>
#include <QFileInfo>

#include "SnapshotsPanePlugin.h"
#include "SnapshotsPane.h"
#include "SnapshotFinder.h"

SnapshotsPanePlugin::SnapshotsPanePlugin(QObject *parent) : KPropertiesDialogPlugin(parent)
{
    KPropertiesDialog *const propertiesDialog = qobject_cast<KPropertiesDialog *>(parent);
    assert(propertiesDialog != nullptr);
    if (!propertiesDialog) {
        return;
    }

    KFileItemList items = propertiesDialog->items();
    if (items.size() != 1) {
        return;
    }

    KFileItem item = items.first();
    if (!item.isLocalFile()) {
        return;
    }

    QString filePath = item.localPath();
    if (!SnapshotFinder::hasSnapshots(filePath)) {
        return;
    }

    SnapshotsPane *snapshotsPane = new SnapshotsPane(filePath, propertiesDialog);
    snapshotsPane->setObjectName(QStringLiteral("SnapshotsPane"));

    propertiesDialog->addPage(snapshotsPane, i18nc("SnapshotsPane", "Snapshots"));
}

K_PLUGIN_CLASS_WITH_JSON(SnapshotsPanePlugin, "kde-dolphin-snapshots-pane.json")

#include "SnapshotsPanePlugin.moc"
