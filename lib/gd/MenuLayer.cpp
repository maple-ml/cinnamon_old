#include "MenuLayer.h"

#include "../../global.hpp"
#include "../../incl/MinHook.h"
#include "../core/ModsButton.h"

void MenuLayer::hook()
{
	MH_CreateHook(
		reinterpret_cast<void*>(_MENU_LAYER),
		MenuLayer::_init,
		(LPVOID*)(&MenuLayer::init)
	);
}

bool __fastcall MenuLayer::_init(CCLayer* self)
{
	// create button

	auto sprite = CCSprite::createWithSpriteFrameName("GJ_button_01.png");
	auto modsbtn = ModsButton::create(sprite, sprite, self, menu_selector(ModsButton::callback));

	// init layer
	bool res = init(self);
	auto children = self->getChildren();
	
	CCMenu* bottomMenu = (CCMenu*)children->objectAtIndex(3);
	bottomMenu->addChild(modsbtn, 500);
	
	return res;
}