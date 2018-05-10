#include "boxgroup.h"
#include "onebox.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>

BoxGroup::BoxGroup()
{
    setFlags(QGraphicsItem::ItemIsFocusable);
    //保存变换矩阵，当boxGroup旋转后，可以用它来恢复
    oldTransform = transform();
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&BoxGroup::moveOneStep);
    currentShape = RandomShape;
}

QRectF BoxGroup::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-40-penWidth/2,-40-penWidth/2,
                  80+penWidth,80+penWidth);
}

bool BoxGroup::isColliding()
{
    QList<QGraphicsItem*> itemList = childItems();
    QGraphicsItem *item;
    foreach(item,itemList){
        if(item->collidingItems().count() > 1)return true;
    }
    return false;
}

void BoxGroup::createBox(const QPointF &point, BoxGroup::BoxShape shape)
{
    static const QColor colorTable[7] = {
        QColor(200,0,0,100),QColor(255,200,0,100),
        QColor(0,0,200,100),QColor(0,200,0,100),
        QColor(0,200,255,100),QColor(200,0,255,100),
        QColor(150,100,100,100)
    };
    int shapeID = shape;
    if(shape == RandomShape){
        shapeID = qrand() % 7;
    }
    QColor color = colorTable[shapeID];
    QList<OneBox*> list;
    //恢复方块组的变换矩阵
    setTransform(oldTransform);
    for(auto i = 0;i < 4;++i){
        OneBox* temp = new OneBox(color);
        list << temp;
        addToGroup(temp);
    }
    switch(shapeID){
    case IShape:
        currentShape = IShape;
        list.at(0)->setPos(-30,-10);
        list.at(1)->setPos(-10,-10);
        list.at(2)->setPos(10,-10);
        list.at(3)->setPos(30,-10);
        break;
    case JShape:
        currentShape = JShape;
        list.at(0)->setPos(10,-10);
        list.at(1)->setPos(10,10);
        list.at(2)->setPos(-10,30);
        list.at(3)->setPos(10,30);
        break;
    case LShape:
        currentShape = LShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(-10,10);
        list.at(2)->setPos(-10,30);
        list.at(3)->setPos(10,30);
        break;
    case OShape:
        currentShape = OShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(-10,10);
        list.at(3)->setPos(10,10);
        break;
    case SShape:
        currentShape = SShape;
        list.at(0)->setPos(10,-10);
        list.at(1)->setPos(30,-10);
        list.at(2)->setPos(-10,10);
        list.at(3)->setPos(10,10);
        break;
    case TShape:
        currentShape = TShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(30,-10);
        list.at(3)->setPos(10,10);
        break;
    case ZShape:
        currentShape = ZShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(10,10);
        list.at(3)->setPos(30,10);
        break;
    default:
        break;
    }
    //设置位置
    setPos(point);
    //如果开始就发生碰撞，说明已经结束游戏
    if(isColliding()){
        stopTimer();
        emit gameFinished();
    }
}

void BoxGroup::clearBoxGroup(bool destroyBox)
{
    QList<QGraphicsItem*> itemList = childItems();
    QGraphicsItem *item;
    foreach(item,itemList){
        removeFromGroup(item);
        if(destroyBox){
            OneBox *box = (OneBox*)item;
            box->deleteLater();
        }
    }
}

void BoxGroup::keyPressEvent(QKeyEvent *event)
{
    static QTransform temp;
    switch(event->key()){
    case Qt::Key_Down:
        moveBy(0,20);
        if(isColliding()){
            moveBy(0,-20);
            clearBoxGroup();
            emit needNewBox();
        }
        break;
    case Qt::Key_Left:
        moveBy(-20,0);
        if(isColliding())moveBy(20,0);
        break;
    case Qt::Key_Right:
        moveBy(20,0);
        if(isColliding())moveBy(-20,0);
        break;
    case Qt::Key_Up:
        temp.rotate(90);
        setTransform(temp);
        if(isColliding())temp.rotate(-90);
        setTransform(temp);
        break;
    case Qt::Key_Space:
        moveBy(0,20);
        while(!isColliding()){
            moveBy(0,20);
        }
        moveBy(0,-20);
        clearBoxGroup();
        emit needNewBox();
        break;
    }
}

void BoxGroup::moveOneStep()
{
    //向下走一步
    moveBy(0,20);
    if(isColliding()){
        moveBy(0,-20);
        clearBoxGroup();
        emit needNewBox();
    }
}

void BoxGroup::startTimer(int interval)
{
    timer->start(interval);
}

void BoxGroup::stopTimer()
{
    timer->stop();
}


