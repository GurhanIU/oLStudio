#include "formbuilderextra_p.h"
#include "abstractformbuilder.h"
#include "resourcebuilder_p.h"
#include "textbuilder_p.h"
#include "ui4_p.h"

#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>

#include <QVariant>
#include <QDebug>
#include <QTextStream>
#include <QStringList>
#include <QCoreApplication>



#ifdef QFORMINTERNAL_NAMESPACE
namespace QFormInternal {
#endif

void uiLibWarning(const QString &message) {
    qWarning("Designer: %s", qPrintable(message));
}


EFormBuilderExtra::CustomWidgetData::CustomWidgetData() :
    isContainer(false)
{
}

EFormBuilderExtra::CustomWidgetData::CustomWidgetData(const DomCustomWidget *dcw) :
    addPageMethod(dcw->elementAddPageMethod()),
    baseClass(dcw->elementExtends()),
    isContainer(dcw->hasElementContainer() && dcw->elementContainer() != 0)
{
#ifndef QT_FORMBUILDER_NO_SCRIPT
    if (const DomScript *domScript = dcw->elementScript())
        script = domScript->text();
#endif
}

EFormBuilderExtra::EFormBuilderExtra() :
    m_layoutWidget(false),
    m_resourceBuilder(0),
    m_textBuilder(0)
{
}

EFormBuilderExtra::~EFormBuilderExtra()
{
    clearResourceBuilder();
    clearTextBuilder();
}

void EFormBuilderExtra::clear()
{
    m_buddies.clear();
    m_parentWidget = 0;
    m_parentWidgetIsSet = false;
#ifndef QT_FORMBUILDER_NO_SCRIPT
    m_FormScriptRunner.clearErrors();
#endif
    m_customWidgetDataHash.clear();
    m_buttonGroups.clear();
}


bool EFormBuilderExtra::applyPropertyInternally(QObject *o, const QString &propertyName, const QVariant &value)
{
    // Store buddies and apply them later on as the widgets might not exist yet.
    QLabel *label = qobject_cast<QLabel*>(o);
    if (!label || propertyName != EFormBuilderStrings::instance().buddyProperty)
        return false;

    m_buddies.insert(label, value.toString());
    return true;
}

void EFormBuilderExtra::applyInternalProperties() const
{
    if (m_buddies.empty())
        return;

    const BuddyHash::const_iterator cend = m_buddies.constEnd();
    for (BuddyHash::const_iterator it = m_buddies.constBegin(); it != cend; ++it )
        applyBuddy(it.value(), BuddyApplyAll, it.key());
}

bool EFormBuilderExtra::applyBuddy(const QString &buddyName, BuddyMode applyMode, QLabel *label)
{
    if (buddyName.isEmpty()) {
        label->setBuddy(0);
        return false;
    }

    const QWidgetList widgets = label->topLevelWidget()->findChildren<QWidget*>(buddyName);
    if (widgets.empty()) {
        label->setBuddy(0);
        return false;
    }

    const QWidgetList::const_iterator cend = widgets.constEnd();
    for ( QWidgetList::const_iterator it =  widgets.constBegin(); it !=  cend; ++it) {
        if (applyMode == BuddyApplyAll || !(*it)->isHidden()) {
            label->setBuddy(*it);
            return true;
        }
    }

    label->setBuddy(0);
    return false;
}

const QPointer<QWidget> &EFormBuilderExtra::parentWidget() const
{
    return m_parentWidget;
}

bool EFormBuilderExtra::parentWidgetIsSet() const
{
    return m_parentWidgetIsSet;
}

void EFormBuilderExtra::setParentWidget(const QPointer<QWidget> &w)
{
    // Parent widget requires special handling of the geometry property.
    m_parentWidget = w;
    m_parentWidgetIsSet = true;
}

//#ifndef QT_FORMBUILDER_NO_SCRIPT
EFormScriptRunner &EFormBuilderExtra::formScriptRunner()
{
    return m_FormScriptRunner;
}

QString EFormBuilderExtra::customWidgetScript(const QString &className) const
{
    const QHash<QString, CustomWidgetData>::const_iterator it = m_customWidgetDataHash.constFind(className);
    if (it != m_customWidgetDataHash.constEnd())
        return it.value().script;
    return QString();
}

//#endif

void EFormBuilderExtra::storeCustomWidgetData(const QString &className, const DomCustomWidget *d)
{
    if (d)
        m_customWidgetDataHash.insert(className, CustomWidgetData(d));
}

QString EFormBuilderExtra::customWidgetBaseClass(const QString &className) const
{
    const QHash<QString, CustomWidgetData>::const_iterator it = m_customWidgetDataHash.constFind(className);
    if (it != m_customWidgetDataHash.constEnd())
            return it.value().baseClass;
    return QString();
}

QString EFormBuilderExtra::customWidgetAddPageMethod(const QString &className) const
{
    const QHash<QString, CustomWidgetData>::const_iterator it = m_customWidgetDataHash.constFind(className);
    if (it != m_customWidgetDataHash.constEnd())
        return it.value().addPageMethod;
    return QString();
}

bool EFormBuilderExtra::isCustomWidgetContainer(const QString &className) const
{
    const QHash<QString, CustomWidgetData>::const_iterator it = m_customWidgetDataHash.constFind(className);
    if (it != m_customWidgetDataHash.constEnd())
        return it.value().isContainer;
    return false;
}

namespace {
    typedef QHash<const EAbstractFormBuilder *, EFormBuilderExtra *> FormBuilderPrivateHash;
}

Q_GLOBAL_STATIC(FormBuilderPrivateHash, g_FormBuilderPrivateHash)

EFormBuilderExtra *EFormBuilderExtra::instance(const EAbstractFormBuilder *afb)
{
    FormBuilderPrivateHash &fbHash = *g_FormBuilderPrivateHash();

    FormBuilderPrivateHash::iterator it = fbHash.find(afb);
    if (it == fbHash.end())
        it = fbHash.insert(afb, new EFormBuilderExtra);
    return it.value();
}

void EFormBuilderExtra::removeInstance(const EAbstractFormBuilder *afb)
{
    FormBuilderPrivateHash &fbHash = *g_FormBuilderPrivateHash();

    FormBuilderPrivateHash::iterator it = fbHash.find(afb);
    if (it != fbHash.end()) {
        delete it.value();
        fbHash.erase(it);
    }
}

void EFormBuilderExtra::setProcessingLayoutWidget(bool processing)
{
    m_layoutWidget = processing;
}

 bool EFormBuilderExtra::processingLayoutWidget() const
{
    return m_layoutWidget;
}
void EFormBuilderExtra::setResourceBuilder(EResourceBuilder *builder)
{
    if (m_resourceBuilder == builder)
        return;
    clearResourceBuilder();
    m_resourceBuilder = builder;
}

EResourceBuilder *EFormBuilderExtra::resourceBuilder() const
{
    return m_resourceBuilder;
}

void EFormBuilderExtra::clearResourceBuilder()
{
    if (m_resourceBuilder) {
        delete m_resourceBuilder;
        m_resourceBuilder = 0;
    }
}

void EFormBuilderExtra::setTextBuilder(ETextBuilder *builder)
{
    if (m_textBuilder == builder)
        return;
    clearTextBuilder();
    m_textBuilder = builder;
}

ETextBuilder *EFormBuilderExtra::textBuilder() const
{
    return m_textBuilder;
}

void EFormBuilderExtra::clearTextBuilder()
{
    if (m_textBuilder) {
        delete m_textBuilder;
        m_textBuilder = 0;
    }
}

void EFormBuilderExtra::registerButtonGroups(const DomButtonGroups *domGroups)
{
    typedef QList<DomButtonGroup*> DomButtonGroupList;
    const DomButtonGroupList domGroupList = domGroups->elementButtonGroup();
    const DomButtonGroupList::const_iterator cend = domGroupList.constEnd();
    for (DomButtonGroupList::const_iterator it = domGroupList.constBegin(); it != cend; ++it)
    {
        DomButtonGroup *domGroup = *it;
        m_buttonGroups.insert(domGroup->attributeName(), ButtonGroupEntry(domGroup, 0));
    }
}

// Utilities for parsing per-cell integer properties that have setters and
//  getters of the form 'setX(int idx, int value)' and 'x(int index)'
// (converting them to comma-separated string lists and back).
// Used for layout stretch and grid per-row/column properties.

// Format a list of cell-properties of one dimension as a ','-separated list
template <class Layout>
inline QString perCellPropertyToString(const Layout *l, int count, int (Layout::*getter)(int) const)
{
    if (count == 0)
        return QString();
    QString rc;
    {
        QTextStream str(&rc);
        for (int i = 0; i < count; i++) {
            if (i)
                str << QLatin1Char(',');
            str << (l->*getter)(i);
        }
    }
    return rc;
}

// Clear the property, set all cells to 0

template <class Layout>
inline void clearPerCellValue(Layout *l, int count, void (Layout::*setter)(int,int), int value = 0)
{
    for (int i = 0; i < count; i++)
        (l->*setter)(i, value);
}

// Parse and set the property from a comma-separated list

template <class Layout>
inline bool parsePerCellProperty(Layout *l, int count, void (Layout::*setter)(int,int), const QString &s, int defaultValue = 0)
{
    if (s.isEmpty()) {
        clearPerCellValue(l, count, setter, defaultValue);
        return true;
    }
    const QStringList list = s.split(QLatin1Char(','));
    if (list.empty()) {
        clearPerCellValue(l, count, setter, defaultValue);
        return true;
    }
    // Apply all values contained in list
    const int ac = qMin(count, list.size());
    bool ok;
    int i = 0;
    for ( ; i < ac; i++) {
        const int value = list.at(i).toInt(&ok);
        if (!ok || value < 0)
            return false;
        (l->*setter)(i, value);
    }
    // Clear rest
    for ( ; i < count; i++)
        (l->*setter)(i, defaultValue);
    return true;
}

// Read and write stretch
static QString msgInvalidStretch(const QString &objectName, const QString &stretch)
{
    //: Parsing layout stretch values
    return QCoreApplication::translate("FormBuilder", "Invalid stretch value for '%1': '%2'").arg(objectName, stretch);
}

QString EFormBuilderExtra::boxLayoutStretch(const QBoxLayout *box)
{
     return perCellPropertyToString(box, box->count(), &QBoxLayout::stretch);
}

bool EFormBuilderExtra::setBoxLayoutStretch(const QString &s, QBoxLayout *box)
{
    const bool rc = parsePerCellProperty(box, box->count(), &QBoxLayout::setStretch, s);
    if (!rc)
        uiLibWarning(msgInvalidStretch(box->objectName(), s));
    return rc;
}

void EFormBuilderExtra::clearBoxLayoutStretch(QBoxLayout *box)
{
    clearPerCellValue(box, box->count(), &QBoxLayout::setStretch);
}

QString EFormBuilderExtra::gridLayoutRowStretch(const QGridLayout *grid)
{
    return perCellPropertyToString(grid, grid->rowCount(), &QGridLayout::rowStretch);
}

bool EFormBuilderExtra::setGridLayoutRowStretch(const QString &s, QGridLayout *grid)
{
    const bool rc = parsePerCellProperty(grid, grid->rowCount(), &QGridLayout::setRowStretch, s);
    if (!rc)
        uiLibWarning(msgInvalidStretch(grid->objectName(), s));
    return rc;
}

void EFormBuilderExtra::clearGridLayoutRowStretch(QGridLayout *grid)
{
    clearPerCellValue(grid, grid->rowCount(), &QGridLayout::setRowStretch);
}

QString EFormBuilderExtra::gridLayoutColumnStretch(const QGridLayout *grid)
{
    return perCellPropertyToString(grid, grid->columnCount(), &QGridLayout::columnStretch);
}

bool EFormBuilderExtra::setGridLayoutColumnStretch(const QString &s, QGridLayout *grid)
{
    const bool rc = parsePerCellProperty(grid, grid->columnCount(), &QGridLayout::setColumnStretch, s);
    if (!rc)
        uiLibWarning(msgInvalidStretch(grid->objectName(), s));
    return rc;
}

void EFormBuilderExtra::clearGridLayoutColumnStretch(QGridLayout *grid)
{
    clearPerCellValue(grid, grid->columnCount(), &QGridLayout::setColumnStretch);
}

// Read and write grid layout row/column size limits

static QString msgInvalidMinimumSize(const QString &objectName, const QString &ms)
{
    //: Parsing grid layout minimum size values
    return QCoreApplication::translate("FormBuilder", "Invalid minimum size for '%1': '%2'").arg(objectName, ms);
}

QString EFormBuilderExtra::gridLayoutRowMinimumHeight(const QGridLayout *grid)
{
    return perCellPropertyToString(grid, grid->rowCount(), &QGridLayout::rowMinimumHeight);
}

bool EFormBuilderExtra::setGridLayoutRowMinimumHeight(const QString &s, QGridLayout *grid)
{
    const bool rc = parsePerCellProperty(grid, grid->rowCount(), &QGridLayout::setRowMinimumHeight, s);
    if (!rc)
        uiLibWarning(msgInvalidMinimumSize(grid->objectName(), s));
    return rc;
}

void EFormBuilderExtra::clearGridLayoutRowMinimumHeight(QGridLayout *grid)
{
     clearPerCellValue(grid, grid->rowCount(), &QGridLayout::setRowMinimumHeight);
}

QString EFormBuilderExtra::gridLayoutColumnMinimumWidth(const QGridLayout *grid)
{
    return perCellPropertyToString(grid, grid->columnCount(), &QGridLayout::columnMinimumWidth);
}

bool EFormBuilderExtra::setGridLayoutColumnMinimumWidth(const QString &s, QGridLayout *grid)
{
    const bool rc = parsePerCellProperty(grid, grid->columnCount(), &QGridLayout::setColumnMinimumWidth, s);
    if (!rc)
        uiLibWarning(msgInvalidMinimumSize(grid->objectName(), s));
    return rc;
}

void EFormBuilderExtra::clearGridLayoutColumnMinimumWidth(QGridLayout *grid)
{
    clearPerCellValue(grid, grid->columnCount(), &QGridLayout::setColumnMinimumWidth);
}

// ------------ EFormBuilderStrings

EFormBuilderStrings::EFormBuilderStrings() :
    buddyProperty(QLatin1String("buddy")),
    cursorProperty(QLatin1String("cursor")),
    objectNameProperty(QLatin1String("objectName")),
    trueValue(QLatin1String("true")),
    falseValue(QLatin1String("false")),
    horizontalPostFix(QLatin1String("Horizontal")),
    separator(QLatin1String("separator")),
    defaultTitle(QLatin1String("Page")),
    titleAttribute(QLatin1String("title")),
    labelAttribute(QLatin1String("label")),
    toolTipAttribute(QLatin1String("toolTip")),
    whatsThisAttribute(QLatin1String("whatsThis")),
    flagsAttribute(QLatin1String("flags")),
    iconAttribute(QLatin1String("icon")),
    pixmapAttribute(QLatin1String("pixmap")),
    textAttribute(QLatin1String("text")),
    currentIndexProperty(QLatin1String("currentIndex")),
    toolBarAreaAttribute(QLatin1String("toolBarArea")),
    toolBarBreakAttribute(QLatin1String("toolBarBreak")),
    dockWidgetAreaAttribute(QLatin1String("dockWidgetArea")),
    marginProperty(QLatin1String("margin")),
    spacingProperty(QLatin1String("spacing")),
    leftMarginProperty(QLatin1String("leftMargin")),
    topMarginProperty(QLatin1String("topMargin")),
    rightMarginProperty(QLatin1String("rightMargin")),
    bottomMarginProperty(QLatin1String("bottomMargin")),
    horizontalSpacingProperty(QLatin1String("horizontalSpacing")),
    verticalSpacingProperty(QLatin1String("verticalSpacing")),
    sizeHintProperty(QLatin1String("sizeHint")),
    sizeTypeProperty(QLatin1String("sizeType")),
    orientationProperty(QLatin1String("orientation")),
    styleSheetProperty(QLatin1String("styleSheet")),
    qtHorizontal(QLatin1String("Qt::Horizontal")),
    qtVertical(QLatin1String("Qt::Vertical")),
    currentRowProperty(QLatin1String("currentRow")),
    tabSpacingProperty(QLatin1String("tabSpacing")),
    qWidgetClass(QLatin1String("QWidget")),
    lineClass(QLatin1String("Line")),
    geometryProperty(QLatin1String("geometry")),
    scriptWidgetVariable(QLatin1String("widget")),
    scriptChildWidgetsVariable(QLatin1String("childWidgets"))
{
    itemRoles.append(qMakePair(Qt::FontRole, QString::fromLatin1("font")));
    itemRoles.append(qMakePair(Qt::TextAlignmentRole, QString::fromLatin1("textAlignment")));
    itemRoles.append(qMakePair(Qt::BackgroundRole, QString::fromLatin1("background")));
    itemRoles.append(qMakePair(Qt::ForegroundRole, QString::fromLatin1("foreground")));
    itemRoles.append(qMakePair(Qt::CheckStateRole, QString::fromLatin1("checkState")));

    foreach (const RoleNName &it, itemRoles)
        treeItemRoleHash.insert(it.second, it.first);

    itemTextRoles.append(qMakePair(qMakePair(Qt::EditRole, Qt::DisplayPropertyRole),
                                   textAttribute)); // This must be first for the loop below
    itemTextRoles.append(qMakePair(qMakePair(Qt::ToolTipRole, Qt::ToolTipPropertyRole),
                                   toolTipAttribute));
    itemTextRoles.append(qMakePair(qMakePair(Qt::StatusTipRole, Qt::StatusTipPropertyRole),
                                   QString::fromLatin1("statusTip")));
    itemTextRoles.append(qMakePair(qMakePair(Qt::WhatsThisRole, Qt::WhatsThisPropertyRole),
                                   whatsThisAttribute));

    // Note: this skips the first item!
    QList<TextRoleNName>::const_iterator it = itemTextRoles.constBegin(), end = itemTextRoles.constEnd();
    while (++it != end)
        treeItemTextRoleHash.insert(it->second, it->first);
}

const EFormBuilderStrings &EFormBuilderStrings::instance()
{
    static const EFormBuilderStrings rc;
    return rc;
}

#ifdef QFORMINTERNAL_NAMESPACE
} // namespace QFormInternal
#endif
