#include <PushBullet.h>

PushBullet::PushBullet(const String api_token, WiFiClientSecure *secure_client, uint16_t port, websockets::MessageCallback onLiveUpdate)
{
	this->secure_client = secure_client;
	this->api_token = api_token;
	this->port = port;
	this->websocketClient.onMessage(onLiveUpdate);
	this->websocketClient.connect("wss://stream.pushbullet.com:443/websocket/" + this->api_token);
}

bool PushBullet::openConnection()
{
	if (!this->secure_client->connect(this->push_bullet_host.c_str(), this->port))
	{
		return false;
	}
	return true;
}

bool PushBullet::closeConnection()
{
	this->secure_client->stop();
	return checkConnection();
}

bool PushBullet::checkConnection()
{
	if (this->secure_client->connected() == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void PushBullet::toggleConnection()
{
	if (checkConnection())
	{
		closeConnection();
	}
	else
	{
		openConnection();
	}
}

String PushBullet::POST(String url, String body)
{
	return String("POST ") + url + " HTTP/1.1\r\n" +
		   "Host: " + this->push_bullet_host + "\r\n" +
		   "User-Agent: ESP8266/NodeMCU 0.9\r\n" +
		   "Accept: */*\r\n" +
		   "Content-Type: application/json\r\n" +
		   "Content-Length: " + body.length() + "\r\n" +
		   "Access-Token: " + this->api_token + "\r\n\r\n" +
		   body;
}

String PushBullet::GET(String url)
{
	return String("GET ") + url + " HTTP/1.1\r\n" +
		   "Host: " + this->push_bullet_host + "\r\n" +
		   "User-Agent: ESP8266/NodeMCU 0.9\r\n" +
		   "Connection: close\r\n\r\n";
}
String PushBullet::DELETE(String url)
{
	return String("DELETE ") + url + " HTTP/1.1\r\n" +
		   "Host: " + this->push_bullet_host + "\r\n" +
		   "User-Agent: ESP8266/NodeMCU 0.9\r\n" +
		   "Connection: close\r\n\r\n";
}

bool PushBullet::sendRequest(String req)
{
#ifdef DEBUGGING
	Serial.println("Request string: ");
	Serial.println(req);
#endif
	return this->secure_client->print(req) > 0;
}

String PushBullet::sendAndToggle(String req)
{
	toggleConnection();
	sendRequest(req);
	String response = receiveAnswer();
	toggleConnection();
	return response;
}

String PushBullet::receiveAnswer()
{
	while (secure_client->connected())
	{
		String line = secure_client->readStringUntil('\n');
		if (line == "\r")
		{
			break;
		}
	}
	return secure_client->readStringUntil('\n');
}

String PushBullet::GetCurrentUser()
{
	return sendAndToggle(GET("/v2/users/me"));
}

String PushBullet::ConvertCodeToAccessToken(String client_id, String client_secret, String code)
{
	return sendAndToggle(POST("/oauth2/token", "'{\"client_id\":\"" + client_id + "\",\"client_secret\":\"" + client_secret + "\",\"code\":\"" + code + "\",\"grant_type\":\"authorization_code\"}"));
}

String PushBullet::ListChats()
{
	return sendAndToggle(GET("/v2/chats"));
}

String PushBullet::CreateChat(String email)
{
	return sendAndToggle(POST("/v2/chats", "{\"email\":\"" + email + "\"}"));
}

String PushBullet::UpdateChat(String iden, boolean muted)
{
	return sendAndToggle(POST("/v2/chats/" + iden, String("{\"muted\":") + muted + "}"));
}

void PushBullet::DeleteChat(String iden)
{
	sendAndToggle(DELETE("/v2/chats/" + iden));
}

String PushBullet::ListDevices()
{
	return sendAndToggle(GET("/v2/devices"));
}

String PushBullet::CreateDevice(String nickname,
								String model,
								String manufacturer,
								String push_token,
								int app_version,
								String icon,
								bool has_sms)
{
	return sendAndToggle(POST("/v2/devices", String("{\"app_version\":") + app_version + ",\"manufacturer\":\"" + manufacturer + "\",\"model\":\"" + model + "\",\"nickname\":\"" + nickname + "\",\"push_token\":\"" + push_token + "\",\"icon\":\"" + icon + "\",\"has_sms\":" + has_sms + "}"));
}

String PushBullet::UpdateDevice(String iden, String nickname, String model, String manufacturer, String push_token, int app_version, String icon, boolean has_sms)
{
	return sendAndToggle(POST("/v2/devices/" + iden, String("{\"app_version\":") + app_version + ",\"manufacturer\":\"" + manufacturer + "\",\"model\":\"" + model + "\",\"nickname\":\"" + nickname + "\",\"push_token\":\"" + push_token + "\",\"icon\":\"" + icon + "\",\"has_sms\":" + has_sms + "}"));
}

void PushBullet::DeleteDevice(String iden)
{
	sendAndToggle(DELETE("/v2/devices" + iden));
}

String PushBullet::ListPushes(String modified_after, String active, String cursor, int limit)
{
	return sendAndToggle(GET("/v2/pushes"));
}

String PushBullet::CreatePush(
	String type,
	String title,
	String body,
	String url,
	String file_name,
	String file_type,
	String file_url,
	String source_device_iden,
	String device_iden,
	String client_iden,
	String channel_tag,
	String email,
	String guid)
{
	return sendAndToggle(
		POST("/v2/pushes",
			 "{\"body\":\"" + body + "\",\"title\":\"" + title + "\",\"type\":\"" + type + "\",\"url\":\"" + url + "\",\"file_name\":\"" + file_type + "\",\"file_url\":\"" + file_url + "\",\"source_device_iden\":\"" + source_device_iden + "\",\"device_iden\":\"" + device_iden + "\",\"client_id\":\"" + client_iden + "\",\"channel_tag\":\"" + channel_tag + "\",\"email\":\"" + email + "\",\"guid\":\"" + guid + "\"}"));
}

String PushBullet::UpdatePush(String iden, bool dismissed)
{
	return sendAndToggle(POST("/v2/pushes/" + iden, String("{\"dismissed\":") + dismissed + "}"));
}

void PushBullet::DeletePush(String iden)
{
	sendAndToggle(DELETE(String("/v2/pushes/") + iden));
}

void PushBullet::DeleteAllPushes()
{
	sendAndToggle(DELETE(String("/v2/pushes/")));
}

String PushBullet::CreateChannel(
	String tag,
	String name,
	String description,
	String image_url,
	String website_url,
	String feed_url,
	ChannelFilter filters[],
	int nFilters,
	bool subscribe)
{
	String sFilters = "[";
	for (int i = 0; i < nFilters; i++)
	{
		ChannelFilter filter = filters[i];
		sFilters += "{";
		sFilters += "\"field\":\"" + filter.field + "\",";
		sFilters += "\"op\":\"" + filter.op + "\",";
		sFilters += "\"value\":\"" + filter.value + "\",";
		sFilters += String("\"not\":") + filter.invertResult + ",";
		sFilters += String("\"ignore_case\":") + filter.ignore_case;
		sFilters += "}";
		if (i < nFilters - 1)
			sFilters += ",";
	}
	sFilters += "]";
	return sendAndToggle(POST("/v2/channels", String("{\"description\":\"") + description + "\",\"image_url\":\"" + image_url + "\",\"name\":\"" + name + "\",\"tag\":\"" + tag + "\",\"website_url\":\"" + website_url + "\",\"feed_url\":\"" + feed_url + "\",\"feed_filters\":" + sFilters + ",\"subscribe\":" + subscribe + "}"));
}

String PushBullet::ListSubscriptions()
{
	return sendAndToggle(GET("/v2/subscriptions"));
}

String PushBullet::CreateSubscription(String channel_tag)
{
	return sendAndToggle(POST("/v2/subscriptions", "{\"channel_tag\":\"" + channel_tag + "\"}"));
}

String PushBullet::UpdateSubscription(String iden, bool muted)
{
	return sendAndToggle(POST(String("/v2/subscriptions/") + iden, String("{\"muted\":") + muted + "}"));
}

void PushBullet::DeleteSubscription(String iden)
{
	sendAndToggle(DELETE(String("/v2/subscriptions/") + iden));
}

String PushBullet::CreateText(Sms data, String file_url, bool skip_delete_file)
{
	String sData = "{";
	sData += "\"addresses\":" + data.addressesJsonList + ",";
	sData += "\"file_type\":\"" + data.fileType + "\",";
	sData += "\"guid\":\"" + data.guid + "\",";
	sData += "\"message\":\"" + data.message + "\",";
	sData += "\"target_device_iden\":\"" + data.targetDeviceIden + "\"";
	sData += "}";

	return sendAndToggle(POST("/v2/texts", String("{\"data\":") + sData + ",\"file_url\":\"" + file_url + "\",\"skip_delete_file\":" + skip_delete_file + "}"));
}

String PushBullet::UpdateText(String iden, Sms data, bool skip_delete_file)
{
	String sData = "{";
	sData += "\"addresses\":" + data.addressesJsonList + ",";
	sData += "\"file_type\":\"" + data.fileType + "\",";
	sData += "\"guid\":\"" + data.guid + "\",";
	sData += "\"message\":\"" + data.message + "\",";
	sData += "\"target_device_iden\":\"" + data.targetDeviceIden + "\"";
	sData += "}";

	return sendAndToggle(POST(String("/v2/texts") + iden, String("{\"data\":") + sData + ",\"skip_delete_file\":" + skip_delete_file + "}"));
}

void PushBullet::DeleteText(String iden)
{
	sendAndToggle(DELETE(String("/v2/texts/") + iden));
}