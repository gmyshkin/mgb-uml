#include "mgbPluginDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>

mgbPluginDialog::mgbPluginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("MGB-UML Plugin Manager");
    resize(550, 350);

    // Set up the table
    table = new QTableWidget(0, 3, this);
    table->setHorizontalHeaderLabels({"Plugin File", "Type", "Status"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
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
        QString name = fi.fileName();
        
        // Only show valid plugin files
        if (!name.contains(".json") && !name.contains(".so") && !name.contains(".dll")) continue;
        if (name.endsWith(".svg") || name.endsWith(".png")) continue; // Hide icons from the list

        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(name));

        bool isDisabled = name.endsWith(".disabled");
        QString type = (name.contains(".json")) ? "JSON Configuration" : "Compiled C++ Extension";
        table->setItem(row, 1, new QTableWidgetItem(type));

        QTableWidgetItem *statusItem = new QTableWidgetItem(isDisabled ? "Disabled" : "Active");
        statusItem->setForeground(isDisabled ? Qt::red : Qt::darkGreen);
        table->setItem(row, 2, statusItem);
    }
}

void mgbPluginDialog::togglePlugin() {
    int row = table->currentRow();
    if (row < 0) return;
    
    QString name = table->item(row, 0)->text();
    QString path = QCoreApplication::applicationDirPath() + "/plugins/" + name;
    QString newPath;
    
    // Toggling works by appending or removing ".disabled" from the filename
    // Our PluginManager automatically ignores anything that doesn't end strictly in .json, .so, or .dll!
    if (name.endsWith(".disabled")) {
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
    
    QString name = table->item(row, 0)->text();
    if (QMessageBox::question(this, "Confirm Uninstall", "Are you sure you want to permanently delete '" + name + "'?") == QMessageBox::Yes) {
        QFile::remove(QCoreApplication::applicationDirPath() + "/plugins/" + name);
        refreshList();
    }
}