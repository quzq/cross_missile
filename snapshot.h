/********************************************/
/*	DirectDraw	SnapShotLiblaly				*/
/********************************************/

void	GetSnapShot(LPDIRECTDRAWSURFACE4*	surface,HWND hWnd)
{
	HBITMAP hBmp, hBmpOld;
	HDC hDC_to, hDC_from;
	HWND	hWnd_snap;


	if( (*surface)->GetDC(&hDC_from) != DD_OK)
	{
//		ReleaseAllObjects();
		DestroyWindow(hWnd);
		return;
	} else {
		// ビットマップの処理のために画面サイズを得る。
		int nScrnW=GetSystemMetrics(SM_CXSCREEN);
		int nScrnH=GetSystemMetrics(SM_CYSCREEN);


		nScrnW=640;
		nScrnH=480;

//		ShowWindow(SW_HIDE);

		// 画面に互換性のあるDCを作る。
		hDC_to=CreateCompatibleDC(hDC_from);
		// ビットマップ領域を作る。
		hBmp=CreateCompatibleBitmap(hDC_from,nScrnW,nScrnH);
		// 作成した領域に画面を転写する。
		SelectObject(hDC_to,hBmp);
		BitBlt(hDC_to,0,0,nScrnW,nScrnH,hDC_from,0,0,SRCCOPY);

		// クリップボードを開く
		OpenClipboard(NULL);
//		// クリップボードを空にする
		EmptyClipboard();
//		// ビットマップをクリップボードに託す
		SetClipboardData(CF_BITMAP,hBmp);
//		// クリップボードを閉じる
		CloseClipboard();

		// 不要になったものを削除
		DeleteDC(hDC_to);
		(*surface)->ReleaseDC(hDC_from);
//		ShowWindow(SW_SHOW); // ※

//		ReleaseAllObjects();
		DestroyWindow(hWnd);

		return;
	}
}