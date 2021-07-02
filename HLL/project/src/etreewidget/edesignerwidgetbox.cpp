#include "edesigner_widgetbox_p.h"

namespace edesigner_internal {



EDesignerWidgetBox::EDesignerWidgetBox(QWidget *parent, Qt::WindowFlags flags)
    : EDesignerWidgetBoxInterface(parent, flags),
      m_loadMode(LoadMerge)
{

}

EDesignerWidgetBox::LoadMode EDesignerWidgetBox::loadMode() const
{
    return m_loadMode;
}

void EDesignerWidgetBox::setLoadMode(LoadMode lm)
{
    m_loadMode = lm;
}

// Convience to find a widget by class name
bool EDesignerWidgetBox::findWidget(const EDesignerWidgetBoxInterface *wbox,
                                    const QString &className,
                                    const QString &category,
                                    Widget *widgetData)
{
    // Note that entry names do not necessarily match the class name
    // (at least, not for the standart widgets), so,
    // look in the XML for the class name of the first widget to apper
    const QString widgetTag = QLatin1String("<widget");
    QString pattern = QLatin1String("^<widget\\s+class\\s*=\\s\"");
    pattern += className;
    pattern += QLatin1String("\".*$");
    const QRegExp regexp(pattern);
    Q_ASSERT(regexp.isValid());
    const int catCount = wbox->categoryCount();
    for (int c = 0; c < catCount; c++)
    {
        const Category cat = wbox->category(c);
        if (category.isEmpty() || cat.name() == category)
        {
            const int widgetCount = cat.widgetCount();
            for (int w = 0; w < widgetCount; w++)
            {
                const Widget widget = cat.widget(w);
                QString xml = widget.domXml(); // Erase the <ui> tag that can be present starting from 4.4
                const int widgetTagIndex = xml.indexOf(widgetTag);
                if ( widgetTagIndex != -1)
                {
                    xml.remove(0, widgetTagIndex);
                    if (regexp.exactMatch(xml))
                    {
                        *widgetData = widget;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Convenience to create a Dom Widget from widget box xml code.
//DomUI *QDesignerWidgetBox::xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel,
//                                   QString *errorMessage)
//{
//    QXmlStreamReader reader(xml);
//    DomUI *ui = 0;

//    // The xml description must either contain a root element "ui" with a child element "widget"
//    // or "widget" as the root element (4.3 legacy)
//    const QString widgetTag = QLatin1String("widget");

//    while (!reader.atEnd()) {
//        if (reader.readNext() == QXmlStreamReader::StartElement) {
//            const QStringRef name = reader.name();
//            if (ui) {
//                reader.raiseError(tr("Unexpected element <%1>").arg(name.toString()));
//                continue;
//            }

//            if (name.compare(QLatin1String("widget"), Qt::CaseInsensitive) == 0) { // 4.3 legacy, wrap into DomUI
//                ui = new DomUI;
//                DomWidget *widget = new DomWidget;
//                widget->read(reader);
//                ui->setElementWidget(widget);
//            } else if (name.compare(QLatin1String("ui"), Qt::CaseInsensitive) == 0) { // 4.4
//                ui = new DomUI;
//                ui->read(reader);
//            } else {
//                reader.raiseError(tr("Unexpected element <%1>").arg(name.toString()));
//            }
//        }
//   }

//    if (reader.hasError()) {
//        delete ui;
//        *errorMessage = tr("A parse error occurred at line %1, column %2 of the XML code "
//                           "specified for the widget %3: %4\n%5")
//                           .arg(reader.lineNumber()).arg(reader.columnNumber()).arg(name)
//                           .arg(reader.errorString()).arg(xml);
//        return 0;
//    }

//    if (!ui || !ui->elementWidget()) {
//        delete ui;
//        *errorMessage = tr("The XML code specified for the widget %1 does not contain "
//                           "any widget elements.\n%2").arg(name).arg(xml);
//        return 0;
//    }

//    if (insertFakeTopLevel)  {
//        DomWidget *fakeTopLevel = new DomWidget;
//        fakeTopLevel->setAttributeClass(QLatin1String("QWidget"));
//        QList<DomWidget *> children;
//        children.push_back(ui->takeElementWidget());
//        fakeTopLevel->setElementWidget(children);
//        ui->setElementWidget(fakeTopLevel);
//    }

//    return ui;
//}

//// Convenience to create a Dom Widget from widget box xml code.
//DomUI *QDesignerWidgetBox::xmlToUi(const QString &name, const QString &xml, bool insertFakeTopLevel)
//{
//    QString errorMessage;
//    DomUI *rc = xmlToUi(name, xml, insertFakeTopLevel, &errorMessage);
//    if (!rc)
//        qdesigner_internal::designerWarning(errorMessage);
//    return rc;
//}

} // namespace edesigner_internal
