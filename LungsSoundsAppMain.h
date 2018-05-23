/***************************************************************
 * Name:      LungsSoundsAppMain.h
 * Purpose:   Defines Application Frame
 * Author:    Willian (wills07@gmail.com)
 * Created:   2017-02-02
 * Copyright: Willian (none)
 * License:
 **************************************************************/

#ifndef LUNGSSOUNDSAPPMAIN_H
#define LUNGSSOUNDSAPPMAIN_H

//(*Headers(LungsSoundsAppDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/slider.h>
#include <wx/statbmp.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class LungsSoundsAppDialog: public wxDialog
{
public:

    LungsSoundsAppDialog(wxWindow* parent,wxWindowID id = -1);
    virtual ~LungsSoundsAppDialog();

private:

    //(*Handlers(LungsSoundsAppDialog)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnbtnReadFileClick(wxCommandEvent& event);
    void OnButton5Click(wxCommandEvent& event);
    void OnButton3Click(wxCommandEvent& event);
    void OnButton4Click(wxCommandEvent& event);
    void OnButton2Click(wxCommandEvent& event);
    void OnInit(wxInitDialogEvent& event);
    void OnSlider1CmdScroll(wxScrollEvent& event);
    void OnSlider1CmdSliderUpdated(wxScrollEvent& event);
    void OnButton2Click1(wxCommandEvent& event);
    void OnButton3Click1(wxCommandEvent& event);
    void OnButton6Click(wxCommandEvent& event);
    //*)

    //(*Identifiers(LungsSoundsAppDialog)
    static const long ID_STATICBITMAP1;
    static const long ID_STATICTEXT2;
    static const long ID_STATICTEXT1;
    static const long ID_SLIDER1;
    static const long ID_TEXTCTRL2;
    static const long ID_STATICTEXT4;
    static const long ID_TEXTCTRL3;
    static const long ID_BUTTON4;
    static const long ID_STATICTEXT3;
    static const long ID_TEXTCTRL1;
    static const long ID_BUTTON2;
    static const long ID_BUTTON1;
    static const long ID_BUTTON3;
    static const long ID_BUTTON6;
    static const long ID_BUTTON5;
    //*)

    //(*Declarations(LungsSoundsAppDialog)
    wxSlider* Slider1;
    wxButton* Button4;
    wxStaticText* StaticText2;
    wxButton* Button1;
    wxStaticBitmap* StaticBitmap1;
    wxStaticText* StaticText1;
    wxBoxSizer* BoxSizer2;
    wxStaticText* StaticText3;
    wxButton* Button2;
    wxButton* Button6;
    wxButton* Button5;
    wxButton* Button3;
    wxTextCtrl* TextCtrl2;
    wxBoxSizer* BoxSizer1;
    wxTextCtrl* TextCtrl1;
    wxBoxSizer* BoxSizer3;
    wxStaticText* StaticText4;
    wxTextCtrl* TextCtrl3;
    //*)

    DECLARE_EVENT_TABLE()
};

#endif // LUNGSSOUNDSAPPMAIN_H
