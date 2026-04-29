#include "mgbUmlClassPlugin.h"
#include "../../src/data/node.h"
#include "../../src/data/style.h"
#include "../../src/util.h"

#include <QFont>
#include <QFontMetrics>
#include <QTextStream>
#include <algorithm>

namespace {

static constexpr qreal UML_SCALEF = 40.0;
const QFont MGB_LABEL_FONT("Helvetica", 10);

QString propertyWithStyleDefault(Node *node, const QString &key, const QString &fallback)
{
    QString value = node->data()->property(key);
    if (value.isEmpty() && node->style() && node->style()->data()) {
        value = node->style()->data()->property(key);
    }
    return value.isEmpty() ? fallback : value;
}

int umlLengthToPixels(QString raw, int fallbackPx)
{
    raw = raw.trimmed();
    if (raw.isEmpty()) return fallbackPx;

    bool ok = false;
    if (raw.endsWith("cm")) {
        raw.chop(2);
        double cm = raw.toDouble(&ok);
        if (ok) return static_cast<int>(cm * UML_SCALEF);
    }

    if (raw.endsWith("pt")) {
        raw.chop(2);
        double pt = raw.toDouble(&ok);
        if (ok) return static_cast<int>(pt * 1.3333);
    }

    double plain = raw.toDouble(&ok);
    if (ok) return static_cast<int>(plain);
    return fallbackPx;
}

QString cm(qreal value)
{
    return floatToString(value) + "cm";
}

QString nodeDrawOptions(Node *node, qreal widthCm, qreal heightCm)
{
    QStringList options;
    options << "draw=" + propertyWithStyleDefault(node, "draw", "black");
    options << "fill=" + propertyWithStyleDefault(node, "fill", "white");
    options << "line width=" + propertyWithStyleDefault(node, "line width", "0.6pt");
    options << "minimum width=" + cm(widthCm);
    options << "minimum height=" + cm(heightCm);
    options << "inner sep=0pt";
    options << "outer sep=0pt";
    options << "shape=rectangle";
    return "[" + options.join(", ") + "]";
}

QStringList umlClassParts(const QString &label)
{
    QString part1 = label;
    QString part2;
    QString part3;

    int idx2 = label.indexOf("\\nodepart{two}");
    int idx3 = label.indexOf("\\nodepart{three}");

    if (idx2 != -1) {
        part1 = label.left(idx2).trimmed();
        if (idx3 != -1) {
            part2 = label.mid(idx2 + 14, idx3 - (idx2 + 14)).trimmed();
            part3 = label.mid(idx3 + 16).trimmed();
        } else {
            part2 = label.mid(idx2 + 14).trimmed();
        }
    }

    return {part1, part2, part3};
}

QString printPart(QString value)
{
    return replaceTexConstants(value).replace("\\\\", "\n").replace(" \\ ", "\n");
}

QString tikzPart(QString value)
{
    return value.replace("\n", "\\\\");
}

}

namespace mgb {

QString UmlClassPlugin::pluginName() const { 
    return "UML Class Element"; 
}

QList<PluginElement> UmlClassPlugin::getElements() const {
    PluginElement e;
    e.name = "UML Class";
    e.type = "node";
    e.category = "UML Elements";
    e.tooltip = "Drag to add a Class";
    e.iconPath = ":/icons/class_icon.png";
    
    e.properties.insert("shape", "rectangle split");
    e.properties.insert("tikzit edge shape", "rectangle");
    e.properties.insert("rectangle split parts", "3");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("align", "left");
    e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");
    e.properties.insert("inner sep", "0pt");
    e.properties.insert("outer sep", "0pt");
    e.properties.insert("rectangle split part align", "{center,left,left}");
    e.properties.insert("default_label",
    "Class \\nodepart{two} + attr \\nodepart{three} + method()");
    e.properties.insert("minimum width", "3cm");
    e.properties.insert("minimum height", "2cm");
    
    // Guarantee that LaTeX includes the multipart library
    e.properties.insert("tikz_libraries", "shapes.multipart");
    
    return {e};
}

QIcon UmlClassPlugin::pluginIcon() const {
    return QIcon(":/icons/class_icon.png");
}

NodeItem* UmlClassPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "rectangle split" || node->styleName() == "UML Class") {
        return new ::ClassNodeItem(node);
    }
    return nullptr;
}

bool UmlClassPlugin::writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const {
    if (node->styleName() != "UML Class") {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    QStringList parts = umlClassParts(node->label());
    QString x = floatToString(node->point().x());
    QString y = floatToString(node->point().y());
    QString stroke = propertyWithStyleDefault(node, "draw", "black");
    QString lineWidth = propertyWithStyleDefault(node, "line width", "0.6pt");
    QString font = propertyWithStyleDefault(node, "font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    QString printPart1 = printPart(parts.value(0));
    QString printPart2 = printPart(parts.value(1));
    QString printPart3 = printPart(parts.value(2));

    QFont titleFont = MGB_LABEL_FONT;
    titleFont.setBold(true);
    QFontMetrics fmTitle(titleFont);
    QFontMetrics fm(MGB_LABEL_FONT);
    int padding = 10;

    QRect b1 = fmTitle.boundingRect(QRect(0, 0, 1000, 1000), Qt::AlignCenter, printPart1);
    QRect b2 = fm.boundingRect(QRect(0, 0, 1000, 1000), Qt::AlignLeft, printPart2);
    QRect b3 = fm.boundingRect(QRect(0, 0, 1000, 1000), Qt::AlignLeft, printPart3);

    int minW = umlLengthToPixels(propertyWithStyleDefault(node, "minimum width", "3cm"), 120);
    int minH = umlLengthToPixels(propertyWithStyleDefault(node, "minimum height", "2cm"), 80);
    int widthPx = std::max(minW, std::max({b1.width(), b2.width(), b3.width(), 60}) + padding * 2);
    int heightPx = std::max(minH, std::max(b1.height() + b2.height() + b3.height() + padding * 3, 40));

    qreal widthCm = widthPx / UML_SCALEF;
    qreal heightCm = heightPx / UML_SCALEF;
    qreal divider1 = (b1.height() + padding) / UML_SCALEF;
    qreal divider2 = (b1.height() + b2.height() + padding * 2) / UML_SCALEF;
    qreal titleOffset = (padding / 2.0) / UML_SCALEF;
    qreal body1Offset = (b1.height() + padding + padding / 2.0) / UML_SCALEF;
    qreal body2Offset = (b1.height() + b2.height() + padding * 2 + padding / 2.0) / UML_SCALEF;

    code << "\t\t\\node " << nodeDrawOptions(node, widthCm, heightCm)
         << " (" << node->name() << ") at (" << x << ", " << y << ") {};\n";
    code << "\t\t\\draw [draw=" << stroke << ", line width=" << lineWidth << "] "
         << "([yshift=-" << cm(divider1) << "]" << node->name() << ".north west) -- "
         << "([yshift=-" << cm(divider1) << "]" << node->name() << ".north east);\n";
    code << "\t\t\\draw [draw=" << stroke << ", line width=" << lineWidth << "] "
         << "([yshift=-" << cm(divider2) << "]" << node->name() << ".north west) -- "
         << "([yshift=-" << cm(divider2) << "]" << node->name() << ".north east);\n";
    code << "\t\t\\node [anchor=north, align=center, inner sep=0pt, outer sep=0pt, font={\\bfseries"
         << font << "}] at ([yshift=-" << cm(titleOffset) << "]" << node->name()
         << ".north) {" << tikzPart(parts.value(0)) << "};\n";
    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, font={"
         << font << "}] at ([xshift=0.12cm,yshift=-" << cm(body1Offset) << "]" << node->name()
         << ".north west) {" << tikzPart(parts.value(1)) << "};\n";
    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, font={"
         << font << "}] at ([xshift=0.12cm,yshift=-" << cm(body2Offset) << "]" << node->name()
         << ".north west) {" << tikzPart(parts.value(2)) << "};\n";

    if (emittedLines) *emittedLines = 6;
    return true;
}

} // namespace mgb
