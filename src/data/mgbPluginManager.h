#ifndef MGBPLUGINMANAGER_H
#define MGBPLUGINMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>

namespace mgb {

struct PluginElement {
    QString name;
    QString type; 
    QString category;
    QString iconPath;
    QString tooltip;
    QMap<QString, QString> properties;
};

class ElementPluginInterface; // Forward declaration

class PluginManager : public QObject {
    Q_OBJECT
public:
    static PluginManager& instance();
    
    void loadPlugins();
    bool loadCompiledPlugin(const QString &filePath, QString &errorMessage);
    
    QList<PluginElement> getLoadedPlugins(); // For the UI Menu
    QList<ElementPluginInterface*> getCompiledInterfaces(); // For Custom C++ Drawing

private:
    PluginManager() = default;
    QList<PluginElement> _plugins;
    QList<ElementPluginInterface*> _interfaces; // NEW: Tracks the loaded C++ code
    bool _isLoaded = false;
};

} // namespace mgb

#endif // MGBPLUGINMANAGER_H