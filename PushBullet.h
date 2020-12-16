#ifndef _PUSHBULLET_H_
#define _PUSHBULLET_H_

#include <SPI.h>
#include <WiFiClientSecure.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

using namespace websockets;

#define DEBUGGING true

class PushBullet
{

public:
	PushBullet(const String api_token, WiFiClientSecure *secure_client, uint16_t port, websockets::MessageCallback onLiveUpdate);
	bool checkConnection();

	String GetCurrentUser();
	String ConvertCodeToAccessToken(String client_id, String client_secret, String code);

	String ListChats();
	String CreateChat(String email);
	String UpdateChat(String iden, boolean muted);
	void DeleteChat(String iden);

	String ListDevices();
	String CreateDevice(
		String nickname,
		String model,
		String manufacturer,
		String push_token,
		int app_version,
		String icon,
		bool has_sms);
	String UpdateDevice(String iden, String nickname, String model, String manufacturer, String push_token, int app_version, String icon, boolean has_sms);
	void DeleteDevice(String iden);

	String ListPushes(String modified_after, String active, String cursor, int limit);
	String CreatePush(
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
		String guid);
	String UpdatePush(String iden, bool dismissed);
	void DeletePush(String iden);
	void DeleteAllPushes();

	struct ChannelFilter
	{
		String field;
		String op;
		String value;
		bool invertResult;
		bool ignore_case;
	};

	String CreateChannel(
		String tag,
		String name,
		String description,
		String image_url,
		String website_url,
		String feed_url,
		ChannelFilter filters[],
		int nFilters,
		bool subscribe);

	String ListSubscriptions();
	String CreateSubscription(String channel_tag);
	String UpdateSubscription(String iden, bool muted);
	void DeleteSubscription(String iden);

	String ChannelInfo(String tag, bool no_recent_pushes);

	struct Sms
	{
		String addressesJsonList;
		String fileType;
		String guid;
		String message;
		String targetDeviceIden;
	};
	String CreateText(Sms data, String file_url, bool skip_delete_file);
	String UpdateText(String iden, Sms data, bool skip_delete_file);
	void DeleteText(String iden);

	bool PollLiveUpdates();

private:
	String POST(String url, String body);
	String GET(String url);
	String DELETE(String url);
	bool sendRequest(String reg);
	String receiveAnswer();
	String sendAndToggle(String req);
	void toggleConnection();

	bool openConnection();
	bool closeConnection();

	WiFiClientSecure *secure_client;
	uint16_t port;

	WebsocketsClient websocketClient;

	String api_token;
	const String push_bullet_host = "api.pushbullet.com";
};

#endif
