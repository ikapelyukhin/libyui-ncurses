/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       NCstyle.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <fstream>

#include "Y2Log.h"
#include "NCurses.h"
#include "NCStyleDef.h"

#include "NCstyle.mono.h"
#include "NCstyle.linux.h"
#include "NCstyle.xterm.h"

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCstyle::Style
//
///////////////////////////////////////////////////////////////////

unsigned NCstyle::Style::sanitycheck() {
  return MaxSTglobal;
}
NCattrset NCstyle::Style::attrGlobal( sanitycheck() );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::Style::initDialog
//	METHOD TYPE : StDialog
//
//	DESCRIPTION : Initialize subclass StDialog in Style constructor
//                    (esp. copy constructor). Default copy constructor
//                    is not appropriate, because StDialog contains
//                    references to subclass NCattrset.
//
NCstyle::StDialog NCstyle::Style::initDialog()
{
  return StDialog( StBase( attr(DialogTitle),
			   attr(DialogBorder) ),
		   StBase( attr(DialogActiveTitle),
			   attr(DialogActiveBorder) ),
		   StBase( attr(DialogHeadline),
			   attr(DialogText) ),
		   StWidget( attr(DialogDisabled),
			     attr(DialogDisabled),
			     attr(DialogDisabled),
			     attr(DialogDisabled),
			     attr(DialogDisabled) ),
		   StWidget( attr(DialogPlain),
			     attr(DialogLabel),
			     attr(DialogData),
			     attr(DialogHint),
			     attr(DialogScrl) ),
		   StWidget( attr(DialogActivePlain),
			     attr(DialogActiveLabel),
			     attr(DialogActiveData),
			     attr(DialogActiveHint),
			     attr(DialogActiveScrl) ),
		   StWidget( attr(DialogFramePlain),
			     attr(DialogFrameLabel),
			     attr(DialogFrameData),
			     attr(DialogFrameHint),
			     attr(DialogFrameScrl) ),
		   StWidget( attr(DialogActiveFramePlain),
			     attr(DialogActiveFrameLabel),
			     attr(DialogActiveFrameData),
			     attr(DialogActiveFrameHint),
			     attr(DialogActiveFrameScrl) ),
		   StList( attr(ListTitle),
			   StItem( attr(ListPlain),
				   attr(ListLabel),
				   attr(ListData),
				   attr(ListHint) ),
			   StItem( attr(ListSelPlain),
				   attr(ListSelLabel),
				   attr(ListSelData),
				   attr(ListSelHint) ) ),
		   StList( attr(ListActiveTitle),
			   StItem( attr(ListActivePlain),
				   attr(ListActiveLabel),
				   attr(ListActiveData),
				   attr(ListActiveHint) ),
			   StItem( attr(ListActiveSelPlain),
				   attr(ListActiveSelLabel),
				   attr(ListActiveSelData),
				   attr(ListActiveSelHint) ) ),
		   StList( attr(DialogDisabled),
			   StItem( attr(DialogDisabled),
				   attr(DialogDisabled),
				   attr(DialogDisabled),
				   attr(DialogDisabled) ),
			   StItem( attr(DialogDisabled),
				   attr(DialogDisabled),
				   attr(DialogDisabled),
				   attr(DialogDisabled) ) ),
		   StProgbar( attr(ProgbarCh),
			      attr(ProgbarBgch) ),
		   StRichtext( attr(RichTextPlain),
			       attr(RichTextTitle),
			       attr(RichTextLink),
			       attr(RichTextArmedlink),
			       attr(RichTextActiveArmedlink),
			       attr(RichTextB),
			       attr(RichTextI),
			       attr(RichTextT),
			       attr(RichTextBI),
			       attr(RichTextBT),
			       attr(RichTextIT),
			       attr(RichTextBIT) ),
		   attr(TextCursor)
		   );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::Style::Style
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstyle::Style::Style()
    : NCattrset( MaxSTlocal )
    , StDialog( initDialog() )
{}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::Style::Style
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstyle::Style::Style( const Style & rhs )
    : NCattrset( rhs )
    , StDialog( initDialog() )
{}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::Style::~Style
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCstyle::Style::~Style()
{}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCstyle
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::dumpName
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
#define PRT(t) case NCstyle::t: return #t;

string NCstyle::dumpName( NCstyle::StyleSet a ) {
  switch( a ) {
    PRT(DefaultStyle);
    PRT(InfoStyle);
    PRT(WarnStyle);
    PRT(PopupStyle);
  case NCstyle::MaxStyleSet:
    break;
  }
  return "unknown";
}

string NCstyle::dumpName( NCstyle::STglobal a ) {
  switch( a ) {
    PRT(AppTitle);
    PRT(AppText);
  case NCstyle::MaxSTglobal:
    break;
  }
  return "unknown";
}

string NCstyle::dumpName( NCstyle::STlocal a ) {
  switch( a ) {
    PRT(DialogBorder);
    PRT(DialogTitle);
    PRT(DialogActiveBorder);
    PRT(DialogActiveTitle);
    //
    PRT(DialogText);
    PRT(DialogHeadline);
    //
    PRT(DialogDisabled);
    //
    PRT(DialogPlain);
    PRT(DialogLabel);
    PRT(DialogData);
    PRT(DialogHint);
    PRT(DialogScrl);
    PRT(DialogActivePlain);
    PRT(DialogActiveLabel);
    PRT(DialogActiveData);
    PRT(DialogActiveHint);
    PRT(DialogActiveScrl);
    //
    PRT(DialogFramePlain);
    PRT(DialogFrameLabel);
    PRT(DialogFrameData);
    PRT(DialogFrameHint);
    PRT(DialogFrameScrl);
    PRT(DialogActiveFramePlain);
    PRT(DialogActiveFrameLabel);
    PRT(DialogActiveFrameData);
    PRT(DialogActiveFrameHint);
    PRT(DialogActiveFrameScrl);
    //
    PRT(ListTitle);
    PRT(ListPlain);
    PRT(ListLabel);
    PRT(ListData);
    PRT(ListHint);
    PRT(ListSelPlain);
    PRT(ListSelLabel);
    PRT(ListSelData);
    PRT(ListSelHint);
    //
    PRT(ListActiveTitle);
    PRT(ListActivePlain);
    PRT(ListActiveLabel);
    PRT(ListActiveData);
    PRT(ListActiveHint);
    PRT(ListActiveSelPlain);
    PRT(ListActiveSelLabel);
    PRT(ListActiveSelData);
    PRT(ListActiveSelHint);
    //
    PRT(RichTextPlain);
    PRT(RichTextTitle);
    PRT(RichTextLink);
    PRT(RichTextArmedlink);
    PRT(RichTextActiveArmedlink);
    PRT(RichTextB);
    PRT(RichTextI);
    PRT(RichTextT);
    PRT(RichTextBI);
    PRT(RichTextBT);
    PRT(RichTextIT);
    PRT(RichTextBIT);
    //
    PRT(ProgbarCh);
    PRT(ProgbarBgch);
    //
    PRT(TextCursor);
  case NCstyle::MaxSTlocal:
    break;
  }
  return "unknown";
}
#undef PRT

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::NCstyle
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCstyle::NCstyle( string term_t )
    : styleName( "linux" )
    , styleSet( MaxStyleSet )
    , fakestyle_e( MaxStyleSet )
{
  if ( NCattribute::colors() ) {
    if ( term_t == "xterm" )
      styleName = "xterm";
  } else {
    styleName = "mono";
  }

  UIMIL << "Init " << term_t << " using " << (NCattribute::colors() ? "color" : "bw" )
    << " => " << MaxStyleSet << " styles in " << styleName << endl;

  if ( styleName == "linux" )
    NCstyleInit_linux( styleSet );
  else if ( styleName == "xterm" )
    NCstyleInit_xterm( styleSet );
  else if ( styleName == "mono" )
    NCstyleInit_mono( styleSet );
  else
    NCstyleInit_linux( styleSet );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::~NCstyle
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCstyle::~NCstyle()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::changeSyle
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCstyle::changeSyle()
{
  NCDefineStyle( *this );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCstyle::fakestyle
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCstyle::fakestyle( StyleSet f )
{
  fakestyle_e = f;
  NCurses::Redraw();
}