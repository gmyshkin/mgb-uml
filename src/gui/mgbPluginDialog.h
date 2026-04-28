#ifndef MGBPLUGINDIALOG_H
#define MGBPLUGINDIALOG_H

#include <QDialog>
#include <QTableWidget>

class mgbPluginDialog : public QDialog {
    Q_OBJECT
public:
    explicit mgbPluginDialog(QWidget *parent = nullptr);

private slots:
    void refreshList();
    void togglePlugin();
    void uninstallPlugin();

private:
    QTableWidget *table;
};

#endif // MGBPLUGINDIALOG_H