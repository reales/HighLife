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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HighLife Editor Implementation                                                                                                      //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include ".\highlife.h"
#include ".\highlifeeditor.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int	useCount=0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HINSTANCE hInstance;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
{
	hInstance=(HINSTANCE)hModule;
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeEditor::CHighLifeEditor(AudioEffect *effect):AEffEditor(effect)
{
	// set instance
	hinstance=hInstance;

	// set effect
	peffect=effect;

	// set editor pointer
	effect->setEditor(this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CHighLifeEditor::~CHighLifeEditor(void)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLifeEditor::getRect(ERect **erect)
{
	static ERect r={0,0,K_EDITOR_HEIGHT,K_EDITOR_WIDTH};
	*erect=&r;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CHighLifeEditor::open(void *ptr)
{
	systemWindow=ptr;
	++useCount;

	if(useCount==1)
	{
		WNDCLASS windowClass;
		windowClass.style=0;
		windowClass.lpfnWndProc=WindowProc;
		windowClass.cbClsExtra=0;
		windowClass.cbWndExtra=0;
		windowClass.hInstance=hinstance;
		windowClass.hIcon=0;
		windowClass.hCursor=0;
		windowClass.hbrBackground=0;
		windowClass.lpszMenuName=0;
		windowClass.lpszClassName="VSTDDHighLife_OSS";
		RegisterClass(&windowClass);
	}

	// create our base window(s)
	hwnd=CreateWindowEx(0,"VSTDDHighLife_OSS","discoDSP HighLife",WS_CHILD | WS_VISIBLE,0,0,K_EDITOR_WIDTH,K_EDITOR_HEIGHT,(HWND)systemWindow,NULL,hinstance,NULL);
	SetWindowLong(hwnd,GWL_USERDATA,(intptr_t)this);

	// set window timer
	SetTimer(hwnd,1,16,NULL);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CHighLifeEditor::close(void)
{
	// decrement window reference counter
	useCount--;

	// kill timer
	KillTimer(hwnd,1);

	// destroy windows
	DestroyWindow(hwnd);

	// unregister class if now window
	if(useCount==0)
		UnregisterClass("VSTDDHighLife_OSS",hinstance);
}

