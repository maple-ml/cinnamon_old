#pragma once
#include <cocos2d.h>

using namespace cocos2d;


namespace MenuLayer
{
	void hook();
	void unhook();

	CCLayer* self;
	
	inline bool(__thiscall* init)(CCLayer* self);
	bool __fastcall _init(CCLayer* self);
};