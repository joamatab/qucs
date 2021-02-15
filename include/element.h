/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** \file element.h
  * \brief Defines drawing elements for schematics
  *
  * element.h contains definitions of various drawing elements used
  * used to render schematics and the schematic symbols. The following
  * structs are defined to hold information on various drawing types:
  *
  *    Line
  *    Arc
  *    Area
  *    Port
  *    Text
  *
  * The Element class is also defined here which is a superclass
  * of every component symbol.
  *
  */

// Element: Superclass of all schematic drawing elements

#ifndef QUCS_ELEMENT_H
#define QUCS_ELEMENT_H
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include <assert.h>
#include "object.h"
#include "io_trace.h"
#include "port.h"
#include "geometry.h"
// #include "qt_compat.h"

class Node;
class QucsDoc;
class QPainter;
class WireLabel;
class SchematicModel;
class Symbol;
class Widget;

typedef unsigned index_t;

// valid values for Element.Type
// The 4 least significant bits of each value are reserved for special
// additionals !!!
#define isDummyElement    0
#define isSpecialMask    -16

#define isComponent        0x30000
#define isComponentText    0x30002
#define isAnalogComponent  0x10000
#define isDigitalComponent 0x20000

#define isGraph            0x0020
//#define isNode             0x0040
#define isMarker           0x0080
//#define isWire             0x0100

#if 0
#define isPainting         0x2000
#define isPaintingResize   0x2001

#define isLabel            0x4000
#define isMovingLabel      0x4001
#define isHMovingLabel     0x4002
#define isVMovingLabel     0x4004

#define isDiagram          0x8000
#define isDiagramResize    0x8001
#define isDiagramHScroll   0x8002
#define isDiagramVScroll   0x8003
#endif



class NetLang;
class ViewPainter;

//static QString incomplete_description="incomplete_description";
// will be gone soon.
// class Component;
class Label;
class WireLabel;
class Diagram;
class Painting;
class Graph;
class Marker;
class Node;
class ViewPainter;
class QWidget;
/*--------------------------------------------------------------------------*/
class SchematicModel;
/*--------------------------------------------------------------------------*/
// Element: has a position.
// TODO: base on sth like card with params but no position.
class Element : public Object {
public:
	friend class ElementGraphics; // BUG?
	typedef rect_t Rect;
protected:
	Element(Element const&);
public:
	Element();
	Element(int cx, int cy) : _position(cx, cy) { unreachable(); }
	virtual ~Element();

public:	// "elaborate"
	virtual void	 prepare()	{} // precalc_first.

public: // make old variables accessible
	int const& cx() const { return _position.first; }
	int const& cy() const { return _position.second; }

	int const& x1_() const { return x1; }
	int const& y1_() const { return y1; }
//	void snapToGrid(SchematicDoc& s);

	void setObsoleteType(int){
		unreachable();
	}

public: // UI stuff.
#if 0 // TODO (QucsDoc is not a QWidget yet);
	virtual QWidget* widget(QObject* parent) { return nullptr; }
#else
	virtual Widget* schematicWidget(QucsDoc*) const { return nullptr; }
	virtual QWidget* newWidget() {return nullptr;}
#endif


public: // other stuff
	virtual bool showLabel() const{ return true; }
	//virtual bool showParam(int i) const{ return true; } // later

	void setCenter(pos_t const& c){ incomplete(); _position = c; }
	void setPosition(pos_t const& c){ _position = c; }
	virtual void getCenter(int&, int&) const; // BUG

	// BUG: remove "center"
	// BUG: not virtual
	virtual pos_t center()const;

	// move to painting?
	pos_t position()const{
		return _position;
	}

public:
	virtual Element* clone()const = 0;
	virtual Element* clone_instance()const{
		return clone();
	}
//	virtual QString const& description() const{return incomplete_description;}
	virtual char const* iconBasename() const{return nullptr;}

public: // params
	virtual unsigned param_count()const{return 0;}
	virtual void set_param_by_index(index_t, std::string const&);
	virtual void set_param_by_name(std::string const& name, std::string const& v);
	virtual std::string get_param_by_name(std::string const& n) const;
	virtual std::string param_value(unsigned) const{return "incomplete";}

public: // compatibility
	virtual bool legacyTransformHack() const{
		return false;
	}

public:
	// BUG; abused in taskElement
	virtual /*bug*/ std::string /* const& */ typeName() const{
		return _type;
	}
	virtual /*bug*/ void setTypeName(std::string const& x){
		_type = x;
	}

	// create a declaration, e.g. subcircuit definition or include directive
	virtual Symbol const* proto(SchematicModel const*) const{return nullptr;}
	SchematicModel const* scope() const;
	virtual SchematicModel* scope();

public: // friend ElementGraphics?
	//  void attachToModel();
	void detachFromModel();

public:
	Element const* owner() const{return _owner;}
	Element* owner(){return _owner;}

public:
	void set_owner(Element* e) { assert(!_owner || e==_owner || !e); _owner=e;}
	const Element* find_looking_out(const std::string& name)const;
	const Element* find_in_parent_scope(const std::string& name)const;
	const Element* find_in_my_scope(const std::string& name)const;

private:
	pos_t _position; // BUG: store in symbol?

protected: // BUG in Painting
	int x1, y1;

private:
	Element* _owner; // can't be const it seems.
	std::string _type;
}; // Element
/*--------------------------------------------------------------------------*/
inline SchematicModel const* Element::scope() const
{
	auto e=const_cast<Element*>(this);
	return e->scope();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
