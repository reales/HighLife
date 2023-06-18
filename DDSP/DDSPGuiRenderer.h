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

#pragma once

#include "windows.h"

struct DDSP_FONT_INFO
{
	int		char_l_bytes;
	int		char_width;
	int		char_height;
	UINT8*	pfontdata;
};

class CDDSPGuiRenderer
{

public:
	CDDSPGuiRenderer(int const cw,int const ch);
	~CDDSPGuiRenderer(void);

private:
	CDDSPGuiRenderer(void);

public:
	void Paint(HWND const hWnd);
	void RenderArea(HWND const hWnd,int const x,int const y,int const w,int const h);
	void Clear(UCHAR const r,UCHAR const g,UCHAR const b);
	void PutSolidPixel(int const x,int const y,UCHAR const r,UCHAR const g,UCHAR const b);
	void PutTransPixel(int const x,int const y,UCHAR const r,UCHAR const g,UCHAR const b,UCHAR const a);
	void FillSolidRect(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b);
	void FillTransRect(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,UCHAR const a);
	void DrawRoundedBox(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b);
	void Draw3dBox(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b);
	void LineAntialiased(int const x1,int const y1,int const x2,int const y2,UCHAR const r,UCHAR const g,UCHAR const b);
	void DrawText(int const font_type,int const x,int const y,UCHAR const r,UCHAR const g,UCHAR const b,char *strString);
	void DrawTextHeader(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString);
	void DrawTextButton(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const state,int const font_type,char *strString);
	void DrawTextLabel(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString);
	void DrawHScrBar(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const min,double const max,double const pos,double const visible_elements,int const state);
	void DrawVScrBar(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const min,double const max,double const pos,double const visible_elements,int const state);
	void DrawHVumeter(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const peakl,double const peakr,bool const is_stereo,int const c_divisions);
	void DrawSpinCtrl(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString1,char* strString2,int const state);
	void DrawMenuCtrl(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString1,char* strString2,int const state);
	void DrawSpinMenu(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,int const font_type,char *strString1,char* strString2,int const state);
	void DrawHSlider(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,double const pos,double const mod,bool const polarized,int const state);
	void DrawWindow(int const x,int const y,int const w,int const h,UCHAR const r,UCHAR const g,UCHAR const b,char* strString1);

private:
	void GetFontInfo(int const font_type,DDSP_FONT_INFO* pfi);

	// framebuffer
	unsigned char*	pframebuffer;
	HBITMAP			hBitmap;

	// client size
	int				client_width;
	int				client_height;

private:
};
