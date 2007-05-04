// $Id$

#ifndef _WMain_H_
#define _WMain_H_

#include "WMain_wxg.h"

class WMain : public WMain_wxg
{
public:
    WMain();

    void	OnButtonEvaluate(wxCommandEvent &);

    void	OnButtonAddVariable(wxCommandEvent &);
    void	OnButtonDelVariable(wxCommandEvent &);

    DECLARE_EVENT_TABLE();
};

#endif // _WMain_H_
