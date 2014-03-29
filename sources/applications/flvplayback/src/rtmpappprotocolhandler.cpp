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
#include "rtmpappprotocolhandler.h"
#include "protocols/rtmp/basertmpprotocol.h"
#include "protocols/rtmp/messagefactories/messagefactories.h"
#include "application/baseclientapplication.h"
#include "streaming/baseinnetstream.h"
#include "streaming/streamstypes.h"
#include "protocols/rtmp/streaming/innetrtmpstream.h"
using namespace app_flvplayback;

RTMPAppProtocolHandler::RTMPAppProtocolHandler(Variant &configuration)
: BaseRTMPAppProtocolHandler(configuration) {

}

RTMPAppProtocolHandler::~RTMPAppProtocolHandler() {
}

bool RTMPAppProtocolHandler::ProcessInvokeConnect(BaseRTMPProtocol *pFrom,
		Variant &request) {
	//1. Get the username and the password
	Variant &username = M_INVOKE_PARAM(request, 1);
	Variant &password = M_INVOKE_PARAM(request, 2);
	if (username != V_STRING || password != V_STRING) {
		FATAL("Invalid connect request:\n%s", STR(request.ToString()));
		return false;
	}

	DEBUG("connect request:\n%s", STR(request.ToString()));
	//2. ***VERY*** basic authentication to get the ball rolling
	if ((username != "xiaoting" || password != "123456") &&(username != "yili" || password != "guosheng") && (username != "test" || password != "guosheng")){
		FATAL("Auth failed");

		Variant response = ConnectionMessageFactory::GetInvokeConnectError(request,"Username or Password Error !");
			if (!pFrom->SendMessage(response)) {
				FATAL("Unable to send message");			
			}
			pFrom->GracefullyEnqueueForDelete();
			return true;
	}

	//3. Auth passes
	return BaseRTMPAppProtocolHandler::ProcessInvokeConnect(pFrom, request);
}


bool RTMPAppProtocolHandler::ProcessInvokeGeneric(BaseRTMPProtocol *pFrom,
		Variant &request) {

	string functionName = M_INVOKE_FUNCTION(request);
	DEBUG("functionName =%s", STR(functionName));
	if (functionName == "getAvailableFlvs") {
		return ProcessGetAvailableFlvs(pFrom, request);
	} else if (functionName == "insertMetadata") {
		return ProcessInsertMetadata(pFrom, request);
	}else if(functionName == "getDemondFlvs"){
		return ProcessGetDemondFlvs(pFrom, request);
	} else if(functionName == "getMembers"){
		return ProcessGetMembers(pFrom, request);
	}else if(functionName == "test"){
		return ProcessTest(pFrom, request);
	}
	else {
		return BaseRTMPAppProtocolHandler::ProcessInvokeGeneric(pFrom, request);
	}
}

bool RTMPAppProtocolHandler::ProcessGetAvailableFlvs(BaseRTMPProtocol *pFrom, Variant &request) {
	Variant parameters;
	parameters.PushToArray(Variant());
	parameters.PushToArray(Variant());

	map<uint32_t, BaseStream *> allInboundStreams =
			GetApplication()->GetStreamsManager()->FindByType(ST_IN_NET, true);

	FOR_MAP(allInboundStreams, uint32_t, BaseStream *, i) {
		parameters[(uint32_t) 1].PushToArray(MAP_VAL(i)->GetName());
	}
	DEBUG("parameters-:\n%s", STR(parameters.ToString()));
	
	Variant message = GenericMessageFactory::GetInvoke(3, 0, 0, false, 0,
			"SetAvailableFlvs", parameters);

	return SendRTMPMessage(pFrom, message);
}

bool RTMPAppProtocolHandler::ProcessInsertMetadata(BaseRTMPProtocol *pFrom, Variant &request) {
	NYIR;
}


bool RTMPAppProtocolHandler::ProcessGetDemondFlvs(BaseRTMPProtocol *pFrom, Variant &request) {
	Variant parameters;
	parameters.PushToArray(Variant());
	parameters.PushToArray(Variant());

	DEBUG("request:\n%s", STR(request.ToString()));
	
	string callbackName = M_INVOKE_PARAM(request,1);
	DEBUG("callbackName =%s", STR(callbackName));
	
	parameters[1].PushToArray("你是我的眼");
	parameters[1].PushToArray("如果的事.mp4");
	parameters[1].PushToArray("我的眼泪.mp4");
	parameters[1].PushToArray("张韶涵.mp4");
	DEBUG("parameters:\n%s", STR(parameters.ToString()));

	Variant message = GenericMessageFactory::GetInvoke(3, 0, 0, false, 0,
			"SetDemondFlvs", parameters);

	return SendRTMPMessage(pFrom, message);
}


bool RTMPAppProtocolHandler::ProcessGetMembers(BaseRTMPProtocol *pFrom, Variant &request) {
	Variant parameters;
	parameters.PushToArray(Variant());
	parameters.PushToArray(Variant());

	DEBUG("request:\n%s", STR(request.ToString()));
	
//	string callbackName = M_INVOKE_PARAM(request,1);
//	DEBUG("callbackName =%s", STR(callbackName));
	
	parameters[1].PushToArray("肖婷");
	parameters[1].PushToArray("张磊");
	parameters[1].PushToArray("朱工");
	parameters[1].PushToArray("周一");
	parameters[1].PushToArray("周发");
	parameters[1].PushToArray("张三");
	parameters[1].PushToArray("李四");
	parameters[1].PushToArray("王二");
	parameters[1].PushToArray("马志");
	DEBUG("parameters:\n%s", STR(parameters.ToString()));

	Variant message = GenericMessageFactory::GetInvoke(3, 0, 0, false, 0,
			"SetMembers", parameters);

	return SendRTMPMessage(pFrom, message);
}

bool RTMPAppProtocolHandler::ProcessTest(BaseRTMPProtocol *pFrom, Variant &request) {
	Variant parameters;
	parameters.PushToArray(Variant());
	parameters.PushToArray(Variant());

	DEBUG("request:\n%s", STR(request.ToString()));
	
	uint32_t protocolId = (uint32_t)M_INVOKE_PARAM(request,1);
	DEBUG("protocolId =%d", protocolId);
	
	if (MAP_HAS1(_connections, protocolId)){
		FOR_MAP(_connections, uint32_t, BaseRTMPProtocol *, i) 
		{
			BaseRTMPProtocol *pProtocol =	MAP_VAL(i);
			if ((pProtocol->GetApplication() != NULL) && (pProtocol ->GetId() ==protocolId)) {
				DEBUG("find  pProtocol");
				pProtocol->CloseAllStream();				
				break;
			}
		}
		return true;
	}
	DEBUG("_connections no  protocolId");
	return true;
}
#endif /* HAS_PROTOCOL_RTMP */

