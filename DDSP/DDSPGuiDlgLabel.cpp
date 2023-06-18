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

#include ".\ddspguidlglabel.h"
#include ".\ddspresource.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

// self pointer
CDDSPGuiDlgLabel* pddspguidlglabel;

// Message handler for dialog box. 
LRESULT CALLBACK ddspguiDlgLabelProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam) 
{ 
	HWND htemp;

	switch (message) 
	{ 
	case WM_INITDIALOG:
		
		// set dialog caption
		SetWindowText(hDlg,pddspguidlglabel->m_dlg_caption);
		
		// set default text
		htemp=GetDlgItem(hDlg,IDC_DDSP_GUI_DLG_LABEL_EDIT_1);
		SetWindowText(htemp,pddspguidlglabel->m_plabel);

		return TRUE; 

	case WM_COMMAND: 
		
		// ok or cancel
		if(LOWORD(wParam)==IDOK || LOWORD(wParam)==IDCANCEL) 
		{ 
			// get text
			htemp=GetDlgItem(hDlg,IDC_DDSP_GUI_DLG_LABEL_EDIT_1);
			GetWindowText(htemp,pddspguidlglabel->m_plabel,pddspguidlglabel->m_max_text);
			
			// close dialog
			EndDialog(hDlg,LOWORD(wParam)); 
			
			return TRUE; 
		} 
		
		break; 
	} 
	return FALSE; 
} 

CDDSPGuiDlgLabel::CDDSPGuiDlgLabel(void)
{
	m_max_text=0;
	m_plabel=NULL;
	sprintf(m_dlg_caption,"DDSP Gui Dialog Label");
	pddspguidlglabel=this;
}

CDDSPGuiDlgLabel::~CDDSPGuiDlgLabel(void)
{
}

void CDDSPGuiDlgLabel::RunModal(HWND const hwnd_parent,char* dlg_caption,char* plabel,int const max_text)
{
	m_max_text=max_text;
	m_plabel=plabel;
	sprintf(m_dlg_caption,dlg_caption);
	HINSTANCE const hinstance=(HINSTANCE)GetWindowLong(hwnd_parent,GWL_HINSTANCE);
	DialogBox(hinstance,LPCSTR(IDD_DDSP_GUI_DLG_LABEL),hwnd_parent,(DLGPROC)ddspguiDlgLabelProc);
	
	// show cursor
	ShowCursor(TRUE);
}
