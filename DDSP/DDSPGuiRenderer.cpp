/*-
 * Copyright (c) discoDSP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by discoDSP
 *        http://www.discodsp.com/ and contributors.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include ".\ddspguirenderer.h"
#include "math.h"
#include "stdio.h"

#include "ddspguifonts.c"

CDDSPGuiRenderer::CDDSPGuiRenderer(void)
{
}

CDDSPGuiRenderer::CDDSPGuiRenderer(int const cw,int const ch)
{
	// set width/height
	client_width=cw;
	client_height=ch;

	//Creating compatible DC and a bitmap to render the image 
	BITMAPINFO bmp_info; 
	bmp_info.bmiHeader.biSize			=	sizeof(BITMAPINFOHEADER); 
	bmp_info.bmiHeader.biWidth			=	client_width; 
	bmp_info.bmiHeader.biHeight			=	client_height; 
	bmp_info.bmiHeader.biPlanes			=	1; 
	bmp_info.bmiHeader.biBitCount		=	32; 
	bmp_info.bmiHeader.biCompression	=	BI_RGB; 
	bmp_info.bmiHeader.biSizeImage		=	0; 
	bmp_info.bmiHeader.biXPelsPerMeter	=	0; 
	bmp_info.bmiHeader.biYPelsPerMeter	=	0; 
	bmp_info.bmiHeader.biClrUsed		=	0; 
	bmp_info.bmiHeader.biClrImportant	=	0; 

	// pointer holder
	pframebuffer=0;

	// bitmap dib sector
	hBitmap=CreateDIBSection(NULL,&bmp_info,DIB_RGB_COLORS,(void**)&pframebuffer,0,0);

	// clear with black
	Clear(0,0,0);
}

CDDSPGuiRenderer::~CDDSPGuiRenderer(void)
{
	// if we already have a bitmap
	if(hBitmap!=NULL)
	{
		DeleteObject(hBitmap);
		hBitmap=NULL;
	}
}

void CDDSPGuiRenderer::Paint(HWND const hWnd)
{
	// get client area
	RECT r;
	GetClientRect(hWnd,&r);
	int const w=r.right-r.left;
	int const h=r.bottom-r.top;

	// painstruct and hdc
	PAINTSTRUCT ps;
	HDC const hdc=BeginPaint(hWnd,&ps);

	// create compatible DC
	HDC mem_dc=CreateCompatibleDC(hdc); 

	// Selecting the object before doing anything allows you 
	// to use render together with native Windows GDI.
	HBITMAP temp=(HBITMAP)SelectObject(mem_dc,hBitmap);

	// display the image
	BitBlt(hdc,0,0,w,h,mem_dc,0,0,SRCCOPY);

	// free resources 
	SelectObject(mem_dc,temp);
	DeleteObject(mem_dc);

	// finish gdi paint
	EndPaint(hWnd,&ps);
}

void CDDSPGuiRenderer::RenderArea(HWND const hWnd,int const x,int const y,int const w,int const h)
{
	// hdc
	HDC const hdc=GetDC(hWnd);

	// dc created success
	if(hdc!=NULL)
	{
		// create compatible DC
		HDC mem_dc=CreateCompatibleDC(hdc); 

		// Selecting the object before doing anything allows you 
		// to use render together with native Windows GDI.
		HBITMAP temp=(HBITMAP)SelectObject(mem_dc,hBitmap);

		// display the image
		BitBlt(hdc,x,y,w,h,mem_dc,x,y,SRCCOPY);

		// free resources 
		SelectObject(mem_dc,temp);
		DeleteObject(mem_dc);

		// finish gdi paint
		ReleaseDC(hWnd,hdc);
	}
}

void CDDSPGuiRenderer::Clear(UCHAR const r,UCHAR const g,UCHAR const b)
{
	COLORREF* pc=(COLORREF*)pframebuffer;
	COLORREF const abgr=(b)|(g<<8)|(r<<16);

	for(int x=0;x<client_width*client_height;x++)
		pc[x]=abgr;
}

void CDDSPGuiRenderer::PutSolidPixel(int const x,int const y,UCHAR const r,UCHAR const g,UCHAR const b)
{
	unsigned char* ppix=pframebuffer+(x+(client_height-(y+1))*client_width)*4;
	ppix[0]=b;
	ppix[1]=g;
	ppix[2]=r;
}

void CDDSPGuiRenderer::PutTransPixel(int const x,int const y,UCHAR const r,UCHAR const g,UCHAR const b,UCHAR const a)
{
	unsigned char* ppix=pframebuffer+(x+(client_height-(y+1))*client_width)*4;
	ppix[0]=((a*b)+(255-a)*ppix[0])/256;
	ppix[1]=((a*g)+(255-a)*ppix[1])/256;
	ppix[2]=((a*r)+(255-a)*ppix[2])/256;
}

void CDDSPGuiRenderer::FillSolidRect(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b)
{
	COLORREF const clr=(b)|(g<<8)|(r<<16);
	COLORREF* pclr=(COLORREF*)pframebuffer+(x+(client_height-(y+1))*client_width);

	int const raster_jump=(w+client_width);

	for(int ry=0;ry<h;ry++)
	{			
		for(int rx=0;rx<w;rx++)
			*(pclr++)=clr;

		pclr-=raster_jump;
	}
}

void CDDSPGuiRenderer::FillTransRect(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,UCHAR const a)
{
	int const neg_a=255-a;
	int const mul_b=a*b;
	int const mul_g=a*g;
	int const mul_r=a*r;

	unsigned char* pline=pframebuffer+(x+(client_height-(y+1))*client_width)*4;
	int const raster_jump=(w+client_width)*4;

	for(int ry=0;ry<h;ry++)
	{
		for(int rx=0;rx<w;rx++)
		{
			pline[0]=(mul_b+neg_a*pline[0])/256;
			pline[1]=(mul_g+neg_a*pline[1])/256;
			pline[2]=(mul_r+neg_a*pline[2])/256;
			pline+=4;
		}

		pline-=raster_jump;
	}
}
void CDDSPGuiRenderer::DrawRoundedBox(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b)
{
	// first line
	PutTransPixel(x,y,r,g,b,64);
	PutTransPixel(x+1,y,r,g,b,128);
	FillSolidRect(x+2,y,w-4,1,r,g,b);
	PutTransPixel(x+w-2,y,r,g,b,128);
	PutTransPixel(x+w-1,y,r,g,b,64);
	
	// second line
	PutTransPixel(x,y+1,r,g,b,128);
	FillSolidRect(x+1,y+1,w-2,1,r,g,b);
	PutTransPixel(x+w-1,y+1,r,g,b,128);

	// box
	FillSolidRect(x,y+2,w,h-4,r,g,b);

	// third line
	PutTransPixel(x,y+h-2,r,g,b,128);
	FillSolidRect(x+1,y+h-2,w-2,1,r,g,b);
	PutTransPixel(x+w-1,y+h-2,r,g,b,128);

	// fourth line
	PutTransPixel(x,y+h-1,r,g,b,64);
	PutTransPixel(x+1,y+h-1,r,g,b,128);
	FillSolidRect(x+2,y+h-1,w-4,1,r,g,b);
	PutTransPixel(x+w-2,y+h-1,r,g,b,128);
	PutTransPixel(x+w-1,y+h-1,r,g,b,64);
}

void CDDSPGuiRenderer::Draw3dBox(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b)
{
	int dr=r-128;if(dr<0)dr=0;
	int dg=g-128;if(dg<0)dg=0;
	int db=b-128;if(db<0)db=0;
	int br=r+64;if(br>255)br=255;
	int bg=g+64;if(bg>255)bg=255;
	int bb=b+64;if(bb>255)bb=255;

	// draw 3d frame
	FillSolidRect(x,y+(h-1),w,1,dr,dg,db);
	FillSolidRect(x+(w-1),y,1,h,dr,dg,db);	
	FillSolidRect(x,y,w-1,1,br,bg,bb);
	FillSolidRect(x,y,1,h-1,br,bg,bb);

	// draw inside solid box
	FillSolidRect(x+1,y+1,w-2,h-2,r,g,b);
}

void CDDSPGuiRenderer::LineAntialiased(int const x1,int const y1,int const x2,int const y2,UCHAR const r,UCHAR const g,UCHAR const b)
{
	// some useful constants first
	double const dw=x2-x1;
	double const dh=y2-y1;
	double const slx=dh/dw;
	double const sly=dw/dh;

	// determine wichever raster scanning behaviour to use
	if(fabs(slx)<1.0)
	{
		// x scan
		int tx1=x1;
		int tx2=x2;
		double raster=y1;

		if(x1>x2)
		{
			tx1=x2;
			tx2=x1;
			raster=y2;
		}

		for(int x=tx1;x<=tx2;x++)
		{
			int const ri=int(raster);

			double const in_y0=1.0-(raster-ri);
			double const in_y1=1.0-(ri+1-raster);

			PutTransPixel(x,ri+0,r,g,b,in_y0*255.0);
			PutTransPixel(x,ri+1,r,g,b,in_y1*255.0);

			raster+=slx;
		}
	}
	else
	{
		// y scan
		int ty1=y1;
		int ty2=y2;
		double raster=x1;

		if(y1>y2)
		{
			ty1=y2;
			ty2=y1;
			raster=x2;
		}

		for(int y=ty1;y<=ty2;y++)
		{
			int const ri=int(raster);

			double const in_x0=1.0-(raster-ri);
			double const in_x1=1.0-(ri+1-raster);

			PutTransPixel(ri+0,y,r,g,b,in_x0*255.0);
			PutTransPixel(ri+1,y,r,g,b,in_x1*255.0);

			raster+=sly;
		}
	}
}

void CDDSPGuiRenderer::GetFontInfo(int const font_type,DDSP_FONT_INFO* pfi)
{
	// 5x7 Font
	if(font_type==0)
	{
		pfi->char_l_bytes=1;
		pfi->char_width=5;
		pfi->char_height=7;
		pfi->pfontdata=fontdata0507;
	}

	// 6x12 Font
	if(font_type==1)
	{
		pfi->char_l_bytes=1;
		pfi->char_width=6;
		pfi->char_height=12;
		pfi->pfontdata=fontdata0612;
	}

	// 8x13 Font
	if(font_type==2)
	{
		pfi->char_l_bytes=1;
		pfi->char_width=8;
		pfi->char_height=13;
		pfi->pfontdata=fontdata0813;
	}

	// 12x24 Font
	if(font_type==3)
	{
		pfi->char_l_bytes=2;
		pfi->char_width=12;
		pfi->char_height=24;
		pfi->pfontdata=fontdata1224;
	}
}

void CDDSPGuiRenderer::DrawText(int const font_type,int const x,int const y,UCHAR const r,UCHAR const g,UCHAR const b,char *strString)
{
	// get color
	COLORREF const clr=(b)|(g<<8)|(r<<16);

	// determine font type
	DDSP_FONT_INFO fi;
	GetFontInfo(font_type,&fi);

	// blit fonts
	int const str_len=strlen(strString);
	int bit=0;

	// get 32bit framebuffer pointer
	COLORREF* pclr=(COLORREF*)pframebuffer+(x+(client_height-(y+1))*client_width);

	// scan text
	for(int c=0;c<str_len;c++)
	{
		int char_byte=int(strString[c])*fi.char_height*fi.char_l_bytes;

		for(int b=0;b<fi.char_height;b++)
		{	
			bit=0;

			for(int cbl=0;cbl<fi.char_l_bytes;cbl++)
			{
				UCHAR const ch=fi.pfontdata[char_byte++];

				if(ch&0x80)pclr[bit]=clr;bit++;
				if(ch&0x40)pclr[bit]=clr;bit++;
				if(ch&0x20)pclr[bit]=clr;bit++;
				if(ch&0x10)pclr[bit]=clr;bit++;
				if(ch&0x08)pclr[bit]=clr;bit++;
				if(ch&0x04)pclr[bit]=clr;bit++;
				if(ch&0x02)pclr[bit]=clr;bit++;
				if(ch&0x01)pclr[bit]=clr;bit++;
			}

			pclr-=client_width;
		}

		pclr+=(client_width*fi.char_height)+fi.char_width;
	}
}

void CDDSPGuiRenderer::DrawTextHeader(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString)
{
	DrawRoundedBox(x,y,w,h,r,g,b);

	// determine font type
	DDSP_FONT_INFO fi;
	GetFontInfo(font_type,&fi);

	// draw centered text
	int const tx=x+(w/2)-((strlen(strString)*fi.char_width)/2);
	int const ty=y+(h/2)-(fi.char_height/2)-1;
	DrawText(font_type,tx+1,ty+1,0,0,0,strString);
	DrawText(font_type,tx,ty,255,255,255,strString);
}
	
void CDDSPGuiRenderer::DrawTextButton(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const state,int const font_type,char *strString)
{
	// determine font type
	DDSP_FONT_INFO fi;
	GetFontInfo(font_type,&fi);

	FillSolidRect(x,y+(h-1),w,1,0,0,0);
	FillSolidRect(x+(w-1),y,1,h,0,0,0);

	if(state)
	{
		FillSolidRect(x,y,w-1,1,0,0,0);
		FillSolidRect(x,y,1,h-1,0,0,0);
		FillSolidRect(x+1,y+1,w-2,1,r-64,g-64,b-64);
		FillSolidRect(x+1,y+1,1,h-2,r-64,g-64,b-64);	
		FillSolidRect(x+1,y+(h-2),w-2,1,r-64,g-64,b-64);
		FillSolidRect(x+w-2,y+1,1,h-2,r-64,g-64,b-64);
		FillSolidRect(x+2,y+2,w-4,h-4,r,g,b);
		DrawText(font_type,1+x+(w/2)-((strlen(strString)*fi.char_width)/2),1+y+(h/2)-(fi.char_height/2)-1,0,0,0,strString);
	}
	else
	{
		FillSolidRect(x,y,w-1,1,255,255,255);
		FillSolidRect(x,y,1,h-1,255,255,255);
		FillSolidRect(x+1,y+(h-2),w-2,1,r-64,g-64,b-64);
		FillSolidRect(x+w-2,y+1,1,h-2,r-64,g-64,b-64);
		FillSolidRect(x+1,y+1,w-3,h-3,r,g,b);
		DrawText(font_type,x+(w/2)-((strlen(strString)*fi.char_width)/2),y+(h/2)-(fi.char_height/2)-1,0,0,0,strString);
	}
}

void CDDSPGuiRenderer::DrawTextLabel(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString)
{
	// Draw Box
	Draw3dBox(x,y,w,h,r,g,b);

	// determine font type
	DDSP_FONT_INFO fi;
	GetFontInfo(font_type,&fi);

	// get color
	UCHAR c;

	if(r<128)
		c=255;
	else
		c=0;

	// draw centered text
	DrawText(font_type,x+(w/2)-((strlen(strString)*fi.char_width)/2),y+(h/2)-(fi.char_height/2),c,c,c,strString);
}

void CDDSPGuiRenderer::DrawHScrBar(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const min,double const max,double const pos,double const visible_elements,int const state)
{
	char la[2]={17,0};
	char ra[2]={16,0};
	char str[1]={0};

	DrawTextButton(x,y,h,h,r,g,b,state==0,0,la);
	DrawTextButton(x+w+h,y,h,h,r,g,b,state==1,0,ra);

	// filled dotted pattern
	COLORREF const clr=(b)|(g<<8)|(r<<16);
	COLORREF* pclr=(COLORREF*)pframebuffer+(x+h+(client_height-(y+1))*client_width);

	int const raster_jump=(w+client_width);

	for(int ry=0;ry<(h-1);ry++)
	{			
		int cnt=ry;

		for(int rx=0;rx<w;rx++)
		{
			if((cnt++)&1)
				*(pclr++)=clr;
			else
				*(pclr++)=0;
		}

		pclr-=raster_jump;
	}

	FillSolidRect(x+h,y+(h-1),w,1,0,0,0);
	FillSolidRect(x+h+(w-1),y,1,h-1,0,0,0);

	/*
	double const drange=max-min;
	double dwl=(visible_elements/drange)*double(h);

	double const dhwl=double(w)-double(h);
	double const dxsl=((pos-min)*dhwl)/drange;

	DrawTextButton(x+h+dxsl,y,h,h,r,g,b,state==2,0,str);*/
}

void CDDSPGuiRenderer::DrawVScrBar(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const min,double const max,double const pos,double const visible_elements,int const state)
{
	char la[2]={30,0};
	char ra[2]={31,0};
	char str[1]={0};

	DrawTextButton(x,y,w,w,r,g,b,state==0,0,la);
	DrawTextButton(x,y+w+h,w,w,r,g,b,state==1,0,ra);

	// filled dotted pattern
	COLORREF const clr=(b)|(g<<8)|(r<<16);
	COLORREF* pclr=(COLORREF*)pframebuffer+(x+(client_height-(y+w+1))*client_width);

	int const raster_jump=(w+client_width);

	for(int ry=0;ry<(h-1);ry++)
	{			
		int cnt=ry;

		for(int rx=0;rx<w;rx++)
		{
			if((cnt++)&1)
				*(pclr++)=clr;
			else
				*(pclr++)=0;
		}

		pclr-=raster_jump;
	}

	FillSolidRect(x+(w-1),y+w,1,h-1,0,0,0);

	double const drange=max-min;

	if(drange>0.0)
	{
		double d_bar_size=(visible_elements/drange)*double(h);

		if(d_bar_size>double(h))
			d_bar_size=double(h);

		double const dysl=((pos-min)*double(h))/drange;

		DrawTextButton(x,y+w+dysl,w,d_bar_size,r,g,b,state==2,0,str);
	}
	else
	{
		DrawTextButton(x,y+w,w,h,r,g,b,0,0,str);
	}
}

void CDDSPGuiRenderer::DrawHVumeter(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const peakl,double const peakr,bool const is_stereo,int const c_divisions)
{
	// backframe
	FillSolidRect(x,y,w,20+h,r,g,b);

	// get vumeter dynamic range
	int const vu_width=w-16;
	int const red_start=vu_width-(vu_width/c_divisions);
	int const db_range=c_divisions*6;

	// draw caption
	for(int ca=0;ca<=c_divisions;ca++)
	{
		int const x_capt=(((ca*vu_width)/c_divisions)/2)*2;
		char buf[8];
		sprintf(buf,"%.2d",(c_divisions-ca)*6);
		DrawText(0,x+x_capt+2,y+1,255,255,255,buf);
		PutSolidPixel(x+x_capt+6,y+8,255,255,255);
		PutSolidPixel(x+x_capt+6,y+h+10,255,255,255);
		DrawText(0,x+x_capt+2,y+h+12,255,255,255,buf);
	}

	// draw peak leds
	int const vu_height=(h/2)-1;
	int const vu_height_mono=h-1;

	int lvu=0;
	int rvu=0;

	if(peakl>0.0)
	{
		double const dbl=double(db_range)+(20.0*log10(peakl));
		double const dbx=(dbl*double(vu_width))/db_range;
		lvu=int(dbx);
	}

	if(peakr>0.0)
	{
		double const dbr=double(db_range)+(20.0*log10(peakr));
		double const dbx=(dbr*double(vu_width))/db_range;
		rvu=int(dbx);
	}

    int xv; 
	for(xv=0;xv<=vu_width;xv+=2)
	{
		int const ax=x+6+xv;
		int const red=(xv*255)/vu_width;

		int gre;

		if(xv>=red_start)
			gre=255-(xv-red_start)*(255/(vu_width/c_divisions));
		else
			gre=255;

		if(is_stereo)
		{
			if(xv<=lvu)
				FillSolidRect(ax,y+10,1,vu_height,red,gre,0);
			else
				FillSolidRect(ax,y+10,1,vu_height,0,0,0);

			if(xv<=rvu)
				FillSolidRect(ax,y+11+vu_height,1,vu_height,red,gre,0);
			else
				FillSolidRect(ax,y+11+vu_height,1,vu_height,0,0,0);
		}
		else
		{
			if(xv<=lvu)
				FillSolidRect(ax,y+10,1,vu_height_mono,red,gre,0);
			else
				FillSolidRect(ax,y+10,1,vu_height_mono,0,0,0);
		}
	}

	if(is_stereo)
	{
		if(peakl>1.0)
			FillSolidRect(x+xv+6,y+10,4,vu_height,255,0,0);
		else
			FillSolidRect(x+xv+6,y+10,4,vu_height,0,0,0);

		if(peakr>1.0)
			FillSolidRect(x+xv+6,y+11+vu_height,4,vu_height,255,0,0);
		else
			FillSolidRect(x+xv+6,y+11+vu_height,4,vu_height,0,0,0);
	}
}

void CDDSPGuiRenderer::DrawSpinCtrl(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString1,char* strString2,int const state)
{
	int const wi=(w-16)/2;
	DrawTextLabel(x,y,wi,h,r,g,b,font_type,strString1);
	DrawTextLabel(x+wi,y,wi,h,255-r,255-g,255-b,font_type,strString2);

	char la[2]={30,0};
	char ra[2]={31,0};

	DrawTextButton(x+w-16,y,16,h/2,r,g,b,state==1,0,la);
	DrawTextButton(x+w-16,y+h/2,16,h/2,r,g,b,state==2,0,ra);
}

void CDDSPGuiRenderer::DrawMenuCtrl(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString1,char* strString2,int const state)
{
	int const wi=(w-16)/2;
	DrawTextLabel(x,y,wi,h,r,g,b,font_type,strString1);
	DrawTextLabel(x+wi,y,wi,h,255-r,255-g,255-b,font_type,strString2);

	char da[2]={31,0};
	DrawTextButton(x+w-16,y,16,h,r,g,b,state==1,0,da);
}

void CDDSPGuiRenderer::DrawSpinMenu(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString1,char* strString2,int const state)
{
	int const wi=(w-32)/2;
	DrawTextLabel(x,y,wi,h,r,g,b,font_type,strString1);
	DrawTextLabel(x+wi,y,wi,h,255-r,255-g,255-b,font_type,strString2);

	char la[2]={30,0};
	char ra[2]={31,0};
	char da[2]={31,0};

	DrawTextButton(x+w-32,y,16,h/2,r,g,b,state==1,0,la);
	DrawTextButton(x+w-32,y+h/2,16,h/2,r,g,b,state==2,0,ra);
	DrawTextButton(x+w-16,y,16,h,r,g,b,state==3,0,da);
}

void CDDSPGuiRenderer::DrawHSlider(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const pos,double const mod,bool const polarized,int const state)
{
	FillSolidRect(x,y,w-1,1,r-64,g-64,b-64);
	FillSolidRect(x,y,1,h-1,r-64,g-64,b-64);
	
	FillSolidRect(x,y+(h-1),w,1,255,255,255);
	FillSolidRect(x+w-1,y,1,h-1,255,255,255);

	FillSolidRect(x+1,y+1,w-3,1,0,0,0);
	FillSolidRect(x+1,y+2,1,h-4,0,0,0);
	
	FillSolidRect(x+1,y+(h-2),w-2,1,r-32,g-32,b-32);
	FillSolidRect(x+(w-2),y+1,1,h-3,r-32,g-32,b-32);
	
	FillSolidRect(x+2,y+2,w-4,h-4,r-28,g-28,b-28);
	FillSolidRect(x+3,y+3,w-6,h-6,r-24,g-24,b-24);

	double d_mod=(mod-0.5)*2.0;

	if((pos+d_mod)<0.0)
		d_mod=-pos;

	if((pos+d_mod)>1.0)
		d_mod=1.0-pos;


	double const d_sl_siz=double(w)-22.0;
	double const d_xs_pos=pos*d_sl_siz;
	int const i_xs_pos=int(d_xs_pos);
	int const i_xs_mod=int(d_sl_siz*d_mod);

	// back fill
	if(polarized)
	{
		FillSolidRect(x+d_sl_siz*0.5+11,y+4,1,h-8,r-64,g-64,b-64);
	
		if(pos<0.5)
			FillSolidRect(x+i_xs_pos+12,y+6,d_sl_siz*0.5-d_xs_pos,h-12,r-64,g-64,b-64);
		else
			FillSolidRect(x+d_sl_siz*0.5+11,y+6,d_xs_pos-d_sl_siz*0.5,h-12,r-64,g-64,b-64);
	}
	else
	{
		FillSolidRect(x+3,y+4,1,h-8,r-64,g-64,b-64);
		FillSolidRect(x+3,y+6,i_xs_pos,h-12,r-64,g-64,b-64);
	}

	// drag button
	if(state==1)
	{
		Draw3dBox(x+3+i_xs_pos,y+3,8,h-6,r+64,g+64,b+64);
		Draw3dBox(x+11+i_xs_pos,y+3,8,h-6,r+64,g+64,b+64);
	}
	else
	{
		Draw3dBox(x+3+i_xs_pos,y+3,8,h-6,r+32,g+32,b+32);
		Draw3dBox(x+11+i_xs_pos,y+3,8,h-6,r+32,g+32,b+32);
	}

	// pos modulation
	if(mod>0.5)
		FillTransRect(x+i_xs_pos+19,y+4,i_xs_mod,h-8,255,0,0,64);

	// neg modulation
	if(mod<0.5)
		FillTransRect(x+i_xs_pos+3+i_xs_mod,y+4,-i_xs_mod,h-8,255,0,0,64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CDDSPGuiRenderer::DrawWindow(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,char* strString1)
{
	Draw3dBox(x,y,w,h,r+32,g+32,b+32);
	DrawRoundedBox(x+8,y+8,w-16,h-16,r,g,b);
	DrawRoundedBox(x+10,y+10,w-20,15,r-32,g-32,b-32);
	DrawText(1,x+16,y+10,255,255,255,strString1);
}