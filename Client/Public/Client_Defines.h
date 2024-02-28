#pragma once

#include <process.h>
#include <time.h>
#include <string>

namespace Client
{
	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TOWN, LEVEL_GAMEPLAY, LEVEL_ENDING, LEVEL_END };

	/* 클라이언트에서 사용하기위한 여러 정의들을 모아둘꺼야. */
	const unsigned int	g_iWinSizeX = 1280;
	const unsigned int	g_iWinSizeY = 720;
}

extern HWND		g_hWnd;
extern HINSTANCE	g_hInst;

extern unsigned int g_CurLevel;
extern unsigned int g_PlayerLevel;
extern unsigned int g_PlayerEXP;
extern unsigned int g_AmountOfBullet;
extern int g_BulletCount;

extern bool g_bTownEnd;
extern bool g_bGamePlayEnd;

extern bool g_bMissionClear;
extern bool g_bFirstMissionClear;
extern bool g_bSceondMissionClear;
extern bool g_bThirdMissionClear;

extern bool g_bFirstDoorOpened;
extern bool g_bSecondDoorOpened;
extern bool g_bSkagDead;
extern bool g_bSpiderAntDead;
extern bool g_bThresherDead;

extern bool g_bCamShake;

extern bool g_bCollidedNPC;

extern bool g_bMonsterCollidedMouse;
extern bool g_bPsychoCollidedMouse;
extern bool g_bPsychoCollidedMouse2;
extern bool g_bPsychoCollidedMouse3;
extern bool g_bGoliathCollidedMouse;
extern bool g_bGoliathCollidedMouse2;
extern bool g_bGoliathCollidedMouse3;
extern std::wstring	g_strMonsterName;
extern bool g_bDestroyedName;
extern int	g_iMonsterHp;

extern bool	g_bThersherSummon;
extern int g_ArmNumber;
extern int g_Arm2Number;
extern int g_MeleeNumber;
extern int g_CollidedArmNumber;
extern int g_CollidedArm2Number;
extern int g_CollidedMeleeNumber;


extern bool	g_bTreasureUsed;
extern bool	g_bInteractionItem;
extern bool	g_bInteractionNPC;
extern bool	g_bInteractionDestroy;
extern int	g_InteractionCount;
extern int	g_DialogueCount;

extern bool g_bSpiderAntScene;
extern bool g_bSkagScene;
extern bool g_bThresherScene;
extern bool g_bMonsterScene;

extern bool g_bBossWave;

extern float g_fViewX;
extern float g_fViewY;

using namespace Client;