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

   File:       NCPadWidget.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef NCPadWidget_h
#define NCPadWidget_h

#include <iosfwd>

#include "NCWidget.h"
#include "NCPad.h"

class NCPadWidget;
class NCScrollbar;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : NCPadWidget
//
//	DESCRIPTION :
//
class NCPadWidget : public NCWidget, protected NCSchrollCB {

  friend std::ostream & operator<<( std::ostream & STREAM, const NCPadWidget & OBJ );

  NCPadWidget & operator=( const NCPadWidget & );
  NCPadWidget            ( const NCPadWidget & );

  private:

    NClabel         label;
    NCursesWindow * padwin;
    NCScrollbar *   hsb;
    NCScrollbar *   vsb;

    wsze  minPadSze;
    bool  multidraw;

  protected:

    NCPad * pad;
    bool    hasHeadline;
    bool    activeLabelOnly;

    void startMultidraw()    { multidraw = true; }
    void stopMultidraw()     { multidraw = false; DrawPad(); }
    bool inMultidraw() const { return multidraw; }

  protected:

    virtual const char * location() const { return "NCPadWidget"; }

    // widget stuff
    unsigned labelWidht() const { return label.width(); }

    virtual void wCreate( const wrect & newrect );
    virtual void wDelete();
    virtual void wRedraw();
    virtual void wRecoded();

    // pad stuff
    wsze defPadSze() const {
      if ( !padwin )
	return 0;
      return wsze( padwin->height(), padwin->width() );
    }
    virtual NCPad * CreatePad();
    virtual void    DrawPad();

    void InitPad();
    void AdjustPad( wsze nsze );
    void DelPad();

    // scroll hints
    virtual void HScroll( unsigned total, unsigned visible, unsigned start );
    virtual void VScroll( unsigned total, unsigned visible, unsigned start );
    virtual void ScrollHead( NCursesWindow & w, unsigned ccol );

    // resize hints
    virtual void AdjustPadSize( wsze & minsze );

    // input
    virtual bool handleInput( int key );

  public:

    NCPadWidget( NCWidget * myparent = 0 );
    virtual ~NCPadWidget();

    void setLabel( const NClabel & nlabel );
};

///////////////////////////////////////////////////////////////////

#endif // NCPadWidget_h