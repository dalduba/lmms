/*
 * ladspa_browser.cpp - dialog to display information about installed LADSPA
 *                      plugins
 *
 * Copyright (c) 2006-2008 Danny McRae <khjklujn/at/users.sourceforge.net>
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of LMMS - http://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */


#include "python_runner.h"


#include <QHBoxLayout>
#include <QLabel>


#include "gui_templates.h"
#include "TabBar.h"
#include "TabButton.h"

#include "embed.cpp"
#include "PythonQt.h"
#include "../extensions/PythonQt_QtAll/PythonQt_QtAll.h"
#include "gui/PythonQtScriptingConsole.h"

extern "C"
{

Plugin::Descriptor PLUGIN_EXPORT python_runner_plugin_descriptor =
{
	STRINGIFY( PLUGIN_NAME ),
    "Python Script Runner",
    QT_TRANSLATE_NOOP( "PythonRunner",
                "Run python script" ),
    "dal",
	0x0100,
	Plugin::Tool,
    new PluginPixmapLoader( "python" ),
	NULL,
	NULL
} ;


// necessary for getting instance out of shared lib
Plugin * PLUGIN_EXPORT lmms_plugin_main( Model * _parent, void * _data )
{
    return new pythonRunner;
}

}




pythonRunner::pythonRunner() :
    ToolPlugin( &python_runner_plugin_descriptor, NULL )
{
//    PythonQt::init(PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
//    PythonQt_QtAll::init();

//    PythonQtObjectPtr  mainContext = PythonQt::self()->getMainModule();
//    PythonQtScriptingConsole console(NULL, mainContext);
//    console.show();
}




pythonRunner::~pythonRunner()
{
}




QString pythonRunner::nodeName() const
{
    return python_runner_plugin_descriptor.name;
}






pythonRunnerView::pythonRunnerView( ToolPlugin * _tool ) :
	ToolPluginView( _tool  )
{
	QHBoxLayout * hlayout = new QHBoxLayout( this );
	hlayout->setSpacing( 0 );
	hlayout->setMargin( 0 );

	m_tabBar = new TabBar( this, QBoxLayout::TopToBottom );
	m_tabBar->setExclusive( true );
	m_tabBar->setFixedWidth( 72 );

	QWidget * ws = new QWidget( this );
	ws->setFixedSize( 500, 480 );
	hlayout->addSpacing( 10 );
	hlayout->addWidget( ws );
	hlayout->addSpacing( 10 );
	hlayout->addStretch();

	setWhatsThis( tr(
"This is dialog for python scripts running" ) );

	hide();
	if( parentWidget() )
	{
		parentWidget()->hide();
		parentWidget()->layout()->setSizeConstraint(
							QLayout::SetFixedSize );
		
		Qt::WindowFlags flags = parentWidget()->windowFlags();
		flags |= Qt::MSWindowsFixedSizeDialogHint;
		flags &= ~Qt::WindowMaximizeButtonHint;
		parentWidget()->setWindowFlags( flags );
	}
}




pythonRunnerView::~pythonRunnerView()
{
}





