/****************************************************************/
/*																*/
/*	Shooting Game Lib	SGLib.h									*/
/*																*/
/*	Ver	0.01	1999/02/04	16:50	first fix					*/
/*																*/
/*																*/
/****************************************************************/


#define	GAMEOBJCODE	int
#define	GAMEMES		int

/*�L�����N�^�O���[�s���O�p�h�c*/
#define	ID_CHR_NOID		0
#define ID_CHR_ALL		1	/*�S�ẴO���[�v�Ɋ܂܂��*/
#define ID_CHR_OTHERS	2	/*�ǂ̃O���[�v�ɂ��܂܂�Ȃ�*/
#define ID_CHR_GROUP_A	16	/*�`�O���[�v�Ɋ܂܂��*/
#define ID_CHR_GROUP_B	17	/*�a�O���[�v�Ɋ܂܂��*/

/*�������ʃR�[�h*/
#define	JYOUKEN_NORMAL	0
#define	JYOUKEN_EQUAL	1

#define	GAMEOBJ_MES_NOTHING			0
#define	GAMEOBJ_MES_ENERGY_LOW		1
#define	GAMEOBJ_MES_SCREEN_OUT		2
#define	GAMEOBJ_MES_MAKE_SPREAD		3
#define	GAMEOBJ_MES_MAKE_DSPREAD	4
#define	GAMEOBJ_MES_TOUCH_BG		5
#define	GAMEOBJ_MES_MAKE_NEW		6
#define	GAMEOBJ_MES_TIME_OUT		7
#define	GAMEOBJ_MES_TERM			8

/*�L�����N�^�����R�[�h*/
#define	ATRB_CODE_SHIP		1
#define	ATRB_CODE_MISSILE	2
#define	ATRB_CODE_SPREAD	3
#define	ATRB_CODE_BLOCK		4


/****************************************************/
/*													*/
/*	�N���X���C�u����								*/
/*													*/
/****************************************************/

/****************************************************/
/*													*/
/*	UsrCharacter�N���X								*/
/*													*/
/*	ver.1.00	1999/01/26							*/
/*													*/
/****************************************************/
class	UsrCharacter
{
private:
	int	x;		/*���݂̂w���W*/
	int	y;		/*���݂̂x���W*/
	int	w;		/*�L�����N�^�[�̉���*/
	int	h;		/*�L�����N�^�[�̍���*/
	int	group_id;	/*�O���[�s���O�p�h�c*/
	int	attribute;	/*�����i���g�p�j*/
	int	power;		/*�ڐG���U����*/
	int	energy;		/*������*/
	int	time;
	int	AnimePattern;
	GAMEOBJCODE	Unique_id;
	bool	SpriteSwitch;
	RECT	SpriteRect;
	RECT	Screen;
	LPDIRECTDRAWSURFACE4*	surface_back;	/*�\���T�[�t�F�X*/
	LPDIRECTDRAWSURFACE4*	surface_offscr;	/*�I�t�X�N���[���T�[�t�F�X*/
	UsrBackGround*	BGaddress;
public:
	/*	�R���X�g���N�^	*/
	UsrCharacter(int atr,int id, int dx,int dy,int p, int e, RECT* scrrect,RECT* offscrrect,LPDIRECTDRAWSURFACE4* back,LPDIRECTDRAWSURFACE4* offscr,UsrBackGround* bg)
	{
		SetXY( dx, dy );
		
		if(offscrrect!=NULL)
		{
			w=offscrrect->right-offscrrect->left;
			h=offscrrect->bottom-offscrrect->top;
			SpriteRect.top=offscrrect->top;
			SpriteRect.bottom=offscrrect->bottom;
			SpriteRect.left=offscrrect->left;
			SpriteRect.right=offscrrect->right;
		}
		if(scrrect!=NULL)
		{
			Screen.left=scrrect->left;Screen.right=scrrect->right;
			Screen.top=scrrect->top;Screen.bottom=scrrect->bottom;
		}
		group_id=id;
		power=p;
		energy=e;
		time=0;
		attribute=atr;
		surface_back=back;
		surface_offscr=offscr;
		BGaddress=bg;
		AnimePattern=0;
		SpriteSwitch=TRUE;
	}
	void	SetCode( GAMEOBJCODE id ){ Unique_id=id; }
	GAMEOBJCODE	GetCode( void ){ return(Unique_id); }
	RECT*	GetScreenRect(void){ return(&Screen); }
	RECT*	GetSpriteRect(void){ return(&SpriteRect); }
	LPDIRECTDRAWSURFACE4*	GetSurfaceBack(void){ return(surface_back); }
	LPDIRECTDRAWSURFACE4*	GetSurfaceOffscr(void){ return(surface_offscr); }
	UsrBackGround*	GetBGObject(void){ return(BGaddress); }
	void	SetTimer(int t)
	{
		time=t;
	}
	GAMEMES	AddXY(int	ax,int	ay)		/*	���W�̈ړ�	*/
	{
		int	dx=x+ax,dy=y+ay;
		if( CheckError(dx,dy) != TRUE )
		{
			return(GAMEOBJ_MES_SCREEN_OUT);
		} else {
			x=dx;y=dy;
			if( BGaddress!=NULL && ( BGaddress->GetBGStatus(x,y) & BG_STATUS_THROUGH )==0 )
			{
				return(GAMEOBJ_MES_TOUCH_BG);
			}
		}
		return(GAMEOBJ_MES_NOTHING);
	}
	bool	CheckError(int	dx,int dy)
	{
		if( dx>Screen.right || dx+w<Screen.left || dy>Screen.bottom || dy+h<Screen.top )
		{
			return(FALSE);	/*���W�͈͊O�ɏo���ꍇ�̓G���[*/
		} else {
			return(TRUE);	/*���W�͈͓�*/
		}
	}
	void	ChangeAnimetion( int anim )
	{
		AnimePattern=anim;
	}
	bool	DownEnergy( int	damage )	/*�G�l���M�[������������֐�*/
	{
		energy-=damage;		/*������ɃG�l���M�[�����s�����ꍇ�̓G���[*/
		if( energy<=0 )
		{
			energy=0;
			return( FALSE );
		} else return ( TRUE );
	}
	void	SetEnergy( int e ){	energy=e;	}
	void	SetXY( int dx, int dy ){	x=dx;y=dy;	}
	void	SetWH( int dw, int dh ){	w=dw;h=dh;	}
	int	GetAttribute( void ){ return( attribute ); }
	int	GetID(void){	return(group_id);	}
	bool	CheckTimer( void )
	{
		time--;
		if(time<0)return(FALSE);
		return( TRUE );
	}
	void	SpriteOff(void){ SpriteSwitch=FALSE; }
	int	GetX( void ){	return(x);	}
	int	GetY( void ){	return(y);	}
	int	GetWidth( void ){	return(w);	}
	int	GetHeight( void ){	return(h);	}
	int	GetEnergy( void ){	return( energy );	}
	int	GetPower( void ){	return( power );	}
	void	Draw( void )
	{
		RECT	rect_from;	/*�I�t�X�N���[���T�[�t�F�X���̈ʒu���*/
		int	sx=GetX();	/*�o�b�N�T�[�t�F�X�ւ̕\���ʒu*/
		int	sy=GetY();	/*�o�b�N�T�[�t�F�X�ւ̕\���ʒu*/

		if( SpriteSwitch==FALSE )return;
		
		rect_from.left = SpriteRect.left+w*AnimePattern;
		rect_from.right = SpriteRect.right+w*AnimePattern;
		rect_from.top = SpriteRect.top;
		rect_from.bottom = SpriteRect.bottom;

		if( SpriteClipping(&sx,&sy,w, h,&rect_from,&Screen) == TRUE )
		{
			(*surface_back)->BltFast( sx, sy, (*surface_offscr), &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
		}
	}
	virtual	GAMEMES	Move( void ){ return(GAMEOBJ_MES_NOTHING); }
};





/****************************************************/
/*													*/
/*	UsrMissile�N���X								*/
/*													*/
/****************************************************/
class	UsrMissile:public	UsrCharacter	/*UsrCharacter�N���X���p��*/
{
private:
	int	addx;		/*�w���W�̑����l*/
	int	addy;		/*�x���W�̑����l*/
public:		/*�R���X�g���N�^*/
	UsrMissile( int atr,int id,int x,int y,int ax, int ay, int p, int e ,RECT* scrrect,RECT* offscrrect,LPDIRECTDRAWSURFACE4* back,LPDIRECTDRAWSURFACE4* offscr,UsrBackGround* bg)
		:UsrCharacter(atr,id,x,y,p,e,scrrect,offscrrect,back,offscr,bg)
	{
		SetBect( ax, ay );
	}
	void	SetParam( int x,int y, int ax, int ay )
	{
		SetXY(x,y);
		SetBect( ax, ay );
	}
	void	SetBect( int ax, int ay ){	addx=ax;addy=ay;	}
	void	SetAX( int ax ){ addx=ax; }
	void	SetAY( int ay ){ addy=ay; }
	int	GetAY( void ){	return( addy );	}
	int	GetAX( void ){	return( addx );	}
	virtual	GAMEMES	Move( void ){ return(GAMEOBJ_MES_NOTHING); }
};

/****************************************************/
/*													*/
/*	UsrDefaultMissile�N���X							*/
/*													*/
/****************************************************/
class	UsrDefaultMissile:public	UsrMissile	/*UsrCharacter�N���X���p��*/
{
private:
public:
	UsrDefaultMissile( int id,int x,int y, int ax, int ay, int p,RECT* scrrect,RECT* offscrrect,LPDIRECTDRAWSURFACE4* back,LPDIRECTDRAWSURFACE4* offscr,UsrBackGround* bg )
		:UsrMissile(ATRB_CODE_MISSILE, id, x, y, ax, ay, p, 1 ,scrrect,offscrrect,back,offscr,bg){}
	GAMEMES	Move( void )
	{
		GAMEMES	message;

		/*���쌋�ʃ��b�Z�[�W���M*/
		if( GetEnergy()<=0 ) return( GAMEOBJ_MES_ENERGY_LOW );
		message=AddXY( GetAX(), GetAY() );
		if( message!=GAMEOBJ_MES_NOTHING )
		{
			return( message );
		}
		return(GAMEOBJ_MES_NOTHING);
	}
};
/****************************************************/
/*													*/
/*	DeathSpread�N���X								*/
/*													*/
/****************************************************/
class	UsrDeathSpread:public	UsrMissile	/*UsrCharacter�N���X���p��*/
{
private:
public:
	UsrDeathSpread( int id,int x,int y, int ax, int ay, RECT* scrrect,RECT* offscrrect,LPDIRECTDRAWSURFACE4* back,LPDIRECTDRAWSURFACE4* offscr,UsrBackGround* bg )
		:UsrMissile(ATRB_CODE_MISSILE, id, x, y, ax, ay, 1, 1 ,scrrect,offscrrect,back,offscr,bg)
	{
		SetTimer(10);
//		SpriteOff();
	}
	GAMEMES	Move( void )
	{
		if( CheckTimer() == FALSE )
		{
			return( GAMEOBJ_MES_TIME_OUT );
		} else {
			AddXY( GetAX(), GetAY() );
			return(GAMEOBJ_MES_MAKE_SPREAD);
		}
	}
};

/****************************************************/
/*													*/
/*	UsrDefaultSpread�N���X							*/
/*													*/
/****************************************************/
class	UsrDefaultSpread:public	UsrMissile	/*UsrCharacter�N���X���p��*/
{
private:
	int	Anime;
public:
	UsrDefaultSpread( int id,int x,int y, int ax, int ay ,RECT* scrrect,RECT* offscrrect,LPDIRECTDRAWSURFACE4* back,LPDIRECTDRAWSURFACE4* offscr,UsrBackGround* bg )
		:UsrMissile(ATRB_CODE_SPREAD, id, x, y, ax, ay, 1, 10 ,scrrect,offscrrect,back,offscr,bg)
	{
		SetTimer(10);
		Anime=0;
	}
	GAMEMES	Move( void )
	{
		ChangeAnimetion(Anime++);
		if( CheckTimer() == FALSE )
		{
			return( GAMEOBJ_MES_TIME_OUT );
		} else {
			AddXY( GetAX(), GetAY() );
			return( GAMEOBJ_MES_NOTHING );
		}
	}
};

struct	UsrMsDat
{
	UsrMsDat*	next;
	UsrMsDat*	prev;
	UsrCharacter*	dat;
};


/****************************************************/
/*													*/
/*	UsrChrObjectMgr�N���X							*/
/*													*/
/****************************************************/
class	UsrChrObjectMgr
{
private:
	UsrMsDat*	top;
	GAMEOBJCODE	Totalgameobj;
	GAMEMES	(*function_procedure)(GAMEMES,UsrCharacter*);
public:
	UsrChrObjectMgr( GAMEMES	(*proc)(GAMEMES,UsrCharacter*) )
	{
		top=new UsrMsDat;
		top->dat=new UsrDefaultSpread(ID_CHR_OTHERS,0,0,0,0,NULL,NULL,NULL,NULL,NULL);
		top->next=NULL;
		top->prev=NULL;
		Totalgameobj=1;
		function_procedure=proc;
	}
	~UsrChrObjectMgr( void )
	{
		UsrMsDat*	p;
		UsrMsDat*	prev;

		p=top;
		while(p->next!=NULL)
		{
			prev=p;
			p=p->next;
			delete	prev->dat;
			delete	prev;
		}
		delete	p->dat;
		delete	p;
	}
	void	Draw( void )
	{
		UsrMsDat*	p;

		p=top->next;
		while( p!=NULL )
		{
			/*�L�����N�^�I�u�W�F�N�g�`��*/
			p->dat->Draw();
			p=p->next;
		}
	}
	GAMEOBJCODE	AddObject( UsrCharacter*	dat)
	{
		UsrMsDat*	p;
		UsrMsDat*	pnew;
		UsrMsDat*	pprev;

		dat->SetCode(Totalgameobj);
		pnew=new UsrMsDat;	/*�V�K�f�[�^�̍쐬*/
		pnew->dat=dat;
		pnew->next=NULL;

		pprev=p=top;
		while( p->next!=NULL )
		{
			pprev=p;
			p=p->next;	/*���X�g�f�[�^�Ō���Ɉړ�*/
		}
		p->next=pnew;		/*�V�K�f�[�^�̐ڑ�*/
		p->prev=pprev;		/*�V�K�f�[�^�̐ڑ�*/

		return(Totalgameobj++);
	}
	int	CheckTouch( int id,int jyouken,int x, int y, int w, int h ,int	dmg)
	{
		UsrMsDat*	p;
		UsrMsDat*	prev;
		int	ret=0;
		prev=top;
		p=top->next;
		while( p!=NULL )
		{
			int	tx=p->dat->GetX();		/*�^�[�Q�b�g�e�̂w���W*/
			int	ty=p->dat->GetY();		/*�^�[�Q�b�g�e�̂x���W*/
			int	tw=p->dat->GetWidth();	/*�^�[�Q�b�g�e�̉���*/
			int	th=p->dat->GetHeight();	/*�^�[�Q�b�g�e�̍���*/
			int	tid=p->dat->GetID();	/*�^�[�Q�b�g�e�̂h�c*/

			if( ( jyouken==JYOUKEN_NORMAL && tid != id && tid != ID_CHR_OTHERS )
				|| ( jyouken==JYOUKEN_EQUAL && tid==id ) )
			{
				 /*�e�ƈ����̈�Ƃ̐ڐG����*/
				if( abs( x-tx+(w-tw)/2 ) < (w+tw)/2
					&& abs( y-ty+(h-th)/2 ) < (h+th)/2
					&&	p->dat->GetEnergy()>0			)
				{
//					AddDat(
//						new UsrDefaultSpread(	/*�ڐG���ɂ̓X�v���b�h����*/
//							ID_CHR_OTHERS,
//							tx+tw/2,
//							ty+th/2,
//							0,
//							0
//						)
//					);
					ret+=p->dat->GetPower();
					p->dat->DownEnergy(dmg);
				}
			}
			prev=p;
			p=p->next;
		}
		return( ret );
	}
	int	Move( void )
	{
		UsrMsDat*	p;
		UsrMsDat*	prev;
		bool	game_over=TRUE;
		int	id_store=ID_CHR_OTHERS;
		int	id_getting=ID_CHR_OTHERS;
		int	ret_id=ID_CHR_OTHERS;
		GAMEMES	message=GAMEOBJ_MES_NOTHING;

		prev=top;
		p=top->next;
		while( p!=NULL )
		{
			/*���݂���L�����N�^�̎�ʂ𒲍�*/
			id_getting=p->dat->GetID();
			if( id_getting != ID_CHR_OTHERS
				&& id_getting != ID_CHR_ALL
				 )
			{	
				if( id_getting != id_store && id_store != ID_CHR_OTHERS )
				{
					game_over=FALSE;
				}
				id_store=id_getting;
			}

			/*�L�����N�^�I�u�W�F�N�g�����ݕs�\�̎�*/
			message=p->dat->Move();
			if( message!=GAMEOBJ_MES_NOTHING )
			{
				if(	function_procedure(message,p->dat) == GAMEOBJ_MES_TERM )
				{
//					DeleteObject(p);
					prev->next=p->next;
					delete	p->dat;
					delete	p;
					p=prev;
				}
			}
			prev=p;
			p=p->next;
		}
		if( game_over == TRUE )	return(id_store);
			else return(ID_CHR_NOID);
	}
	void	DeleteObject(UsrMsDat* t)
	{
		if(t==top)return;

		if( t->prev!=NULL )t->prev->next=t->next;
		if( t->next!=NULL )t->next->prev=t->prev;
		delete	t->dat;
		delete	t;
	}
	UsrCharacter*	GetObject(int id,int atr)
	{
		UsrMsDat*	p;
		UsrMsDat*	prev;
		UsrCharacter*	ret=NULL;
		prev=top;
		p=top->next;
		while( p!=NULL )
		{
			int	tid=p->dat->GetID();	/*�^�[�Q�b�g�e�̂h�c*/
			int	tat=p->dat->GetAttribute();
			
			if( tid==id && tat==atr )
			{
				ret=p->dat;
				break;
			}
			prev=p;
			p=p->next;
		}
		return( ret );
	}
};
