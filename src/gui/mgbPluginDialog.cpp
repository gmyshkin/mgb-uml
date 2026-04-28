#include "mgbPluginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>
#include <QPluginLoader>
#include <QJsonObject>
#include <QJsonValue>

mgbPluginDialog::mgbPluginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("MGB-UML Plugin Manager");
    resize(700, 400); // Made it a bit wider to fit the descriptions

    // Set up the table with new Metadata columns
    table = new QTableWidget(0, 4, this);
    table->setHorizontalHeaderLabels({"Plugin Name", "Version", "Description", "Status"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    // Set up buttons
    QPushButton *btnToggle = new QPushButton("Enable / Disable");
    QPushButton *btnUninstall = new QPushButton("Uninstall");
    QPushButton *btnClose = new QPushButton("Close");

    connect(btnToggle, &QPushButton::clicked, this, &mgbPluginDialog::togglePlugin);
    connect(btnUninstall, &QPushButton::clicked, this, &mgbPluginDialog::uninstallPlugin);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(btnToggle);
    btnLayout->addWidget(btnUninstall);
    btnLayout->addStretch();
    btnLayout->addWidget(btnClose);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(table);
    mainLayout->addLayout(btnLayout);

    refreshList();
}

void mgbPluginDialog::refreshList() {
    table->setRowCount(0);
    QDir dir(QCoreApplication::applicationDirPath() + "/plugins");
    if (!dir.exists()) return;

    QFileInfoList list = dir.entryInfoList(QDir::Files);
    for (const QFileInfo &fi : list) {
        QString rawFilename = fi.fileName();
        
        // Only show valid plugin files (and .disabled files)
        if (!rawFilename.contains(".json") && !rawFilename.contains(".so") && !rawFilename.contains(".dll")) continue;
        if (rawFilename.endsWith(".svg") || rawFilename.endsWith(".png")) continue;

        // Default fallback values
        QString displayName = rawFilename;
        QString version = "1.0";
        QString desc = "Compiled C++ Extension";
        bool isDisabled = rawFilename.endsWith(".disabled");

        // --- X-RAY SCANNER: Extract JSON Metadata ---
        // We only scan it if it is an actual compiled binary, even if disabled
        if (rawFilename.contains(".so") || rawFilename.contains(".dll") || rawFilename.contains(".dylib")) {
            QPluginLoader loader(fi.absoluteFilePath());
            QJsonObject fullMetaData = loader.metaData();
            
            if (fullMetaData.contains("MetaData")) {
                QJsonObject customMeta = fullMetaData.value("MetaData").toObject();
                
                if (customMeta.contains("name")) displayName = customMeta.value("name").toString();
                if (customMeta.contains("version")) version = customMeta.value("version").toString();
                if (customMeta.contains("description")) desc = customMeta.value("description").toString();
            }
        } else if (rawFilename.contains(".json")) {
            desc = "Standalone JSON Configuration";
        }

        // --- POPULATE THE UI TABLE ---
        int row = table->rowCount();
        table->insertRow(row);

        // Column 0: Name (Hide the raw filename inside this item so buttons still work!)
        QTableWidgetItem *nameItem = new QTableWidgetItem(displayName);
        nameItem->setData(Qt::UserRole, rawFilename); 
        table->setItem(row, 0, nameItem);

        // Column 1: Version
        table->setItem(row, 1, new QTableWidgetItem(version));

        // Column 2: Description
        table->setItem(row, 2, new QTableWidgetItem(desc));

        // Column 3: Status
        QTableWidgetItem *statusItem = new QTableWidgetItem(isDisabled ? "Disabled" : "Active");
        statusItem->setForeground(isDisabled ? Qt::red : Qt::darkGreen);
        table->setItem(row, 3, statusItem);
    }
}

void mgbPluginDialog::togglePlugin() {
    int row = table->currentRow();
    if (row < 0) return;
    
    // Retrieve the hidden raw filename we stashed in UserRole!
    QString rawName = table->item(row, 0)->data(Qt::UserRole).toString();
    QString path = QCoreApplication::applicationDirPath() + "/plugins/" + rawName;
    QString newPath;
    
    if (rawName.endsWith(".disabled")) {
        newPath = path.left(path.length() - 9); 
    } else {
        newPath = path + ".disabled";
    }
    
    QFile::rename(path, newPath);
    refreshList();
    QMessageBox::information(this, "Restart Required", "You must restart MGB-UML for these changes to take effect.");
}

void mgbPluginDialog::uninstallPlugin() {
    int row = table->currentRow();
    if (row < 0) return;
    
    // Retrieve the hidden raw filename we stashed in UserRole!
    QString rawName = table->item(row, 0)->data(Qt::UserRole).toString();
    QString displayName = table->item(row, 0)->text();

    if (QMessageBox::question(this, "Confirm Uninstall", "Are you sure you want to permanently delete '" + displayName + "'?") == QMessageBox::Yes) {
        QFile::remove(QCoreApplication::applicationDirPath() + "/plugins/" + rawName);
        refreshList();
    }
}