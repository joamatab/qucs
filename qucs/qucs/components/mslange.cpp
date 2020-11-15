/***************************************************************************
    copyright            : (C) 2010 by Frans Schreuder
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "component.h"
#include "globals.h"
#include "module.h"

namespace{

class MSlange : public Component  {
private:
  MSlange(MSlange const& m) : Component(m) {};
public:
  MSlange();
  ~MSlange();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
private:
  Element* clone() const{ return new MSlange(*this); }
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "MLANGE", &d0);
static Module::INSTALL pp("RF", &d0);

MSlange::MSlange()
{
  setLabel("microstrip lange coupler");
  setTypeName("MLANGE");

  Lines.append(new Line(-30,-30,-30, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 30,-30, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 20,  0, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 10, 30, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-25,  0, 25,  0,QPen(Qt::darkBlue,2)));      
  Lines.append(new Line(-30,-10, 30,-10,QPen(Qt::darkBlue,2)));    
  Lines.append(new Line(  0,-20, 30,-20,QPen(Qt::darkBlue,2)));    
  Lines.append(new Line( 30,-30, 30,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,-10, 30, 30,QPen(Qt::darkBlue,2)));          

  Ports.append(new ComponentPort(-30,-30));
  Ports.append(new ComponentPort( 30, 30));
  Ports.append(new ComponentPort( 30,-30));
  Ports.append(new ComponentPort(-30, 30));

  x1 = -30; y1 =-33;
  x2 =  30; y2 = 33;

  tx = x1+4;
  ty = y2+4;
  Model = "MLANGE";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.append(new Property("W", "1 mm", true,
	QObject::tr("width of the line")));
  Props.append(new Property("L", "10 mm", true,
	QObject::tr("length of the line")));
  Props.append(new Property("S", "1 mm", true,
	QObject::tr("spacing between the lines")));
  Props.append(new Property("Model", "Kirschning", false,
	QObject::tr("microstrip model")+" [Kirschning, Hammerstad]"));
  Props.append(new Property("DispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+
	" [Kirschning, Getsinger]"));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
}

MSlange::~MSlange()
{
}

Component* MSlange::newOne()
{
  return new MSlange();
}

Element* MSlange::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Lange Coupler");
  BitmapFile = (char *) "mslange";

  if(getNewOne)  return new MSlange();
  return 0;
}

}
