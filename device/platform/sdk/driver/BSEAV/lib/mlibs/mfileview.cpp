/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mfileview.cpp $
 * $brcm_Revision: 4 $
 * $brcm_Date: 6/26/09 3:36p $
 *
 * Module Description: MListView adapted to browsing a file system
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mfileview.cpp $
 * 
 * 4   6/26/09 3:36p erickson
 * PR56373: call closedir
 *
 * 3   8/24/05 6:18p arbisman
 * PR16678: Add support for WinCE
 *
 * 2   6/14/05 2:14p erickson
 * PR15216: converted to BKNI_Snprintf
 *
 * 1   2/7/05 11:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/5   1/29/04 2:25p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/4   11/6/03 5:11p erickson
 * PR8570: I'm not done, but I need to work on another bug.
 *
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 11:40a erickson
 * I'm not done, but I need to work on another bug.
 *
 * Irvine_BSEAVSW_Devel/3   6/16/03 9:47a erickson
 * improved error msg
 *
 * Irvine_BSEAVSW_Devel/2   6/11/03 11:55a erickson
 * 7319 uclibc support
 *
 * Irvine_BSEAVSW_Devel/6   4/30/03 9:06a erickson
 * added findItem and made file method
 *
 * SanJose_Linux_Devel\5   6/14/02 12:16p erickson
 * fixed focus bug
 *
 ***************************************************************************/
#include "mfileview.h"
#include "bstd.h"
#include "bkni.h"

#if !defined (_WIN32_WCE)
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#else
#include <Windows.h>
#include <Winbase.h>
#endif

#define CURRENT() ((MFileViewItem*)current())

MFileView::MFileView(MWidget *parent, const MRect &rect, const char *name) :
    MListView(parent, rect, name), _timer(parent->app())
{
    setView(ListView);
    _timer.addHandler(this);
}

void MFileView::setHomeDir(const char *homedir) {
    _homeDir = homedir;
    refresh();
}

void MFileView::refresh() {
    bool setfocus = viewport()->containsFocus();
    MListView::clear();

    if (!currentDir()) {
        _currentDir = _homeDir;
        if (!currentDir())
            return;
    }
    bool isHome = (_homeDir == _currentDir);

    //printf("reading %s\n", currentDir());
#if defined (__vxworks)
    DIR *dir = opendir((char *)currentDir());
    if (!dir)
        return;
#elif defined (LINUX)
    DIR *dir = opendir(currentDir());
    if (!dir)
        return;
#endif

    beginUpdates();
#if !defined (_WIN32_WCE)
    struct dirent *d;
    while ((d = readdir(dir))) {
        if (!strcmp(d->d_name, "."))
            continue;
        if (isHome && !strcmp(d->d_name, ".."))
            continue;

        //printf("add item: %s\n", d->d_name);
        MFileViewItem *item = (MFileViewItem *)add();
        struct stat st;
        char buf[300];
        BKNI_Snprintf(buf, 300, "%s/%s", currentDir(), d->d_name);
        if (!stat(buf, &st))
            item->isDir = S_ISDIR(st.st_mode);
        else
            printf("ERROR: MFileView::refresh stat %s/%s %d\n",
                currentDir(), d->d_name, errno);
        if (item->isDir)
            item->setLabel(MString("[") + d->d_name + "]");
        else
            item->setLabel(d->d_name);
        // pure name goes into description
        item->setDescription(d->d_name);
    }
#else
        TCHAR*     wchFileName;
        CHAR        chFileName[MAX_PATH];
        WIN32_FIND_DATA findData;
        BOOL    bKeepSearching = TRUE;
        int str_size = strlen(currentDir());
        // Alloc enough space to hold dir name + "\*.*\0"
        wchFileName = (TCHAR*)malloc(sizeof(TCHAR) *(str_size +6));
        mbstowcs(wchFileName, currentDir(), str_size+6);
        wcscat(wchFileName, L"\\*.*");

        HANDLE hFile = FindFirstFile(wchFileName, &findData);
        if(hFile != INVALID_HANDLE_VALUE)
        {
            while(bKeepSearching)
            {
                wcstombs(chFileName, findData.cFileName, MAX_PATH);
                MFileViewItem *item = (MFileViewItem *)add();
                if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    item->setLabel(MString("[") + chFileName + "]");
                else
                    item->setLabel(chFileName);

                // pure name goes into description
                item->setDescription(chFileName);

                bKeepSearching = FindNextFile(hFile, &findData);
            }
            FindClose(hFile);
        }
        free(wchFileName);

#endif //_WIN32_WCE

    endUpdates();
    if (setfocus) {
        // this finds the focus in the viewport
        focus();
    }

#if defined (__vxworks) || defined (LINUX)
    closedir(dir);
#endif
}

void MFileView::gotoDir(const char *dir) {
    if (dir == NULL)
        dir = _homeDir;
    _currentDir = dir;
    refresh();
}

void MFileView::addToPath(const char *path, bool dorefresh) {
    if (!strcmp(path, "..")) {
        int x = _currentDir.findRev('/'); //, _currentDir.length()-2);
        if (x == -1)
            _currentDir.clear();
        else
            _currentDir.setLength(x);
    }
    else {
        if (path[0] == '/')
            _currentDir = path;
        else {
            if (_currentDir.length() && _currentDir[_currentDir.length()-1] != '/')
                _currentDir += '/';
            _currentDir += path;
        }
    }
    if (dorefresh)
        refresh();
}

MString MFileView::file(MListViewItem *item)
{
    //TODO: danger...what is someone passed in a non-MFileViewItem?
    MFileViewItem *i = (MFileViewItem *)item;
    if (i && !i->isDir) {
        return currentDir() + MString("/") + i->description();
    }
    else
        return MString();
}

void MFileView::onClick(MButton *sender) {
    MFileViewItem *item = CURRENT();
    if (item && item->isDir) {
        // we have to defer the refresh until the next message processing
        // loop because the MButton can't be deleted yet!
        addToPath(item->description(), false);
        _timer.start(0);
    }
    MListView::onClick(sender);
}

void MFileView::onTimeout(MTimer *sender) {
    refresh();
}

MListViewItem *MFileView::createItem() {
    return new MFileViewItem(this, viewport());
}

bool MFileView::currentFileIsDir() {
    MFileViewItem *item = CURRENT();
    return item && item->isDir;
}

MListViewItem *MFileView::findItem(const char *pathAndFile)
{
    int len = strlen(currentDir());
    // quick test to eliminate most
    if (pathAndFile &&
        strstr(pathAndFile, currentDir()) == pathAndFile &&
        pathAndFile[len] == '/')
    {
        const char *filecompare = &pathAndFile[len+1];

        // now search for the filename
        for (MListViewItem *item = first(); item; item = next()) {
            if (!strcmp(item->description(), filecompare))
                return item;
        }
    }
    return NULL;
}
