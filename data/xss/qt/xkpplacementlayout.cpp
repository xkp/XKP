#include <QtGui>
#include <QString>

//#define DEBUG_XKPPLACEMENT

#include "xkpplacementlayout.h"

XKPPlacementLayout::XKPPlacementLayout(QWidget *parent, int spacing, int margin)
    : QLayout(parent), inSetGeometry(false)
{
    setMargin(margin);
    setSpacing(spacing);
    setSizeType(PlacementSize);
}

XKPPlacementLayout::XKPPlacementLayout(int spacing)
    : inSetGeometry(false)
{
    inSetGeometry = false;
    setMargin(0);
    setSpacing(spacing);
    setSizeType(PlacementSize);
}

XKPPlacementLayout::~XKPPlacementLayout()
{
    QLayoutItem *l;
    while((l = takeAt(0)))
        delete l;
}

void XKPPlacementLayout::addItem(QLayoutItem *item)
{
    add(item, Top, 0);
}

void XKPPlacementLayout::addWidget(QWidget *widget, Placement position)
{
    add(new QWidgetItem(widget), position, widget);
    widget->installEventFilter(this);
}

void XKPPlacementLayout::addWidget(QWidget *widget)
{
    add(new QWidgetItem(widget), Placement(widget->property(placementName).toUInt()), widget);
    widget->installEventFilter(this);
}

void XKPPlacementLayout::add(QLayoutItem *item, Placement position, QWidget *widget)
{
    list.append(new ItemWrapper(item, position, widget));
}

QLayoutItem *XKPPlacementLayout::takeAt(int index)
{
    if(index >= 0 && index < list.size()) {
        ItemWrapper *layoutStruct = list.takeAt(index);
        return layoutStruct->item;
    }
    return 0;
}

Qt::Orientations XKPPlacementLayout::expandingDirections() const
{
    return Qt::Horizontal | Qt::Vertical;
}

bool XKPPlacementLayout::hasHeightForWidth() const
{
    return false;
}

int XKPPlacementLayout::count() const
{
    return list.size();
}

QLayoutItem *XKPPlacementLayout::itemAt(int index) const
{
    ItemWrapper *wrapper = list.value(index);
    if(wrapper)
        return wrapper->item;
    else
        return 0;
}

void XKPPlacementLayout::setSizeType(SizeType st)
{
    sizeType = st;
}

XKPPlacementLayout::SizeType XKPPlacementLayout::getSizeType() const
{
    return sizeType;
}

QSize XKPPlacementLayout::sizeHint() const
{
    return calculateSize(sizeType);
}

void XKPPlacementLayout::setGeometry(const QRect &rect)
{
#ifdef DEBUG_XKPPLACEMENT
    qDebug() << "setGeometry:: rect(" << rect.x() << "," << rect.y() <<
            "," << rect.width() << "," << rect.height() << ")";
#endif
    inSetGeometry = true;
    ItemWrapper *center = 0;
    int eastWidth = 0;
    int westWidth = 0;
    int northHeight = 0;
    int southHeight = 0;
    int centerHeight = 0;
    int i;

    QLayout::setGeometry(rect);

    // update lastRect for updateGeometry method
    lastRect = rect;

    for(i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        Placement position = wrapper->position;
        QSize placementSize = wrapper->widget->property(placementSizeName).toSize();
        QRect itemRect;

#ifdef DEBUG_XKPPLACEMENT
        qDebug() << "setGeometry::Vertical object->" << wrapper->widget->objectName();
        qDebug() << "setGeometry::Vertical placementSize(" << placementSize.width() <<
                "," << placementSize.height() << ")";
#endif

        // to prevent object without assign size
        if(placementSize == QSize(-1, -1)) {
            wrapper->widget->resize(item->sizeHint());
            wrapper->widget->setProperty(placementSizeName, item->sizeHint());
        }

        if(position == None) {
            continue;
        } else
        if(position == Top) {
            itemRect = QRect(rect.x(), northHeight, rect.width(),
                             placementSize.height());
#ifdef DEBUG_XKPPLACEMENT
            qDebug() << "setGeometry::TOP itemRect(" << itemRect.x() << "," << itemRect.y() <<
                    "," << itemRect.width() << "," << itemRect.height() << ")";
#endif
            wrapper->widget->setGeometry(itemRect);

            northHeight += itemRect.height() + spacing();
        } else
        if(position == Bottom) {
            itemRect = QRect(wrapper->widget->geometry().x(),
                            wrapper->widget->geometry().y(), rect.width(),
                            placementSize.height());
            wrapper->widget->setGeometry(itemRect);

            southHeight += wrapper->widget->geometry().height() + spacing();
            itemRect = QRect(rect.x(),
                            rect.y() + rect.height() - southHeight + spacing(),
                            wrapper->widget->geometry().width(),
                            placementSize.height());
#ifdef DEBUG_XKPPLACEMENT
            qDebug() << "setGeometry::BOTTOM itemRect(" << itemRect.x() << "," << itemRect.y() <<
                    "," << itemRect.width() << "," << itemRect.height() << ")";
#endif
            wrapper->widget->setGeometry(itemRect);
        } else
        if(position == Center) {
            center = wrapper;
        }
    }

    centerHeight = rect.height() - northHeight - southHeight;

    for(i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        Placement position = wrapper->position;
        QSize placementSize = wrapper->widget->property(placementSizeName).toSize();
        QRect itemRect;

#ifdef DEBUG_XKPPLACEMENT
        qDebug() << "setGeometry::Horizontal object->" << wrapper->widget->objectName();
        qDebug() << "setGeometry::Horizontal placementSize(" << placementSize.width() <<
                "," << placementSize.height() << ")";
#endif

        // to prevent object without assign size
        if(placementSize == QSize(-1, -1)) {
            wrapper->widget->resize(item->sizeHint());
            wrapper->widget->setProperty(placementSizeName, item->sizeHint());
        }

        if(position == None) {
            continue;
        } else
        if(position == Left) {
            itemRect = QRect(rect.x() + westWidth, northHeight,
                             placementSize.width(), centerHeight);
#ifdef DEBUG_XKPPLACEMENT
            qDebug() << "setGeometry::LEFT itemRect(" << itemRect.x() << "," << itemRect.y() <<
                    "," << itemRect.width() << "," << itemRect.height() << ")";
#endif
            wrapper->widget->setGeometry(itemRect);

            westWidth += itemRect.width() + spacing();
        } else
        if(position == Right) {
            itemRect = QRect(wrapper->widget->geometry().x(),
                             wrapper->widget->geometry().y(),
                             wrapper->widget->geometry().width(), centerHeight);
            wrapper->widget->setGeometry(itemRect);

            eastWidth += wrapper->widget->geometry().width() + spacing();
            itemRect = QRect(rect.x() + rect.width() - eastWidth + spacing(),
                             northHeight, wrapper->widget->geometry().width(),
                             wrapper->widget->geometry().height());
#ifdef DEBUG_XKPPLACEMENT
            qDebug() << "setGeometry::RIGHT itemRect(" << itemRect.x() << "," << itemRect.y() <<
                    "," << itemRect.width() << "," << itemRect.height() << ")";
#endif
            wrapper->widget->setGeometry(itemRect);
        }
    }

    if(center)
        center->item->setGeometry(QRect(westWidth, northHeight,
                                        rect.width() - eastWidth - westWidth,
                                        centerHeight));
    inSetGeometry = false;
}

QSize XKPPlacementLayout::calculateSize(SizeType sizeType) const
{
#ifdef DEBUG_XKPPLACEMENT
    qDebug() << "calculateSize()";
#endif
    QSize totalSize;

    for(int i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        Placement position = wrapper->position;
        QSize itemSize;

        if(sizeType == MinimumSize)
            itemSize = wrapper->item->minimumSize();
        else if(sizeType == SizeHint)
            itemSize = wrapper->item->sizeHint();
        else if(sizeType == PlacementSize && wrapper->widget) {
            if(wrapper->widget->property(placementSizeName).toSize() == QSize(-1, -1)) {
                wrapper->widget->setProperty(placementSizeName, wrapper->widget->size());
                itemSize = wrapper->widget->size();
            } else {
                itemSize = wrapper->widget->property(placementSizeName).toSize();
            }
        }

        if(position == Top || position == Bottom || position == Center)
            totalSize.rheight() += itemSize.height();

        if(position == Left || position == Right || position == Center)
            totalSize.rwidth() += itemSize.width();
    }

    return totalSize;
}

void XKPPlacementLayout::updateGeometry()
{
#ifdef DEBUG_XKPPLACEMENT
    qDebug() << "updateGeometry()";
#endif
    setGeometry(lastRect);
}

bool XKPPlacementLayout::eventFilter(QObject *object, QEvent *event)
{
    // if event is resize
    if(event->type() == QEvent::Resize && !inSetGeometry) {

        QResizeEvent *resizeEvent =
                static_cast<QResizeEvent *>(event);

#ifdef DEBUG_XKPPLACEMENT
        qDebug() << "eventFilter:: object " << object->objectName() << " was resized";
#endif
        // update dynamic property 'placementSize' with widget size
        object->setProperty(placementSizeName, resizeEvent->size());

        // update geometry in the general widget
        updateGeometry();

        return true;
    } else
    // if event is dynamic property changed
    if(event->type() == QEvent::DynamicPropertyChange && !inSetGeometry) {

        QDynamicPropertyChangeEvent *dynamicPropertyChangeEvent =
                static_cast<QDynamicPropertyChangeEvent *>(event);

        // if dynamic property 'placement' is changed
        if(dynamicPropertyChangeEvent->propertyName() == placementName) {

#ifdef DEBUG_XKPPLACEMENT
        qDebug() << "eventFilter:: propery 'placement' of object " <<
                object->objectName() << " was changed";
#endif
            // find object in list
            for(int i = 0; i < list.size(); ++i) {
                ItemWrapper *wrapper = list.at(i);
                QWidget *widget = static_cast<QWidget *>(object);

                if(wrapper->widget == widget) {
                    wrapper->position =
                            Placement(wrapper->widget->property(placementName).toUInt());
                    break;
                }
            }

            // update geometry in the general widget
            updateGeometry();

            return true;
        } else {
            // standard event processing
            return QObject::eventFilter(object, event);
        }
    } else {
        // standard event processing
        return QObject::eventFilter(object, event);
    }
}
