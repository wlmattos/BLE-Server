/***************************************************************
 * Name:      LungsSoundsAppApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Willian (wills07@gmail.com)
 * Created:   2017-02-02
 * Copyright: Willian (none)
 * License:
 **************************************************************/

#include "LungsSoundsAppApp.h"


//(*AppHeaders
#include "LungsSoundsAppMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(LungsSoundsAppApp);

bool LungsSoundsAppApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
        LungsSoundsAppDialog Dlg(0);
        SetTopWindow(&Dlg);
        Dlg.ShowModal();
        wxsOK = false;
    }
    //*)
    return wxsOK;

}
