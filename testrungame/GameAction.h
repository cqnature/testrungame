//
//  GameAction.h
//  testrungame
//
//  Created by 熊 杰 on 13-5-5.
//
//

#ifndef __testrungame__GameAction__
#define __testrungame__GameAction__

#include <iostream>
#include "cocos2d.h"

NS_CC_BEGIN

/** @brief Moves a CCNode object to the position x,y. x and y are absolute coordinates by modifying it's position attribute.
 */
class CC_DLL CCGameMoveBy : public CCActionInterval
{
public:
    /** initializes the action */
    bool initWithDuration(float duration, const CCPoint& position);
    
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual void update(float time);
    virtual CCActionInterval* reverse(void);

public:
    /** creates the action */
    static CCGameMoveBy* create(float duration, const CCPoint& position);
protected:
    CCPoint m_endPosition;
    CCPoint m_startPosition;
    CCPoint m_delta;
    CCPoint m_previousPos;
};

NS_CC_END

#endif /* defined(__testrungame__GameAction__) */
