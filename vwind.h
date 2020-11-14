/*
#define	DELUX_MODE	
*/


/************************************************/
/*												*/
/*		UsrBGWindow	Class						*/
/*												*/
/*		ver.1.00	1999/03/13					*/
/************************************************/


class	UsrBGWindow
{
private:
	int	x,y;
	int	w,h;
	int	cur_x,cur_y;
	int	cur_x_max,cur_y_max;
	LPDIRECTDRAWSURFACE4*	surface_back;	/*�\���T�[�t�F�X*/
	LPDIRECTDRAWSURFACE4*	surface_offscr;	/*�I�t�X�N���[���T�[�t�F�X*/
	UsrBGWindow*	prev;
	UsrBGWindow*	next;
	bool	(*function_move)(UsrBGWindow*);
	void	(*function_draw)(UsrBGWindow*);
public:
	UsrBGWindow( LPDIRECTDRAWSURFACE4*	s1,LPDIRECTDRAWSURFACE4*	s2,int dx,int dy,int dw,int dh,bool (*movefunc)(UsrBGWindow*),void (*drawfunc)(UsrBGWindow*) )
	{
		surface_back=s1;
		surface_offscr=s2;
		x=dx;y=dy;
		w=dw;h=dh;
		prev=next=NULL;
		function_move=movefunc;
		function_draw=drawfunc;
		cur_x_max=cur_x=0;
		cur_y_max=cur_y=0;
	}
	bool	CheckFuncName(bool (*movefunc)(UsrBGWindow*))
	{
		if(movefunc==function_move)
		{
			return(TRUE);
		}
		return(FALSE);
	}
	void	InitCursor(int dx,int dy,int dx_max,int dy_max)
	{
		cur_x=dx;
		cur_y=dy;
		cur_x_max=dx_max;
		cur_y_max=dy_max;
	}
	int	GetCursorX(void){ return(cur_x); };
	int	GetCursorY(void){ return(cur_y); };
	void	CursorMove(int	dx,int	dy)
	{
		cur_x=( (cur_x+dx+cur_x_max+1) % (cur_x_max+1) );
		cur_y=( (cur_y+dy+cur_y_max+1) % (cur_y_max+1) );
	}
	int	GetX(void){ return(x); }
	int	GetY(void){ return(y); }
	void	SetPrev(UsrBGWindow* ins){ prev=ins; }
	void	SetNext(UsrBGWindow* ins){ next=ins; }
	UsrBGWindow*	GetPrev(void){ return(prev); }
	UsrBGWindow*	GetNext(void){ return(next); }
	void	CreateNextWindow(LPDIRECTDRAWSURFACE4*	dat,int dx,int dy,int dw,int dh,int cxmax,int cymax,int sx,int sy,bool (*movefunc)(UsrBGWindow*),void (*drawfunc)(UsrBGWindow*))
	{
		UsrBGWindow*	obj;
		obj=new	UsrBGWindow(surface_back,dat,dx,dy,dw,dh,movefunc,drawfunc);
		SetNext(obj);
		obj->SetPrev(this);
		obj->InitCursor(sx,sy,cxmax,cymax);
	}
	bool	DeleteThis(void)
	{
		if(prev==NULL)
		{
			return(FALSE);
		} else {
			prev->SetNext(next);
			if( next!=NULL )next->SetPrev(prev);
			delete	this;
			return(TRUE);
		}
	}
	void	DeleteAll(void)
	{
		UsrBGWindow*	t=this;
		while( t->GetNext()!=NULL ){t=t->GetNext();}

		while(t->GetPrev()!=NULL)
		{
			t=t->GetPrev();
			delete	t->GetNext();
		}
		delete	t;
	}
	void	Print(int	dx,int dy,int	size_x,int	size_y,char* str)
	{
		HDC	hDC;
		int	i;

		if( (*surface_back)->GetDC(&hDC) == DD_OK)
		{
			HFONT	hFont,hFontOld;
			hFont = CreateFont(
			size_y,			//�t�H���g����
			size_x,			//������
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
#ifdef	DELUX_MODE
			SetTextColor(hDC, RGB(0, 0, 0));
			for(i=0;i<9;i++)
			{
				TextOut(hDC,x+dx+(i%3)*2,y+dy+(i/3)*2,str,strlen(str));
			}
#endif
			SetTextColor(hDC, RGB(255, 255, 255));
			TextOut(hDC,x+dx+2,y+dy+2,str,strlen(str));

			SelectObject(hDC, hFontOld);
			DeleteObject(hFont);

			(*surface_back)->ReleaseDC(hDC);
		}
	}
	/****************************************************************/
	/*	ChrPut														*/
	/*		dx:�E�C���h��̂w���W									*/
	/*		dx:�E�C���h��̂x���W									*/
	/*		num:�L�����N�^�ԍ�										*/
	/****************************************************************/
	void	ChrPut(int	dx,int dy,int num,LPDIRECTDRAWSURFACE4* offscr)
	{
		RECT	rect;
		RECT	rect_screen;

		/*���́Arect�ł́A�\������Ȃ����s�� 99/03/15*/

		rect_screen.top=0;
		rect_screen.bottom=480;
		rect_screen.left=0;
		rect_screen.right=640;

//		rect.left=x;
//		rect.right=rect.left+w*32;
//		rect.top=y;
//		rect.bottom=rect.top+h*32;

		BG_Put(
			surface_back,
			x+dx,y+dy,
			32,32,
			num,
			&rect_screen,
			offscr
		);
	}
	void	DrawAll()
	{
		UsrBGWindow*	t=this;
		while( t->GetNext()!=NULL )
		{
			t->Draw();
 			t=t->GetNext();
		}
		t->Draw();
	}
	bool	UpDate()
	{
		UsrBGWindow*	t=this;
		int	cnt=0;
//		char	str[50];
		while( t->GetNext()!=NULL ){cnt++;t=t->GetNext();}

//		sprintf(str,"num:%d",cnt);
//		Print(400,100,15,15,str);
		return( t->CallMoveFunc() );
	}
	bool	CallMoveFunc(void)
	{
		return(function_move(this));
	}
	void	Draw()
	{
		int	i;
		RECT	rect;
		RECT	rect_screen;

		rect_screen.top=0;
		rect_screen.bottom=480;
		rect_screen.left=0;
		rect_screen.right=640;

			
		for(i=0;i<w*h;i++)
		{
			BG_Put(
				surface_back,
				x+(i%w)*32,
				y+(i/w)*32,
				32,
				32,
				0,
				&rect_screen,
				surface_offscr
			);
		}
		BG_Put(surface_back,x			,y			,32,32,1,&rect_screen,surface_offscr);
		BG_Put(surface_back,x+(w-1)*32	,y			,32,32,2,&rect_screen,surface_offscr);
		BG_Put(surface_back,x			,y+(h-1)*32	,32,32,3,&rect_screen,surface_offscr);
		BG_Put(surface_back,x+(w-1)*32	,y+(h-1)*32	,32,32,4,&rect_screen,surface_offscr);
		BG_Put(surface_back,x-32		,y-32		,32,32,5,&rect_screen,surface_offscr);
		BG_Put(surface_back,x+w*32		,y-32		,32,32,7,&rect_screen,surface_offscr);
		BG_Put(surface_back,x-32		,y+h*32		,32,32,10,&rect_screen,surface_offscr);
		BG_Put(surface_back,x+w*32		,y+h*32		,32,32,12,&rect_screen,surface_offscr);
		for(i=0;i<w;i++)
		{
			BG_Put(surface_back,x+i*32	,y-32	,32,32,6,&rect_screen,surface_offscr);
			BG_Put(surface_back,x+i*32	,y+h*32	,32,32,11,&rect_screen,surface_offscr);
		}
		for(i=0;i<h;i++)
		{
			BG_Put(surface_back,x-32	,y+i*32	,32,32,8,&rect_screen,surface_offscr);
			BG_Put(surface_back,x+w*32	,y+i*32	,32,32,9,&rect_screen,surface_offscr);
		}
		function_draw(this);
	}
};








class	UsrSubWindow
{
private:
	int	x,y;
	int	w,h;
	HWND	hWnd;
	HINSTANCE hInstance;
	int	makefail;
public:
	UsrSubWindow( HINSTANCE hInst,HWND hWnd_parent,int dx,int dy,int dw,int dh,WNDPROC wndproc )
	{
		x=dx;y=dy;w=dw;h=dh;
		hInstance=hInst;
		makefail=0;

		WNDCLASS	myProg;

		/*WNDCLASS�\���̂̊e�����o�̒�`*/

		/*style:CS_HREDRAW�i���������̃T�C�Y�ύX����������S�̂�`�������j	*/
		/*CS_VREDRAW�i���������̃T�C�Y�ύX�őS�̂�`�������j�Ȃǂ̒萔���w��*/
		myProg.style		=	CS_HREDRAW | CS_VREDRAW;
		/*lpfnWndProc:�E�B���h�E �v���V�[�W��(���b�Z�[�W�̏�������ɍs���֐�)��*/
		myProg.lpfnWndProc	=	wndproc;
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
		myProg.lpszClassName=	"subwind";

		/*���ɂ���CreateWindow��CreateWindowEx�֐����R�[�����邽�߂�*/
		/*�K�v�ȃE�B���h�E�N���X��o�^����							*/
		if( !RegisterClass( &myProg ) )
		{
			makefail=1;
			delete	this;
			return;
		}

		/*�E�C���h�̐���*/
		/*CreateWindow�֐��̖߂�l�����o�����E�B���h�E�̃E�B���h�E�n���h���ƂȂ�B*/
		/*���̃E�B���h�E�ɑ΂��ĉ�������Ƃ��͂��̃E�B���h�E�n���h�����K�v�ɂȂ�B*/
		hWnd = CreateWindowEx(
			WS_EX_TOPMOST,
			"subwind",				/*class name:�o�^����N���X��*/
 			"",/*window name:�^�C�g���o�[�\���L���v�V����*/
			/*window style:�E�B���h�E�X�^�C���ɂ́uWS_���Ƃ��v���w��B*/
			/*�ʏ��WS_OVERLAPPEDWINDOW�w�肵�Ă����Ɗy�B*/
			/*����́A�uWS_���Ƃ��v�������������Ďw�肵���̂Ɠ����B*/
			WS_POPUP,
			0,
			0,
			GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN),

			/*handle to parent or owner window:�e�E�B���h�E�̃n���h���B*/
			/*�q���E�B���h�E�����Ƃ��K�v�B�e�����Ƃ���NULL*/
			hWnd_parent,					/*handle to parent or owner window */
			/*handle to menu or child-window identifier:���j���[�n���h���B*/
			/*���j���[��t����Ƃ��K�v�B�N���X���j���[(WNDCLASS�ɓo�^���ꂽ���j���[)*/
			/*���g���Ƃ���NULL*/
			NULL,					/*handle to menu or child-window identifier */
			hInstance,				/*handle to application instance */
			/*pointer to window-creation data:NULL���w�肷��*/
			NULL					/*pointer to window-creation data */
		);
		Draw();
	}
	~UsrSubWindow( void )
	{
		if(makefail==0)
		{
			DestroyWindow( hWnd );
		}
	}
	void	Draw(void)
	{
		PAINTSTRUCT	ps;
		RECT	rect;
		HBRUSH	hBrush, hOldBrush;

		HDC hDC = BeginPaint(hWnd, &ps);
		
		hBrush = CreateSolidBrush( RGB(155,155,63) );
		hOldBrush = SelectObject(hDC, hBrush);

		rect.top=y;
		rect.bottom=y+h;
		rect.left=x;
		rect.right=x+w;
		FillRect(hDC,&rect,hBrush);

		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);

		EndPaint(hWnd, &ps);
	}
};
