#ifndef MYVIEW_H
#define MYVIEW_H
#include <QGraphicsView>

class BoxGroup;
class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyView(QWidget *parent = nullptr);
    ~MyView();
public slots:
    void startGame();
    void clearFullRows();
    void moveBox();
    void gameOver();
    void restartGame();
    void finishGame();
    void pauseGame();
    void returnGame();
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    BoxGroup *boxGroup;
    BoxGroup *nextBoxGroup;
    QGraphicsLineItem *topLine;
    QGraphicsLineItem *bottomLine;
    QGraphicsLineItem *leftLine;
    QGraphicsLineItem *rightLine;
    QGraphicsTextItem *gameScoreText;
    QGraphicsTextItem *gameLevelText;

    //遮罩面板
    QGraphicsWidget *maskWidget;
    //各种按钮
    QGraphicsWidget *startButton;
    QGraphicsWidget *finishButton;
    QGraphicsWidget *restartButton;
    QGraphicsWidget *pauseButton;
    QGraphicsWidget *optionButton;
    QGraphicsWidget *returnButton;
    QGraphicsWidget *helpButton;
    QGraphicsWidget *exitButton;
    QGraphicsWidget *showMenuButton;
    //各种文本
    QGraphicsTextItem *gameWelcomeText;
    QGraphicsTextItem *gamePausedText;
    QGraphicsTextItem *gameOverText;

    qreal gameSpeed;
    QList<int> rows;
    void initView();
    void initGame();
    void updateScore(const int fullRowNum = 0);
};

#endif // MYVIEW_H
