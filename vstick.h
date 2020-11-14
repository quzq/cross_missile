/****************************************************/
/*													*/
/*	UsrCtrlDevice�N���X								*/
/*													*/
/*	ver.1.00	1999/01/26							*/
/*	ver.1.01	1999/03/13							*/
/****************************************************/
class	UsrCtrlDevice
{
private:
	int	up;
	int	down;
	int	left;
	int	right;
	int	btn1;
	int	btn2;
	int	type;	/*0=�s��,1=�X�e�B�b�N,2=����*/
	int	btn1harder;
	int	btn2harder;
public:
	UsrCtrlDevice( int c ){
		type=c;
		up=0;
		down=0;
		left=0;
		right=0;
		btn1=0;
		btn2=0;
	}
	void	SetBtn1( int b ){ btn1=b; }
	void	SetBtn2( int b ){ btn2=b; }
	void	SetUp( int u ){ up=u; }
	void	SetDown( int d ){ down=d; }
	void	SetLeft( int l ){ left=l; }
	void	SetRight( int r ){ right=r; }
	void	SetBtnHarder( int a,int b)
	{
		btn1harder=a;
		btn2harder=b;
	}
	void	SetStick(int sw,UINT nChar)
	{
		switch( nChar )
		{
			case	VK_UP:	/*��L�[����*/
				SetUp(sw);
				break;
			case	VK_DOWN:	/*���L�[����*/
				SetDown(sw);
				break;
			case	VK_LEFT:	/*���L�[����*/
				SetLeft(sw);
				break;
			case	VK_RIGHT:	/*�E�L�[����*/
				SetRight(sw);
				break;
			case	VK_SPACE:	/*�r�o�b�L�[����*/
				SetBtn1(sw);
				break;
			case	VK_RETURN:	/*RET�L�[����*/
				SetBtn2(sw);
				break;
			default:
				break;
		}
	}

	int	GetBtn1Harder( void )
	{
		return(btn1harder);
	}
	int	GetBtn2Harder( void )
	{
		return(btn1harder);
	}
	int	GetPos( void )
	{
		int	pos=0;
		if( down==0 && up==0 )
		{
			pos=4;
		}else if( down ){
			pos=1;
		} else pos=7;
		if( left==0 && right==0 )
		{
			pos+=1;
		} else if( right ) pos+=2;
		if( pos==5 )	pos=0;	/*�L�[�S�I�t����pos=0��*/
		 return( pos );
	}
	int	GetBtn1( void ){ return( btn1 ); }
	int	GetBtn2( void ){ return( btn2 ); }
	int	GetCtrlType(void){ return(type); }
	virtual void	AutoCtrl( int x,int y,int tx,int ty ){}
};
