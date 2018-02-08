/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2014, Live Networks, Inc.  All rights reserved
// A subclass of "RTSPServer" that creates "ServerMediaSession"s on demand,
// based on whether or not the specified stream name exists as a file
// Implementation

#include "DynamicRTSPServer.hh"
#include <liveMedia.hh>
#include <string.h>
#include "debuginfo.h"
#include "H265VideoLiveServerMediaSubsession.hh"

DynamicRTSPServer*
DynamicRTSPServer::createNew(UsageEnvironment& env, Port ourPort,
UserAuthenticationDatabase* authDatabase,
unsigned reclamationTestSeconds) {
	int ourSocket = setUpOurSocket(env, ourPort);
	if (ourSocket == -1) return NULL;

	return new DynamicRTSPServer(env, ourSocket, ourPort, authDatabase, reclamationTestSeconds);
}

DynamicRTSPServer::DynamicRTSPServer(UsageEnvironment& env, int ourSocket,
	Port ourPort,
	UserAuthenticationDatabase* authDatabase, unsigned reclamationTestSeconds)
	: RTSPServerSupportingHTTPStreaming(env, ourSocket, ourPort, authDatabase, reclamationTestSeconds) {
}

DynamicRTSPServer::~DynamicRTSPServer() {
}

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
	char const* fileName, FILE* fid); // forward

ServerMediaSession* DynamicRTSPServer
::lookupServerMediaSession(char const* streamName, Boolean isFirstLookupInSession) {
	// First, check whether the specified "streamName" exists as a local file:
	DEBUG_DEBUG("streamName:%s",streamName);

	Boolean fileExists = 0;
	if (strcmp(streamName, "TypeCodec=H.265") == 0)
	{
        fileExists = 1;
	}

	// Next, check whether we already have a "ServerMediaSession" for this file:
	ServerMediaSession* sms = RTSPServer::lookupServerMediaSession(streamName);
	Boolean smsExists = sms != NULL;

	// Handle the four possibilities for "fileExists" and "smsExists":
	if (!fileExists) {
		if (smsExists) {
			// "sms" was created for a file that no longer exists. Remove it:
			removeServerMediaSession(sms);
			sms = NULL;
		}

		return NULL;
	}
	else {
		if (smsExists && isFirstLookupInSession) {
			// Remove the existing "ServerMediaSession" and create a new one, in case the underlying
			// file has changed in some way:
			removeServerMediaSession(sms);
			sms = NULL;
		}

		if (sms == NULL) {
			sms = createNewSMS(envir(), streamName, 0);
			addServerMediaSession(sms);
			DEBUG_DEBUG("addServerMediaSession(sms);");
		}

		return sms;
	}
}

#define NEW_SMS(description) do {\
	char const* descStr = description\
	", streamed by the LIVE555 Media Server"; \
	sms = ServerMediaSession::createNew(env, fileName, fileName, descStr); \
} while (0)

static ServerMediaSession* createNewSMS(UsageEnvironment& env,
	char const* fileName, FILE* /*fid*/) {
	// Use the file name extension to determine the type of "ServerMediaSession":
	DEBUG_DEBUG("fileName:%s",fileName);
	ServerMediaSession* sms = NULL;
    Boolean const reuseSource = False;

	if (strcmp(fileName, "TypeCodec=H.265") == 0) {
		// Assumed to be a H.265 Video Elementary Stream
		NEW_SMS("TypeCodec=H.265 Video");
		OutPacketBuffer::maxSize = 64 * 1024;//100000; // allow for some possibly large H.265 frames
        H265VideoLiveMediaSource* H265InputDevice = H265VideoLiveMediaSource::createNew(env);
        DEBUG_DEBUG("H265VideoLiveMediaSource");
		sms->addSubsession(H265VideoLiveServerMediaSubsession::createNew(env, *H265InputDevice, reuseSource));
        DEBUG_DEBUG("H265VideoLiveServerMediaSubsession");
	}

	return sms;
}
