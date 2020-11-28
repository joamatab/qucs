/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "io.h"
#include "docfmt.h"
#include "globals.h"
#include "language.h"
#include "schematic_lang.h"
#include "schematic_model.h"
#include "misc.h" //?!!

#include "painting.h"
#include "diagram.h" // BUG?
#include "task_element.h" // BUG?
#include "u_parameter.h" // BUG?

namespace{

class LegacySchematicFormat : public DocumentFormat{
public:
	explicit LegacySchematicFormat()
		: DocumentFormat() {}

private: //Command
	void save(ostream_t& stream, Object const*) const override;
	void load(istream_t& stream, Object*) const override;
	virtual void do_it(istream_t&, SchematicModel*) {incomplete();}
private: // legacy cruft
	bool isSymbolMode() const{ return false; }
	ElementList const& symbolPaints(SchematicSymbol const& m) const{
		assert( m.symbolPaintings());
		return *m.symbolPaintings();
	}
	DiagramList const& diagrams(SchematicSymbol const& m) const{
		return m.diagrams();
	}
	PaintingList const& paintings(SchematicSymbol const& m) const{
		return m.paintings();
	}
	NodeMap const& nodes(SchematicSymbol const& m) const{
		return m.nodes();
	}
	ElementList const& components(SchematicSymbol const& m) const{
		return m.components();
	}
}d0;
static Dispatcher<Command>::INSTALL p0(&command_dispatcher, "leg_sch", &d0);

void LegacySchematicFormat::load(istream_t& s, Object* c) const
{
// TODO: move stuff here that does not belong to leg_sch.
	auto l=doclang_dispatcher["leg_sch"];
	assert(l);
	auto L=dynamic_cast<SchematicLanguage const*>(l);
	assert(L);

	if(auto cc=dynamic_cast<SchematicSymbol*>(c)){
		while(!s.atEnd()){
			L->parse(s, cc); // BUG BUG only pass SchematicModel
			assert(s.atEnd()); // happens with legacy lang
		}
	}else if(auto cc=dynamic_cast<SubcktBase*>(c)){
		while(!s.atEnd()){
			L->parse(s, cc); // BUG BUG only pass SchematicModel
			assert(s.atEnd()); // happens with legacy lang
		}
	}else{
		unreachable();
	}

}

static QString QG(SchematicSymbol const& m, std::string const& key)
{
	return QString::fromStdString(m.paramValue(key));
}

static void printProperties(SchematicSymbol const& m, DocumentStream& stream)
{
	// get legacy "parameters"
	float tmpScale;
	int tmpViewX1;
	int tmpViewX2;
	int tmpViewY1;
   int tmpViewY2;
   int tmpPosX;
   int tmpPosY;
	int ViewX1;
	int ViewY1;
	int ViewX2;
	int ViewY2;
	int Scale;
	int GridX;
	int GridY;
	int GridOn;

	try{
		ViewX1=std::stoi(m.paramValue("ViewX1"));
		ViewY1=std::stoi(m.paramValue("ViewY1"));
		ViewX2=std::stoi(m.paramValue("ViewX2"));
		ViewY2=std::stoi(m.paramValue("ViewY2"));
		Scale=std::stoi(m.paramValue("Scale"));
		GridX=std::stoi(m.paramValue("GridX"));
		GridY=std::stoi(m.paramValue("GridY"));
		GridOn=std::stoi(m.paramValue("GridOn"));

		tmpViewX1=std::stoi(m.paramValue("tmpViewX1"));
		tmpViewX2=std::stoi(m.paramValue("tmpViewX2"));
		tmpViewY1=std::stoi(m.paramValue("tmpViewY1"));
		tmpViewY2=std::stoi(m.paramValue("tmpViewY2"));
		tmpScale=std::stof(m.paramValue("tmpScale"));
		tmpPosX=std::stoi(m.paramValue("tmpPosX"));
		tmpPosY=std::stoi(m.paramValue("tmpPosY"));
	}catch (std::invalid_argument const&){
		incomplete();
	}
	stream << "<Properties>\n";
	if(0) { // } isSymbolMode()) {
		incomplete();
		assert(false); // => symbol_format.cc
		stream << "  <View=" << tmpViewX1<<","<<tmpViewY1<<","
			<< tmpViewX2<<","<<tmpViewY2<< ",";
		stream <<tmpScale<<","<<tmpPosX<<","<<tmpPosY << ">\n";
	}else{
		stream << "  <View=" << ViewX1<<","<<ViewY1<<","
			<< ViewX2<<","<<ViewY2<< ",";
		incomplete(); // scale.
		/// \todo  stream << Scale <<","<<contentsX()<<","<<contentsY() << ">\n";
		stream << Scale <<","<< 0 <<","<< 0 << ">\n";
	}
	stream << "  <Grid=" << GridX << "," << GridY <<"," << GridOn << ">\n";
	stream << "  <DataSet=" << QG(m, "DataSet") << ">\n";
	stream << "  <DataDisplay=" << QG(m, "DataDisplay") << ">\n";
	stream << "  <OpenDisplay=" << QG(m, "SimOpenDpl") << ">\n";
	stream << "  <Script=" << QG(m, "Script") << ">\n";
	stream << "  <RunScript=" << QG(m, "SimRunScript") << ">\n";
	stream << "  <showFrame=" << QG(m, "showFrame") << ">\n";

	QString Frame_Text0(QG(m,"FrameText0"));
	QString Frame_Text1(QG(m,"FrameText1"));
	QString Frame_Text2(QG(m,"FrameText2"));
	QString Frame_Text3(QG(m,"FrameText3"));

	QString t;
	t = misc::convert2ASCII(Frame_Text0);
	stream << "  <FrameText0=" << t << ">\n";
	t = misc::convert2ASCII(Frame_Text1);
	stream << "  <FrameText1=" << t << ">\n";
	t = misc::convert2ASCII(Frame_Text2);
	stream << "  <FrameText2=" << t << ">\n";
	t = misc::convert2ASCII(Frame_Text3);
	stream << "  <FrameText3=" << t << ">\n";
	stream << "</Properties>\n";
}

void LegacySchematicFormat::save(DocumentStream& stream, Object const* o) const
{
	auto m_ = dynamic_cast<SchematicSymbol const*>(o);

/// these are separate in legacy format.
	std::vector<Element const*> wires;
	std::vector<Element const*> paintings;

	if(!m_){
		incomplete();
		return;
	}else{
	}

	auto& m = *m_;

	auto D=doclang_dispatcher["leg_sch"];
	auto L = dynamic_cast<DocumentLanguage const*>(D);
	assert(L);

	stream << "<Qucs Schematic 0.0.20>\n";

	if(0){
		printProperties(m, stream);
	}else{
		stream << "<Properties>\n";
		assert(m.subckt()->params());
		for(auto i : *(m.subckt()->params())){
			stream << "  <" << i.first << "=" << i.second.string() <<">\n";
		}
		stream << "</Properties>\n";
	}

	stream << "<Symbol>\n";     // save all paintings for symbol
	for(auto sp : symbolPaints(m)){
		// BUG callback
		if(auto pp = dynamic_cast<Painting const*>(sp)){
			auto mp = const_cast<Painting*>(pp); // yikes.
			stream << "  <" << mp->save() << ">\n";
		}else{
		}
	}
	stream << "</Symbol>\n";

	stream << "<Components>\n";    // save all components
	for(auto pc : components(m)){
		if(dynamic_cast<Conductor const*>(pc)){
			wires.push_back(pc);
		}else if(dynamic_cast<Painting const*>(pc)){
			paintings.push_back(pc);
		}else if(pc->label()==":SymbolSection:"){
			incomplete();
			continue;
		}else{
			stream << "  ";
			L->printItem(pc, stream);
			stream << "\n"; // BUG?
		}
	}
	if(m.commands()){
		for(auto pc : *(m.commands())){
			stream << "  ";
			L->printItem(pc, stream);
			stream << "\n"; // BUG?
		}
	}else{
	}
	stream << "</Components>\n";

	stream << "<Wires>\n";
	for(Element const* pw : wires){
		L->printItem(pw, stream);
//		stream << "  " << pw->save() << "\n";
	}

#if 0
	// BUG labels are stored as wires.
	for(auto pn : nodes(m)){
		if(pn->Label) stream << "  " << pn->Label->save() << "\n";
	}
#endif
	stream << "</Wires>\n";

	stream << "<Diagrams>\n";    // save all diagrams
	for(auto pd : diagrams(m)){
		stream << "  " << pd->save() << "\n";
	}
	stream << "</Diagrams>\n";

	stream << "<Paintings>\n";     // save all paintings
	for(auto pp : paintings){
		L->printItem(pp, stream);
	}
	stream << "</Paintings>\n";

#if 0 // yikes (wtf?)
	// additionally save symbol C++ code if in a symbol drawing and the
	// associated file is a Verilog-A file
	if (fileSuffix () == "sym") {
		if (fileSuffix (DataDisplay) == "va") {
			saveSymbolCpp ();
			saveSymbolJSON ();

			// TODO slit this into another method, or merge into saveSymbolJSON
			// handle errors in separate
			qDebug() << "  -> Run adms for symbol";

			QString vaFile;

			//      QDir prefix = QDir(QucsSettings.BinDir);

			QDir include = QDir(QucsSettings.BinDir+"../include/qucs-core");

			//pick admsXml from settings
			QString admsXml = QucsSettings.AdmsXmlBinDir.canonicalPath();

#ifdef __MINGW32__
			admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml.exe");
#else
			admsXml = QDir::toNativeSeparators(admsXml+"/"+"admsXml");
#endif
			// BUG: duplicated from qucs_actions.cpp
			char const* var = getenv("QUCS_USE_PATH");
			if(var != NULL) {
				// just use PATH. this is currently bound to a contition, to maintain
				// backwards compatibility with QUCSDIR
				qDebug() << "QUCS_USE_PATH";
				admsXml = "admsXml";
			}else{
			}

			QString workDir = QucsSettings.QucsWorkDir.absolutePath();

			qDebug() << "App path : " << qApp->applicationDirPath();
			qDebug() << "workdir"  << workDir;
			qDebug() << "homedir"  << QucsSettings.QucsHomeDir.absolutePath();
			qDebug() << "projsdir"  << QucsSettings.projsDir.absolutePath();

			vaFile = QucsSettings.QucsWorkDir.filePath(fileBase()+".va");

			QStringList Arguments;
			Arguments << QDir::toNativeSeparators(vaFile)
				<< "-I" << QDir::toNativeSeparators(include.absolutePath())
				<< "-e" << QDir::toNativeSeparators(include.absoluteFilePath("qucsMODULEguiJSONsymbol.xml"))
				<< "-A" << "dyload";

			//      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

			QFile file(admsXml);
			if(var) {
				// don't do this. it will always report an error.
			}else if ( !file.exists() ){
				QMessageBox::critical(this, tr("Error"),
						tr("Program admsXml not found: %1\n\n"
							"Set the admsXml location on the application settings.").arg(admsXml));
				return -1;
			}

			qDebug() << "taskElement: " << admsXml << Arguments.join(" ");

			// need to cd into project to run admsXml?
			QDir::setCurrent(workDir);

			QProcess builder;
			builder.setProcessChannelMode(QProcess::MergedChannels);

			builder.start(admsXml, Arguments);


			// how to capture [warning]? need to modify admsXml?
			// TODO put stdout, stderr into a dock window, not messagebox
			if (!builder.waitForFinished()) {
				QString cmdString = QString("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
				cmdString = cmdString + builder.errorString();
				QMessageBox::critical(this, tr("Error"), cmdString);
			}
			else {
				QString cmdString = QString("%1 %2\n\n").arg(admsXml, Arguments.join(" "));
				cmdString = cmdString + builder.readAll();
				QMessageBox::information(this, tr("Status"), cmdString);
			}

			// Append _sym.json into _props.json, save into _symbol.json
			QFile f1(QucsSettings.QucsWorkDir.filePath(fileBase()+"_props.json"));
			QFile f2(QucsSettings.QucsWorkDir.filePath(fileBase()+"_sym.json"));
			f1.open(QIODevice::ReadOnly | QIODevice::Text);
			f2.open(QIODevice::ReadOnly | QIODevice::Text);

			QString dat1 = QString(f1.readAll());
			QString dat2 = QString(f2.readAll());
			QString finalJSON = dat1.append(dat2);

			// remove joining point
			finalJSON = finalJSON.replace("}{", "");

			QFile f3(QucsSettings.QucsWorkDir.filePath(fileBase()+"_symbol.json"));
			f3.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream out(&f3);
			out << finalJSON;

			f1.close();
			f2.close();
			f3.close();

			// TODO choose icon, default to something or provided png

		} // if DataDisplay va
	} // if suffix .sym
#endif
}

}
