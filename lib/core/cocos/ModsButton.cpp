#include "ModsButton.h"

void ModsButton::callback(CCObject* pSender)
{
	CCMenuItemSpriteExtra* btn = (CCMenuItemSpriteExtra*)pSender;
	btn->stopAllActions();
	btn->setScale(1.1);
}