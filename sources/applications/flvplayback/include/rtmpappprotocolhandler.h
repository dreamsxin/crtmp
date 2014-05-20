/*
 *  Copyright (c) 2010,
 *  Gavriloaie Eugen-Andrei (shiretu@gmail.com)
 *
 *  This file is part of crtmpserver.
 *  crtmpserver is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  crtmpserver is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with crtmpserver.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifdef HAS_PROTOCOL_RTMP
#ifndef _RTMPAPPPROTOCOLHANDLER_H
#define	_RTMPAPPPROTOCOLHANDLER_H

#include "protocols/rtmp/basertmpappprotocolhandler.h"

namespace app_flvplayback {

	class RTMPAppProtocolHandler
	: public BaseRTMPAppProtocolHandler {
	public:
		RTMPAppProtocolHandler(Variant &configuration);
		virtual ~RTMPAppProtocolHandler();
		virtual bool ProcessInvokeConnect(BaseRTMPProtocol *pFrom, Variant &request);
		virtual bool ProcessInvokeClose(BaseRTMPProtocol *pFrom, Variant &request) ;
		virtual bool ProcessInvokeGeneric(BaseRTMPProtocol *pFrom,
				Variant &request);
	private:
		bool ProcessGetAvailableFlvs(BaseRTMPProtocol *pFrom, Variant &request);
		bool ProcessInsertMetadata(BaseRTMPProtocol *pFrom, Variant &request);
		bool ProcessGetDemondFlvs(BaseRTMPProtocol *pFrom, Variant &request) ;
		bool ProcessGetMembers(BaseRTMPProtocol *pFrom, Variant &request) ;
		bool ProcessCloseStream(BaseRTMPProtocol *pFrom, Variant &request);
		bool ProcessShotout(BaseRTMPProtocol *pFrom, Variant &request) ;
		bool ProcessSOTest(BaseRTMPProtocol *pFrom, Variant &request);
		bool TrackMemberSO(BaseRTMPProtocol *pFrom, Variant &request,string name) ;
		bool TrackMemberDelSO(BaseRTMPProtocol *pFrom, Variant &request) ;
		bool ProcessSendMessage(BaseRTMPProtocol *pFrom, Variant &request) ;
	};
}
#endif	/* _RTMPAPPPROTOCOLHANDLER_H */
#endif /* HAS_PROTOCOL_RTMP */

