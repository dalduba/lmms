/*
 * midi_control_listener.cpp - implementation of the MIDI listener that
 *                             controls LMMS' transportation and other things
 *
 * Copyright (c) 2009 Achim Settelmeier <lmms/at/m1.sirlab.de>
 *
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
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


#include <QtCore/QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>

#include "midi_control_listener.h"
#include "mixer.h"
#include "midi_client.h"
#include "midi_port.h"
#include "engine.h"
#include "note.h"
#include "song.h"
#include "config_mgr.h"

const QString MidiControlListener::configClass = "MidiControlListener";
QDomElement MidiControlListener::s_configTree;

const MidiControlListener::ActionNameMap MidiControlListener::actionNames[] = 
{
	{ MidiControlListener::ActionNone, "", "" },
	{ MidiControlListener::ActionControl, "Control key", "control" },
	{ MidiControlListener::ActionPlay, "Play", "play" },
	{ MidiControlListener::ActionStop, "Stop", "stop" }
};




MidiControlListener::MidiControlListener() :
	m_port( "UnnamedMidiController",
	       engine::getMixer()->getMidiClient(), this, NULL,
	       midiPort::Input ),
	m_controlKeyCount( 0 ),
	m_listenerEnabled( false ),
	m_channel( -1 ),
	m_useControlKey( false )
{
	readConfiguration();  // reads previously remembered configuration
}




MidiControlListener::~MidiControlListener()
{
}




void MidiControlListener::processInEvent( const midiEvent & _me,
					 const midiTime & _time )
{
	// don't do anything unless the listener is enabled
	if( ! m_listenerEnabled )
	{
		return;
	}
	
	// pre-check whether this MIDI packet suits our configuration
	switch( _me.m_type )
	{
		case MidiNoteOn:
		case MidiNoteOff:
		case MidiControlChange:
			// ignore commands for other channels
			if( m_channel != -1 && m_channel != _me.channel() )
			{
				return;
			}
			break;
			
		default:
			// ignore commands other than note on/off and control change
			return;
	}
	
	// check MIDI packet type and act upon
	switch( _me.m_type )
	{
		case MidiNoteOn:
			if( m_actionMapKeys.contains( _me.key() ) )
			{
				if( m_actionMapKeys.value( _me.key(), ActionNone ) == ActionControl )
				{
					if( _me.velocity() > 0 )
					{
						m_controlKeyCount++;
					}
					else
					{
						m_controlKeyCount--;
						if( m_controlKeyCount < 0 )
						{
							m_controlKeyCount = 0;
						}
					}
				}
				else if( _me.velocity() > 0 &&
					( !m_useControlKey || m_controlKeyCount > 0) ) 
				{
					act( m_actionMapKeys.value( _me.key(), ActionNone ) );
				}
			}
			break;
			
		case MidiNoteOff:
			break;
			
		case MidiControlChange:
			// controller changed to a value other than zero
			if( _me.m_data.m_param[1] > 0 )
			{
				act( m_actionMapControllers.value( _me.m_data.m_param[0], ActionNone ) );
			}
			break;
			
		default:
			// nop
			break;
	}
}




void MidiControlListener::act( EventAction _action )
{
	switch( _action )
	{
		case ActionNone:
			break;
		case ActionControl:
			break;
		case ActionPlay:
			engine::getSong()->play();
			break;
		case ActionStop:
			engine::getSong()->stop();
			break;
	}
}




/**
 * add DOM nodes to the configuration
 */
void MidiControlListener::saveConfiguration( QDomDocument & doc )
{
	// The root node must not have any attributes, otherwise it would
	// conflict with the configManager. Instead, attributes are moved 
	// to a subnode.
	QDomElement confRoot = doc.createElement( configClass );
	QDomElement conf = doc.createElement( "config" );
	confRoot.appendChild( conf );
	
	// add basic configuration variables
	conf.setAttribute( "enabled", m_listenerEnabled );
	conf.setAttribute( "useControlKey", m_useControlKey );
	conf.setAttribute( "channel", m_channel + 1 );
	
	// get subscribed MIDI device
	midiPort::map map = m_port.readablePorts();
	for( midiPort::map::iterator it = map.begin();
	    it != map.end(); ++it )
	{
		if( it.value() )
		{
			QDomElement device = doc.createElement( "device" );
			device.appendChild( doc.createTextNode( it.key() ) );
			conf.appendChild( device );
		}
	}
	
	// add key actions
	for( ActionMap::const_iterator it = m_actionMapKeys.begin(); 
	    it != m_actionMapKeys.end(); ++it )
	{
		QDomElement actionNode = doc.createElement( "action" );
		actionNode.setAttribute( "type", "key" );
		actionNode.setAttribute( "key", it.key() );
		actionNode.setAttribute( "actionName", 
					action2ActionNameMap( it.value() ).nameShort );
		confRoot.appendChild( actionNode );
	}
	
	// add controller actions
	for( ActionMap::const_iterator it = m_actionMapControllers.begin(); 
	    it != m_actionMapControllers.end(); ++it )
	{
		QDomElement actionNode = doc.createElement( "action" );
		actionNode.setAttribute( "type", "controller" );
		actionNode.setAttribute( "controller", it.key() );
		actionNode.setAttribute( "actionName", 
					action2ActionNameMap( it.value() ).nameShort );
		confRoot.appendChild( actionNode );
	}
	
	QDomElement lmmsConfig = doc.documentElement();
	lmmsConfig.appendChild( confRoot );
}




/**
 * Remember configuration for later retrieval. 
 * This is necessary because at the time the configManager loads the 
 * configuration, the engine is not yet initialized and there's no 
 * MidiControlListener object.
 */
void MidiControlListener::rememberConfiguration( QDomDocument & doc )
{
	QDomNodeList list = doc.elementsByTagName( configClass );
	if( list.isEmpty() )
	{
		return;
	}
	
	s_configTree = list.item(0).cloneNode( true ).toElement();
}




/**
 * reads the configuration from the previously stored configuration subtree.
 */
void MidiControlListener::readConfiguration()
{
	// default settings
	m_listenerEnabled = false;  // turn off by default
	m_useControlKey = true;     // use control key
	m_channel = -1;             // listen on all channels
	m_actionMapKeys.clear();    // empty action lists
	m_actionMapControllers.clear();
	
	// unsubscribe all ports
	midiPort::map map = m_port.readablePorts();
	for( midiPort::map::iterator it = map.begin();
	    it != map.end(); ++it )
	{
		if( it.value() )
		{
			m_port.subscribeReadablePort( it.key(), false );
		}
	}
	
	
	// check whether there's a configuration tree at all
	if( s_configTree.isNull() || ! s_configTree.hasChildNodes() )
	{
		return;
	}
	
	QDomElement conf = s_configTree.firstChildElement( "config" );
	// check whether config tag is present
	if( ! conf.isNull() )
	{
		QString val;
		
		// read config parameters
		val = conf.attribute( "enabled" );
		if( val.toInt() )  // default: false
		{
			m_listenerEnabled = true;
		}
		
		val = conf.attribute( "channel" );
		if( val != "" )
		{
			m_channel = val.toInt() - 1;
		}
		
		val = conf.attribute( "useControlKey" );
		if( val.toInt() == 0 ) // default: true
		{
			m_useControlKey = false;
		}
		
		for( QDomElement deviceNode = s_configTree.firstChildElement( "device" );
		    !deviceNode.isNull();
		     deviceNode = deviceNode.nextSiblingElement( "device" ) )
		{
			if( deviceNode.text() != "" )
			{
				m_port.subscribeReadablePort( deviceNode.text(), true );
			}
		}
	}
	
	// now read action tags
	// use iterator instead of elementsByTagName(), which under unsetteled circumstances returns an empty list
	for( QDomElement actionNode = s_configTree.firstChildElement( "action" );
	    !actionNode.isNull();
	     actionNode = actionNode.nextSiblingElement( "action" ) )
	{
		EventAction action = actionName2ActionNameMap( actionNode.attribute( "actionName" ) ).action;
		
		if( actionNode.attribute( "type" ) == "key" )
		{
			int key = actionNode.attribute( "key" ).toInt();
			m_actionMapKeys[key] = action;
		}
		else if( actionNode.attribute( "type" ) == "controller" )
		{
			int controller = actionNode.attribute( "controller" ).toInt();
			m_actionMapControllers[controller] = action;
		}
	}
}




MidiControlListener::ActionNameMap MidiControlListener::action2ActionNameMap( EventAction _action )
{
	for( int i = 0; i < numActions; ++i )
	{
		if( actionNames[i].action == _action )
		{
			return actionNames[i];
		}
	}
	return actionNames[0];
}




MidiControlListener::ActionNameMap MidiControlListener::actionName2ActionNameMap( QString _actionName )
{
	for( int i = 0; i < numActions; ++i )
	{
		if( actionNames[i].name == _actionName || 
		    actionNames[i].nameShort == _actionName )
		{
			return actionNames[i];
		}
	}
	return actionNames[0];
}



