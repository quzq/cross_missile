#include	<iostream.h>
#include	<fstream.h>

/*BGステータスセット*/
#define	BG_STATUS_THROUGH	1

static	bool	SpriteClipping(int*,int*,int,int,RECT*,RECT* );

bool	BG_Put(LPDIRECTDRAWSURFACE4*,int,int,int,int,int,RECT*,LPDIRECTDRAWSURFACE4*);

/***********************************/
/*	背景描画クラス					*/
/***********************************/
class	UsrBackGround
{
private:
	int	x,y;
	int	map_x_max,map_y_max;
	int	map_x_max_next,map_y_max_next;
	int	chr_width,chr_height;
	int	width,height;	/*表示範囲*/
	char*	mapdat;		/*マップデータ*/
	RECT	rect_screen;
	LPDIRECTDRAWSURFACE4*	surface_primary;	/*primaryサーフェス*/
	LPDIRECTDRAWSURFACE4*	surface_back;	/*表示サーフェス*/
	LPDIRECTDRAWSURFACE4*	surface_offscr;	/*オフスクリーンサーフェス*/
public:
	UsrBackGround(LPDIRECTDRAWSURFACE4*	s1, LPDIRECTDRAWSURFACE4*	s2, int	dx,int	dy,int xmx,int ymx,LPDIRECTDRAWSURFACE4*	d )
	{
		surface_primary=s1;
		surface_back=s2;
		x=dx;
		y=dy;
		map_x_max=xmx;
		map_y_max=ymx;
		InitNextMapSize();
		width=20;
		height=15;
		chr_width=32;
		chr_height=32;
		surface_offscr=d;
		rect_screen.top=0;
		rect_screen.bottom=480;
		rect_screen.left=0;
		rect_screen.right=640;
		mapdat=new	char[map_x_max*map_y_max];
	}
	char*	GetMapDat(void){ return(mapdat); }
	void	InitNextMapSize(void)
	{
		map_x_max_next=map_x_max;
		map_y_max_next=map_y_max;
	}
	void	SetMap(int	dw,int	dh,char* newmap)
	{
		delete	mapdat;
		mapdat=new	char[dw*dh];
		for(int i=0;i<dw*dh;i++)mapdat[i]=newmap[i];
		map_x_max=dw;
		map_y_max=dh;
		InitNextMapSize();
	}
	int	GetNextMapSizeW(void){return(map_x_max_next);}
	int	GetNextMapSizeH(void){return(map_y_max_next);}
	void	SetNextMapSizeW(int nx){ map_x_max_next=nx; }
	void	SetNextMapSizeH(int ny){ map_y_max_next=ny; }
	void	UpdateMapSize(void)
	{
		char*	newmap;
		int	i,j;

		newmap=new	char[map_x_max_next*map_y_max_next];
		for(j=0;j<map_y_max && map_y_max_next;j++)
		{
			for(i=0;i<map_x_max && map_x_max_next;i++)
			{
				newmap[i+j*map_x_max_next]=mapdat[i+j*map_x_max];
			}
		}
		delete	mapdat;
		mapdat=newmap;
		map_x_max=map_x_max_next;
		map_y_max=map_y_max_next;
	}
	void	DrawAll( void )
	{
		int	i,j;
		for(j=0;j<height+1;j++)
		{
			for(i=0;i<width+1;i++)
			{
				/*put*/
				BG_Put(
					surface_back,
					((x%chr_width)*-1)+i*chr_width,
					((y%chr_height)*-1)+j*chr_height,
					chr_width,
					chr_height,

					GetBG(x+i*chr_width,y+j*chr_height),
					&rect_screen,
					surface_offscr
				);
			}
		}
	}
	char	GetBG(int gx,int gy)
	{
		/*座標変換（実座標をＢＧ座標へ）*/
		gx=( gx/chr_width )%map_x_max;
		gy=( gy/chr_width )%map_y_max;

		return(
			mapdat[gx+map_x_max*gy]
		);
	}
	int	GetBGStatus(int gx,int gy)
	{
		int	status=0;

		/*座標変換（実座標をＢＧ座標へ）*/
		gx=( (gx+x)/chr_width )%map_x_max;
		gy=( (gy+y)/chr_width )%map_y_max;

		if(	mapdat[gx+map_x_max*gy]<20) status+=BG_STATUS_THROUGH;
		return(status);
	}
	bool	Save(char*	filename)
	{
		char	str[64];
		char	outstr[256];
		int		i;
		
		
		str[0]=0x00;
		outstr[0]=0x00;
		
		ofstream	outfile(filename);
		
		outfile << "//////////////////////////////////////////////////////\n";
		sprintf(outstr,"//		BG Map Datas	filename:%s\n",filename );
		outfile << outstr;
		outfile << "//\n";
		
		sprintf(outstr,"char	MapDat[%d*%d]={\n",map_x_max,map_y_max );
		outfile << outstr;

		outstr[0]='\t';
		outstr[1]='\0';
		for(i=0;i<map_x_max*map_y_max;i++)
		{
			if( i%20 == 19 )
			{
				if( i==map_x_max*map_y_max-1 )
				{
					sprintf(str,"%d\n",(int)mapdat[i] );
				} else {
					sprintf(str,"%d,\n",(int)mapdat[i] );
				}
				strcat(outstr,str);
				outfile << outstr;
				str[0]=0x00;
				outstr[0]='\t';
				outstr[1]='\0';
			} else {
				sprintf(str,"%d,",(int)mapdat[i] );
				strcat(outstr,str);
			}
		}
		outfile << "};";
		return(TRUE);
	}
	bool	Load(char*	filename)
	{
		char	str[256];
		int	cnt=0;
		int	i,j,dat;
		int	dw,dh;
		bool	sizeset=FALSE;
		char*	dmymap=NULL;
		bool	status=TRUE;
		bool	loop=TRUE;
		ifstream	infile(filename);

		for(j=0;j<255;j++)
		{
			infile.getline(str,255,'\n');
			if( str[0]=='\0' )continue;
			if(str[0]=='/' && str[1]=='/')	continue;
			if(str[0]=='c' && str[1]=='h' && str[2]=='a' && str[3]=='r' )
			{
				if(sizeset==TRUE)status=FALSE;
				/*Get width & height*/
				for(i=0;str[i]!='\0' && str[i]!='[';i++);
				if( str[i]=='\0' )
				{
					continue;
				}
				for(;str[i]<'0' || str[i]>'9';i++);
				dat=0;
				for(;str[i]>='0'&& str[i]<='9';i++  )
				{
					dat=dat*10+str[i]-'0';
				}
				dw=dat;
				for(;str[i]<'0' || str[i]>'9';i++);
				dat=0;
				for(;str[i]>='0'&& str[i]<='9';i++  )
				{
					dat=dat*10+str[i]-'0';
				}
				dh=dat;
				sizeset=TRUE;
				dmymap=new	char[dw*dh];
				continue;
			}
			for(i=0;str[i]==' ' || str[i]=='\t';i++  );
			if( str[i]>='0' && str[i]<='9')
			{
				if(sizeset!=TRUE)
				{
					status=FALSE;
					continue;
				}
				loop=TRUE;
				while(loop==TRUE)
				{
					int	dmyloop=0;
					dmyloop++;
					dat=0;
					for(;str[i]>='0'&& str[i]<='9';i++  )
					{
						dat=dat*10+str[i]-'0';
					}
					dmymap[cnt++]=(char)dat;
					if(cnt==dw*dh || dmyloop>999)
					{
						j=999;/*exit*/
						break;
					}
					for(;str[i]<'0' || str[i]>'9';i++)
					{
						if(str[i]=='\0')
						{
							loop=FALSE;
							break;
						}
					}
				}
			}
		}
		if(status==FALSE)
		{
			if( sizeset==TRUE )delete	dmymap;
		} else {
			delete	mapdat;
			map_x_max=dw;
			map_y_max=dh;
			mapdat=dmymap;
		}
		return(status);
	}
	int	SearchBGChara( char	num )
	{
		int	i;
		int	ret=0;
		for(i=0;i<width*height;i++)
		{
			if(mapdat[i]==num)ret++;
		}
		return(ret);
	}
	void	PutBG(int px,int py,char	num)
	{
		/*座標変換（実座標をＢＧ座標へ）*/
		px=( (x+px)/chr_width )%map_x_max;
		py=( (y+py)/chr_width )%map_y_max;

		mapdat[px+map_x_max*py]=num;
	}
	bool	AddXY(int	ax,int	ay)
	{
		int	backup_x;
		int	backup_y;

		backup_x=x+=ax;
		backup_y=y+=ay;

		if( x<0 )	x=map_x_max*chr_width+x;
		if( y<0 )	y=map_y_max*chr_height+y;
		if( x>=map_x_max*chr_width )	x=x%map_x_max*chr_width;
		if( y>=map_y_max*chr_height )	y=y%map_y_max*chr_height;

		if( x!=backup_x || y!=backup_y )/*座標が修正された場合*/
		{
			return(FALSE);
		}
		return(TRUE);
	}
	bool	Scroll(int	sx,int sy)
	{
		RECT	rect_from;
		int	dx=0,dy=0;
		int	bg_dx_min,bg_dy_min;
		int	bg_dx_max,bg_dy_max;
		int	i,j;

		/*Set Param Failed */
		if(sx>16 || sx<-16 || sy<-16 || sy>16 )	return(FALSE);
		if(sx==0 && sy==0) return(FALSE);

		AddXY(sx,sy);

		dx-=sx;
		dy-=sy;
		rect_from.top = rect_screen.top;
		rect_from.bottom = rect_screen.bottom;
		rect_from.left = rect_screen.left;
		rect_from.right = rect_screen.right;

		if( SpriteClipping( &dx, &dy,rect_from.right-rect_from.left,rect_screen.bottom-rect_from.top,&rect_from,&rect_screen ) == TRUE )
		{
			(*surface_back)->BltFast( dx, dy, (*surface_primary), &rect_from, DDBLTFAST_WAIT);
		}

		if( sx!=0 )
		{
			if(sx<0)
			{
				bg_dx_min=0;
			} else {
				bg_dx_min=width;
			}
			for(j=0;j<height+1;j++)
			{
				BG_Put(
					surface_back,
					((x%chr_width)*-1)+bg_dx_min*chr_width,
					((y%chr_height)*-1)+j*chr_height,
					chr_width,
					chr_height,
					GetBG(x+bg_dx_min*chr_width,y+j*chr_height),
//					GetBG(x+bg_dx_min*chr_width,y+j*chr_height)%9,/*debug*/
					&rect_screen,
					surface_offscr
				);
			}
		}
		if( sy!=0 )
		{
			if(sy<0)
			{
				bg_dy_min=0;
			} else {
				bg_dy_min=height;
			}
			for(i=0;i<width+1;i++)
			{
				BG_Put(
					surface_back,
					((x%chr_width)*-1)+i*chr_width,
					((y%chr_height)*-1)+bg_dy_min*chr_height,
					chr_width,
					chr_height,

					GetBG(x+i*chr_width,y+bg_dy_min*chr_height),
//					GetBG(x+i*chr_width,y+bg_dy_min*chr_height)%9,/*debug*/
					&rect_screen,
					surface_offscr
				);
			}
		}

		return(TRUE);
	}
};

/****************************************************************/
/*	About	BG													*/
/*		32*32固定スプライト										*/
/*		バッファサイズ:640*384									*/
/*		最大定義数:240(0-239)									*/
/*	BG_Put(表示対象サーフェス,Ｘ,Ｙ,定義番号)						*/
/****************************************************************/
bool	BG_Put(LPDIRECTDRAWSURFACE4*	target,int	x,int	y,int	w,int	h,int	num,RECT* rect_screen,LPDIRECTDRAWSURFACE4*	dat)
{
	RECT	rect_from;	/*オフスクリーンサーフェス内の位置情報*/
	int		x_from,y_from;
	int	bg_w,bg_h;

	/*chech	error*/
	if( num<0 || num>239 )	return(FALSE);
	if( target==NULL )	return(FALSE);

	bg_h=( rect_screen->bottom-rect_screen->top)/h;	/*対象画面のＢＧheight*/
	bg_w=( rect_screen->right-rect_screen->left)/w;	/*対象画面のＢＧwidth*/
//	y_from=num/bg_h;			/*データ読み込みＸ座標*/
//	x_from=num-y_from*bg_w;		/*データ読み込みＹ座標*/
	y_from=num/bg_w;			/*データ読み込みＸ座標*/
	x_from=num%bg_w;		/*データ読み込みＹ座標*/
	/*オフスクリーンサーフェス内の位置情報*/
	rect_from.top = y_from*h;
	rect_from.bottom = rect_from.top+h;
	rect_from.left = x_from*w;
	rect_from.right = rect_from.left+w;
	if( SpriteClipping( &x, &y,w,h,&rect_from,rect_screen ) == TRUE )
	{
		(*target)->BltFast( x, y, (*dat), &rect_from, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);
	}
	return(TRUE);
}
static	bool	SpriteClipping(int*	x,int*	y,int w,int h,RECT* rect,RECT*	rect_screen )
{
	if( *x<rect_screen->left )
	{
		rect->left -= ( *x-rect_screen->left );
		*x = rect_screen->left;
	}
	if( *x+w>rect_screen->right )
	{
		rect->right -= ( *x+w-rect_screen->right );
	}
	if( *y<rect_screen->top )
	{
		rect->top -= ( *y-rect_screen->top );
		*y = rect_screen->top;
	}
	if( *y+h>rect_screen->bottom )
	{
		rect->bottom -= ( *y+h-rect_screen->bottom );
	}
	if( rect->left >= rect->right ) return(FALSE);
	if( rect->top >= rect->bottom ) return(FALSE);

	return(TRUE);
}
/*
BOOL	SpriteClippingRect(RECT* rect_to,RECT* rect_from )
{
	int	width_to=rect_to->right-rect_to->left;
	int	width_from=rect_from->right-rect_from->left;
	int	height_to=rect_to->bottom-rect_to->top;
	int	height_from=rect_from->bottom-rect_from->top;

	if( rect_to->left<USR_SCR_XMIN )
	{
		rect_from->left -= ( rect_to->left-USR_SCR_XMIN )*width_from/width_to;
		rect_to->left = 0;
	}
	if( rect_to->right>USR_SCR_XMAX )
	{
		rect_from->right -= ( rect_to->right-USR_SCR_XMAX )*width_from/width_to;
		rect_to->right=USR_SCR_XMAX;
	}
	if( rect_to->top<USR_SCR_YMIN )
	{
		rect_from->top -= ( rect_to->top-USR_SCR_YMIN )*height_from/height_to;
		rect_to->top=0;
	}
	if( rect_to->bottom>USR_SCR_YMAX )
	{
		rect_from->bottom -= ( rect_to->bottom-USR_SCR_YMAX )*height_from/height_to;
		rect_to->bottom=USR_SCR_YMAX;
	}
	if( rect_from->left >= rect_from->right ) return(FALSE);
	if( rect_from->top >= rect_from->bottom ) return(FALSE);

	return(TRUE);
}
*/

