#include "stylelist.h"

#include <QTextStream>

StyleList::StyleList(bool edgeStyles, QObject *parent) : QAbstractListModel(parent), _edgeStyles(edgeStyles)
{
    if (edgeStyles) {
        _styles << noneEdgeStyle;
    } else {
        _styles << noneStyle;
    }
}

Style *StyleList::style(QString name)
{
    foreach (Style *s, _styles)
        if (s->name() == name) return s;
    return nullptr;
}

Style *StyleList::style(int i)
{
    return _styles[i];
}

int StyleList::length() const
{
    return _styles.length();
}

void StyleList::addStyle(Style *s)
{
    s->setParent(this);
    if (s->category() == _category) {
        int n = numInCategory();
        beginInsertRows(QModelIndex(), n, n);
        _styles << s;
        endInsertRows();
    } else {
        _styles << s;
    }
}

void StyleList::removeNthStyle(int n)
{
    beginRemoveRows(QModelIndex(), n, n);
    _styles.remove(nthInCategory(n));
    endRemoveRows();
}

void StyleList::clear()
{
    int n = numInCategory();
    if (n > 1) {
        beginRemoveRows(QModelIndex(), 1, n - 1);
        _styles.clear();
        if (_edgeStyles) _styles << noneEdgeStyle;
        else _styles << noneStyle;
        endRemoveRows();
    } else {
        _styles.clear();
        if (_edgeStyles) _styles << noneEdgeStyle;
        else _styles << noneStyle;
    }

    _category = "";
}

QString StyleList::tikz()
{
    QString str;
    QTextStream code(&str);
    for (int i = 1; i < _styles.length(); ++i)
        code << _styles[i]->tikz() << "\n";
    code.flush();
    return str;
}

int StyleList::numInCategory() const
{
    int c = 0;
    foreach (Style *s, _styles) {
        // ==============================================================
        // MGB-UML: STRICT FILTERING
        // Removed the forced 's->isNone()' override. 
        // If _category == "", it naturally shows everything.
        // Otherwise, it strictly matches the category, blocking the blank node!
        // ==============================================================
        if (_category == "" || s->category() == _category) {
            ++c;
        }
    }
    return c;
}

int StyleList::nthInCategory(int n) const
{
    int c = 0;
    for (int j = 0; j < _styles.length(); ++j) {
        // ==============================================================
        // MGB-UML: STRICT FILTERING
        // Removed the forced '_styles[j]->isNone()' override.
        // ==============================================================
        if (_category == "" || _styles[j]->category() == _category) {
            if (c == n) return j;
            else ++c;
        }
    }
    return -1;
}

Style *StyleList::styleInCategory(int n) const
{
    return _styles[nthInCategory(n)];
}

QVariant StyleList::data(const QModelIndex &index, int role) const
{
    Style *s = styleInCategory(index.row());

    if (role == Qt::DisplayRole) {
        if (_edgeStyles && s->name() == "none") {
            return QVariant("Association");
        }
        return QVariant(s->name());
    } else if (role == Qt::DecorationRole) {
        return QVariant(s->icon());
    } else {
        return QVariant();
    }
}

int StyleList::rowCount(const QModelIndex &/*parent*/) const
{
    return numInCategory();
}

bool StyleList::moveRows(const QModelIndex &sourceParent,
                                int sourceRow,
                                int /*count*/,
                                const QModelIndex &destinationParent,
                                int destinationRow)
{
    if (sourceRow >= 1 && sourceRow < numInCategory() &&
        destinationRow >= 1 && destinationRow <= numInCategory())
    {
        beginMoveRows(sourceParent, sourceRow, sourceRow, destinationParent, destinationRow);
        int sourceIndex = nthInCategory(sourceRow);
        int destinationIndex = nthInCategory(destinationRow);
        if (destinationIndex == -1)
            destinationIndex = _styles.length();
        Style *s = _styles[sourceIndex];
        _styles.remove(sourceIndex);
        if (sourceIndex < destinationIndex) {
            _styles.insert(destinationIndex - 1, s);
        } else {
            _styles.insert(destinationIndex, s);
        }
        endMoveRows();
        return true;
    } else {
        return false;
    }
}

QString StyleList::category() const
{
    return _category;
}

void StyleList::setCategory(const QString &category)
{
    if (category != _category) {
        beginResetModel();
        _category = category;
        endResetModel();
    }
}
