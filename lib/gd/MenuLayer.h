#pragma once
#include <cocos2d.h>

using namespace cocos2d;

class MenuLayer
{
public:
	static void hook();
	static void unhook();

	CCLayer* self;
	
	static bool(__thiscall* init)(cocos2d::CCLayer* self);
	static bool __fastcall _init(CCLayer* self);
};