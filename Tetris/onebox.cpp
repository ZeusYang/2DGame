#include "onebox.h"
#include <QPainter>

OneBox::OneBox(const QColor &color)
    :brushColor(color)
{
}

QRectF OneBox::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-10-penWidth/2,-10-penWidth/2,
                  20+penWidth,20+penWidth);
}

void OneBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    //对小方块贴图
    painter->drawPixmap(-10,-10,20,20,QPixmap(":/images/box.gif"));
    painter->setBrush(brushColor);
    QColor penColor = brushColor;
    //将颜色的透明度减小
    penColor.setAlpha(20);
    painter->setPen(penColor);
    painter->drawRect(-10,-10,20,20);
}

QPainterPath OneBox::shape() const
{
    //形状比边框矩形小0.5像素,这样方块组中的小方块才不会发生碰撞
    QPainterPath path;
    path.addRect(-9.5,-9.5,19,19);
    return path;
}
