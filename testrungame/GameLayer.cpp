//
//  GameLayer.cpp
//  testrungame
//
//  Created by 熊 杰 on 13-5-2.
//
//

#include "GameLayer.h"
#include "SimpleAudioEngine.h"
#include "GameAction.h"

using namespace cocos2d;
using namespace CocosDenshion;

CCScene* GameLayer::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    GameLayer *layer = GameLayer::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    // 2. load texture
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("GameScene.plist");
    
    // init basic data
    runDistance = 0.0f;
    coinCount = 0;
    prevPos = CCPointZero;
    isGameStart = false;
    isGameEnd = false;
    isRunning = false;
    isSliding = false;
    isJumpping = false;
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();

    // init map nodes
    for (int i = 0; i < kGameMapCount; i ++) {
        CCNode *mapNode = CCNode::create();
        this->addChild(mapNode, MapBottomZorder, MapStartTag + i);
        mapNode->setPosition(ccp(0, winSize.height * i));
        this->initMapNode(mapNode);
        this->resetMapNode(mapNode);
    }
    
    // init map barrier node
    for (int i = 0; i < kGameMapCount; i ++) {
        CCNode *barrierNode = CCNode::create();
        this->addChild(barrierNode, MapBarrierZorder, MapBarrierStartTag + i);
        barrierNode->setPosition(ccp(0, winSize.height * i));
        this->initBarrierNode(barrierNode);
        this->resetBarrierNode(barrierNode, MapBarrierStartTag + i);
    }
    
    // init player
    CCSprite *playerSprite = CCSprite::createWithSpriteFrameName("Role.png");
    playerSprite->setPosition(ccp(kGameMapSecondRoad, kPlayerStartAtScreenPosY));
    this->addChild(playerSprite, PlayerZorder, PlayerTag);
    
    // init score
    CCLabelTTF *scoreLabel = CCLabelTTF::create("score: 0", "Helvetica", 24);
    scoreLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    scoreLabel->setPosition(ccp(50.0f, winSize.height - 20.0f));
    this->addChild(scoreLabel, ScoreLabelZorder, ScoreLabelTag);
    
    // touch enabled
    this->setTouchEnabled(true);
    
    return true;
}

#pragma mark - map init -
void GameLayer::initMapNode(CCNode *node)
{
    // window size
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // use batchnode to reduce rendering
    CCSpriteBatchNode *batchNode = CCSpriteBatchNode::create("GameScene.pvr.ccz");
    node->addChild(batchNode, 0, MapBatchNodeTag);

    // add background
    CCSprite *backgroundSprite = CCSprite::createWithSpriteFrameName("Background.png");
    backgroundSprite->setPosition(ccp(winSize.width/2.f, winSize.height/2.f));
    batchNode->addChild(backgroundSprite);
    
    // add glass road
    CCSprite *glassRoadSprite = CCSprite::createWithSpriteFrameName("Road.png");
    glassRoadSprite->setPosition(ccp(winSize.width/2.f, winSize.height/2.f));
    batchNode->addChild(glassRoadSprite);
    
    // add monster 
    CCSprite *monsterSprite = CCSprite::createWithSpriteFrameName("Monster.png");
    batchNode->addChild(monsterSprite, 0, MapMonsterTag);
    
    // add river
    CCSprite *riverSprite = CCSprite::createWithSpriteFrameName("River.png");
    batchNode->addChild(riverSprite, 0, MapRiverTag);
    
    // add coins
    for (int i = 0; i < kGameCoinCount; i ++) {
        CCSprite *coinSprite = CCSprite::createWithSpriteFrameName("Gold.png");
        batchNode->addChild(coinSprite, 0, MapCoinStartTag + i);
    }
}

void GameLayer::resetMapNode(CCNode *node)
{
    // window size
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCLOG("winSize %f %f random %f %f", winSize.width, winSize.height, CCRANDOM_0_1(), CCRANDOM_MINUS1_1());
    // get batchnode
    CCSpriteBatchNode *batchNode = (CCSpriteBatchNode *)node->getChildByTag(MapBatchNodeTag);
    
    // reset monster position
    float positionX = kGameMapFirstRoad + (rand()%3) * kGameMapRoadSpace;
    CCSprite *monsterSprite = (CCSprite *)batchNode->getChildByTag(MapMonsterTag);
    monsterSprite->setPosition(ccp(positionX, CCRANDOM_0_1() * winSize.height));

    // reset river position
    CCSprite *riverSprite = (CCSprite *)batchNode->getChildByTag(MapRiverTag);
    float iptr = winSize.height;
    float positionY = modff((CCRANDOM_0_1() * 100 + 50 + monsterSprite->getPosition().y), &iptr);
    riverSprite->setPosition(ccp(winSize.width/2.f, positionY));
    
    // reset coins position
    positionX = kGameMapFirstRoad + (rand()%3) * kGameMapRoadSpace;
    float startPositionY = CCRANDOM_0_1() * winSize.height;
    for (int i = 0; i < kGameCoinCount; i ++) {
        CCSprite *coinSprite = (CCSprite *)batchNode->getChildByTag(MapCoinStartTag + i);
        iptr = winSize.height;
        positionY = modff(startPositionY + 60 * i, &iptr);
        coinSprite->setPosition(ccp(positionX, positionY));
        coinSprite->setVisible(true);
    }
}

void GameLayer::initBarrierNode(CCNode *node)
{
    // window size
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // use batchnode to reduce rendering
    CCSpriteBatchNode *batchNode = CCSpriteBatchNode::create("GameScene.pvr.ccz");
    node->addChild(batchNode, 0, MapBatchNodeTag);
    
    // add barrier
    CCSprite *barrierSprite = CCSprite::createWithSpriteFrameName("Barrier.png");
    batchNode->addChild(barrierSprite, 0, MapBarrierTag);
}

void GameLayer::resetBarrierNode(CCNode *node, int nodeTag)
{
    // window size
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // get batchnode
    CCSpriteBatchNode *batchNode = (CCSpriteBatchNode *)node->getChildByTag(MapBatchNodeTag);
    
    CCNode *mapNode = this->getChildByTag(nodeTag - MapBarrierStartTag + MapStartTag);
    CCSpriteBatchNode *mapBatchNode = (CCSpriteBatchNode *)mapNode->getChildByTag(MapBatchNodeTag);
    CCSprite *riverSprite = (CCSprite *)mapBatchNode->getChildByTag(MapRiverTag);
    
    // reset barrier position
    CCSprite *barrierSprite = (CCSprite *)batchNode->getChildByTag(MapBarrierTag);
    float iptr = winSize.height;
    float positionY = modff((CCRANDOM_0_1() * 100 + 50 + riverSprite->getPosition().y), &iptr);
    barrierSprite->setPosition(ccp(winSize.width/2.f, positionY));
}

#pragma mark - game logic -
void GameLayer::startGame()
{
    runDistance = 0.0f;
    coinCount = 0;
    isGameStart = true;
    isGameEnd = false;
    isRunning = false;
    isSliding = false;
    isJumpping = false;
    
    CCSprite *playerSprite = (CCSprite *)this->getChildByTag(PlayerTag);
    playerSprite->setPosition(ccp(kGameMapSecondRoad, kPlayerStartAtScreenPosY));
    prevPos = playerSprite->getPosition();

    CCLabelTTF *scoreLabel = (CCLabelTTF *)this->getChildByTag(ScoreLabelTag);
    scoreLabel->setString("score: 0");
    
    this->playerRun();
    this->scheduleUpdate();
}

void GameLayer::endGame()
{
    isGameEnd = true;
    CCSprite *playerSprite = (CCSprite *)this->getChildByTag(PlayerTag);
    playerSprite->stopAllActions();
    this->unscheduleUpdate();
}

void GameLayer::gameOver()
{
    this->endGame();
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCLabelTTF *overLabel = CCLabelTTF::create("Game Over", "Helvetica", 48);
    overLabel->setPosition(ccp(winSize.width/2, winSize.height/2));
    this->addChild(overLabel, GameOverLabelZorder, GameOverLabelTag);
    this->scheduleOnce(schedule_selector(GameLayer::gameReset), 1.0f);
}

void GameLayer::gameReset()
{
    this->removeChildByTag(GameOverLabelTag, true);
    isGameStart = false;
}

void GameLayer::update(float dt)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSprite *playerSprite = (CCSprite *)this->getChildByTag(PlayerTag);
    
    CCPoint currentPos = playerSprite->getPosition();
    // update run distance
    runDistance += (currentPos.y - prevPos.y);
    
    // stay the player at specified position
    if (currentPos.y > kPlayerStayAtScreenPosY) {
        CCPoint newPos = ccp(currentPos.x, kPlayerStayAtScreenPosY);
        CCPoint diff = ccpSub(newPos, currentPos);
        
        // move the player and map
        playerSprite->setPosition(newPos);
        
        // move map nodes
        for (int i = 0; i < kGameMapCount; i ++) {
            CCNode *mapNode = this->getChildByTag(MapStartTag + i);
            CCPoint mapPos = ccpAdd(mapNode->getPosition(), diff);
            mapNode->setPosition(mapPos);
            // if map get out of scene
            if (mapPos.y <= -winSize.height) {
                mapPos = ccp(mapPos.x, (kGameMapCount - 1) * winSize.height);
                mapNode->setPosition(mapPos);
                this->resetMapNode(mapNode);
            }
        }
        
        // move map barrier node
        for (int i = 0; i < kGameMapCount; i ++) {
            CCNode *barrierNode = this->getChildByTag(MapBarrierStartTag + i);
            CCPoint barrierPos = ccpAdd(barrierNode->getPosition(), diff);
            barrierNode->setPosition(barrierPos);
            // if map get out of scene
            if (barrierPos.y <= -winSize.height) {
                barrierPos = ccp(barrierPos.x, (kGameMapCount - 1) * winSize.height);
                barrierNode->setPosition(barrierPos);
                this->resetBarrierNode(barrierNode, MapBarrierStartTag + i);
            }
        }
    }
    // save current pos
    prevPos = playerSprite->getPosition();
    
    // collision detection
    for (int i = 0; i < kGameMapCount; i ++) {
        CCNode *mapNode = this->getChildByTag(MapStartTag + i);
        CCSpriteBatchNode *batchNode = (CCSpriteBatchNode *)mapNode->getChildByTag(MapBatchNodeTag);
        
        CCPoint playerPos = batchNode->convertToNodeSpace(this->convertToWorldSpace(playerSprite->getPosition()));
        CCRect playerBox = CCRect(-playerSprite->getContentSize().width/2.0f + playerPos.x, -playerSprite->getContentSize().height/2.0f + playerPos.y, playerSprite->getContentSize().width, playerSprite->getContentSize().height);
        
        CCSprite *monsterSprite = (CCSprite *)batchNode->getChildByTag(MapMonsterTag);
        if (playerBox.intersectsRect(monsterSprite->boundingBox())) {
            // game over
            this->gameOver();
        }
        
        CCSprite *riverSprite = (CCSprite *)batchNode->getChildByTag(MapRiverTag);
        if (playerBox.intersectsRect(riverSprite->boundingBox())) {
            // not in jumpping
            if (!isJumpping) {
                // game over
                this->gameOver();
            }
        }
        
        for (int i = 0; i < kGameCoinCount; i ++) {
            CCSprite *coinSprite = (CCSprite *)batchNode->getChildByTag(MapCoinStartTag + i);
            if (playerBox.intersectsRect(coinSprite->boundingBox())) {
                coinSprite->setVisible(false);
                coinCount ++;
            }
        }
    }
    
    for (int i = 0; i < kGameMapCount; i ++) {
        CCNode *barrierNode = this->getChildByTag(MapBarrierStartTag + i);
        CCSpriteBatchNode *batchNode = (CCSpriteBatchNode *)barrierNode->getChildByTag(MapBatchNodeTag);
        
        CCPoint playerPos = batchNode->convertToNodeSpace(this->convertToWorldSpace(playerSprite->getPosition()));
        CCRect playerBox = CCRect(-playerSprite->getContentSize().width/2.0f + playerPos.x, -playerSprite->getContentSize().height/2.0f + playerPos.y, playerSprite->getContentSize().width, playerSprite->getContentSize().height);
        
        CCSprite *barrierSprite = (CCSprite *)batchNode->getChildByTag(MapBarrierTag);
        if (playerBox.intersectsRect(barrierSprite->boundingBox())) {
            if (!isSliding) {
                // game over
                this->gameOver();
            }
        }
    }
    
    // update score label
    CCLabelTTF *scoreLabel = (CCLabelTTF *)this->getChildByTag(ScoreLabelTag);
    int totalScore = runDistance + coinCount;
    scoreLabel->setString(CCString::createWithFormat("score: %d", totalScore)->getCString());
}

#pragma mark - player action -
void GameLayer::playerRun()
{
    // player begin run
    CCSprite *playerSprite = (CCSprite *)this->getChildByTag(PlayerTag);
    if (playerSprite->getActionByTag(PlayerChangeRoadTag)) {
        return;
    }
    
    CCGameMoveBy *moveBy = CCGameMoveBy::create(1.0f, ccp(0.0f, kPlayerRunSpeed));
    CCRepeatForever *moveForever = CCRepeatForever::create(moveBy);
    moveForever->setTag(PlayerRunningTag);
    
    // run forever
    playerSprite->stopActionByTag(PlayerRunningTag);
    if (isSliding) {
        playerSprite->stopActionByTag(PlayerSlidingTag);
        isSliding = false;
    }
    if (isJumpping) {
        playerSprite->stopActionByTag(PlayerJumppingTag);
        isJumpping = false;
    }
    isRunning = true;
    playerSprite->runAction(moveForever);
}

void GameLayer::playerChangeRoad(PlayerMoveDirection direction)
{
    // check wether player can move to the new road
    CCSprite *playerSprite = (CCSprite *)this->getChildByTag(PlayerTag);
    CCPoint playerPos = playerSprite->getPosition();
    
    if (direction == PlayerMoveLeft && playerPos.x < kGameMapSecondRoad) {
        // already move to left
        return;
    }
    if (direction == PlayerMoveRight && playerPos.x > kGameMapSecondRoad) {
        // already move to right
        return;
    }
    if (playerSprite->getActionByTag(PlayerChangeRoadTag)) {
        return;
    }
    
    float distanceX = kGameMapRoadSpace;
    CCGameMoveBy *moveBy = CCGameMoveBy::create(kPlayerChangeRoadDuration, ccp((direction == PlayerMoveLeft ? - distanceX : distanceX), kPlayerChangeRoadDuration * kPlayerRunSpeed));
    CCAction *action = CCSequence::createWithTwoActions(moveBy, CCCallFunc::create(this, callfunc_selector(GameLayer::playerRun)));
    action->setTag(PlayerChangeRoadTag);
    
    if (isSliding) {
        playerSprite->stopActionByTag(PlayerSlidingTag);
        isSliding = false;
    }
    if (isJumpping) {
        playerSprite->stopActionByTag(PlayerJumppingTag);
        isJumpping = false;
    }
    isRunning = true;
    playerSprite->runAction(action);
}

void GameLayer::playerSilde()
{
    // check wether can slide
    CCSprite *playerSprite = (CCSprite *)this->getChildByTag(PlayerTag);
    if (playerSprite->getActionByTag(PlayerChangeRoadTag)) {
        return;
    }
    if (playerSprite->getActionByTag(PlayerSlidingTag)) {
        // already slide
        return;
    }
    
    CCScaleTo *scaleTo = CCScaleTo::create(0.2, 0.8);
    CCDelayTime *delay = CCDelayTime::create(0.6);
    CCScaleTo *scaleBack = CCScaleTo::create(0.2, 1.0);
    CCAction *action = CCSequence::create(scaleTo, delay, scaleBack, CCCallFunc::create(this, callfunc_selector(GameLayer::playerRun)), NULL);
    action->setTag(PlayerSlidingTag);
    
    if (isJumpping) {
        playerSprite->stopActionByTag(PlayerJumppingTag);
        isJumpping = false;
    }
    isSliding = true;
    playerSprite->runAction(action);
}

void GameLayer::playerJump()
{
    // check whether can jump
    CCSprite *playerSprite = (CCSprite *)this->getChildByTag(PlayerTag);
    if (playerSprite->getActionByTag(PlayerChangeRoadTag)) {
        return;
    }
    if (playerSprite->getActionByTag(PlayerJumppingTag)) {
        // already jump
        return;
    }
    
    CCScaleTo *scaleTo = CCScaleTo::create(0.2, 1.2);
    CCDelayTime *delay = CCDelayTime::create(0.6);
    CCScaleTo *scaleBack = CCScaleTo::create(0.2, 1.0);
    CCAction *action = CCSequence::create(scaleTo, delay, scaleBack, CCCallFunc::create(this, callfunc_selector(GameLayer::playerRun)), NULL);
    action->setTag(PlayerJumppingTag);
    
    if (isSliding) {
        playerSprite->stopActionByTag(PlayerSlidingTag);
        isSliding = false;
    }
    isJumpping = true;
    playerSprite->runAction(action);
}

#pragma mark - touch event -
void GameLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    CCSetIterator it = pTouches->begin();
    CCTouch *touch = (CCTouch *)*it;
    
    touchStartPos = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
    // game has't started, start it
    if (!isGameStart) {
        this->startGame();
    }
}

void GameLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    CCSetIterator it = pTouches->begin();
    CCTouch *touch = (CCTouch *)*it;
    
    if (touchEndPos.x == -1 && touchEndPos.y == -1) {
        return;
    }
    
    touchEndPos = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
    // touch has moved with the effect distance
    if (ccpLength(ccpSub(touchStartPos, touchEndPos)) > kTouchEffectDistance) {
        this->touchEffect();
        touchEndPos = ccp(-1, -1);
    }
}

void GameLayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    CCSetIterator it = pTouches->begin();
    CCTouch *touch = (CCTouch *)*it;
    
    if (touchEndPos.x == -1 && touchEndPos.y == -1) {
        touchEndPos = ccp(0, 0);
    }
    else {
        touchEndPos = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
        this->touchEffect();
        touchEndPos = ccp(0, 0);
    }
}

void GameLayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
    touchEndPos = ccp(0, 0);
}

void GameLayer::touchEffect()
{
    // game has't started or has ended
    if (!isGameStart || isGameEnd) {
        return;
    }
    
    float angle = atan2f(touchEndPos.y - touchStartPos.y, touchEndPos.x - touchStartPos.x);
    float degree = CC_RADIANS_TO_DEGREES(angle);
    CCLOG("degree is %f", degree);
    if (fabsf(degree) < kSlideToRightDegree) {
        // slide to right
        this->playerChangeRoad(PlayerMoveRight);
    }
    else if (fabsf(degree) > kSliderToLeftDegree) {
        // slide to left
        this->playerChangeRoad(PlayerMoveLeft);
    }
    else if (degree > 0) {
        this->playerJump();
    }
    else {
        this->playerSilde();
    }
}
