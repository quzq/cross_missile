/********************************************************************/
/*																	*/
/*	title:	DirectX core											*/
/*	source name:	directXcore.cpp									*/
/*	coded by:	quz													*/
/*	update:	1999/02/10												*/
/*	comment:														*/
/*		win32API��p�����E�C���h�E�쐬�iDirectX�d�l�j				*/
/*																	*/
/*																	*/
/*																	*/
/********************************************************************/



/*	�C���N���[�h�t�@�C��	*/
#include	<windows.h>
#include	<ddraw.h>
#include	<stdio.h>
#include	<time.h>	/*�����������Ɏg�p*/
#include	"resource.h"

#include	"vstick.h"	/*virturl stick*/
#include	"vbg.h"	/*virturl BG_chr*/

#include	"vwind.h"	/*virturl Sub Window*/
/*snap shot*/
#include	"snapshot.h"

#include	"gamebase.h"


//-----------------------------------------------------------------------------
// Local definitions
//-----------------------------------------------------------------------------
#define	TIMER_ID	1

#define NAME                "CROSSMISSILE"
#define TITLE               "Direct Draw Pazzle Game"
#define	TIMER_ID	1
#define	SPRITE_SURFACE_MAX	3
#define	GAMEMODE_NORMAL		0
#define	GAMEMODE_CONTINUE	1
#define	GAMEMODE_EDIT		2

#define	GAMESCREEN_TITLE		0
#define	GAMESCREEN_PLAY			1
#define	GAMESCREEN_EDIT			2
#define	GAMESCREEN_SELECT		3
#define	GAMESCREEN_END			4

#define	GAMESTAGE_MAX		20	/*�ő�ʐ�*/


/*	�v���g�^�C�v�錾	*/
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT	CALLBACK SubWinProc(HWND, UINT, WPARAM, LPARAM);

static	HRESULT	DirectDrawInit(HWND);
static	void	ReleaseAllObjects(void);
static	HRESULT	InitFail(HWND, HRESULT, LPCTSTR,...);
static	BOOL	RestoreSurfaces(void);
static	void	FlipEachSurfaces(void);
void	DDAppMain_Play( void );
void	DDAppMain_Make( void );
void	DDAppMain_Title( void );
void	DDAppMain_Ending( void );
void	SpriteDebug( void );
bool	SaveGameData( char* );
bool	SaveGameData2( char* );
bool	LoadGameData( char* );
void	InitGameStage(void);

GAMEMES	GameProcedure(GAMEMES,UsrCharacter*);

bool	BGWMoveBreak(UsrBGWindow*);
void	BGWDrawBreak(UsrBGWindow*);
bool	BGWMoveMissile(UsrBGWindow*);
void	BGWDrawMissile(UsrBGWindow*);
bool	BGWMoveCommand(UsrBGWindow*);
void	BGWDrawCommand(UsrBGWindow*);
bool	BGWMoveSelect(UsrBGWindow*);
void	BGWDrawSelect(UsrBGWindow*);
bool	BGWMoveChara(UsrBGWindow*);
void	BGWDrawChara(UsrBGWindow*);
bool	BGWMoveSave(UsrBGWindow*);
void	BGWDrawSave(UsrBGWindow*);
bool	BGWMoveWrite(UsrBGWindow*);
void	BGWDrawWrite(UsrBGWindow*);
bool	BGWMoveLoad(UsrBGWindow*);
void	BGWDrawLoad(UsrBGWindow*);
bool	BGWMoveMsg(UsrBGWindow*);
void	BGWDrawMsgFail(UsrBGWindow*);



/*	�O���[�o���ϐ��錾	*/
char	szClassNme[] = "NewWindowClass";
BOOL	g_bActive = FALSE;   // Is application active?
int		g_SelectBG=0;
int		g_Counter=0;
int		g_PositionX=32*10;
int		g_PositionY=32*7;
int		g_Stage=1;
int		g_MissileRest;
int		g_TargetRest;
int		g_GameMode=GAMEMODE_NORMAL;
int		g_Screen=GAMESCREEN_TITLE;
RECT	g_ScreenRect;
int		g_Wait=0;
int		g_StageClearMax=0;
int		g_DmyParam;

//-----------------------------------------------------------------------------
// Global data	for	DirectDraw
//-----------------------------------------------------------------------------
/*DirectDraw object*/
LPDIRECTDRAW4			g_pDD = NULL;
/*DirectDraw primary surface*/
LPDIRECTDRAWSURFACE4	g_pDDSPrimary = NULL;
/*DirectDraw back surface*/
LPDIRECTDRAWSURFACE4	g_pDDSBack = NULL;
/*DirectDraw�C���[�W�����[�h����I�t�X�N���[���T�[�t�F�X*/
LPDIRECTDRAWSURFACE4	g_pDDSOffScreen[SPRITE_SURFACE_MAX] = {NULL,NULL};
/*DirectDraw�p���b�g*/
IDirectDrawPalette*		g_pDDPalette=NULL;
/*�p���b�g*/
PALETTEENTRY Pal[256];

#define	SP_TABLE_BG	0
#define	SP_TABLE_WINDOW	1
#define	SP_TABLE_BASE	2


UsrCtrlDevice*	Stick=NULL;
UsrBackGround*	BG=NULL;
UsrBGWindow*	SubWind=NULL;
UsrChrObjectMgr*	GCMgr=NULL;

/*include MapData*/
#include	"defmap.h"


const	char	transBG[12]={0,1,2,3,4,5,20,21,22,23,24,25};

void	CheckWindFunc(void);
void	DebugPrint(int,int,char*);
void	SearchFunc(UsrBGWindow*,char*,int);


void	DDAppMain_Ending( void )
{
	RECT	rect_from,rect_to;
	DDBLTFX	ddbltfx;

	// Use the blter to do a color fill to clear the back buffer
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = 0;
	g_pDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	int	s=Stick->GetPos();
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();

	int	size=60;

	rect_from.top		=	64;
	rect_from.bottom	=	rect_from.top+32;
	rect_from.left	=	0;
	rect_from.right	=	rect_from.left+128;
	rect_to.top		=	240-size*3;
	rect_to.bottom	=	rect_to.top+size*2;
	rect_to.left	=	30;
	rect_to.right	=	rect_to.left+	size*4;
	g_pDDSBack->Blt( &rect_to, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLT_KEYSRC | DDBLT_WAIT,NULL);
	rect_from.top		=	64;
	rect_from.bottom	=	rect_from.top+32;
	rect_from.left	=	128;
	rect_from.right	=	rect_from.left+128;
	rect_to.top		+=	(size*2);
	rect_to.bottom	=	rect_to.top+size*2;
	rect_to.left	=	30;
	rect_to.right	=	rect_to.left+	size*4;
	g_pDDSBack->Blt( &rect_to, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLT_KEYSRC | DDBLT_WAIT,NULL);
	rect_from.top		=	64;
	rect_from.bottom	=	rect_from.top+32;
	rect_from.left	=	256;
	rect_from.right	=	rect_from.left+128;
	rect_to.top		+=	(size*2);
	rect_to.bottom	=	rect_to.top+size*2;
	rect_to.left	=	30;
	rect_to.right	=	rect_to.left+	size*4;
	g_pDDSBack->Blt( &rect_to, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLT_KEYSRC | DDBLT_WAIT,NULL);

	HDC	hDC;
	if(g_pDDSBack->GetDC(&hDC) == DD_OK)
	{
		HFONT	hFont;
		HFONT	hFontOld;

		hFont = CreateFont(
		20,			//�t�H���g����
		20,			//������
		0,		//�e�L�X�g�̊p�x
		0,			//�x�[�X���C���Ƃ����Ƃ̊p�x
		FW_BOLD,	//�t�H���g�̏d���i�����j
		TRUE,		//�C�^���b�N��
		FALSE,		//�A���_�[���C��
		FALSE,		//�ł�������
		SHIFTJIS_CHARSET,	//�����Z�b�g
		OUT_DEFAULT_PRECIS,	//�o�͐��x
		CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
		PROOF_QUALITY,		//�o�͕i��
		FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[
		"�l�r ����"	//���̖�
		);
		hFontOld = SelectObject(hDC, hFont);
		SetBkMode(hDC,TRANSPARENT);
		SetTextColor(hDC, RGB(255, 0, 0));
		g_Counter++;
		if( (g_Counter % 16) >5 )TextOut(hDC,280,70,"CONGRATURATIONS",strlen("CONGRATURATIONS"));
		if( g_Counter>255) g_Counter=128;
		TextOut(hDC,280,200,"-CROSS MISSILE-",strlen("-CROSS MISSILE-"));
		TextOut(hDC,280,260,"*SUPPORT*",strlen("*SUPPORT*"));
		TextOut(hDC,280,300,"mail:",strlen("mail:"));
		TextOut(hDC,280,350,"URL:",strlen("URL:"));
		SelectObject(hDC, hFontOld);
		DeleteObject(hFont);
		hFont = CreateFont(
		20,			//�t�H���g����
		8,			//������
		0,		//�e�L�X�g�̊p�x
		0,			//�x�[�X���C���Ƃ����Ƃ̊p�x
		FW_NORMAL,	//�t�H���g�̏d���i�����j
		FALSE,		//�C�^���b�N��
		FALSE,		//�A���_�[���C��
		FALSE,		//�ł�������
		SHIFTJIS_CHARSET,	//�����Z�b�g
		OUT_DEFAULT_PRECIS,	//�o�͐��x
		CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
		PROOF_QUALITY,		//�o�͕i��
		FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[
		"�l�r ����"	//���̖�
		);
		hFontOld = SelectObject(hDC, hFont);
		SetTextColor(hDC, RGB(255, 255, 255));

		TextOut(hDC,280,320,"qwert99@geocities.co.jp",strlen("qwert99@geocities.co.jp"));
		TextOut(hDC,280,370,"http://www.geocities.co.jp/SiliconValley/4977",strlen("http://www.geocities.co.jp/SiliconValley/4977"));

		SelectObject(hDC, hFontOld);
		DeleteObject(hFont);
	
		g_pDDSBack->ReleaseDC(hDC);
	}
	if(g_Counter>64 && ( b1 || b2 ) )
	{
		Stick->SetBtn1(0);
		Stick->SetBtn2(0);
		g_Screen=GAMESCREEN_TITLE;
		g_Counter=0;
	}
}


GAMEMES	GameProcedure(GAMEMES	mes,UsrCharacter*	chara)
{
	RECT	sp_rect;
	char	BGgot;
	switch(mes)
	{
		case	GAMEOBJ_MES_SCREEN_OUT:
		case	GAMEOBJ_MES_TIME_OUT:
			return(GAMEOBJ_MES_TERM);
		case	GAMEOBJ_MES_MAKE_SPREAD:
			sp_rect.top=32;
			sp_rect.bottom=64;
			sp_rect.left=320;
			sp_rect.right=352;
			GCMgr->AddObject(
				new	UsrDefaultSpread(
					ID_CHR_OTHERS,
					chara->GetX()+( (int)rand()%chara->GetWidth() ),
					chara->GetY()+( (int)rand()%chara->GetHeight() ),
					-2,-2,
					&g_ScreenRect,
					&sp_rect,
					&g_pDDSBack,
					&g_pDDSOffScreen[SP_TABLE_BG],
					BG
				)
			);
			break;
		case	GAMEOBJ_MES_TOUCH_BG:
			BGgot=(chara->GetBGObject())->GetBG(chara->GetX(),chara->GetY());
			if( BGgot==20 || BGgot==24 )
			{
				/*�u���b�N����*/
				(chara->GetBGObject())->PutBG(chara->GetX(),chara->GetY(),0);
				if( BGgot==20 )
				{
					g_TargetRest--;
					if( g_TargetRest==0 )g_Wait=20;
				}
				/*Make a Death Spread Object*/
				sp_rect.top=32;
				sp_rect.bottom=64;
				sp_rect.left=(BGgot-20)*32;
				sp_rect.right=sp_rect.left+32;
				GCMgr->AddObject(
					new	UsrDeathSpread(
						ID_CHR_OTHERS,
						( chara->GetX()/32)*32,( chara->GetY()/32)*32,
						0,1,
						&g_ScreenRect,
						&sp_rect,
						&g_pDDSBack,
						&g_pDDSOffScreen[SP_TABLE_BG],
						BG
					)
				);
			}
			return(GAMEOBJ_MES_TERM);
		default:
			break;
	}
	return(mes);
}


void	InitGameStage(void)
{
	char	str[15];

	Stick->SetBtn1( 0 );
	Stick->SetBtn2( 0 );
	if(GCMgr!=NULL)delete	GCMgr;
	GCMgr=new	UsrChrObjectMgr(GameProcedure);
	sprintf(str,"stage%03d.map",g_Stage);
//	if( BG->Load(str)==FALSE )
	if( LoadGameData(str)==FALSE )
	{
		g_PositionX=MapDat[g_Stage-1][0]*32;
		g_PositionY=MapDat[g_Stage-1][1]*32;
		g_MissileRest=MapDat[g_Stage-1][2];
		BG->SetMap(20,15,&(MapDat[g_Stage-1][3]));
	}
	g_TargetRest=BG->SearchBGChara(20);
	g_Wait=20;
	if(g_Stage-1>g_StageClearMax)g_StageClearMax=g_Stage-1;
}

/****************************************************************************/
/*																			*/
/*		�v�����l�������֐�													*/
/*																			*/
/****************************************************************************/
int	WINAPI	WinMain( HINSTANCE hInstance, HINSTANCE hPreInst,
		LPSTR lpszCmdLine, int nCmdShow )
{
	HWND	hWnd;
	MSG	lpMsg;
	WNDCLASS	myProg;

	if( !hPreInst )/*�C���X�^���X�n���h���̃`�F�b�N(32bit�łł�hPreInst�͏��NULL)*/
	{
		/*WNDCLASS�\���̂̊e�����o�̒�`*/

		/*style:CS_HREDRAW�i���������̃T�C�Y�ύX����������S�̂�`�������j	*/
		/*CS_VREDRAW�i���������̃T�C�Y�ύX�őS�̂�`�������j�Ȃǂ̒萔���w��*/
		myProg.style		=	CS_HREDRAW | CS_VREDRAW;
		/*lpfnWndProc:�E�B���h�E �v���V�[�W��(���b�Z�[�W�̏�������ɍs���֐�)��*/
		myProg.lpfnWndProc	=	WndProc;
		myProg.cbClsExtra	=	0;		/*�������̈�̒ǉ��ݒ�̎��g���B�ʏ�0*/
		myProg.cbWndExtra	=	0;		/*�������̈�̒ǉ��ݒ�̎��g���B�ʏ�0*/
		myProg.hInstance	=	hInstance;				/*�C���X�^���X�̓o�^*/
		myProg.hIcon		=	LoadCursor( NULL, "IDI_ICON1" );		/*�t���[�� �E�B���h�E�̃A�C�R��*/
		/*hCursor:�}�E�X���E�B���h�E��ɂ���Ƃ��̃J�[�\��*/
		myProg.hCursor		=	LoadCursor( NULL, IDC_ARROW );
		myProg.hbrBackground=	GetStockObject(WHITE_BRUSH);/*�w�i�F(�w�i)�̓o�^*/
		/*lpszMenuName:���j���[�̖��O��o�^�B�Ȃ��ꍇ��NULL*/
		myProg.lpszMenuName	=	NULL;
		/*lpszClassName:�N���X����o�^�B���̃E�B���h�E�Əd���s�B*/
		myProg.lpszClassName=	szClassNme;

		/*���ɂ���CreateWindow��CreateWindowEx�֐����R�[�����邽�߂�*/
		/*�K�v�ȃE�B���h�E�N���X��o�^����							*/
		if( !RegisterClass( &myProg ) )	return( FALSE );
	}

	g_ScreenRect.top=0;
	g_ScreenRect.bottom=GetSystemMetrics(SM_CYSCREEN);
	g_ScreenRect.left=0;
	g_ScreenRect.right=GetSystemMetrics(SM_CXSCREEN);

	/*�E�C���h�̐���*/
	/*CreateWindow�֐��̖߂�l�����o�����E�B���h�E�̃E�B���h�E�n���h���ƂȂ�B*/
	/*���̃E�B���h�E�ɑ΂��ĉ�������Ƃ��͂��̃E�B���h�E�n���h�����K�v�ɂȂ�B*/
	hWnd = CreateWindowEx(
		WS_EX_TOPMOST,
		szClassNme,				/*class name:�o�^����N���X��*/
 		TITLE,/*window name:�^�C�g���o�[�\���L���v�V����*/
		/*window style:�E�B���h�E�X�^�C���ɂ́uWS_���Ƃ��v���w��B*/
		/*�ʏ��WS_OVERLAPPEDWINDOW�w�肵�Ă����Ɗy�B*/
		/*����́A�uWS_���Ƃ��v�������������Ďw�肵���̂Ɠ����B*/
//		WS_OVERLAPPEDWINDOW,	/*window style */
//		CW_USEDEFAULT,			/*horizontal position of window */
//		CW_USEDEFAULT,			/*vertical position of window */
//		CW_USEDEFAULT,			/*window width */
//		CW_USEDEFAULT,			/*window height */
		WS_POPUP,
		g_ScreenRect.left,
		g_ScreenRect.top,
		g_ScreenRect.right,
		g_ScreenRect.bottom,

		/*handle to parent or owner window:�e�E�B���h�E�̃n���h���B*/
		/*�q���E�B���h�E�����Ƃ��K�v�B�e�����Ƃ���NULL*/
		NULL,					/*handle to parent or owner window */
		/*handle to menu or child-window identifier:���j���[�n���h���B*/
		/*���j���[��t����Ƃ��K�v�B�N���X���j���[(WNDCLASS�ɓo�^���ꂽ���j���[)*/
		/*���g���Ƃ���NULL*/
		NULL,					/*handle to menu or child-window identifier */
		hInstance,				/*handle to application instance */
		/*pointer to window-creation data:NULL���w�肷��*/
		NULL					/*pointer to window-creation data */
	);
	ShowWindow( hWnd, nCmdShow );	/*�E�C���h�̕\����Ԃ�ݒ�*/
	/*The UpdateWindow function updates the client area						*/
	/*of the specified window by sending a WM_PAINT message					*/
	/*to the window if the window�fs update region is not empty.			*/
	/*The function sends a WM_PAINT message directly 						*/
	/*to the window procedure of the specified window,						*/
	/*bypassing the application queue. If the update region is empty,		*/
	/*no message is sent. */
	UpdateWindow(hWnd);		/*�E�C���h�̍ĕ`��iWM_PAINT���b�Z�[�W�̑��M�j*/

	SetFocus(hWnd);	/*�����E�C���h�Ƀt�H�[�J�X*/

	/*DirectDraw�̏�����*/
    if( DirectDrawInit(hWnd) != DD_OK)	return( FALSE );


	/*make instance*/
	Stick=new	UsrCtrlDevice(1);
	BG=new	UsrBackGround(&g_pDDSPrimary,&g_pDDSBack,0,0,20,15,&g_pDDSOffScreen[SP_TABLE_BG]);
	GCMgr=new	UsrChrObjectMgr(GameProcedure);

	if(BG!=NULL)
	{
//		BG->SetMap(20,15,MapDat);
//		g_TargetRest=BG->SearchBGChara(20);
		BG->DrawAll();
	}
	FlipEachSurfaces();

	/****************************/
	/*	���b�Z�[�W���[�v		*/
	/****************************/

	/*The GetMessage function retrieves a message from the calling			*/
	/*thread�fs message queue and places it in the specified structure.		*/
	/*This function can retrieve both messages 								*/
	/*associated with a specified window and thread messages 				*/
	/*posted via the PostThreadMessage function.							*/
	/*The function retrieves messages that lie within a specified range 	*/
	/*of message values. GetMessage does not retrieve messages for windows 	*/
	/*that belong to other threads or applications. 						*/
	/*���b�Z�[�W�̎擾(WM_QUIT���b�Z�[�W���擾����܂Ń��[�v*/
	while( TRUE )
	{
		if( PeekMessage( &lpMsg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if( !GetMessage( &lpMsg, NULL, 0, 0 ) ){
				return lpMsg.wParam;
			}
		/*The TranslateMessage function translates virtual-key messages			*/
		/*into character messages. The character messages						*/
		/*are posted to the calling thread�fs message queue,					*/
		/*to be read the next time the thread calls the GetMessage				*/
		/*or PeekMessage function												*/
			TranslateMessage(&lpMsg);	/*���b�Z�[�W�̖|��*/
		/*The DispatchMessage function dispatches a message						*/
		/*to a window procedure. It is typically used to dispatch a message		*/
		/*retrieved by the GetMessage function. 								*/
			DispatchMessage(&lpMsg);/*�E�B���h�E�E�v���V�[�W���փ��b�Z�[�W�𑗂�*/
		}
	}
	return( lpMsg.wParam );
}

void	DDAppMain_Play( void )
{
	RECT	rect_from;
	HDC	hDC;
	char	str[10];
	int	s=Stick->GetPos();
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();

	if(g_Wait>0)
	{
		s=b1=b2=0;	/*key ����*/
	}


	rect_from.top		=	0;
	rect_from.bottom	=	480;
	rect_from.left	=	0;
	rect_from.right	=	640;

	g_pDDSBack->BltFast( 0,0, g_pDDSOffScreen[SP_TABLE_BASE], &rect_from, DDBLTFAST_WAIT);

	/*game main*/
	if(BG!=NULL)
	{
		RECT	rect;
		int	i;
		int	sp=32;
		BG->DrawAll();

		if( SubWind!=NULL )
		{
			if( SubWind->UpDate()==FALSE )
			{
				SubWind=NULL;
			} else {
				SubWind->DrawAll();
			}
		} else {
			GCMgr->Move();
			GCMgr->Draw();
			
			if( g_Wait > 0)
			{
				g_Wait--;
 				/*Put sprite*/
				rect.top	=	0;
				rect.left	=	128+64;
				rect.right	=	rect.left+64;
				rect.bottom	=	rect.top+64;
				g_pDDSBack->BltFast( g_PositionX-16, g_PositionY-16, g_pDDSOffScreen[SP_TABLE_BG], &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

				if( g_TargetRest!=0 )
				{
					if(g_pDDSBack->GetDC(&hDC) == DD_OK )
					{
						char	str[20];
						HFONT	hFont;
						HFONT	hFontOld;

						hFont= CreateFont(
						80,			//�t�H���g����
						0,			//������
						0,		//�e�L�X�g�̊p�x
						0,			//�x�[�X���C���Ƃ����Ƃ̊p�x
						FW_BOLD,	//�t�H���g�̏d���i�����j
						FALSE,		//�C�^���b�N��
						FALSE,		//�A���_�[���C��
						FALSE,		//�ł�������
						SHIFTJIS_CHARSET,	//�����Z�b�g
						OUT_DEFAULT_PRECIS,	//�o�͐��x
						CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
						PROOF_QUALITY,		//�o�͕i��
						FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[
						"�l�r ����"	//���̖�
						);
						hFontOld = SelectObject(hDC, hFont);
						SetBkColor(hDC, 0);
						SetBkMode(hDC,TRANSPARENT);
						SetTextColor(hDC, RGB(0, 0, 0));
						sprintf(str,"STAGE %02d",g_Stage);
						for(i=0;i<9;i++)
						{
							TextOut(hDC,170+(i/3)*3,120+(i%3)*3,str,strlen(str));
							if( g_Wait % 8 >4 )
							{
								TextOut(hDC,220+(i/3)*3,230+(i%3)*3,"Ready",strlen("Ready"));
							}
						}
						SetTextColor(hDC, RGB(255, 0, 0));
						TextOut(hDC,170+3,120+3,str,strlen(str));
						if( g_Wait % 8 >4 )
						{
							TextOut(hDC,220+3,230+3,"Ready",strlen("Ready"));
						}
						SelectObject(hDC, hFontOld);
						DeleteObject(hFont);
						g_pDDSBack->ReleaseDC(hDC);
					}
				}				
			} else {

				if( s && SubWind==NULL )
				{
					if( s>6 && ( BG->GetBGStatus(g_PositionX,g_PositionY-sp) & BG_STATUS_THROUGH ) )
					{
						if(g_PositionY>1*32) g_PositionY-=sp;
					}else if( s<4 && ( BG->GetBGStatus(g_PositionX,g_PositionY+sp) & BG_STATUS_THROUGH) ){
						if(g_PositionY<13*32) g_PositionY+=sp;
					}else if( ( s % 3 ) == 1 && ( BG->GetBGStatus(g_PositionX-sp,g_PositionY) & BG_STATUS_THROUGH) ){
						if(g_PositionX>1*32) g_PositionX-=sp;
					}else if( ( s % 3 ) == 0 && ( BG->GetBGStatus(g_PositionX+sp,g_PositionY) & BG_STATUS_THROUGH) ){
						if(g_PositionX<18*32) g_PositionX+=sp;
					}
				}
 				/*Put sprite*/
				rect.top	=	0;
				rect.left	=	128+64;
				rect.right	=	rect.left+64;
				rect.bottom	=	rect.top+64;
				g_pDDSBack->BltFast( g_PositionX-16, g_PositionY-16, g_pDDSOffScreen[SP_TABLE_BG], &rect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

				if(g_TargetRest==0)
				{
					g_Stage++;
					if(g_Stage>GAMESTAGE_MAX)
					{
						g_Screen=GAMESCREEN_END;
						g_StageClearMax=GAMESTAGE_MAX;
						g_Counter=0;
					} else {
						InitGameStage();
					}
					return;
				}
			}

			if( b1 && SubWind==NULL )
			{
				Stick->SetBtn1( 0 );
				SubWind=new	UsrBGWindow( &g_pDDSBack,&g_pDDSOffScreen[SP_TABLE_WINDOW],30,34,10,4,BGWMoveBreak,BGWDrawBreak );
				SubWind->InitCursor(0,0,0,1);
			} else if( b2 && g_MissileRest>0 )	{
				int	ms_sp=16;
				Stick->SetBtn2( 0 );
				g_MissileRest--;
				/*Eject Missile */
				rect.top	=	0;
				rect.left	=	320;
				rect.right	=	rect.left+32;
				rect.bottom	=	rect.top+32;
				GCMgr->AddObject(
					new UsrDefaultMissile(
						ID_CHR_GROUP_A,
						g_PositionX,g_PositionY,
						0,-ms_sp,
						1,
						&g_ScreenRect,
						&rect,
						&g_pDDSBack,
						&g_pDDSOffScreen[SP_TABLE_BG],
						BG
					)
				);
				GCMgr->AddObject(
					new UsrDefaultMissile(
						ID_CHR_GROUP_A,
						g_PositionX,g_PositionY,
						0,ms_sp,
						1,
						&g_ScreenRect,
						&rect,
						&g_pDDSBack,
						&g_pDDSOffScreen[SP_TABLE_BG],
						BG
					)
				);
				GCMgr->AddObject(
					new UsrDefaultMissile(
						ID_CHR_GROUP_A,
						g_PositionX,g_PositionY,
						-ms_sp,0,
						1,
						&g_ScreenRect,
						&rect,
						&g_pDDSBack,
						&g_pDDSOffScreen[SP_TABLE_BG],
						BG
					)
				);
				GCMgr->AddObject(
					new UsrDefaultMissile(
						ID_CHR_GROUP_A,
						g_PositionX,g_PositionY,
						ms_sp,0,
						1,
						&g_ScreenRect,
						&rect,
						&g_pDDSBack,
						&g_pDDSOffScreen[SP_TABLE_BG],
						BG
					)
				);
			}
		}

	}
	/*Draw Decolation*/
	rect_from.top		=	64;
	rect_from.bottom	=	96;
	rect_from.left	=	0*32;
	rect_from.right	=	4*32;

	g_pDDSBack->BltFast( 15*32+16,2*32+16, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	rect_from.top		=	64;
	rect_from.bottom	=	96;
	rect_from.left	=	4*32;
	rect_from.right	=	8*32;

	g_pDDSBack->BltFast( 15*32+16,3*32+16, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	rect_from.top		=	64;
	rect_from.bottom	=	96;
	rect_from.left	=	8*32;
	rect_from.right	=	12*32;

	g_pDDSBack->BltFast( 15*32+16,4*32+16, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	rect_from.top		=	0;
	rect_from.bottom	=	32;
	rect_from.left	=	14*32;
	rect_from.right	=	17*32;

	g_pDDSBack->BltFast( 16*32,7*32, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	rect_from.top		=	0;
	rect_from.bottom	=	32;
	rect_from.left	=	11*32;
	rect_from.right	=	14*32;

	g_pDDSBack->BltFast( 16*32,9*32, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	rect_from.top		=	0;
	rect_from.bottom	=	32;
	rect_from.left	=	17*32;
	rect_from.right	=	20*32;

	g_pDDSBack->BltFast( 16*32,11*32, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	if(g_pDDSBack->GetDC(&hDC) == DD_OK)
	{
		HFONT	hFont;
		HFONT	hFontOld;

		hFont= CreateFont(
		40,			//�t�H���g����
		0,			//������
		0,		//�e�L�X�g�̊p�x
		0,			//�x�[�X���C���Ƃ����Ƃ̊p�x
		FW_BOLD,	//�t�H���g�̏d���i�����j
		TRUE,		//�C�^���b�N��
		FALSE,		//�A���_�[���C��
		FALSE,		//�ł�������
		SHIFTJIS_CHARSET,	//�����Z�b�g
		OUT_DEFAULT_PRECIS,	//�o�͐��x
		CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
		PROOF_QUALITY,		//�o�͕i��
		FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[
		"�l�r ����"	//���̖�
		);
		hFontOld = SelectObject(hDC, hFont);
		SetBkMode(hDC,TRANSPARENT);
		SetTextColor(hDC, RGB(255, 0, 0));
		sprintf(str,"%03d",g_Stage);
		TextOut(hDC,16*32+24,8*32,str,strlen(str));
		sprintf(str,"%03d",g_MissileRest);
		TextOut(hDC,16*32+24,10*32,str,strlen(str));
		sprintf(str,"%03d",g_TargetRest);
		TextOut(hDC,16*32+24,12*32,str,strlen(str));
		SelectObject(hDC, hFontOld);
		DeleteObject(hFont);
		g_pDDSBack->ReleaseDC(hDC);
	}

}
void	DDAppMain_Make( void )
{
	RECT	rect_from;
	rect_from.top		=	0;
	rect_from.bottom	=	480;
	rect_from.left	=	0;
	rect_from.right	=	640;
	HDC	hDC;
	char	str[10];

	g_pDDSBack->BltFast( 0,0, g_pDDSOffScreen[SP_TABLE_BASE], &rect_from, DDBLTFAST_WAIT);

	/*game main*/
	if(BG!=NULL)
	{
		int	sp=32;
		int	s=Stick->GetPos();
		int	b1=Stick->GetBtn1();
		int	b2=Stick->GetBtn2();

		if( s && SubWind==NULL )
		{
			if( s>6 )
			{
				if(g_PositionY>1*32) g_PositionY-=sp;
			}else if( s<4 ){
				if(g_PositionY<13*32) g_PositionY+=sp;
			}else if( ( s % 3 ) == 1 ){
				if(g_PositionX>1*32) g_PositionX-=sp;
			}else if( ( s % 3 ) == 0 ){
				if(g_PositionX<14*32) g_PositionX+=sp;
			}
		}
		BG->DrawAll();
		if( SubWind!=NULL )
		{
			if( SubWind->UpDate()==FALSE )
			{
				SubWind=NULL;
			} else {
				SubWind->DrawAll();
			}
		} else {
			HDC	hDC;
			HPEN	hPen,hPen_Old;
			HBRUSH	hBruch,hBrush_Old;
			g_pDDSBack->GetDC( &hDC );

			g_Counter++;
			if(g_Counter>1)
			{
				g_Counter=0;
				hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
			} else {
				hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			}
			hPen_Old = SelectObject(hDC, hPen);
			hBrush_Old=SelectObject(hDC, GetStockObject(NULL_BRUSH));

			RECT	rect;

			rect.top	=	g_PositionY;
			rect.left	=	g_PositionX;
			rect.right	=	rect.left+32;
			rect.bottom	=	rect.top+32;
			
			Rectangle(hDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);
			SelectObject(hDC, hBrush_Old);
			SelectObject(hDC, hPen_Old);
			DeleteObject(hPen);
			g_pDDSBack->ReleaseDC( hDC );

			if( b1 && SubWind==NULL )
			{
				Stick->SetBtn1( 0 );
				SubWind=new	UsrBGWindow( &g_pDDSBack,&g_pDDSOffScreen[SP_TABLE_WINDOW],20,50,11,8,BGWMoveCommand,BGWDrawCommand );
				SubWind->InitCursor(0,0,0,5);
			} else if( b2 )	{
				Stick->SetBtn2( 0 );
				BG->PutBG(g_PositionX,g_PositionY,(char)transBG[g_SelectBG]);
			}
		}
	}
	rect_from.top		=	0;
	rect_from.bottom	=	32;
	rect_from.left	=	14*32;
	rect_from.right	=	17*32;

	g_pDDSBack->BltFast( 16*32,7*32, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	rect_from.top		=	0;
	rect_from.bottom	=	32;
	rect_from.left	=	11*32;
	rect_from.right	=	14*32;

	g_pDDSBack->BltFast( 16*32,9*32, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

	if(g_pDDSBack->GetDC(&hDC) == DD_OK)
	{
		HFONT	hFont;
		HFONT	hFontOld;

		hFont= CreateFont(
		40,			//�t�H���g����
		0,			//������
		0,		//�e�L�X�g�̊p�x
		0,			//�x�[�X���C���Ƃ����Ƃ̊p�x
		FW_BOLD,	//�t�H���g�̏d���i�����j
		TRUE,		//�C�^���b�N��
		FALSE,		//�A���_�[���C��
		FALSE,		//�ł�������
		SHIFTJIS_CHARSET,	//�����Z�b�g
		OUT_DEFAULT_PRECIS,	//�o�͐��x
		CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
		PROOF_QUALITY,		//�o�͕i��
		FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[
		"�l�r ����"	//���̖�
		);
		hFontOld = SelectObject(hDC, hFont);
		SetBkMode(hDC,TRANSPARENT);
		SetTextColor(hDC, RGB(255, 0, 0));
		sprintf(str,"%03d",g_Stage);
		TextOut(hDC,16*32+24,8*32,str,strlen(str));
		sprintf(str,"%03d",g_MissileRest);
		TextOut(hDC,16*32+24,10*32,str,strlen(str));
//		sprintf(str,"%03d",g_TargetRest);
//		TextOut(hDC,16*32+24,12*32,str,strlen(str));
		SelectObject(hDC, hFontOld);
		DeleteObject(hFont);
		g_pDDSBack->ReleaseDC(hDC);
	}

}
void	DDAppMain_Title( void )
{
	RECT	rect_from,rect_to;
	DDBLTFX	ddbltfx;
	int	size;
	int	i,j;

	// Use the blter to do a color fill to clear the back buffer
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = 0;
	g_pDDSBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

	g_Counter++;

	int	s=Stick->GetPos();
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();

	size=g_Counter;
	if( g_Counter > 88 )
	{
		rect_from.top		=	32;
		rect_from.bottom	=	rect_from.top+32;
		rect_from.left	=	64;
		rect_from.right	=	rect_from.left+32;
		if( g_Counter >890 )g_Counter=0;
		size=88;

		for(j=2;j<7;j++)
		{
			for(i=3;i<17;i++)
			{
				g_pDDSBack->BltFast( i*32,j*32, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
			}
		}
	} else {
		if( s || b1 || b2 )
		{
			s=b1=b2=0;
			g_Counter=89;
			Stick->SetUp(0);
			Stick->SetBtn1(0);
			Stick->SetBtn2(0);
			Stick->SetDown(0);
		}
	}

	rect_from.top		=	64;
	rect_from.bottom	=	rect_from.top+32;
	rect_from.left	=	0;
	rect_from.right	=	rect_from.left+128;
	rect_to.top		=	140-size*3/4;
	rect_to.bottom	=	rect_to.top+size/2;
	rect_to.left	=	320-size*2;
	rect_to.right	=	rect_to.left+	size*4;
	g_pDDSBack->Blt( &rect_to, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLT_KEYSRC | DDBLT_WAIT,NULL);
	rect_from.top		=	64;
	rect_from.bottom	=	rect_from.top+32;
	rect_from.left	=	128;
	rect_from.right	=	rect_from.left+128;
	rect_to.top		+=	(size/2);
	rect_to.bottom	=	rect_to.top+size/2;
	rect_to.left	=	320-size*2;
	rect_to.right	=	rect_to.left+	size*4;
	g_pDDSBack->Blt( &rect_to, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLT_KEYSRC | DDBLT_WAIT,NULL);
	rect_from.top		=	64;
	rect_from.bottom	=	rect_from.top+32;
	rect_from.left	=	256;
	rect_from.right	=	rect_from.left+128;
	rect_to.top		+=	(size/2);
	rect_to.bottom	=	rect_to.top+size/2;
	rect_to.left	=	320-size*2;
	rect_to.right	=	rect_to.left+	size*4;
	g_pDDSBack->Blt( &rect_to, g_pDDSOffScreen[SP_TABLE_BG], &rect_from, DDBLT_KEYSRC | DDBLT_WAIT,NULL);

	HDC	hDC;
	if( g_Counter > 88 )
	{
		if(g_pDDSBack->GetDC(&hDC) == DD_OK)
		{
			HFONT	hFont;
			HFONT	hFontOld;
			hFont = CreateFont(
			30,			//�t�H���g����
			0,			//������
			0,		//�e�L�X�g�̊p�x
			0,			//�x�[�X���C���Ƃ����Ƃ̊p�x
			FW_BOLD,	//�t�H���g�̏d���i�����j
			FALSE,		//�C�^���b�N��
			FALSE,		//�A���_�[���C��
			FALSE,		//�ł�������
			SHIFTJIS_CHARSET,	//�����Z�b�g
			OUT_DEFAULT_PRECIS,	//�o�͐��x
			CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
			PROOF_QUALITY,		//�o�͕i��
			FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[
			"�l�r ����"	//���̖�
			);
			hFontOld = SelectObject(hDC, hFont);
			SetBkMode(hDC,TRANSPARENT);
			SetTextColor(hDC, RGB(255, 127, 0));
			TextOut(hDC,140,384,"Copyright(C)1999 QWERT",strlen("Copyright(C)1999 QWERT"));
			SelectObject(hDC, hFontOld);
			DeleteObject(hFont);

			hFont = CreateFont(
			20,			//�t�H���g����
			20,			//������
			0,		//�e�L�X�g�̊p�x
			0,			//�x�[�X���C���Ƃ����Ƃ̊p�x
			FW_BOLD,	//�t�H���g�̏d���i�����j
			TRUE,		//�C�^���b�N��
			FALSE,		//�A���_�[���C��
			FALSE,		//�ł�������
			SHIFTJIS_CHARSET,	//�����Z�b�g
			OUT_DEFAULT_PRECIS,	//�o�͐��x
			CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
			PROOF_QUALITY,		//�o�͕i��
			FIXED_PITCH | FF_MODERN,	//�s�b�`�ƃt�@�~���[
			"�l�r ����"	//���̖�
			);
			hFontOld = SelectObject(hDC, hFont);
			SetBkMode(hDC,TRANSPARENT);
			SetTextColor(hDC, RGB(255, 255, 255));
			TextOut(hDC,280,250,"START",strlen("START"));
			if(g_StageClearMax>0)TextOut(hDC,280,280,"CONTINUE",strlen("CONTINUE"));
			if(g_StageClearMax>0)TextOut(hDC,280,310,"EDIT",strlen("EDIT"));
			SelectObject(hDC, hFontOld);
			DeleteObject(hFont);
		
			g_pDDSBack->ReleaseDC(hDC);
		}

		rect_from.top		=	0;
		rect_from.bottom	=	rect_from.top+32;
		rect_from.left	=	32*13;
		rect_from.right	=	rect_from.left+32;
		g_pDDSBack->BltFast( 240,244+g_GameMode*30, g_pDDSOffScreen[SP_TABLE_WINDOW], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	}	
	if( SubWind!=NULL )
	{
		if( SubWind->UpDate()==FALSE )
		{
			SubWind=NULL;
		} else {
			SubWind->DrawAll();
		}
	} else {

		if( b2 )
		{
			switch(g_GameMode)
			{
				case	GAMEMODE_NORMAL:
					g_Stage=1;
					g_Screen=GAMESCREEN_PLAY;
					break;
				case	GAMEMODE_CONTINUE:
					Stick->SetBtn2( 0 );
					SubWind=new	UsrBGWindow( &g_pDDSBack,&g_pDDSOffScreen[SP_TABLE_WINDOW],50,40,14,4,BGWMoveSelect,BGWDrawSelect );
					SubWind->InitCursor(0,0,0,0);
					g_Stage=g_StageClearMax+1;
					if(g_Stage>GAMESTAGE_MAX)g_Stage=GAMESTAGE_MAX;
					break;
				case	GAMEMODE_EDIT:
					Stick->SetBtn2( 0 );
					SubWind=new	UsrBGWindow( &g_pDDSBack,&g_pDDSOffScreen[SP_TABLE_WINDOW],50,90,14,4,BGWMoveSelect,BGWDrawSelect );
					SubWind->InitCursor(0,0,0,0);
					g_Stage=g_StageClearMax;
					break;
				default:
					break;
			}
			InitGameStage();
			return;
		}
		if( s || b1 )
		{
			Stick->SetUp(0);
			Stick->SetBtn1(0);
			Stick->SetDown(0);
			if( s>6 )
			{
				g_GameMode--;
				if(g_GameMode<0)
				{
					if(g_StageClearMax==0)g_GameMode=0; else g_GameMode=2;
				}
			} else if( s<4 || b1 ){
				g_GameMode++;
				if(g_GameMode>2)g_GameMode=0;
				if(g_StageClearMax==0)g_GameMode=0;
			}
		}
	}
		
}

/****************************************************************************/
/*																			*/
/*		�E�B���h�E �v���V�[�W��(���b�Z�[�W�̏�������ɍs���֐�)			*/
/*																			*/
/****************************************************************************/
LRESULT	CALLBACK	WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
		case	WM_CREATE:
			/*�}�E�X�J�[�\������*/
			while(ShowCursor(FALSE)>=0);
			break;
		case	WM_ACTIVATEAPP:
			// Pause if minimized or not the top window
			g_bActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			break;
		case	WM_TIMER:
			 // Update and flip surfaces
			if(g_bActive && TIMER_ID == wParam)
			{
				switch(g_Screen)
				{
					case	GAMESCREEN_TITLE:
						DDAppMain_Title();
						break;
					case	GAMESCREEN_PLAY:
						DDAppMain_Play();
						break;
					case	GAMESCREEN_EDIT:
						DDAppMain_Make();
						break;
					case	GAMESCREEN_END:
						DDAppMain_Ending();
						break;
					default:
						break;
				}
				FlipEachSurfaces();
			}
			break;
		case	WM_DESTROY:
			// Clean up and close the app
			if( GCMgr!=NULL)delete	GCMgr;
			GCMgr=NULL;
			if( SubWind!=NULL )SubWind->DeleteAll();
			SubWind=NULL;
			if( Stick!=NULL )delete	Stick;
			Stick=NULL;
			if( BG!=NULL )delete	BG;
			BG=NULL;
			ReleaseAllObjects();
			/*�}�E�X�J�[�\���o��*/
			while(ShowCursor(TRUE)<0);
	/*The PostQuitMessage function indicates to Windows 					*/
	/*that a thread has made a request to terminate (quit).					*/
	/* It is typically used in response to a WM_DESTROY message. 			*/
			PostQuitMessage( 0 );
			return 0L;
		case	WM_KEYDOWN:
			// Handle any non-accelerated key commands
			switch (wParam)
			{
				case	VK_F11:
					GetSnapShot(&g_pDDSPrimary,hWnd);
					break;
				case	VK_ESCAPE:
				case	VK_F12:
					DestroyWindow( hWnd );
					return 0L;
				case	VK_RETURN:
				case	VK_SPACE:
//					if( SubWind==NULL )
//					{
//						SubWind=new	UsrBGWindow( &g_pDDSBack,&g_pDDSOffScreen[SP_TABLE_WINDOW],20,50,11,6,BGWMoveCommand,BGWDrawCommand );
//						SubWind->InitCursor(0,0,0,3);
//						BG->PutBG(g_PositionX,g_PositionY,1);
//						break;
//					}
				case	VK_UP:
				case	VK_DOWN:
				case	VK_LEFT:
				case	VK_RIGHT:
					Stick->SetStick(1, (UINT) wParam );
					return(0L);
			}
			break;
		case	WM_KEYUP:
			switch (wParam)
			{
				case	VK_UP:
				case	VK_DOWN:
				case	VK_LEFT:
				case	VK_RIGHT:
				case	VK_SPACE:
				case	VK_RETURN:
					Stick->SetStick(0, (UINT) wParam );
					return(0L);
			}
			break;
		default:
			break;
	}
	/*The DefWindowProc function calls the default window procedure 		*/
	/*to provide default processing for any window messages 				*/
	/*that an application does not process. This function ensures 			*/
	/*that every message is processed. DefWindowProc is called with 		*/
	/*the same parameters received by the window procedure. 				*/
	return( DefWindowProc(hWnd, msg, wParam, lParam) );
}

///////////////////////////////////////////////////////////////////////////
// Create the main DirectDraw object
///////////////////////////////////////////////////////////////////////////
static	HRESULT	DirectDrawInit(HWND hWnd)
{
	HRESULT	hRet;
	/*DirectDraw�I�u�W�F�N�g����*/
	LPDIRECTDRAW	pDD;
	int	i;

	hRet=DirectDrawCreate(NULL, &pDD, NULL);
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "DirectDrawCreate FAILED");
	}
	// Fetch DirectDraw4 interface
	hRet=pDD->QueryInterface(IID_IDirectDraw4, (LPVOID *) & g_pDD);
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "QueryInterface FAILED");
	}
	// �������x���̐ݒ� Get exclusive mode
	hRet=g_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "SetCooperativeLevel FAILED");
	}
	// Set the video mode to 640x480x8
	hRet=g_pDD->SetDisplayMode(640, 480, 8, 0, 0);
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "SetDisplayMode FAILED");
	}

	// Create the primary surface with 1 back buffer
	DDSURFACEDESC2	ddsd;

	ZeroMemory(&ddsd, sizeof(ddsd));

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
		DDSCAPS_FLIP |
		DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;
	hRet=g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL);
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "CreateSurface FAILED");
	}
    // Get a pointer to the back buffer
	DDSCAPS2	ddscaps;
	ZeroMemory( &ddscaps, sizeof( ddscaps ) );
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	hRet=g_pDDSPrimary->GetAttachedSurface(&ddscaps, &g_pDDSBack);
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "GetAttachedSurface FAILED");
	}


	/*640*480�I�t�X�N���[���T�[�t�F�X�쐬*/
	ddsd.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = 640;
	ddsd.dwHeight = 480;
	hRet=g_pDD->CreateSurface( &ddsd, &g_pDDSOffScreen[0], NULL );
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "CreateOffScreenSurface FAILED");
	}
	ddsd.dwWidth = 640;
	ddsd.dwHeight = 32;
	hRet=g_pDD->CreateSurface( &ddsd, &g_pDDSOffScreen[1], NULL );
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "CreateOffScreenSurface FAILED");
	}
	ddsd.dwWidth = 640;
	ddsd.dwHeight = 480;
	hRet=g_pDD->CreateSurface( &ddsd, &g_pDDSOffScreen[2], NULL );
	if( hRet != DD_OK )
	{
		return InitFail(hWnd, hRet, "CreateOffScreenSurface FAILED");
	}


	/*�p���b�g�̐���	19990214 quz*/
	g_pDD->CreatePalette( DDPCAPS_8BIT , Pal , &g_pDDPalette , NULL ); 
	g_pDDSPrimary->SetPalette( g_pDDPalette );

	/*�e�T�[�t�F�X�̃��[�h*/
	if( !RestoreSurfaces() )
	{
		ReleaseAllObjects();
		return InitFail(hWnd, hRet, "RestoreSurface FAILED");
	}
	
	// Create a timer to flip the pages
	if( TIMER_ID != SetTimer(hWnd, TIMER_ID, 50/*TIMER_RATE*/, NULL) )
	{
		return InitFail(hWnd, hRet, "SetTimer FAILED");
	}
	
	return( DD_OK );
}
//-----------------------------------------------------------------------------
// Name: ReleaseAllObjects()
// Desc: Finished with all objects we use; release them
//-----------------------------------------------------------------------------
static	void	ReleaseAllObjects(void)
{
	if( g_pDD != NULL )
	{
		/*�I�t�X�N���[���T�[�t�F�X�J��*/
		for(int i=0;i<SPRITE_SURFACE_MAX;i++)
		{
			if(g_pDDSOffScreen[i] != NULL)
			{
				g_pDDSOffScreen[i]->Release();
				g_pDDSOffScreen[i] = NULL;
			}
		}
		if (g_pDDSPrimary != NULL)
		{
			g_pDDSPrimary->Release();/*�v���C�}���T�[�t�F�X�̊J��*/
			g_pDDSPrimary = NULL;
		}
		g_pDD->Release();	/*DirectDraw�I�u�W�F�N�g�̊J��*/
		g_pDD = NULL;
    }
}
// �T�[�t�F�X�̃��[�h
static	BOOL	RestoreSurfaces(void)
{
	HRSRC hRes;
	LPBITMAPINFOHEADER lpBmiHeader;
	RGBQUAD *pRGB;
	HBITMAP hBmp, hBmpOld;
	HDC hDCSrc, hDCDest;
	DDCOLORKEY ddck;
	int	i;
	RECT	rect_to;
	RECT	rect_from;

	g_pDDSPrimary->Restore();
	for(i=0;i<SPRITE_SURFACE_MAX;i++)	g_pDDSOffScreen[i]->Restore();

	// �p���b�g���\�[�X��������
	if( (hRes=FindResource( NULL, MAKEINTRESOURCE(IDB_BITMAP1), RT_BITMAP ))==NULL ){
		return FALSE;
	}

	// �p���b�g���\�[�X�̃��[�h
	lpBmiHeader = (LPBITMAPINFOHEADER)LockResource( LoadResource( NULL, hRes ) );
	pRGB = (RGBQUAD*)((char*)lpBmiHeader+lpBmiHeader->biSize);
	if( lpBmiHeader->biClrUsed==0 ) lpBmiHeader->biClrUsed = 1<<lpBmiHeader->biBitCount;

	// �p���b�g���擾
	for( i=0; i<(int)lpBmiHeader->biClrUsed; i++ ){
		Pal[i].peRed   = pRGB[i].rgbRed;
		Pal[i].peGreen = pRGB[i].rgbGreen;
		Pal[i].peBlue  = pRGB[i].rgbBlue;
		Pal[i].peFlags = 0;
	}

	// �p���b�g��ݒ�
	g_pDDPalette->SetEntries( 0, 0, lpBmiHeader->biClrUsed, Pal );

	// ���\�[�X����r�b�g�}�b�v�����[�h
	if( (hBmp = LoadImage( GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP,
		0, 0, LR_CREATEDIBSECTION ))==NULL ){
		return FALSE;
	}
	
	// �J���[�L�[��ݒ�
	ddck.dwColorSpaceLowValue  = 255; // �����w��
	ddck.dwColorSpaceHighValue = 255;
	
	for(i=0;i<SPRITE_SURFACE_MAX;i++)
	{
		g_pDDSOffScreen[i]->SetColorKey( DDCKEY_SRCBLT, &ddck ); 
	}
	
	// �X�v���C�g�o�b�t�@�����O�T�[�t�F�Xg_pDDSOffScreen[2]�փR�s�[����
	hDCSrc = CreateCompatibleDC( NULL );	// �������f�o�C�X�R���e�L�X�g���쐬
	hBmpOld = SelectObject( hDCSrc, hBmp );	// �r�b�g�}�b�v��I��
	g_pDDSOffScreen[2]->GetDC( &hDCDest );			// �X�v���C�g�T�[�t�F�X��DC���擾
	rect_from.top = 0;
	rect_from.bottom = 64;
	rect_from.left = 0;
	rect_from.right = 320;
	BitBlt(
		hDCDest,
		rect_from.left,
		rect_from.top,
		rect_from.right-rect_from.left,
		rect_from.bottom-rect_from.top,
		hDCSrc, 0,0, SRCCOPY
	); // �R�s�[
	g_pDDSOffScreen[2]->ReleaseDC( hDCDest );		// �X�v���C�g�T�[�t�F�X��DC���J��
	SelectObject( hDCSrc, hBmpOld );		// �r�b�g�}�b�v�𕜋A
	DeleteDC( hDCSrc );						// �������f�o�C�X�R���e�L�X�g���J��
	/*g_pDDSOffScreen[2]��g_pDDSOffScreen[0]�֊g��R�s�[*/
	rect_to.top		=	0;
	rect_to.bottom	=	128;
	rect_to.left	=	0;
	rect_to.right	=	640;
	g_pDDSOffScreen[0]->Blt( &rect_to, g_pDDSOffScreen[2], &rect_from, DDBLT_WAIT,NULL);
//	g_pDDSOffScreen[1]->Blt( &rect_from, g_pDDSOffScreen[0], &rect_from, DDBLT_WAIT,NULL);
	
	// ���\�[�X����r�b�g�}�b�v�����[�h
	if( (hBmp = LoadImage( GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP,
		0, 0, LR_CREATEDIBSECTION ))==NULL ){
		return FALSE;
	}

	// �X�v���C�g�o�b�t�@�����O�T�[�t�F�Xg_pDDSOffScreen[2]�փR�s�[����
	hDCSrc = CreateCompatibleDC( NULL );	// �������f�o�C�X�R���e�L�X�g���쐬
	hBmpOld = SelectObject( hDCSrc, hBmp );	// �r�b�g�}�b�v��I��
	g_pDDSOffScreen[2]->GetDC( &hDCDest );			// �X�v���C�g�T�[�t�F�X��DC���擾
	rect_from.top = 0;
	rect_from.bottom = 16;
	rect_from.left = 0;
	rect_from.right = 320;
	BitBlt(
		hDCDest,
		rect_from.left,
		rect_from.top,
		rect_from.right-rect_from.left,
		rect_from.bottom-rect_from.top,
		hDCSrc, 0,0, SRCCOPY
	); // �R�s�[
	g_pDDSOffScreen[2]->ReleaseDC( hDCDest );		// �X�v���C�g�T�[�t�F�X��DC���J��
	SelectObject( hDCSrc, hBmpOld );		// �r�b�g�}�b�v�𕜋A
	DeleteDC( hDCSrc );						// �������f�o�C�X�R���e�L�X�g���J��
	/*g_pDDSOffScreen[2]��g_pDDSOffScreen[0]�֊g��R�s�[*/
	rect_to.top		=	0;
	rect_to.bottom	=	32;
	rect_to.left	=	0;
	rect_to.right	=	640;
	g_pDDSOffScreen[1]->Blt( &rect_to, g_pDDSOffScreen[2], &rect_from, DDBLT_WAIT,NULL);

	rect_from.top		=	0;
	rect_from.bottom	=	16;
	rect_from.left	=	0;
	rect_from.right	=	16;

	for(i=0;i<4;i++)
	{
		rect_to.top		=	(i/2)*16;
		rect_to.bottom	=	rect_to.top+16;
		rect_to.left	=	(i%2)*16;
		rect_to.right	=	rect_to.left+16;
		g_pDDSOffScreen[1]->BltFast( rect_to.left,rect_to.top, g_pDDSOffScreen[2], &rect_from, DDBLTFAST_WAIT);
	}


	g_pDDSOffScreen[2]->GetDC( &hDCDest );			// �X�v���C�g�T�[�t�F�X��DC���擾
	HBRUSH	hBrush[20], hOldBrush;
	RECT	rect;
	for(i=0;i<20;i++)hBrush[i] = CreateSolidBrush( RGB(0,0,0) );
//	for(i=0;i<20;i++)hBrush[i] = CreateSolidBrush( 98+i );
	hOldBrush = SelectObject(hDCDest, hBrush[0]);
	rect.top=0;
	rect.bottom=480;
	rect.left=0;
	rect.right=640;
	FillRect(hDCDest,&rect,hBrush[0]);
	for(i=0;i<20;i++)DeleteObject(hBrush[i]);
	for(i=0;i<20;i++)hBrush[i] = CreateSolidBrush( RGB(i*12,i*12,i*12) );
	for(i=0;i<19;i++)
	{
		SelectObject(hDCDest, hBrush[19-i]);
		rect.top=16;
		rect.bottom=480-16;
		rect.left=i*32+16;
		rect.right=rect.left+32;
		FillRect(hDCDest,&rect,hBrush);
	}
	for(i=0;i<19*14;i+=2)
	{
		SelectObject(hDCDest, hBrush[i/14]);
		rect.top=( i / 19 )*32+16;
		rect.bottom=rect.top+32;
		rect.left=(i % 19)*32+16;
		rect.right=rect.left+32;
		FillRect(hDCDest,&rect,hBrush);
	}
	SelectObject(hDCDest, hOldBrush);
	for(i=0;i<20;i++)DeleteObject(hBrush[i]);
	
	g_pDDSOffScreen[2]->ReleaseDC( hDCDest );		// �X�v���C�g�T�[�t�F�X��DC���J��
	
	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: InitFail()
// Desc: This function is called if an initialization function fails
//-----------------------------------------------------------------------------
static	HRESULT	InitFail(HWND hWnd, HRESULT hRet, LPCTSTR szError,...)
{
	char	szBuff[128];
	va_list	vl;

	va_start(vl, szError);
	{
		vsprintf(szBuff, szError, vl);
		ReleaseAllObjects();
		MessageBox(hWnd, szBuff, TITLE, MB_OK);
		DestroyWindow(hWnd);
	}
	va_end(vl);
	return( hRet );
}



static	void	FlipEachSurfaces(void)
{
	/*�T�[�t�F�X�̃t���b�v*/
	while(TRUE)	/*�t���b�v����^�C�~���O��҂�*/
	{
		HRESULT	hRet;
		hRet = g_pDDSPrimary->Flip(NULL, 0);
		if( hRet == DD_OK )	break;		/*����t���b�v�ŒE�o*/
		if( hRet == DDERR_SURFACELOST )	/*�T�[�t�F�X���󎞏���*/
		{
			hRet = g_pDDSPrimary->Restore();
			if ( hRet != DD_OK )	break;
		}
		if( hRet != DDERR_WASSTILLDRAWING )	break;
	}
}

void	SpriteDebug( void )
{
	RECT	rect_from;	/*�I�t�X�N���[���T�[�t�F�X���̈ʒu���*/

	/*�I�t�X�N���[���T�[�t�F�X���̈ʒu���*/
	rect_from.top = 0;
	rect_from.bottom = 480;
	rect_from.left = 0;
	rect_from.right = 640;


	g_pDDSBack->BltFast( 0, 0, g_pDDSOffScreen[0], &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT );
//	g_pDDSBack->BltFast( 0, 0, g_pDDSOffScreen[0], &rect_from,  DDBLTFAST_WAIT );

}

void	DebugPrint(int	x,int	y,char*	str)
{
	HDC	hDC;
	g_pDDSBack->GetDC( &hDC );			// �X�v���C�g�T�[�t�F�X��DC���擾
	TextOut(hDC,x,y,str,strlen(str));
	g_pDDSBack->ReleaseDC( hDC );		// �X�v���C�g�T�[�t�F�X��DC���J��
}
void	CheckWindFunc(void)
{
	UsrBGWindow*	tg=SubWind;
	char	msg[40];
	int	cnt=0;
	while(tg!=NULL)
	{
		SearchFunc(tg,msg,cnt);
		DebugPrint(300,cnt*16,msg);
		tg=tg->GetNext();
		cnt++;
	}
}
void	SearchFunc(UsrBGWindow* t,char*	str,int	c)
{
//	if( t->CheckFuncName(BGWMoveCommand) )sprintf(str,"%d:BGWMove%s()",c,"Command");
//		else if( t->CheckFuncName(BGWMoveSize) )sprintf(str,"%d:BGWMove%s()",c,"Size");
//		else if( t->CheckFuncName(BGWMoveChara) )sprintf(str,"%d:BGWMove%s()",c,"Chara");
//		else if( t->CheckFuncName(BGWMoveSave) )sprintf(str,"%d:BGWMove%s()",c,"Save");
//		else	sprintf(str,"%d:Unknown",c);
}
bool	SaveGameData(char* filename)
{
	char	*mp;
	FILE*	fp=fopen(filename,"wb");
	if(fp==NULL)return(FALSE);
	/*format version*/
	fputc(1,fp);
	/*start locate*/
	fputc(g_PositionX/32,fp);fputc(g_PositionY/32,fp);
	/*missile*/
	fputc(g_MissileRest,fp);
	mp=BG->GetMapDat();
	for(int	i=0;i<20*15;i++)
	{
		fputc(mp[i],fp);
	}
	fcloseall();
	return(TRUE);
}
bool	SaveGameData2(char* filename)
{
	char	*mp;
	char	str[20];
	FILE*	fp=fopen(filename,"wb");
	if(fp==NULL)return(FALSE);
	/*format version*/
	/*start locate*/
	/*missile*/
	fprintf(fp,"%d,%d,%d,",g_PositionX/32,g_PositionY/32,g_MissileRest);
	mp=BG->GetMapDat();
	for(int	i=0;i<20*15;i++)
	{
		if(i%15==0)fprintf(fp,"\n");
		fprintf(fp,"%d,",mp[i]);
	}
	fprintf(fp,"\n");
	fcloseall();
	return(TRUE);
}
bool	LoadGameData(char*filename)
{
	char	mp[20*15];
	FILE*	fp=fopen(filename,"rb");
	int	ver,x,y,m,dmy;

	if(fp==NULL)return(FALSE);
	/*format version*/
	ver=fgetc(fp);
	if(ver==EOF){ fcloseall();return(FALSE); }
	/*start locate*/
	x=fgetc(fp);y=fgetc(fp);
	if(x==EOF || y==EOF ){ fcloseall();return(FALSE); }
	/*missile*/
	m=fgetc(fp);
	if(m==EOF){ fcloseall();return(FALSE); }
	for(int	i=0;i<20*15;i++)
	{
		dmy=fgetc(fp);
		if(dmy==EOF)break;
		mp[i]=(char)dmy;
	}
	if(dmy==EOF){ fcloseall();return(FALSE); }

	g_PositionX=x*32;
	g_PositionY=y*32;
	g_MissileRest=m;
	BG->SetMap(20,15,mp);
	fcloseall();
	return(TRUE);
}


bool	BGWMoveBreak(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	
//	DebugPrint(0,0,"Command");
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();
	int	pos=Stick->GetPos();

	if( pos )
	{
		if( pos>6 )
		{
			Stick->SetUp(0);
			this_ptr->CursorMove(0,-1);
		} else if( pos<4 ){
			Stick->SetDown(0);
			this_ptr->CursorMove(0,1);
		}
	}
	
	if( b1 )
	{
		Stick->SetBtn1( 0 );
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	} else if( b2 ){
		Stick->SetBtn2( 0 );
		switch(this_ptr->GetCursorY())
		{
			case	0:
				g_Screen=GAMESCREEN_TITLE;
				g_Counter=0;
				if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
				break;
			case	1:	/*����*/
				if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
				break;
			default:
				break;
		}
	}
	return(TRUE);
}
void	BGWDrawBreak(UsrBGWindow* this_ptr)
{
	this_ptr->ChrPut(32,32+this_ptr->GetCursorY()*32,13,&g_pDDSOffScreen[SP_TABLE_WINDOW]);

	this_ptr->Print(64,32,16,32,"�Q�[���̒��~");
	this_ptr->Print(64,64,16,32,"����");
}
bool	BGWMoveCommand(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	
//	DebugPrint(0,0,"Command");
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();
	int	pos=Stick->GetPos();

	if( pos )
	{
		if( pos>6 )
		{
			Stick->SetUp(0);
			this_ptr->CursorMove(0,-1);
		} else if( pos<4 ){
			Stick->SetDown(0);
			this_ptr->CursorMove(0,1);
		}
	}
	
	if( b1 )
	{
		Stick->SetBtn1( 0 );
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	} else if( b2 ){
		Stick->SetBtn2( 0 );
		switch(this_ptr->GetCursorY())
		{
			case	0:
				this_ptr->CreateNextWindow(&g_pDDSOffScreen[SP_TABLE_WINDOW],this_ptr->GetX()+21,this_ptr->GetY()+32+this_ptr->GetCursorY()*32+20,14,3,11,0,g_SelectBG%16,g_SelectBG/16,BGWMoveChara,BGWDrawChara );
				break;
			case	1:
				this_ptr->CreateNextWindow(&g_pDDSOffScreen[SP_TABLE_WINDOW],this_ptr->GetX()+21,this_ptr->GetY()+32+this_ptr->GetCursorY()*32+20,15,3,0,0,0,0,BGWMoveMissile,BGWDrawMissile );
				g_DmyParam=g_MissileRest;
				break;
			case	2:
				this_ptr->CreateNextWindow(&g_pDDSOffScreen[SP_TABLE_WINDOW],this_ptr->GetX()+21,this_ptr->GetY()+32+this_ptr->GetCursorY()*32+20,10,5,0,1,0,0,BGWMoveLoad,BGWDrawLoad );
				break;
			case	3:
				this_ptr->CreateNextWindow(&g_pDDSOffScreen[SP_TABLE_WINDOW],this_ptr->GetX()+21,this_ptr->GetY()+32+this_ptr->GetCursorY()*32+20,9,5,0,1,0,0,BGWMoveSave,BGWDrawSave );
				break;
			case	4:
				g_Screen=GAMESCREEN_TITLE;
				g_Counter=0;
				if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
				break;
			case	5:	/*����*/
				if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
				break;
			default:
				break;
		}
	}
	return(TRUE);
}
void	BGWDrawCommand(UsrBGWindow* this_ptr)
{
	this_ptr->ChrPut(32,32+this_ptr->GetCursorY()*32,13,&g_pDDSOffScreen[SP_TABLE_WINDOW]);

	this_ptr->Print(64,32,16,32,"�L�����N�^�̕ύX");
	this_ptr->Print(64,64,16,32,"�~�T�C�����̕ύX");
	this_ptr->Print(64,96,16,32,"�f�[�^�̓ǂݍ���");
	this_ptr->Print(64,128,16,32,"�f�[�^�̋L�^");
	this_ptr->Print(64,160,16,32,"�d�c�h�s�I��");
	this_ptr->Print(64,192,16,32,"����");
}
bool	BGWMoveSelect(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();
	int	pos=Stick->GetPos();
	if( pos )
	{
		if( pos % 3 ==1 ){
			/*Stage��������*/
			if( g_Stage>1 )g_Stage--;
		} else if( pos % 3 ==0 ){
			/*Stage��傫��*/
			if( g_GameMode==GAMEMODE_EDIT && g_Stage<g_StageClearMax )g_Stage++;
			if( g_GameMode==GAMEMODE_CONTINUE && g_Stage<g_StageClearMax+1 )g_Stage++;
			if( g_Stage>GAMESTAGE_MAX ) g_Stage=GAMESTAGE_MAX;
		}
	}
	if( b1 || b2 )
	{
		Stick->SetBtn1( 0 );
		Stick->SetBtn2( 0 );
		if( b2 )
		{
			if(g_GameMode==GAMEMODE_EDIT)g_Screen=GAMESCREEN_EDIT;
				else g_Screen=GAMESCREEN_PLAY;
			InitGameStage();
		}
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	}
	return(TRUE);
}
void	BGWDrawSelect(UsrBGWindow* this_ptr)
{
	char	str[12];
	this_ptr->ChrPut(32,64,13,&g_pDDSOffScreen[SP_TABLE_WINDOW]);

	this_ptr->Print(64,32,16,32,"�X�e�[�W�I��");
	sprintf(str,"STAGE: %03d",g_Stage);
	this_ptr->Print(64,64,32,32,str);
}
bool	BGWMoveMissile(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();
	int	pos=Stick->GetPos();
	if( pos )
	{
		if( pos % 3 ==1 ){
			/*Stage��������*/
			if( g_DmyParam>1 )g_DmyParam--;
		} else if( pos % 3 ==0 ){
			/*Stage��傫��*/
			if( g_DmyParam<99 )g_DmyParam++;
		}
	}
	if( b1 || b2 )
	{
		Stick->SetBtn1( 0 );
		Stick->SetBtn2( 0 );
		if( b2 )g_MissileRest=g_DmyParam;
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	}
	return(TRUE);
}
void	BGWDrawMissile(UsrBGWindow* this_ptr)
{
	char	str[12];
	this_ptr->ChrPut(32,32,13,&g_pDDSOffScreen[SP_TABLE_WINDOW]);

	sprintf(str,"MISSILE: %02d",g_DmyParam);
	this_ptr->Print(64,32,32,32,str);
}

bool	BGWMoveChara(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	
//	DebugPrint(0,0,"Chara");
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();
	int	pos=Stick->GetPos();

	if( pos )
	{
		if( pos>6 )
		{
			Stick->SetUp(0);
			this_ptr->CursorMove(0,-1);
		} else if( pos<4 ){
			Stick->SetDown(0);
			this_ptr->CursorMove(0,1);
		}
		if( pos % 3 ==1 ){
			Stick->SetLeft(0);
			this_ptr->CursorMove(-1,0);
		} else if( pos % 3 ==0 ){
			Stick->SetRight(0);
			this_ptr->CursorMove(1,0);
		}
	}
	
	if( b1 || b2 )
	{
		Stick->SetBtn1( 0 );
		Stick->SetBtn2( 0 );
		if( b2 )g_SelectBG=this_ptr->GetCursorY()*16+this_ptr->GetCursorX();
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	}
	return(TRUE);
}
void	BGWDrawChara(UsrBGWindow* this_ptr)
{
	int	i;
	for(i=0;i<12;i++)
	{
//		char	str[10];
//		sprintf(str,"%d",i);
		this_ptr->ChrPut(32+(i%16)*32,32+(i/16)*32,transBG[i],&g_pDDSOffScreen[SP_TABLE_BG]);
//		this_ptr->Print(32+(i%16)*32,32+(i/16)*32,10,10,str);
	}
	HDC	hDC;
	HPEN	hPen,hPen_Old;
	HBRUSH	hBruch,hBrush_Old;
	g_pDDSBack->GetDC( &hDC );

	g_Counter++;
	if(g_Counter==2)
	{
		g_Counter=0;
		hPen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
	} else {
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	}
    hPen_Old = SelectObject(hDC, hPen);
    hBrush_Old=SelectObject(hDC, GetStockObject(NULL_BRUSH));
    RECT	rect;

    rect.top	=	32+this_ptr->GetY()+this_ptr->GetCursorY()*32;
	rect.left	=	32+this_ptr->GetX()+this_ptr->GetCursorX()*32;
	rect.right	=	rect.left+32;
	rect.bottom	=	rect.top+32;
	
    Rectangle(hDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);
    SelectObject(hDC, hBrush_Old);
    SelectObject(hDC, hPen_Old);
    DeleteObject(hPen);
	g_pDDSBack->ReleaseDC( hDC );
	
}
bool	BGWMoveSave(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	
//	DebugPrint(0,0,"Save");
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();
	int	pos=Stick->GetPos();

	if( pos )
	{
		if( pos>6 )
		{
			Stick->SetUp(0);
			this_ptr->CursorMove(0,-1);
		} else if( pos<4 ){
			Stick->SetDown(0);
			this_ptr->CursorMove(0,1);
		}
	}
	
	if( b1 || ( this_ptr->GetCursorY()==0 && b2 ))
	{
		Stick->SetBtn1( 0 );
		Stick->SetBtn2( 0 );
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	} else if( this_ptr->GetCursorY()==1 && b2 ){
		Stick->SetBtn2( 0 );
		this_ptr->CreateNextWindow(&g_pDDSOffScreen[SP_TABLE_WINDOW],this_ptr->GetX()+21,this_ptr->GetY()+32+this_ptr->GetCursorY()*32+20,13,3,0,0,0,0,BGWMoveWrite,BGWDrawWrite );
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	}
	return(TRUE);
}
void	BGWDrawSave(UsrBGWindow* this_ptr)
{
	this_ptr->ChrPut(128,64+this_ptr->GetCursorY()*32,13,&g_pDDSOffScreen[SP_TABLE_WINDOW]);

	this_ptr->Print(32,32,16,32,"�L�^���܂����H");
	this_ptr->Print(128+32,64,16,32,"������");
	this_ptr->Print(128+32,96,16,32,"�͂�");
}

bool	BGWMoveWrite(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	char	str[15],str2[15];
	sprintf(str,"stage%03d.map",g_Stage);
	sprintf(str2,"stage%03d.txt",g_Stage);
	if( g_Counter==3 )	SaveGameData(str);
//	if( g_Counter==3 )	SaveGameData2(str2);
	if( g_Counter++>10 )
	{
		g_Counter=0;
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	}
	return(TRUE);
}
void	BGWDrawWrite(UsrBGWindow* this_ptr)
{
	this_ptr->Print(32,32,16,32,"�f�[�^���L�^���Ă��܂�");
}
bool	BGWMoveLoad(UsrBGWindow* this_ptr)
{
	UsrBGWindow*	t;
	
//	DebugPrint(0,0,"Save");
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();
	int	pos=Stick->GetPos();
	char	str[15];
	sprintf(str,"stage%03d.map",g_Stage);

	if( pos )
	{
		if( pos>6 )
		{
			Stick->SetUp(0);
			this_ptr->CursorMove(0,-1);
		} else if( pos<4 ){
			Stick->SetDown(0);
			this_ptr->CursorMove(0,1);
		}
	}
	if( b1 || ( this_ptr->GetCursorY()==0 && b2 ))
	{
		Stick->SetBtn1( 0 );
		Stick->SetBtn2( 0 );
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	} else if( this_ptr->GetCursorY()==1 && b2 ){
		Stick->SetBtn2( 0 );
//		if( BG->Load("str")==FALSE )
		if( LoadGameData(str)==FALSE )
		{		/*���s������*/
			this_ptr->CreateNextWindow(&g_pDDSOffScreen[SP_TABLE_WINDOW],this_ptr->GetX()+21,this_ptr->GetY()+32+this_ptr->GetCursorY()*32+20,10,4,0,0,0,0,BGWMoveMsg,BGWDrawMsgFail );
		}
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	}
	return(TRUE);
}
void	BGWDrawLoad(UsrBGWindow* this_ptr)
{
	this_ptr->ChrPut(128,64+this_ptr->GetCursorY()*32,13,&g_pDDSOffScreen[SP_TABLE_WINDOW]);

	this_ptr->Print(32,32,16,32,"�ǂݍ��݂܂����H");
	this_ptr->Print(128+32,64,16,32,"������");
	this_ptr->Print(128+32,96,16,32,"�͂�");
}
bool	BGWMoveMsg(UsrBGWindow* this_ptr)
{
	int	b1=Stick->GetBtn1();
	int	b2=Stick->GetBtn2();

	if( b1 || b2 )
	{
		if( this_ptr->DeleteThis()==FALSE )	return(FALSE);
	}
	return(TRUE);
}
void	BGWDrawMsgFail(UsrBGWindow* this_ptr)
{
	this_ptr->Print(32,32,16,32,"�����s���܂�����");
	this_ptr->ChrPut(128+16,64,14,&g_pDDSOffScreen[SP_TABLE_WINDOW]);
}
