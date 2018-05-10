#ifndef BOXGROUP_H
#define BOXGROUP_H
#include <QGraphicsItemGroup>

class BoxGroup : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    enum BoxShape{IShape,JShape,LShape,OShape,SShape,
                 TShape,ZShape,RandomShape};
    BoxGroup();
    QRectF boundingRect()const;
    bool isColliding();
    void createBox(const QPointF &point = QPointF(0,0),
            BoxShape shape = RandomShape);
    void clearBoxGroup(bool destroyBox = false);
    BoxShape getCurrentShape(){return currentShape;}
protected:
    void keyPressEvent(QKeyEvent *event);
signals:
    void needNewBox();
    void gameFinished();
public slots:
    void moveOneStep();
    void startTimer(int interval);
    void stopTimer();
private:
    BoxShape currentShape;
    QTransform oldTransform;
    QTimer *timer;
};

#endif // BOXGROUP_H
