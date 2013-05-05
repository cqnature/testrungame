//
//  GameLayer.h
//  testrungame
//
//  Created by 熊 杰 on 13-5-2.
//
//

#ifndef __testrungame__GameLayer__
#define __testrungame__GameLayer__

#define kTouchEffectDistance 5

#define kGameMapCount 2     // 循环滚动地图个数
#define kGameCoinCount 4    // 金币个数
#define kPlayerRunSpeed 120 // 每秒移动距离
#define kPlayerChangeRoadDuration 0.2 // 切换路径的速度

#define kPlayerStartAtScreenPosY 100
#define kPlayerStayAtScreenPosY 160

#define kSlideToRightDegree 20.0f
#define kSliderToLeftDegree 160.0f

#define kGameMapFirstRoad  80
#define kGameMapSecondRoad 160
#define kGameMapThirdRoad 240
#define kGameMapRoadSpace 80

enum GameLayerChildZorder {
    MapBottomZorder,
    MapFirstZorder,
    MapSecondZorder,
    MapThirdZorder,
    PlayerZorder,
    MapBarrierZorder,
    ScoreLabelZorder,
    GameOverLabelZorder,
};

enum GameLayerChildTag {
    MapStartTag = 1,        // 地图节点的开始tag
    MapBarrierStartTag = 10,
    
    MapBatchNodeTag = 20,
    MapMonsterTag,          // 地图怪物节点
    MapBarrierTag,          // 地图障碍节点
    MapRiverTag,            // 地图河道节点
    MapCoinStartTag = 30,   // 地图金币节点开始tag
    
    PlayerTag = 40,
    ScoreLabelTag,
    GameOverLabelTag,
};

enum PlayerActionTag {
    PlayerRunningTag = 1,   // 跑步动作tag
    PlayerChangeRoadTag,
    PlayerSlidingTag,
    PlayerJumppingTag,
};

typedef enum PlayerMoveDirection_
{
    PlayerMoveLeft,
    PlayerMoveRight,
} PlayerMoveDirection;

#include <iostream>
#include "cocos2d.h"

class GameLayer : public cocos2d::CCLayer
{
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static cocos2d::CCScene* scene();
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(GameLayer);
    
    void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    void ccTouchesCancelled(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
    
protected:
    // init nodes on map
    void initMapNode(CCNode *node);
    // reset nodes'position on map
    void resetMapNode(CCNode *node);
    
    void initBarrierNode(CCNode *node);
    void resetBarrierNode(CCNode *node, int nodeTag);
    
    // game main logic
    void startGame();
    void endGame();
    void update(float dt);
    void gameOver();
    void gameReset();

    // player action
    void playerRun();
    void playerChangeRoad(PlayerMoveDirection direction);
    void playerSilde();
    void playerJump();

    // touch event
    void touchEffect();

private:
    // score
    float runDistance;
    int coinCount;
    cocos2d::CCPoint prevPos;
    cocos2d::CCPoint touchStartPos;
    cocos2d::CCPoint touchEndPos;

    // status
    bool isGameStart;
    bool isGameEnd;
    bool isRunning;
    bool isSliding;
    bool isJumpping;
};


#endif /* defined(__testrungame__GameLayer__) */
