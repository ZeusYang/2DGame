#include "myview.h"
#include "boxgroup.h"
#include "onebox.h"
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QLabel>
#include <QGraphicsTextItem>
#include <QFileInfo>

static const qreal INITSPEED = 500;

MyView::MyView(QWidget *parent)
    :QGraphicsView(parent)
{
    initView();
}

MyView::~MyView()
{
    delete gameScoreText;
    delete gameLevelText;
    delete gameWelcomeText;
    delete gamePausedText;
    delete gameOverText;
}

void MyView::startGame()
{
    gameWelcomeText->hide();
    startButton->hide();
    optionButton->hide();
    helpButton->hide();
    exitButton->hide();
    maskWidget->hide();
    initGame();
}

void MyView::clearFullRows()
{
    //清空满行
    for(auto y = 429;y > 50;y -= 20){
        QList<QGraphicsItem *> list =
                scene()->items(QRectF((qreal)199, (qreal)y, (qreal)202, (qreal)22), Qt::ContainsItemShape);
        if(list.count() == 10){
            //如果改行已满
            foreach(QGraphicsItem *item,list){
                OneBox *box = (OneBox*)(item);
                QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
                box->setGraphicsEffect(blurEffect);
                QPropertyAnimation *animation = new QPropertyAnimation(box,"scale");
                animation->setEasingCurve(QEasingCurve::OutBounce);
                animation->setDuration(250);
                animation->setStartValue(4);
                animation->setEndValue(0.25);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                connect(animation,&QPropertyAnimation::finished,box,&OneBox::deleteLater);
            }
            //保存满行的位置
            rows << y;
        }
    }
    //如果有满行，下移满行上面的各行再出现新的方块组
    if(rows.count() > 0){
        QTimer::singleShot(400,this,&MyView::moveBox);
    }
    else{
        boxGroup->createBox(QPointF(300,70),nextBoxGroup->getCurrentShape());
        //清空并销毁提示方块组中的所有小方块
        nextBoxGroup->clearBoxGroup(true);
        nextBoxGroup->createBox(QPointF(500,70));
    }
}

void MyView::moveBox()
{
    //下移满行上面的所有小方块
    //从位置最靠上的满行开始
    for(auto i = rows.count();i > 0;--i){
        auto row = rows.at(i - 1);
        foreach(QGraphicsItem *item,scene()->items(QRectF(199,49,202,row - 47),Qt::ContainsItemShape))
        {
            item->moveBy(0,20);
        }
    }
    //更新分数
    updateScore(rows.count());
    //将满行列表清空为0
    rows.clear();
    boxGroup->createBox(QPointF(300,70),nextBoxGroup->getCurrentShape());
    nextBoxGroup->clearBoxGroup(true);
    nextBoxGroup->createBox(QPointF(500,70));
}

void MyView::gameOver()
{
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gameOverText->show();
    restartButton->setPos(370,200);
    finishButton->show();
}

void MyView::restartGame()
{
    maskWidget->hide();
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600, 150);

    // 销毁提示方块组和当前方块移动区域中的所有小方块
    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();
    foreach (QGraphicsItem *item, scene()->items(QRectF(199, 49, 202, 402), Qt::ContainsItemShape)) {
        // 先从场景中移除小方块，因为使用deleteLater()是在返回主事件循环后才销毁
        // 小方块的，为了在出现新的方块组时不发生碰撞，所以需要先从场景中移除小方块
        scene()->removeItem(item);
        OneBox *box = (OneBox*) item;
        box->deleteLater();
    }

    initGame();
}

void MyView::finishGame()
{
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600, 150);
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    gameScoreText->hide();
    gameLevelText->hide();

    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();

    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();

    foreach (QGraphicsItem *item, scene()->items(QRectF(199, 49, 202, 402), Qt::ContainsItemShape)) {
        OneBox *box = (OneBox*) item;
        box->deleteLater();
    }

    // 可能是在进行游戏时按下“主菜单”按钮
    maskWidget->show();
    gameWelcomeText->show();
    startButton->show();
    optionButton->show();
    helpButton->show();
    exitButton->show();
    scene()->setBackgroundBrush(QPixmap(":/images/background.png"));

    // 在2-5中添加的代码
//    backgroundMusic->setCurrentSource(Phonon::MediaSource(SOUNDPATH + "background.mp3"));
//    backgroundMusic->play();
}

void MyView::pauseGame()
{
    boxGroup->stopTimer();
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gamePausedText->show();
    returnButton->show();
}

void MyView::returnGame()
{
    returnButton->hide();
    gamePausedText->hide();
    maskWidget->hide();
    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    boxGroup->startTimer(gameSpeed);
}

void MyView::keyPressEvent(QKeyEvent *event)
{
    if(pauseButton->isVisible())
        boxGroup->setFocus();
    else
        boxGroup->clearFocus();
    QGraphicsView::keyPressEvent(event);
}

void MyView::initView()
{
    //初始化游戏界面
    setRenderHint(QPainter::Antialiasing);
    //设置缓存背景，这样可以加快渲染速度
    setCacheMode(CacheBackground);
    setWindowTitle(tr("俄罗斯方块游戏"));
    setWindowIcon(QIcon(":/images/icon.png"));
    setMinimumSize(810,510);
    setMaximumSize(810,510);
    //设置场景
    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(5,5,800,500);
    scene->setBackgroundBrush(QPixmap(":/images/background.png"));
    setScene(scene);
    //方块可移动区域的4条边界线
    topLine = scene->addLine(197,47,403,47);
    bottomLine = scene->addLine(197,453,403,453);
    leftLine = scene->addLine(197,47,197,453);
    rightLine = scene->addLine(403,47,403,453);
    //当前方块组和提示方块组
    boxGroup = new BoxGroup;
    connect(boxGroup,&BoxGroup::needNewBox,this,&MyView::clearFullRows);
    connect(boxGroup,&BoxGroup::gameFinished,this,&MyView::gameOver);
    scene->addItem(boxGroup);
    nextBoxGroup = new BoxGroup;
    scene->addItem(nextBoxGroup);

    //得分文本
    gameScoreText = new QGraphicsTextItem();
    gameScoreText->setFont(QFont("Times",20,QFont::Bold));
    gameScoreText->setPos(650,150);
    //关卡文本
    gameLevelText = new QGraphicsTextItem();
    gameLevelText->setFont(QFont("Times",30,QFont::Bold));
    gameLevelText->setPos(20,150);

    //设置初始为隐藏状态
    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();
    gameScoreText->hide();
    gameLevelText->hide();
    //黑色遮罩
    // 黑色遮罩
    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0, 0, 0, 80)));
    mask->resize(900, 600);
    maskWidget = scene->addWidget(mask);
    maskWidget->setPos(-50, -50);
    // 设置其Z值为1，这样可以处于Z值为0的图形项上面
    maskWidget->setZValue(1);

    // 选项面板
    QWidget *option = new QWidget;
    QPushButton *optionCloseButton = new QPushButton(tr("关   闭"), option);
    QPalette palette;
    palette.setColor(QPalette::ButtonText, Qt::black);
    optionCloseButton->setPalette(palette);
    optionCloseButton->move(120, 300);
    connect(optionCloseButton, SIGNAL(clicked()), option, SLOT(hide()));
    option->setAutoFillBackground(true);
    option->setPalette(QPalette(QColor(0, 0, 0, 180)));
    option->resize(300, 400);
    QGraphicsWidget *optionWidget = scene->addWidget(option);
    optionWidget->setPos(250, 50);
    optionWidget->setZValue(3);
    optionWidget->hide();

    // 帮助面板
    QWidget *help = new QWidget;
    QPushButton *helpCloseButton = new QPushButton(tr("关   闭"), help);
    helpCloseButton->setPalette(palette);
    helpCloseButton->move(120, 300);
    connect(helpCloseButton, SIGNAL(clicked()), help, SLOT(hide()));
    help->setAutoFillBackground(true);
    help->setPalette(QPalette(QColor(0, 0, 0, 180)));
    help->resize(300, 400);
    QGraphicsWidget *helpWidget = scene->addWidget(help);
    helpWidget->setPos(250, 50);
    helpWidget->setZValue(3);
    helpWidget->hide();

    QLabel *helpLabel = new QLabel(help);
    helpLabel->setText(tr("<h1><font color=white>俄罗斯方块游戏"
                          "</font></h1>"));
    helpLabel->setAlignment(Qt::AlignCenter);
    helpLabel->move(30, 150);


    // 游戏欢迎文本
    gameWelcomeText = new QGraphicsTextItem();
    gameWelcomeText->setHtml(tr("<font color=white>俄罗斯方块游戏</font>"));
    gameWelcomeText->setFont(QFont("Times", 30, QFont::Bold));
    gameWelcomeText->setPos(250, 100);
    gameWelcomeText->setZValue(2);

    // 游戏暂停文本
    gamePausedText = new QGraphicsTextItem();
    gamePausedText->setHtml(tr("<font color=white>游戏暂停中！</font>"));
    gamePausedText->setFont(QFont("Times", 30, QFont::Bold));
    gamePausedText->setPos(300, 100);
    gamePausedText->setZValue(2);
    gamePausedText->hide();

    // 游戏结束文本
    gameOverText = new QGraphicsTextItem();
    gameOverText->setHtml(tr("<font color=white>游戏结束！</font>"));
    gameOverText->setFont(QFont("Times", 30, QFont::Bold));
    gameOverText->setPos(320, 100);
    gameOverText->setZValue(2);
    gameOverText->hide();

    // 游戏中使用的按钮
    QPushButton *button1 = new QPushButton(tr("开    始"));
    QPushButton *button2 = new QPushButton(tr("选    项"));
    QPushButton *button3 = new QPushButton(tr("帮    助"));
    QPushButton *button4 = new QPushButton(tr("退    出"));
    QPushButton *button5 = new QPushButton(tr("重新开始"));
    QPushButton *button6 = new QPushButton(tr("暂    停"));
    QPushButton *button7 = new QPushButton(tr("主 菜 单"));
    QPushButton *button8 = new QPushButton(tr("返回游戏"));
    QPushButton *button9 = new QPushButton(tr("结束游戏"));

    connect(button1, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(button2, SIGNAL(clicked()), option, SLOT(show()));
    connect(button3, SIGNAL(clicked()), help, SLOT(show()));
    connect(button4, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(button5, SIGNAL(clicked()), this, SLOT(restartGame()));
    connect(button6, SIGNAL(clicked()), this, SLOT(pauseGame()));
    connect(button7, SIGNAL(clicked()), this, SLOT(finishGame()));
    connect(button8, SIGNAL(clicked()), this, SLOT(returnGame()));
    connect(button9, SIGNAL(clicked()), this, SLOT(finishGame()));
    startButton = scene->addWidget(button1);
    optionButton = scene->addWidget(button2);
    helpButton = scene->addWidget(button3);
    exitButton = scene->addWidget(button4);
    restartButton = scene->addWidget(button5);
    pauseButton = scene->addWidget(button6);
    showMenuButton = scene->addWidget(button7);
    returnButton = scene->addWidget(button8);
    finishButton = scene->addWidget(button9);

    startButton->setPos(370, 200);
    optionButton->setPos(370, 250);
    helpButton->setPos(370, 300);
    exitButton->setPos(370, 350);
    restartButton->setPos(600, 150);
    pauseButton->setPos(600, 200);
    showMenuButton->setPos(600, 250);
    returnButton->setPos(370, 200);
    finishButton->setPos(370, 250);

    startButton->setZValue(2);
    optionButton->setZValue(2);
    helpButton->setZValue(2);
    exitButton->setZValue(2);
    restartButton->setZValue(2);
    returnButton->setZValue(2);
    finishButton->setZValue(2);

    restartButton->hide();
    finishButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    returnButton->hide();

}

void MyView::initGame()
{
    boxGroup->createBox(QPointF(300,70));
    boxGroup->setFocus();
    boxGroup->startTimer(INITSPEED);
    gameSpeed = INITSPEED;
    nextBoxGroup->createBox(QPointF(500,70));
    gameScoreText->setHtml(tr("<font color=red>0</font>"));
    gameScoreText->setHtml(tr("<font color=white>第<br>一<br>幕</font>"));

    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    gameScoreText->show();
    gameLevelText->show();
    topLine->show();
    bottomLine->show();
    leftLine->show();;
    rightLine->show();
    boxGroup->show();
}

void MyView::updateScore(const int fullRowNum)
{
    int score = fullRowNum * 100;
    int currentScore = gameScoreText->toPlainText().toInt();
    currentScore += score;
    //显示当前分数
    gameScoreText->setHtml(tr("<font color=red>%1</font>").arg(currentScore));
    //判断级别
    if(currentScore<500){
        //第一级，什么都不用做
    }
    else if(currentScore < 1000){
        gameLevelText->setHtml(tr("<font color=white>第<br>二<br>幕</font>"));
        scene()->setBackgroundBrush(QPixmap(":/images/background1.png"));
        gameSpeed = 300;
        boxGroup->stopTimer();
        boxGroup->startTimer(gameSpeed);
    }
    else{
        //添加下一个级别的设置
    }
}


