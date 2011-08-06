#ifndef XKPPLACEMENTLAYOUT_H
#define XKPPLACEMENTLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

#if !defined(placementName) && !defined(placementSizeName)
#define placementName "placement"
#define placementSizeName "placementSize"
#endif

class XKPPlacementLayout : public QLayout
{
public:
    enum Placement { None, Left, Top, Bottom, Right, Center, Client };
    enum SizeType { MinimumSize, SizeHint, PlacementSize };

    XKPPlacementLayout(QWidget *parent, int spacing = -1, int margin = 0);
    XKPPlacementLayout(int spacing = -1);
    ~XKPPlacementLayout();

    void addItem(QLayoutItem *item);
    void addWidget(QWidget *widget, Placement position);
    void addWidget(QWidget *widget);
    void add(QLayoutItem *item, Placement position, QWidget *widget);
    QLayoutItem *takeAt(int index);

    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int count() const;
    QLayoutItem *itemAt(int index) const;

    QSize sizeHint() const;
    void setSizeType(SizeType st);
    SizeType getSizeType() const;

    void setGeometry(const QRect &rect);

private:
    struct ItemWrapper
    {
        ItemWrapper(QLayoutItem *i, Placement p, QWidget *w) {
            item = i;
            position = p;
            widget = w;
        }

        QLayoutItem *item;
        QWidget *widget;
        Placement position;
    };

    SizeType sizeType;
    QList<ItemWrapper *> list;
    QRect lastRect;
    bool inSetGeometry;

    QSize calculateSize(SizeType sizeType) const;
    void updateGeometry();

protected:
    bool eventFilter(QObject *object, QEvent *event);

};

#endif
