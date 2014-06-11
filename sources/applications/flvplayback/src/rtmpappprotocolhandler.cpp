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
#include <iostream>
#include "rtmpappprotocolhandler.h"
#include "protocols/rtmp/basertmpprotocol.h"
#include "protocols/rtmp/messagefactories/messagefactories.h"
#include "application/baseclientapplication.h"
#include "streaming/baseinnetstream.h"
#include "streaming/streamstypes.h"
#include "protocols/rtmp/streaming/innetrtmpstream.h"
#include "protocols/rtmp/sharedobjects/so.h"

using namespace app_flvplayback;

RTMPAppProtocolHandler::RTMPAppProtocolHandler(Variant &configuration)
: BaseRTMPAppProtocolHandler(configuration) {

}

RTMPAppProtocolHandler::~RTMPAppProtocolHandler() {
}

bool RTMPAppProtocolHandler::ProcessInvokeClose(BaseRTMPProtocol *pFrom, Variant &request) {
	TrackMemberDelSO(pFrom,request);
	return BaseRTMPAppProtocolHandler::ProcessInvokeClose(pFrom,request);
}

bool RTMPAppProtocolHandler::ProcessInvokeConnect(BaseRTMPProtocol *pFrom,
		Variant &request) {
	//1. Get the username and the password

	DEBUG("connect request:\n%s", STR(request.ToString()));
	
	Variant username = M_INVOKE_PARAM(request, 1);
/*	Variant &password = M_INVOKE_PARAM(request, 2);
	if (username != V_STRING || password != V_STRING) {
		FATAL("Invalid connect request:\n%s", STR(request.ToString()));
		return false;
	}
*/
	
	if ((username == V_NULL) || (username == V_UNDEFINED) || username == "" ) {
		FATAL("Auth failed");
		//WARN("no Name!");
		//pFrom->clientname="unknow";
		//return BaseRTMPAppProtocolHandler::ProcessInvokeConnect(pFrom, request);
			return false;
	}

	pFrom->_clientname=format("%s",STR(username));
	DEBUG("clientname:%s", STR(pFrom->_clientname));

	TrackMemberSO(pFrom,request,pFrom->_clientname);
	//2. ***VERY*** basic authentication to get the ball rolling
/*	if ((username != "xiaoting" || password != "123456") &&(username != "yili" || password != "guosheng") && (username != "test" || password != "guosheng")){
		FATAL("Auth failed");

		Variant response = ConnectionMessageFactory::GetInvokeConnectError(request,"Username or Password Error !");
			if (!pFrom->SendMessage(response)) {
				FATAL("Unable to send message");			
			}
			pFrom->GracefullyEnqueueForDelete();
			return false;
	}*/

	//3. Auth passes
	return BaseRTMPAppProtocolHandler::ProcessInvokeConnect(pFrom, request);
}


bool RTMPAppProtocolHandler::ProcessInvokeCreateStream(BaseRTMPProtocol *pFrom,
		Variant &request) {
	DEBUG("request =%s", STR(request.ToString()));
	return BaseRTMPAppProtocolHandler::ProcessInvokeCreateStream(pFrom,request);
}

bool RTMPAppProtocolHandler::ProcessInvokePublish(BaseRTMPProtocol *pFrom,
		Variant &request) {
	DEBUG("request =%s", STR(request.ToString()));
	return BaseRTMPAppProtocolHandler::ProcessInvokePublish(pFrom,request);
}

bool RTMPAppProtocolHandler::ProcessInvokePlay(BaseRTMPProtocol *pFrom,
		Variant & request){
	DEBUG("request =%s", STR(request.ToString()));
	return BaseRTMPAppProtocolHandler::ProcessInvokePlay(pFrom,request);
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
	}else if(functionName == "shotout"){
		return ProcessShotout(pFrom, request);
	}else if(functionName == "sotest"){
		return ProcessSOTest(pFrom, request);
	}
	else if(functionName == "chatMessage"){
		return ProcessSendMessage(pFrom, request);
	}else if(functionName == "chatMessageto"){
		return ProcessSendMessageTo(pFrom, request);
	}
	else {
		return BaseRTMPAppProtocolHandler::ProcessInvokeGeneric(pFrom, request);
	}
}

bool RTMPAppProtocolHandler::ProcessAttendClass(BaseRTMPProtocol *pFrom,Variant & request){
	DEBUG("request =%s", STR(request.ToString()));
	return true;
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
	return true;
}


bool RTMPAppProtocolHandler::ProcessGetDemondFlvs(BaseRTMPProtocol *pFrom, Variant &request) {
	Variant parameters;
	parameters.PushToArray(Variant());
//	parameters.PushToArray(Variant());

	DEBUG("request:\n%s", STR(request.ToString()));
	
	string callbackName = M_INVOKE_PARAM(request,1);
	DEBUG("callbackName =%s", STR(callbackName));
	
	Variant compxValue;
	compxValue["ifs.mp4"]="你是我的眼";
	compxValue["mytears.mp4"]="如果的事.mp4";
	compxValue["nswdy.flv"]="我的眼泪.mp4";
	compxValue["zhang.mp4"]="张韶涵.mp4";
	DEBUG("parameters:\n%s", STR(compxValue.ToString()));

	parameters["flvs"] = Variant(compxValue);
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

bool RTMPAppProtocolHandler::ProcessShotout(BaseRTMPProtocol *pFrom, Variant &request) {
	Variant parameters;
	parameters.PushToArray(Variant());
	parameters.PushToArray(Variant());

	DEBUG("request:\n%s", STR(request.ToString()));
	
//	uint32_t protocolId = (uint32_t)M_INVOKE_PARAM(request,1);
//	DEBUG("protocolId =%d", protocolId);
	string clientname = M_INVOKE_PARAM(request,1);

	
//	if (MAP_HAS1(_connections, protocolId))
		{
		FOR_MAP(_connections, uint32_t, BaseRTMPProtocol *, i) 
		{
			BaseRTMPProtocol *pProtocol =	MAP_VAL(i);
			if ((pProtocol->GetApplication() != NULL) && (strcmp(pProtocol ->_clientname.c_str(),clientname.c_str())==0)) {
				DEBUG("find  pProtocol");
			//	pProtocol->CloseAllStream();	

			Variant panull;
			panull.PushToArray(Variant());
			panull.PushToArray("dafdsfasdfds");
			Variant message = GenericMessageFactory::GetInvoke(3, 0, 0, false, 0,"ServerShotout",panull);
			SendRTMPMessage(pProtocol, message);
			
			pProtocol->GracefullyEnqueueForDelete();
			break;
			}
		}
		return true;
	}
	DEBUG("_connections no  protocolId");
	return true;
}

bool RTMPAppProtocolHandler::ProcessSOTest(BaseRTMPProtocol *pFrom, Variant &request) {

	DEBUG("pFrom:%ld", (long)pFrom);
	DEBUG("request:\n%s", STR(request.ToString()));
	
	Variant propValue;
	propValue["x"]=45;
	propValue["y"]=36;
	string soname="boxData";
	string propName="boxXY";

	SOManager * soM=GetSOManager();
	if(soM->ContainsSO(soname)==false)
	{
		WARN("don't have boxData");
		return false;
	}

	SO *so=soM->GetSO("boxData", true);
	Variant message = SOMessageFactory::GetFlexSharedObject(3, 0, 0, false, soname,1, false);
	SOMessageFactory::AddSOPrimitiveSetProperty(message,propName, propValue);
	M_SO_PRIMITIVES(message).IsArray(false);

	DEBUG("333333,message=%s",STR(message.ToString()));

	for (uint32_t i = 0; i < M_SO_PRIMITIVES(message).MapSize(); i++) {
		Variant primitive = M_SO_PRIMITIVE(message, i);
			FOR_MAP(primitive[RM_SHAREDOBJECTPRIMITIVE_PAYLOAD], string, Variant, i) {

				so->SetAll((string &) MAP_KEY(i), MAP_VAL(i), M_SO_VER(request),
						pFrom->GetId());
			}
	}

	if (so != NULL)
		so->Track();
	return true;
}

bool RTMPAppProtocolHandler::TrackMemberSO(BaseRTMPProtocol *pFrom, Variant &request,string name) {

	string soname="memberList";
	string propName="list";
	SOManager * soM=GetSOManager();
	SO *so=soM->GetSO(soname, false);
	string key = format("%u",pFrom->GetId());
	Variant propValue	;
//	DEBUG("------------propValue=\n%s",STR(propValue.ToString()));
	if(so->HasProperty(propName)){
		Variant payload=so->GetPayload();
		propValue=payload[propName];
		propValue[key]=name;
	}
	else{
		propValue[key]=name;
	}

//	DEBUG("------333------M_SO_VER=\n%s",STR(M_SO_VER(request).ToString()));
	//DEBUG("------333------propValue=\n%s",STR(propValue.ToString()));

	Variant message = SOMessageFactory::GetFlexSharedObject(3, 0, 0, false, soname,1, false);
	SOMessageFactory::AddSOPrimitiveSetProperty(message,propName, propValue);
	M_SO_PRIMITIVES(message).IsArray(false);

	DEBUG("message=\n%s",STR(message.ToString()));

	for (uint32_t i = 0; i < M_SO_PRIMITIVES(message).MapSize(); i++) {
		Variant primitive = M_SO_PRIMITIVE(message, i);
			FOR_MAP(primitive[RM_SHAREDOBJECTPRIMITIVE_PAYLOAD], string, Variant, i) {

				so->SetAll((string &) MAP_KEY(i), MAP_VAL(i), M_SO_VER(request),
						pFrom->GetId());
			}
	}

	if (so != NULL)
		so->Track();
	return true;

}

bool RTMPAppProtocolHandler::TrackMemberDelSO(BaseRTMPProtocol *pFrom, Variant &request) {

	string soname="memberList";
	string propName="list";
	SOManager * soM=GetSOManager();
	SO *so=soM->GetSO(soname, false);
	//so->RegisterProtocol(pFrom->GetId());
	Variant propValue	;
	string key = format("%u",pFrom->GetId());
	if(so->HasProperty(propName)){
		Variant payload=so->GetPayload();
		propValue=payload[propName];
		propValue.RemoveKey(key);
	}else{
		return false;
	}
//	DEBUG("------333------propValue=\n%s",STR(propValue.ToString()));
//	DEBUG("------333------M_SO_VER=\n%s",STR(M_SO_VER(request).ToString()));


	Variant message = SOMessageFactory::GetFlexSharedObject(3, 0, 0, false, soname,1, false);
	SOMessageFactory::AddSOPrimitiveSetProperty(message,propName, propValue);
	M_SO_PRIMITIVES(message).IsArray(false);

	DEBUG("message=\n%s",STR(message.ToString()));

	for (uint32_t i = 0; i < M_SO_PRIMITIVES(message).MapSize(); i++) {
		Variant primitive = M_SO_PRIMITIVE(message, i);
			FOR_MAP(primitive[RM_SHAREDOBJECTPRIMITIVE_PAYLOAD], string, Variant, i) {

				so->SetAll((string &) MAP_KEY(i), MAP_VAL(i), M_SO_VER(request),
						pFrom->GetId());
			}
	}

	if (so != NULL)
		so->Track();
	return true;

}


bool RTMPAppProtocolHandler::ProcessSendMessage(BaseRTMPProtocol *pFrom, Variant &request) {
	DEBUG("request:\n%s", STR(request.ToString()));
	
	Variant  contextV= M_INVOKE_PARAM(request,1);
	if((contextV == V_NULL) || (contextV == V_UNDEFINED) || contextV == "" ){
		return true;
	}
	
	string context = M_INVOKE_PARAM(request,1);
	Variant  clientName= M_INVOKE_PARAM(request,2);

	Variant compxValue;
	compxValue["sender"]=pFrom->_clientname;
	compxValue["message"]=context;
	compxValue["time"]=Variant::Now() ;
	if ((clientName == V_NULL) || (clientName == V_UNDEFINED) || clientName == "" ) {
		compxValue["clientname"]="";
	}else{
		compxValue["clientname"]=clientName;
	}

	Variant parameters;
	parameters.PushToArray(Variant());
	parameters["message"] = Variant(compxValue);
	
	DEBUG("parameters:\n%s", STR(parameters.ToString()));
	Variant message = GenericMessageFactory::GetInvoke(3, 0, 0, false, 0,
	"OnMessage", parameters);

	DEBUG("message:\n%s", STR(message.ToString()));
	
	if ((clientName == V_NULL) || (clientName == V_UNDEFINED) || clientName == "" ) {
		FOR_MAP(_connections, uint32_t, BaseRTMPProtocol *, i) 
		{
			BaseRTMPProtocol *pProtocol =	MAP_VAL(i);
			SendRTMPMessage(pProtocol, message);
		}
	}
	else{
		string  clientname= M_INVOKE_PARAM(request,2);
		
		FOR_MAP(_connections, uint32_t, BaseRTMPProtocol *, i) 
		{
			BaseRTMPProtocol *pProtocol =	MAP_VAL(i);
			if ((pProtocol->GetApplication() != NULL) && (strcmp(pProtocol ->_clientname.c_str(),clientname.c_str())==0)) {
				DEBUG("find  name");
				SendRTMPMessage(pProtocol, message);
				break;
			}
		}
		SendRTMPMessage(pFrom, message);
	}
	return true;
}


bool RTMPAppProtocolHandler::ProcessSendMessageTo(BaseRTMPProtocol *pFrom, Variant &request) {
	DEBUG("request:\n%s", STR(request.ToString()));
	
	Variant  contextV= M_INVOKE_PARAM(request,1);
	if((contextV == V_NULL) || (contextV == V_UNDEFINED) || contextV == "" ){
		return true;
	}
	
	string context = M_INVOKE_PARAM(request,1);
	Variant  clientID= M_INVOKE_PARAM(request,2);

	Variant compxValue;
	compxValue["sender"]=pFrom->_clientname;
	compxValue["message"]=context;
	compxValue["time"]=Variant::Now() ;
	if ((clientID == V_NULL) || (clientID == V_UNDEFINED) || clientID == "" ) {
		compxValue["clientID"]="";
	}else{
		compxValue["clientID"]=clientID;
	}

	Variant parameters;
	parameters.PushToArray(Variant());
	parameters["message"] = Variant(compxValue);
	
	DEBUG("parameters:\n%s", STR(parameters.ToString()));
	Variant message = GenericMessageFactory::GetInvoke(3, 0, 0, false, 0,
	"OnMessage", parameters);

	DEBUG("message:\n%s", STR(message.ToString()));
	
	if ((clientID == V_NULL) || (clientID == V_UNDEFINED) || clientID == "" ) {
		FOR_MAP(_connections, uint32_t, BaseRTMPProtocol *, i) 
		{
			BaseRTMPProtocol *pProtocol =	MAP_VAL(i);
			SendRTMPMessage(pProtocol, message);
		}
	}
	else{
		uint32_t clientid=0;
		if(clientID.IsNumeric()){
		 	clientid=M_INVOKE_PARAM(request,2);}
		else{
			WARN("clientID is not number type");
			return false;
		}
		if(MAP_HAS1(_connections,clientID)){
			BaseRTMPProtocol *pProtocol =	_connections[clientid];
			SendRTMPMessage(pProtocol, message);
			SendRTMPMessage(pFrom, message);
		}else{
			WARN("can't find  clientID %u",clientid);
		}
	}
	return true;
}




#endif /* HAS_PROTOCOL_RTMP */

