#ifndef CONTAINER_H
#define CONTAINER_H

#include "extension.h"
#include <QObject>
class QWidget;

class EDesignerContainerExtension
{
public:
    virtual ~EDesignerContainerExtension() {}

    virtual int count() const = 0;
    virtual QWidget *widget(int index) const = 0;

    virtual int currentIndex() const = 0;
    virtual void setCurrentIndex(int index) = 0;

    virtual void addWidget(QWidget *widget) = 0;
    virtual void insertWidget(int index, QWidget *widget) = 0;
    virtual void remove(int index) = 0;
};
Q_DECLARE_EXTENSION_INTERFACE(EDesignerContainerExtension, "com.trolltech.Qt.Designer.Container")

#endif // CONTAINER_H
