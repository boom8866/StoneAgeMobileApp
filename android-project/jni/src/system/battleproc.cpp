/************************/
/*	battleProc.cpp		*/
/************************/
#include "../systeminc/version.h"
#include "../systeminc/system.h"
//#include "../caryIme.h"
//#include "../systeminc/ime_sa.h"
#include "../systeminc/map.h"
#include "../systeminc/menu.h"
#include "../systeminc/battlemap.h"
#include "../systeminc/battleProc.h"
#include "../systeminc/battleMenu.h"
#include "../systeminc/battleMap.h"
#include "../systeminc/produce.h"
#include "../systeminc/lssproto_cli.h"
#include "../systeminc/netmain.h"
#include "../systeminc/t_music.h"
#include "../systeminc/pc.h"
#include "../systeminc/character.h"
#include "../systeminc/handletime.h"
#include "../systeminc/savedata.h"
#include "../systeminc/field.h"

extern	ACTION* oft_test();
extern void damage_num( ACTION *a0 );
extern void WindowDisp( ACTION *a0 );
extern int piyo_point;
extern void set_bc( void );
extern void disp_kanji(ACTION *a0);
extern	bool	bNewServer;

#ifdef _PROFESSION_ADDSKILL
extern ACTION *boundary_2,*boundary_mark[2];
#endif

#ifdef __ATTACK_MAGIC

// Global vars
extern int			g_iRunEarthQuake;			// �����״��: 0 --> û���� , 1 --> ��ʼ������ , 2 --> ������
extern int			g_iCurRunEarthQuake;		// Ŀǰ�����λ����
extern int			g_iNumRunEarthQuake;		// ������ܴ���

#endif

extern bool BattlingFlag;  

int flash_pal_cnt;
PALETTEENTRY	Palette2[256];	// ?????�B�l

// ?????????
bool EncountFlag = false;
// ???????
bool DuelFlag = false;
// ???��???
bool NoHelpFlag = false;
#ifdef _MAGIC_NOCAST//��Ĭ
bool NoCastFlag = false;
#endif
// ???????????
bool EncountOffFlag = false;

// ?��??�K��?????
bool Battle1P2PFlag;

// ��?�e?����?????
ACTION* pActSurprisalWnd;
ACTION* pActAudienceExitWnd;

#ifdef _NEWDRAWBATTLEMAP		   // (���ɿ���) Syu ADD �Զ�����BattleMap
static ACTION *BattleWaterAct[8];   //��������
#endif

// �u��??????
extern int check_all_dead( void );

// ????????
extern char att_select_flg;


// ???
#ifdef _DEBUG

// ??????????
int BattleDebTurnNo = 0;
// ?????????
char *BattleBcDeb[] =
{
#include "../systeminc/BattleBcDeb.h"
};
// ??????????
char *BattleCmdDeb[] =
{
#include "../systeminc/BattleMvDeb.h"
};

#endif



	extern 
/* �P�f���� *********************************************************/
void battle_quake( void )
{
	switch(quake_vct_no){
	case 0:		//??��?
		break;
	case 1:		//����?��
		quake_flg = 60;
		quake_vct_no = 2;
		break;
	case 2:		//����?
		if(quake_flg >= 16){
			piyo_point += 16;
		} else {
			piyo_point += quake_flg;
		}
		piyo_point &= 63;
		if(!--quake_flg){		//������???
			quake_vct_no = 0;
		}
		break;
	}

	#ifdef __ATTACK_MAGIC

	switch( g_iRunEarthQuake )
	{
		// û����
		case 0:

			break;

		// ��ʼ������
		case 1:

			g_iRunEarthQuake = 2;
			break;

		// ������
		case 2:

			if( g_iNumRunEarthQuake >= 16 )
				g_iCurRunEarthQuake += 16;
			else
				g_iCurRunEarthQuake += g_iNumRunEarthQuake;

			g_iCurRunEarthQuake &= 63;

			if( 0 == --g_iNumRunEarthQuake )
				g_iRunEarthQuake = 0;

			break;

	}

	#endif
}

/* ?��????? *********************************************************/
void die_flash( void )
{
	int d0 = 0;
	int d1,d7;
	return;
	switch(flash_vct_no){
	case 0:
		break;
//??????????  ?��  ??????????
	case 1:
		flash_vct_no++;
		break;

	case 2:
		for( d7 = 0; d7 < 10; d7++ ){
			Palette2[d7].peBlue  	= Palette[d7].peBlue;
			Palette2[d7].peGreen 	= Palette[d7].peGreen;
			Palette2[d7].peRed 		= Palette[d7].peRed;
			Palette2[d7].peFlags 	= Palette[d7].peFlags;

			Palette2[d7+246].peBlue  = Palette[d7+246].peBlue;
			Palette2[d7+246].peGreen = Palette[d7+246].peGreen;
			Palette2[d7+246].peRed 	= Palette[d7+246].peRed;
			Palette2[d7+246].peFlags = Palette[d7].peFlags;
		}
		for( d7 = 10; d7 < 246; d7++ ){		//??????????
			Palette2[d7].peBlue = Palette[d7].peBlue;
			Palette2[d7].peGreen = Palette[d7].peGreen;
			Palette2[d7].peRed = 0xff;
			Palette2[d7].peFlags = Palette[d7].peFlags;
		}
		//lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette2 );
		flash_pal_cnt = 20;
		flash_vct_no++;
		break;

	case 3:
		if(--flash_pal_cnt){
			break;
		}
		flash_vct_no++;
		break;

	case 4:
		for(d7=10; d7<246; d7++){
			if(Palette2[d7].peRed == Palette[d7].peRed){
				continue;
			}
			d0 = 1;
			d1 = Palette2[d7].peRed - Palette[d7].peRed;
			if(d1 > 3)
				d1 = 3;
			if(d1 < -3)
				d1 = -3;
			Palette2[d7].peRed -= d1;
		}
		if(WindowMode){		//??????????
			flash_pal_cnt++;
			if(flash_pal_cnt == 10){		//��??�u??
				flash_pal_cnt = 0;
				//lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette2 );
			} else {
				d0 = 1;		//��????
			}
		} else {
			//lpDraw->lpPALETTE->SetEntries( 0, 0, 256, Palette2 );
		}
		if(d0 == 0){		//��???
			flash_vct_no = 0;
		}
		break;
	}
}

/* ???????�k���� *********************************************************/
void damage_dispx( void )
{
	ACTION *pActLoop = pActTop->pNext; 	/* �I�d????????��x */
	ACTION *pActLoopBak;  /* ?��???????? */
	// ?????	
	while(1){
		/* ??��???????? */	
		if( pActLoop == pActBtm ) break;
		/* ��????? */
		if( pActLoop->deathFlag == false ){
			/* Pd( pActLoop->prio ); */
			/* ?????????��? */
			if( pActLoop->func == damage_num ){
				damage_num(pActLoop);
			}
			if( pActLoop->func == WindowDisp ){
				WindowDisp(pActLoop);
			}
			if( pActLoop->func == disp_kanji ){
				disp_kanji(pActLoop);
			}
			/* ?????????? */
			pActLoop = pActLoop->pNext;
		}else{	/* ?????? */
			/* �q????? */
			pActLoop->pPrev->pNext = pActLoop->pNext;
			pActLoop->pNext->pPrev = pActLoop->pPrev;
			/* ?????? */
			pActLoopBak = pActLoop->pNext;
			/* ??????�� */
			ClearAction( pActLoop );
			/* ???????? */
			pActLoop = pActLoopBak;
		}
	}
}

// ��?�e?����?? **************************************************************/
void SurprisalDisp( void )
{
	// ��???????????
	if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL || BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
		// ?????��????
		if( pActSurprisalWnd == NULL ){
			// ?????����????��
			pActSurprisalWnd = MakeWindowDisp( 320 - 96, 240 - 48, 3, 2, NULL, 1 );
			// ????????
			play_se( 202, 320, 240 );
		}
		// ?????��????????
		if( pActSurprisalWnd->hp > 0 ){
			// ????????��????
			if( MenuToggleFlag & JOY_CTRL_E || MenuToggleFlag & JOY_CTRL_A ){ 
				if( pActSurprisalWnd->x < 320 - 96 + 56 ) pActSurprisalWnd->x += 6;
			}else{ 
				if( pActSurprisalWnd->x > 320 - 96 ) pActSurprisalWnd->x -= 6;
			}
			// ��?��???????��???????
			if( BattleBpFlag & BATTLE_BP_ENEMY_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, " ���͵Ϯ ", 0 );
			}
			// ??????��??????
			if( BattleBpFlag & BATTLE_BP_PLAYER_SURPRISAL ){
				StockFontBuffer( pActSurprisalWnd->x + 38, pActSurprisalWnd->y + 40, FONT_PRIO_FRONT, 0, "���䲻��Ĺ���", 0 );
			}
		}
	}
}

int xxx;

/*Battle Process����******************************************************************/
#ifdef _AI_OTHER
extern int AI_OtherSetting[1];
#endif

void BattleProc( void )
{
	int i;
	int z;
	s_timer++;	
	xxx = 0;
#ifndef _STREET_VENDOR
	#ifdef _TABLEOFSKILL
		extern short skillBtn ;
		skillBtn = 0 ; 
	#endif
#endif
#ifdef _FRIENDCHANNEL      //ROG ADD ����Ƶ��
	extern short chatRoomBtn ;
	chatRoomBtn = 0;
	SelRoomBtn = 0;
#endif

	switch( SubProcNo ){
		case BATTLE_SUBPROC_INIT:	//���ڻ�
#ifdef __AI
			extern char *AI_Choosen;
			AI_Choosen = NULL;
#endif
			BattlingFlag = true;  

#ifdef _PROFESSION_ADDSKILL
			if( boundary_2 != NULL ){
				DeathAction( boundary_2 );
			    boundary_2 = NULL;
			}
			if( boundary_mark[0] ){
				DeathAction( boundary_mark[0] );
				boundary_mark[0] = NULL;
			}
			if( boundary_mark[1] ){
				DeathAction( boundary_mark[1] );
				boundary_mark[1] = NULL;
			}
#endif
			//Action����
			DeathAllAction();

			// ս����ʼʱ�ͷŵ�ˮ�������еĶ���
#ifdef _AniRandom   // Syu ADD ���������������
			extern void AniRandomRelease();
			AniRandomRelease();
#endif
#ifdef _AniCrossFrame	   // Syu ADD �������ι���������
			extern void crossAniRelease();
			crossAniRelease();
#endif
#ifdef _AniCharBubble	   // Syu ADD �����������³�����
			extern void CharbubbleRelease();
			CharbubbleRelease();
#endif
#ifdef _AniImmobile	 // Syu ADD ��������ض�����
			extern void ImmobileAniRelease();
			ImmobileAniRelease();
#endif
#ifdef _SPECIALSPACEANIM	// Syu ADD ���ⳡ����������
			extern void ReleaseSpecAnim();
			ReleaseSpecAnim();
#endif
#ifdef _NEWDRAWBATTLEMAP		   // Syu ADD �Զ�����BattleMap
			if ( nowFloor == 817 || nowFloor == 8007 || nowFloor == 8101 || nowFloor == 8100 || 
				nowFloor == 8027 || nowFloor == 8028 || nowFloor == 8029 || nowFloor == 8015 || nowFloor == 8113 || nowFloor == 8114 )
			{
				for ( z = 0 ; z < 8 ; z ++ ) 
				{
					if ( BattleWaterAct [ z ] == NULL )
					{
						BattleWaterAct[ z ] = MakeAnimDisp ( 
						( 80 * (z + 1) ), 
						Rnd ( 5 , 450 ) , 
						Rnd( 101502 , 101509) , 
						0);
						BattleWaterAct[ z ]->dispPrio = 30 ;
					}
				}
			}
#endif
			slow_flg = 0;
			action_inf = 0;
			oft_test();
			// Menu��ʼ��
			InitMenu();
			// BattleMenu��ʼ��
			InitBattleMenu();
			pActSurprisalWnd = NULL;
			pActAudienceExitWnd = NULL;
			// Produce��ʼ��
			ProduceInitFlag = true;
			// ����focusȡ��
			GetKeyInputFocus( &MyChatBuffer );
			// ?��????��
			// Buffer��ʼ��
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			// BattleMap��ȡ
			ReadBattleMap( BattleMapNo );
			ChatProc();				// Chat����
			ChatBufferToFontBuffer(); // ??????????????????????
			// ???????????????
			ClearBackSurface();	
			// ????????????????
#ifdef __SKYISLAND
			fastDrawTileFlag = 0;
			PutBmp();	
			fastDrawTileFlag = 1;
#else
			PutBmp();	
			// ??????????????????????
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
#endif
			// ???????
			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			//?�e��???????
			if(DuelFlag == true || eventEnemyFlag == 1 || vsLookFlag == 1)
			{
#ifdef _NEWMUSICFILE6_0		   // Syu ADD 6.0 �µ�ͼ����
				if ( nowFloor != 817 && nowFloor != 8007 && nowFloor != 8101 && nowFloor != 8100 && 
					nowFloor != 8027 && nowFloor != 8028 && nowFloor != 8029 && nowFloor != 8015 && nowFloor != 8113 && nowFloor != 8114 )
				{
#endif
					//BossսBGM����
					if( map_bgm_no>=15 && map_bgm_no<=21)
						play_bgm( 13 );
					else
						play_bgm( 6 );
					if(nowFloor == 8519) play_bgm(14);
#ifdef _NEWMUSICFILE6_0		   // Syu ADD 6.0 �µ�ͼ����
				}
				else	play_bgm ( 24 ) ;
#endif
			}
			else
			{
#ifdef _NEWMUSICFILE6_0		   // Syu ADD 6.0 �µ�ͼ����
				if ( nowFloor != 817 && nowFloor != 8007 && nowFloor != 8101 && nowFloor != 8100 && 
					nowFloor != 8027 && nowFloor != 8028 && nowFloor != 8029 && nowFloor != 8015 && nowFloor != 8113 && nowFloor != 8114 )
				{
#endif
					//ͨ��ս��BGM����
					if( map_bgm_no>=15 && map_bgm_no<=21)
						play_bgm( 12 );
					else
						play_bgm( 5 );
#ifdef _NEWMUSICFILE6_0		   // Syu ADD 6.0 �µ�ͼ����
				}
				else
					play_bgm ( 24 ) ;
#endif
			}
			// ????????
			NowTime = TimeGetTime();
			// ????????��?������?
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// ?��?
			DrawProduce( PRODUCE_HAGARE_OCHI_IN );
			// ����?��???
			MenuProc();				// ??????
			////ImeProc();				// ???????
			// ���� field menu ???????????
			fieldProc2();
			// menu flag on ????????? 
			battleMenuFlag2 = true;
			// ������ ??????????
			for( i = 0 ; i < 5 ; i++ ){
				// ????????????
				if( pet[ i ].useFlag == false ){
					// ??????
					if( i == pc.battlePetNo ) pc.battlePetNo = -1;
					// ?????
					if( i == pc.mailPetNo ) pc.mailPetNo = -1;
					// ??????????
					pc.selectPetNo[ i ] = false;
				}
			}
			// ????????
			saveUserSetting();
			// ????�k????
			SubProcNo++;
			break;
			
		case BATTLE_SUBPROC_IN_PRODUCE:			// ??????��
			// ת�� ?��?
			if( DrawProduce( PRODUCE_HAGARE_OCHI_IN ) == true ){
				// ???????
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// ??????��????????????????�� */
				ReadBattleMap( BattleMapNo );
				// ???????????????
				ClearBackSurface();	
				// ????????????????
#ifdef __SKYISLAND
				fastDrawTileFlag = 0;
				PutBmp();	
				fastDrawTileFlag = 1;
#else
				PutBmp();	
				// ??????????????????????
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
#endif
				// ???????
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// ????????
				NowTime = TimeGetTime();
				// ????????��?������?
				BackBufferDrawType = DRAW_BACK_BATTLE; 
				// ?????????��?
				/* ?????? */
				ChatProc();
				// ??????????????????????
				ChatBufferToFontBuffer(); 
				// ?????????����??
				FlashKeyboardCursor();
				// ??????
				MenuProc();
				// ???????
				////ImeProc();
				// ???????????
				fieldProc2();
				SubProcNo++;
			}else{
				MenuProc();	// ??????
				////ImeProc();	// ???????
				// ???????????
				fieldProc2();
			}
			break;
			
		case BATTLE_SUBPROC_RECEIVE_BC:			//ÿ�غ���ʼ����״̬��ʾ
#ifdef _DEBUG
			if(offlineFlag == true){
				strcpy(BattleStatus,"BC|3|"
				"0|ishikawa|title|18768|1|20|20|0|"
				"1|ishikawa|title|18768|1|20|20|0|"
				"2|ishikawa|title|18768|1|20|20|0|"
				"3|ishikawa|title|18768|1|20|20|0|"
				"4|ishikawa|title|18768|1|20|20|0|"
				"5|beron|title|18813|1|20|20|0|"
				"6|beron|title|18813|1|20|20|0|"
				"7|beron|title|18813|1|20|20|0|"
				"8|beron|title|18813|1|20|20|0|"
				"9|beron|title|18813|1|20|20|0|"
				"A|ishikawa|title|18813|1|20|20|0|"
				"B|ishikawa|title|18813|1|20|20|0|"
				"C|ishikawa|title|18813|1|20|20|0|"
				"D|ishikawa|title|18813|1|20|20|0|"
				"E|ishikawa|title|18813|1|20|20|0|"
				"F|beron|title|187BA|1|20|20|0|"
				"10|beron|title|187BA|1|20|20|0|"
				"11|beron|title|187BA|1|20|20|0|"
				"12|beron|title|187BA|1|20|20|0|"
				"13|beron|title|187BA|1|20|20|0|"
				);
	strcpy( BattleStatus, BattleBcDeb[ BattleDebTurnNo ] );
				// ??�@�e
				set_bc();
				SubProcNo++;
				break;
			}
#endif
			/* ?????? */
			ChatProc();
/*
#ifndef __AI
			//cary �����ͣ�Զ�����
			if( PauseAI == 2)
				PauseAI = 0;
			if( PauseAI == 1)
				PauseAI = 2;
			extern int battleButtonBak;
			if( AI == AI_SELECT)
				battleButtonBak = -1;
#endif
*/
			// ??????????????????????
			ChatBufferToFontBuffer(); 
			// ?????????����??
			FlashKeyboardCursor();
			// ??????
			MenuProc();
			// ???????
			//ImeProc();
			// ???????????
			fieldProc2();
			// ??�q?????????????
			TimeZoneProc();
			// �ӡP??
			if( BattleBpFlag & BATTLE_BP_JOIN ){
				// ??????
				strcpy( OneLineInfoStr,"�ȴ��غϽ�����");
			}
			// ?????????????????
			if( EncountFlag == false ){
				// ??????????
				if( !( s_timer & 7 ) ){
					piyo_point++;
					piyo_point &= 63;
				}
				/* ????????? */
				RunAction();
				// ???����????????????
				StockTaskDispBuffer();
			}
			if( BattleStatusReadPointer != BattleStatusWritePointer ){
				strcpy( BattleStatus, BattleStatusBak[ BattleStatusReadPointer ] );
				BattleStatusReadPointer = ( BattleStatusReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				// ????????????
				EncountFlag = false;
				// ??�@�e
				set_bc();

				// Robin 0729 ride Pet
				if( BattleMyNo < 20 )	{
					if( p_party[ BattleMyNo ]->petFall == 2 )	{
						lssproto_TK_recv( sockfd, 0, "P|���������������������˳�ս����", FONT_PAL_YELLOW);
						p_party[ BattleMyNo ]->petFall = 0;
					}else if( p_party[ BattleMyNo ]->onRide == -1 )	{
						lssproto_TK_recv( sockfd, 0, "P|����������˹��أ����˳�ս����", FONT_PAL_YELLOW);
					}
#ifdef _PETSKILL_BECOMEFOX
					else if( p_party[ BattleMyNo ]->onRide == -2 )	{
						lssproto_TK_recv( sockfd, 0, "P|�������Ļ������������˳�ս����", FONT_PAL_YELLOW);				
					}
#endif
#ifdef _PETSKILL_BECOMEPIG
                    else if( p_party[ BattleMyNo ]->onRide == -3 )	{
						lssproto_TK_recv( sockfd, 0, "P|�㴦�����������������˳�ս����", FONT_PAL_YELLOW);				
					}
#endif
#ifdef _PETSKILL_EXPLODE
					else if( p_party[ BattleMyNo ]->onRide == -4 )	{
						lssproto_TK_recv( sockfd, 0, "P|�����˱��ѹ������������˳�ս����", FONT_PAL_YELLOW);				
					}
#endif 
				}


				// �P�f??��????????
				CheckBattleNewPet();
				// ?�P???????
				if( BattleMyNo < 20 ){
					// ???�@�e
					p_party[ BattleMyNo ]->mp = BattleMyMp;
				}
				// ?��??�K��??????
				Battle1P2PFlag = CheckBattle1P2P();
				SubProcNo++;
			}
			// ?�P?????????����
			if( BattleMyNo >= 20 ){
				// ?????��????
				if( pActAudienceExitWnd == NULL ){
					// ?????����????��
					pActAudienceExitWnd = MakeWindowDisp( 444, 4, 3, 2, NULL, 1 );
					// ????????
					play_se( 202, 320, 240 );
				}
			}
			break;
			
		case BATTLE_SUBPROC_CHAR_IN:			// �K��?��
			// ??????????l
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// ��?�e?����??
			SurprisalDisp();
			// ????�V???????????
			CheckBattleAnimFlag();
			/* ????????? */
			RunAction();
			// ???����????????????
			StockTaskDispBuffer();
			/* ?????? */
			ChatProc();
			// ??????????????????????
			ChatBufferToFontBuffer(); 
			// ?????????����??
			FlashKeyboardCursor();
			// ??????
			MenuProc();
			// ???????
			//ImeProc();
			// ???????????
			fieldProc2();
			// ??�q?????????????
			TimeZoneProc();
			// ���q?����
			BattleNameDisp();
			// �K����???
			if( action_inf == 3 ){
				action_inf = 0;
				// �P�f�ӡP??????
				BattleBpFlag &= ~BATTLE_BP_JOIN; 
				// �u��??????
				if( check_all_dead() == 1 ){
					SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
					break;
				}
				// ��?�e??????��????
				if( pActSurprisalWnd != NULL ){
					// ??????��
					DeathAction( pActSurprisalWnd );
					pActSurprisalWnd = NULL;
					// ��?�e???????
					BattleBpFlag &= ~BATTLE_BP_ENEMY_SURPRISAL; 
					BattleBpFlag &= ~BATTLE_BP_PLAYER_SURPRISAL; 
					
				}
				// ???????
				SubProcNo++;
			}
			//�P�f�D��??
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//�P�f��??��?
				action_inf = 0;
				// ??????????
				BattleCmd[ 0 ] = NULL;
			}
			break;
			
		case BATTLE_SUBPROC_CMD_INPUT:			// ????�V?
#ifdef _DEBUG
			if(offlineFlag == true){
				SubProcNo++;
				break;
			}
#endif
			// ??????????
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
#ifndef __AI
			if( mouse.onceState == MOUSE_RIGHT_CRICK)
				PauseAI = 2;
			// ?????????  
			BattleMenuProc();
#else
			extern void AI_ChooseAction();
			if( AI == AI_SELECT){
				void CloseInfoWnd();
				CloseInfoWnd();
				AI_ChooseAction();
			}else
				BattleMenuProc();
#endif
			// ????�V???????????
			CheckBattleAnimFlag();
			/* ????????? */
			RunAction();
			// ???����????????????
			StockTaskDispBuffer();
			/* ?????? */
			ChatProc();
			// ??????????????????????
			ChatBufferToFontBuffer(); 
			// ?????????����??
			FlashKeyboardCursor();
			// ??????
			MenuProc();
			// ???????
			//ImeProc();
			// ???????????
			fieldProc2();
			// ??�q?????????????
			TimeZoneProc();
			// ���q?����
			BattleNameDisp();
			break;
			
		case BATTLE_SUBPROC_RECEIVE_MOVIE:		// ????����r?
#ifdef _DEBUG
			if(offlineFlag == true){
				strcpy( BattleCmd,"BY teki_A jibun_0 flg_0 damage_0 jibun_1 flg_0 damage_0 jibun_2 flg_0 damage_0"
				" jibun_3 flg_0 damage_0 jibun_4 flg_0 damage_0 jibun_5 flg_0 damage_0"
				" jibun_6 flg_0 damage_0 jibun_7 flg_0 damage_0 jibun_8 flg_0 damage_0"
				" jibun_9 flg_0 damage_1 FF");
				strcpy( BattleCmd, BattleCmdDeb[ BattleDebTurnNo ] );
				BattleMyNo = 0;
				//????????
				att_select_flg = false;
				SubProcNo++;
				break;
			}
#endif
#ifndef __AI
			if( mouse.onceState == MOUSE_RIGHT_CRICK)
				PauseAI = 1;
#else
			extern char *AI_Choosen;
#endif
			// ??????????
			if( !( s_timer & 7 ) ){
				piyo_point++;
				piyo_point &= 63;
			}
			// ????�V???????????
			CheckBattleAnimFlag();
			/* ????????? */
			RunAction();
			// ???����????????????
			StockTaskDispBuffer();
			/* ?????? */
			ChatProc();
			// ??????????????????????
			ChatBufferToFontBuffer(); 
			// ?????????����??
			FlashKeyboardCursor();
			// ??????
			MenuProc();
			// ???????
			//ImeProc();
			// ???????????
			fieldProc2();
			// ??�q?????????????
			TimeZoneProc();
			// ���q?����
			BattleNameDisp();
			// ???????�r?
			if( BattleCmdReadPointer != BattleCmdWritePointer ){
				strcpy( BattleCmd, BattleCmdBak[ BattleCmdReadPointer ] );
				BattleCmdReadPointer = ( BattleCmdReadPointer + 1 ) & ( BATTLE_BUF_SIZE-1 );
				action_inf = 0;
				//?????????
				att_select_flg = false;
				SubProcNo++;
			}
			// �K��???
			if( Battle1P2PFlag == 2 ){
				// ??????
				strcpy( OneLineInfoStr,"�ȴ�������ҡ�");
			}
			// ?�P??
			if( BattleMyNo >= 20 ){
				// ��?��???
				for( i = 0 ; i < 20 ; i++ ){
					// �o??????����
					HpMeterDisp( i );
				}
			}else{
				// ????????
				if( BattleMyNo < 10 ){
					for( i  = 0 ; i < 10 ; i++ ) HpMeterDisp( i ); // �o??????����
				}else 
				// ????????
				if( BattleMyNo >= 10 ){ 
					for( i = 10 ; i < 20 ; i++ ) HpMeterDisp( i ); // �o??????����
				}
			}
			break;
			
		case BATTLE_SUBPROC_MOVIE:				// ??????
#ifdef _DEBUG
			if(offlineFlag == true){
				if(joy_trg[0]&JOY_A){		//??????
					DeathAllAction();
					SubProcNo = 0;
					BattleCmd[0] = NULL;
					break;
				}
			}
#endif
			//????????��???
#ifndef __AI
			if( mouse.onceState == MOUSE_RIGHT_CRICK)
				PauseAI = 1;
#else
			if( AI_Choosen){
				
				StockFontBuffer( 640-strlen(AI_Choosen)*7-4 + DISPLACEMENT_X, 440 + DISPLACEMENT_Y, FONT_PRIO_FRONT, FONT_PAL_YELLOW, AI_Choosen, 0 );
			}
#endif
			if( slow_flg ){		
				if( !( s_timer & 31 ) ){	// ????????
					piyo_point++;	// ?????????��???
					piyo_point &= 63;	// ????????
				}
				if( s_timer & 3 ){		// ????????????????????
					damage_dispx();		// ?????????�k����
				}else{
					RunAction();		/* ????????? */
					battle_quake();		//�P�f����
				}
			} else { // ???????
				if( !( s_timer & 7 ) ){	
					piyo_point++;
					piyo_point &= 63;
				}
				RunAction();		/* ????????? */
				battle_quake();		//�P�f����
			}
			//?��???????
			if( flash_vct_no ) die_flash();	//?��?????
			// ???����????????????
			StockTaskDispBuffer();
			/* ?????? */
			ChatProc();
			// ??????????????????????
			ChatBufferToFontBuffer(); 
			// ?????????����??
			FlashKeyboardCursor();
			// ??????
			MenuProc();
			// ???????
			//ImeProc();
			// ???????????
			fieldProc2();
			// ??�q?????????????
			TimeZoneProc();
			// ���q?����
			//BattleNameDisp();
			//???��???
			if( action_inf == 1 ){		
				SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//?????

				BattleCliTurnNo++;							// ?????????��??

#ifdef _DEBUG
				if(offlineFlag == true){
					SubProcNo = BATTLE_SUBPROC_RECEIVE_BC;		//?????????
					BattleDebTurnNo++;
					// ????????
					if( BattleDebTurnNo >= sizeof( BattleCmdDeb )/sizeof(int) ) BattleDebTurnNo = 0;
				}
#endif
				action_inf = 0;
				// ??????????
				BattleCmd[ 0 ] = NULL;
				// ????????? 
				battleMenuFlag2 = true;
			}
			//�P�f�D��??
			if( action_inf == 2 ){		
				SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;		//�P�f��??��?
				action_inf = 0;
				// ??????????
				BattleCmd[ 0 ] = NULL;
			}
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE_INIT:	// ��??��???
			//??????????
			fade_out_bgm();
			// ????????????�� 
			CopyBackBuffer();
			// ????????��?������?
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
#ifdef _NEWDRAWBATTLEMAP		   // (���ɿ���) Syu ADD �Զ�����BattleMap
			extern int RandBattleBg ; 
			RandBattleBg = 0 ;
			for ( z = 0 ; z < 8 ; z ++ ) 
			{
				if ( BattleWaterAct[ z ] != NULL ) 
				{
					DeathAction(BattleWaterAct[ z ]);
					BattleWaterAct[ z ] = NULL;
				}
			}
#endif
			break;
			
		case BATTLE_SUBPROC_OUT_PRODUCE:		// ��??��
			// ?��?
			if( DrawProduce( PRODUCE_4WAY_OUT ) == true ){
				// ????????
				ChangeProc( PROC_GAME, 1 );
				// ?????�u?��
				DeathAllAction();
				// ?????????????????��

				BattlingFlag = false;

				clearPtActCharObj();
				// ????????????????��?��??
				encountNowFlag = 0;
				// ????????????????��?????????
				if( bNewServer)
					lssproto_EO_send( sockfd, 0 );
				else
					old_lssproto_EO_send( sockfd, 0 );
				//??????????��
				// �P�f???????��???�`????????????��???
				if( battleResultMsg.useFlag >= 1 ){ 
					BattleResultWndFlag = battleResultMsg.useFlag;	// ?????????
					battleResultMsg.useFlag = false; // ?�B�l???????
				}

#ifdef _AI_OTHER
				if(AI_OtherSetting[0] && (AI == AI_SELECT)){
					ITEM *pItem = pc.item;

					for(int i=0;i<15;i++){
						// ���������ϵĵ�����û����
						if(pItem[i+9].useFlag && (pItem[i+9].graNo >= 24000 && pItem[i+9].graNo <= 24044)){
							// ����
							if(bNewServer) lssproto_DI_send(sockfd,nowGx,nowGy,i+9);
							else old_lssproto_DI_send(sockfd,nowGx,nowGy,i+9);
							break;
						}
					}
				}
#endif
				return;
			}
			break;
			
		case 12:	// ?��
			break;
	}

#ifndef _CHANNEL_MODIFY
	// Robin 0805 channel
	if( bNewServer && joy_trg[1] & JOY_CTRL_C )
	{
		char buf[64];
		if( pc.channel == -1 ) {
			if( (pc.quickChannel != -1) && (pc.familyName[0] != NULL) ) {
				sprintf( buf, "C|J|%d", pc.quickChannel);
				if( bNewServer)
					lssproto_FM_send( sockfd, buf );
				else
					old_lssproto_FM_send( sockfd, buf );
			}
		}
		else{
			if( bNewServer)
				lssproto_FM_send( sockfd, "C|J|-1" );
			else
				old_lssproto_FM_send( sockfd, "C|J|-1" );
		}
	}
#endif
	// ????????????
	if( BackBufferDrawType != DRAW_BACK_PRODUCE ){ 
		// �P�f?����???
		if( BattleEscFlag == true ){	
			// ��??????
			SubProcNo = BATTLE_SUBPROC_OUT_PRODUCE_INIT;
			// ??????
			BattleEscFlag = false;
		}
		// ?�P???????????
		if( mouse.onceState & MOUSE_RIGHT_CRICK && BattleMyNo >= 20 ){
			// ?�P��????????
			if( bNewServer)
				lssproto_B_send( sockfd, "U" );
			else
				old_lssproto_B_send( sockfd, "U" );
		}
		// ????????
		if( pActAudienceExitWnd != NULL ){
			// ?�P��?��?����??????��????????
			if( pActAudienceExitWnd->hp > 0 ){
				// ��??����
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 28, FONT_PRIO_FRONT, 0, 	"�������Ҽ�", 0 );
				StockFontBuffer( pActAudienceExitWnd->x + 30, pActAudienceExitWnd->y + 52, FONT_PRIO_FRONT, 0, 	"������ս", 0 );
			}
		}
	}

}
