//
//  GameAction.cpp
//  testrungame
//
//  Created by 熊 杰 on 13-5-5.
//
//

#include "GameAction.h"

NS_CC_BEGIN
//
// MoveTo
//

CCGameMoveBy* CCGameMoveBy::create(float duration, const CCPoint& position)
{
    CCGameMoveBy *pMoveTo = new CCGameMoveBy();
    pMoveTo->initWithDuration(duration, position);
    pMoveTo->autorelease();
    
    return pMoveTo;
}

bool CCGameMoveBy::initWithDuration(float duration, const CCPoint& position)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_endPosition = position;
        return true;
    }
    
    return false;
}

CCObject* CCGameMoveBy::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCGameMoveBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCGameMoveBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCGameMoveBy();
        pZone = pNewZone = new CCZone(pCopy);
    }
    
    CCActionInterval::copyWithZone(pZone);
    
    pCopy->initWithDuration(m_fDuration, m_endPosition);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCGameMoveBy::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_previousPos = m_startPosition = pTarget->getPosition();
    m_delta = ccpSub(m_endPosition, m_startPosition);
}

void CCGameMoveBy::update(float time)
{
    if (m_pTarget)
    {
        CCNode *m_node = (CCNode *)m_pTarget;
        CCPoint currentPos = m_node->getPosition();
        CCPoint diff = ccpSub(currentPos, m_previousPos);
        m_startPosition = ccpAdd(m_startPosition, diff);
        CCPoint newPos = ccpAdd(m_startPosition, ccpMult(m_delta, time));
        
        m_pTarget->setPosition(newPos);
        m_previousPos = newPos;
    }
}

CCActionInterval* CCGameMoveBy::reverse(void)
{
    return CCGameMoveBy::create(m_fDuration, ccp(-m_delta.x, -m_delta.y));
}


NS_CC_END
