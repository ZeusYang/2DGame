#ifndef ONEBOX_H
#define ONEBOX_H
#include <QGraphicsObject>

class OneBox : public QGraphicsObject
{
public:
    OneBox(const QColor &color = Qt::red);
    QRectF boundingRect()const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QPainterPath shape()const;
private:
    QColor brushColor;
};

#endif // ONEBOX_H
