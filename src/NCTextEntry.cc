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

   File:       NCTextEntry.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#include <climits>

#include "Y2Log.h"
#include "NCurses.h"
#include "NCTextEntry.h"

#if 0
#undef  DBG_CLASS
#define DBG_CLASS "_NCTextEntry_"
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::NCTextEntry
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
NCTextEntry::NCTextEntry( NCWidget * parent, YWidgetOpt & opt,
			  const YCPString & nlabel,
			  const YCPString & ntext,
			  unsigned maxInput,
			  unsigned maxFld )
    : YTextEntry( opt, nlabel )
    , NCWidget( parent )
    , passwd( opt.passwordMode.value() )
    , label( nlabel )
    , lwin( 0 )
    , twin( 0 )
    , maxFldLength  ( maxFld )
    , maxInputLength( maxInput )
    , fldstart ( 0 )
    , fldlength( 0 )
    , curpos   ( 0 )
    , fldtype  ( PLAIN )
    , returnOnReturn_b( false )
{
  WIDDBG << endl;
  if ( maxInputLength &&
       ( !maxFldLength || maxFldLength > maxInputLength ) ) {
     maxFldLength = maxInputLength;
  }
  hotlabel = &label;
  setLabel( nlabel );
  setText( ntext );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::~NCTextEntry
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
NCTextEntry::~NCTextEntry()
{
  delete lwin;
  delete twin;
  WIDDBG << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::nicesize
//	METHOD TYPE : long
//
//	DESCRIPTION :
//
long NCTextEntry::nicesize( YUIDimension dim )
{
  return dim == YD_HORIZ ? wGetDefsze().W : wGetDefsze().H;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::setSize
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::setSize( long newwidth, long newheight )
{
  wRelocate( wpos( 0 ), wsze( newheight, newwidth ) );
  YTextEntry::setSize( newwidth, newheight );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::setDefsze
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::setDefsze()
{
  unsigned defwidth = maxFldLength ? maxFldLength : 5;
  if ( label.width() > defwidth )
    defwidth = label.width();
  defsze = wsze( label.height() + 1, defwidth );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::wCreate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::wCreate( const wrect & newrect )
{
  NCWidget::wCreate( newrect );

  wrect lrect( 0, wsze::min( newrect.Sze,
			     wsze( label.height(), newrect.Sze.W ) ) );
  if ( lrect.Sze.H == newrect.Sze.H )
    lrect.Sze.H -= 1;

  wrect trect( 0, wsze( 1, newrect.Sze.W ) );
  trect.Pos.L = lrect.Sze.H > 0 ? lrect.Sze.H : 0;

  lwin = new NCursesWindow( *win,
			    lrect.Sze.H, lrect.Sze.W,
			    lrect.Pos.L, lrect.Pos.C,
			    'r' );
  twin = new NCursesWindow( *win,
			    trect.Sze.H, trect.Sze.W,
			    trect.Pos.L, trect.Pos.C,
			    'r' );
  if ( maxFldLength && maxFldLength < (unsigned)newrect.Sze.W )
    trect.Sze.W = maxFldLength;
  fldlength=trect.Sze.W;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::wDelete
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::wDelete()
{
  delete lwin;
  delete twin;
  lwin = 0;
  twin = 0;
  NCWidget::wDelete();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::setLabel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::setLabel( const YCPString & nlabel )
{
  label  = NCstring( nlabel );
  YTextEntry::setLabel( nlabel );
  setDefsze();
  Redraw();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::setText
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::setText( const YCPString & ntext )
{
  buffer = NCstring( ntext ).str();
  if ( maxInputLength && buffer.length() > maxInputLength ) {
    buffer = buffer.erase( maxInputLength );
  }
  fldstart = 0;
  curpos   = buffer.length();
  tUpdate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::getText
//	METHOD TYPE : YCPString
//
//	DESCRIPTION :
//
YCPString NCTextEntry::getText()
{
  NCstring text( buffer );
  return text.YCPstr();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::setValidChars
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::setValidChars( const YCPString & validchars )
{
  validChars = validchars;
  YTextEntry::setValidChars( validchars );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::validKey
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool NCTextEntry::validKey( int key ) const
{
  const string & vch( validChars.str() );

  if ( vch.empty() )
    return true;

  if ( key < 0 || UCHAR_MAX < key )
    return false;

  return( vch.find( (unsigned char)key ) != string::npos );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::wRedraw
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::wRedraw()
{
  if ( !win )
    return;

  // label
  const NCstyle::StWidget & style( widgetStyle( true ) );
  lwin->bkgd( style.plain );
  lwin->clear();
  label.drawAt( *lwin, style );
  tUpdate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::bufferFull
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
inline bool NCTextEntry::bufferFull() const
{
  return( maxInputLength && buffer.length() == maxInputLength );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::maxCursor
//	METHOD TYPE : unsigned
//
//	DESCRIPTION :
//
inline unsigned NCTextEntry::maxCursor() const
{
  return( bufferFull() ? buffer.length() - 1 : buffer.length() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::tUpdate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void NCTextEntry::tUpdate()
{
  if ( !win )
    return;

  unsigned maxc = maxCursor();

  // adjust cursor
  if ( curpos > maxc ) {
    curpos = maxc;
  }

  // adjust fldstart that cursor is visible
  if ( maxc < fldlength ) {
    fldstart = 0;
  } else {
    if ( curpos <= fldstart ) {
      fldstart = curpos ? curpos - 1 : 0;
    }
    if ( curpos >= fldstart + fldlength - 1 ) {
      fldstart = curpos + (curpos == maxc ? 1 : 2) - fldlength;
    }
  }

  const NCstyle::StWidget & style( widgetStyle() );
  twin->bkgd( widgetStyle( true ).plain );
  twin->move( 0, 0 );

  unsigned i      = 0;
  unsigned end    = fldlength;
  const char * cp = buffer.c_str() + fldstart;

  // draw left scrollhint if
  if ( *cp && fldstart ) {
    twin->bkgdset( style.scrl );
    twin->addch( ACS_LARROW );
    ++i;
    ++cp;
  }

  // check for right scrollhint
  if ( fldstart + fldlength <= maxc ) {
    --end;
  }

  // draw field
  twin->bkgdset( style.data );
  for ( /*adjusted i*/; *cp && i < end; ++i ) {
    twin->addch( passwd ? '*' : *cp );
    ++cp;
  }
  twin->bkgdset( style.plain );
  for ( /*adjusted i*/; i < end; ++i ) {
    twin->addch( ACS_CKBOARD );
  }

  // draw right scrollhint if
  if ( end < fldlength ) {
    twin->bkgdset( style.scrl );
    twin->addch( ACS_RARROW );
  }

  // reverse curpos
  if ( GetState() == NC::WSactive ) {
    twin->move( 0, curpos - fldstart );
    twin->bkgdset( wStyle().cursor );
    twin->addch( twin->inchar() );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : NCTextEntry::wHandleInput
//	METHOD TYPE : NCursesEvent
//
//	DESCRIPTION :
//
NCursesEvent NCTextEntry::wHandleInput( int key )
{
  NCursesEvent ret;
  bool   beep   = false;
  bool   update = true;

  switch ( key ) {

  case KEY_BACKSPACE:
    if ( bufferFull() && curpos == maxCursor() ) {
      // if we're on the last char in a full buffer delete this char
      // and not the previous one.
      buffer.erase( curpos, 1 );
    } else if ( curpos ) {
      buffer.erase( --curpos, 1 );
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_DC:
    if ( curpos < buffer.length() ) {
      buffer.erase( curpos, 1 );
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_HOME:
    if ( curpos ) {
      curpos = 0;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_END:
    if ( curpos < maxCursor() ) {
      curpos = maxCursor();
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_LEFT:
    if ( curpos ) {
      --curpos;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_RIGHT:
    if ( curpos < maxCursor() ) {
      ++curpos;
    } else {
      update = false;
      beep   = true;
    }
    break;

  case KEY_RETURN:
    if ( getNotify() || returnOnReturn_b )
      ret = NCursesEvent::button;
    break;

  default:
    if ( key < 32 || ( key >= 127 && key < 160 ) || UCHAR_MAX < key ) {
      update = false;
      beep   = true;
    } else if ( fldtype == NUMBER ) {

      if ( bufferFull() && key != '+' ) {
	update = false;
	beep   = true;
      } else {
	switch ( key ) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  if ( curpos || buffer.empty() || buffer[0] != '-' ) {
	    buffer.insert( curpos, 1, key );
	    if ( curpos < maxCursor() )
	      ++curpos;
	  } else {
	    update = false;
	    beep   = true;
	  }
	  break;

	case '+':
	  if ( !buffer.empty() && buffer[0] == '-' ) {
	    buffer.erase(string::size_type(0), 1);
	    if ( curpos )
	      --curpos;
	  } else {
	    update = false;
	  }
	  break;

	case '-':
	  if ( buffer.empty() || buffer[0] != '-' ) {
	    buffer.insert(string::size_type(0), 1, '-');
	    if ( curpos < maxCursor() )
	      ++curpos;
	  } else {
	    update = false;
	  }
	  break;

	default:
	  update = false;
	  beep   = true;
	  break;
	}
      }

    } else { // PLAIN

      if ( bufferFull() || !validKey( key ) ) {
	update = false;
	beep   = true;
      } else {
	buffer.insert( curpos, 1, key );
	if ( curpos < maxCursor() )
	  ++curpos;
      }

    }
    break;
  }

  if ( update ) {
    tUpdate();
    if ( getNotify() )
      ret = NCursesEvent::button;
  }

  if ( beep )
    ::beep();

  return ret;

}