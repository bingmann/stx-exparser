// $Id$

#include "WMain.h"

#include "ExpressionParser.h"

#include <iostream>
#include <map>

WMain::WMain()
    : WMain_wxg(NULL, -1, wxT(""))
{
    // set fixed monospace fonts for most controls

    wxFont fixedfont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    textctrlExpression->SetFont(fixedfont);
    textctrlStringExpression->SetFont(fixedfont);
    textctrlResultType->SetFont(fixedfont);
    textctrlResultValue->SetFont(fixedfont);
    textctrlXmlTree->SetFont(fixedfont);

    gridVariables->SetLabelFont(fixedfont);
    gridVariables->SetDefaultCellFont(fixedfont);

    // xpm images for the buttons
    {
	#include "img/addrow.xpm"
	#include "img/delrow.xpm"

	buttonAddVariable->SetBitmapLabel( wxBitmap(addrow) );
	buttonDelVariable->SetBitmapLabel( wxBitmap(delrow) );
    }

    textctrlExpression->SetFocus();
}

class VariableTable : public stx::BasicSymbolTable
{
public:
    typedef std::map<std::string, stx::AnyScalar> map_type;

    map_type			map;

    virtual stx::AnyScalar lookupVariable(const std::string &varname) const
    {
	map_type::const_iterator mi = map.find(varname);

	if (mi == map.end()) {
	    throw(stx::UnknownSymbolException(std::string("Unknown variable ") + varname));
	}

	return mi->second;
    }
};

void WMain::OnButtonEvaluate(wxCommandEvent &)
{
    if (notebookResults->GetSelection() == 0) {
	notebookResults->SetSelection(1);
    }

    // parse text into parse tree
    wxString text = textctrlExpression->GetValue();

    const stx::ParseNode* pn = NULL;
    try
    {
	pn = stx::parseExpressionString( std::string(text.mb_str()) );
	std::string strexpr = pn->toString();
	textctrlStringExpression->SetValue( wxString(strexpr.data(), wxConvUTF8, strexpr.size()) );
    }
    catch (stx::ExpressionParserException &e)
    {
	textctrlStringExpression->SetValue( wxString(e.what(), wxConvUTF8) );
	textctrlXmlTree->Clear();
	textctrlResultType->Clear();
	textctrlResultValue->Clear();
	return;
    }
    catch (stx::ConversionException &e)
    {
	textctrlStringExpression->SetValue( wxT("Exception: ") + wxString(e.what(), wxConvUTF8) );
	textctrlXmlTree->Clear();
	textctrlResultType->Clear();
	textctrlResultValue->Clear();
	return;
    }

    // load variable table
    VariableTable	vartable;
	
    for(int i = 0; i < gridVariables->GetNumberRows(); ++i)
    {
	wxString c0 = gridVariables->GetCellValue(i, 0);
	wxString c1 = gridVariables->GetCellValue(i, 1);

	vartable.map.insert( VariableTable::map_type::value_type(std::string(c0.mb_str()),
								 stx::AnyScalar(c1.mb_str())) );
    }

    // fill in xml tree
    std::string xmlstr = stx::parseExpressionStringXML( std::string(text.mb_str()) );
    textctrlXmlTree->SetValue( wxString(xmlstr.data(), wxConvUTF8, xmlstr.size()) );

    // evaluate parse tree with variables table
    try
    {
	stx::AnyScalar val = pn->evaluate(vartable);

	textctrlResultType->SetValue( wxString(val.getTypeString().c_str(), wxConvUTF8) );
	textctrlResultValue->SetValue( wxString(val.getString().c_str(), wxConvUTF8) );
    }
    catch (stx::ExpressionParserException &e)
    {
	textctrlResultType->SetValue( wxT("exception") );
	textctrlResultValue->SetValue( wxString(e.what(), wxConvUTF8) );
    }
    catch (stx::ConversionException &e)
    {
	textctrlResultType->SetValue( wxT("exception") );
	textctrlResultValue->SetValue( wxString(e.what(), wxConvUTF8) );
    }
}

void WMain::OnButtonAddVariable(wxCommandEvent &)
{
    gridVariables->AppendRows(1);
}

void WMain::OnButtonDelVariable(wxCommandEvent &)
{
    int selrow = gridVariables->GetGridCursorRow();
    if (selrow >= 0)
    {
	gridVariables->DeleteRows(selrow);
    }
    else
    {
	gridVariables->DeleteRows( gridVariables->GetNumberRows()-1 );
    }
}

BEGIN_EVENT_TABLE(WMain, wxFrame)

    EVT_BUTTON		(ID_BUTTON_EVALUATE,	WMain::OnButtonEvaluate)

    EVT_BUTTON		(ID_BUTTON_ADD_VARIABLE, WMain::OnButtonAddVariable)
    EVT_BUTTON		(ID_BUTTON_DEL_VARIABLE, WMain::OnButtonDelVariable)

END_EVENT_TABLE();

// *** Main Application

class AppParserDemo : public wxApp
{
public:
    bool 		OnInit();
};

IMPLEMENT_APP(AppParserDemo)

bool AppParserDemo::OnInit()
{
    wxInitAllImageHandlers();

    WMain* wm = new WMain();
    SetTopWindow(wm);
    wm->Show();

    return true;
}

