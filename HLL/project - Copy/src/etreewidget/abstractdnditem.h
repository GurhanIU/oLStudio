#ifndef ABSTRACTDNDITEM_H
#define ABSTRACTDNDITEM_H

//class DomUI;
class QWidget;
class QPoint;

class EDesignerDnDItemInterface
{
public:
    enum DropType { MoveDrop, CopyDrop };

    EDesignerDnDItemInterface() {}
    virtual ~EDesignerDnDItemInterface() {}

//    virtual DomUI *domUi() const = 0;
    virtual QWidget *widget() const = 0;
    virtual QWidget *decoration() const = 0;
    virtual QPoint hotSpot() const = 0;
    virtual DropType type() const = 0;
    virtual QWidget *source() const = 0;
};


#endif // ABSTRACTDNDITEM_H
