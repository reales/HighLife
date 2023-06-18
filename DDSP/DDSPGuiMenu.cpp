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

#include ".\ddspguimenu.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

CDDSPGuiMenu::CDDSPGuiMenu(void)
{
	hmenu=CreatePopupMenu();
	opt_count=0;
	opt_id=0;
}

CDDSPGuiMenu::~CDDSPGuiMenu(void)
{
	DestroyMenu(hmenu);
}

void CDDSPGuiMenu::Reset(void)
{
	DestroyMenu(hmenu);
	opt_count=0;
	hmenu=CreatePopupMenu();
}

void CDDSPGuiMenu::SetID(int const id)
{
	opt_id=id;
}

void CDDSPGuiMenu::AddTextOption(char* txt)
{
	// temp item
	MENUITEMINFO item;

	// fill up the menu struct
	item.cbSize		=	sizeof(MENUITEMINFO);
	item.fMask		=	MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
	item.fType		=	MFT_STRING;
	item.hSubMenu	=	NULL;
	item.wID		=	opt_id+opt_count;
	item.dwTypeData	=	txt;
	item.cch		=	UINT(strlen(txt));

	// insert into menu
	InsertMenuItem(hmenu,0,FALSE,&item);

	// add count
	opt_count++;
}

void CDDSPGuiMenu::AddSeparator(void)
{
	// temp item
	MENUITEMINFO item;

	// fill up the menu struct
	item.cbSize		=	sizeof(MENUITEMINFO);
	item.fMask		=	MIIM_ID | MIIM_TYPE | MIIM_SUBMENU;
	item.fType		=	MFT_SEPARATOR;
	item.hSubMenu	=	NULL;
	item.wID		=	opt_id+opt_count;
	item.dwTypeData	=	NULL;
	item.cch		=	0;

	// insert into menu
	InsertMenuItem(hmenu,0,FALSE,&item);
	
	// add count
	opt_count++;
}

int	CDDSPGuiMenu::GetOptionID(int const option)
{
	return opt_id+option;
}

int CDDSPGuiMenu::VerityCommandID(int const wID)
{
	if(wID>=opt_id && wID<(opt_id+opt_count))
		return wID-opt_id;

	return -1;
}

void CDDSPGuiMenu::TrackPopUpMenu(HWND const hwnd,int const x,int const y)
{
	POINT pnt;
	pnt.x=x;
	pnt.y=y;
	ClientToScreen(hwnd,&pnt);
	TrackPopupMenu(hmenu,TPM_LEFTALIGN,pnt.x,pnt.y,0,hwnd,NULL);
}

HMENU CDDSPGuiMenu::GetMenuHandler(void)
{
	return hmenu;
}
