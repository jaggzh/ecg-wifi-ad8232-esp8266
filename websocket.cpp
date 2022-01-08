#define _IN_WEBSOCKET_C
#include "websocket.h"
#include "wifi_config.h"

// https://github.com/gilmaimon/ArduinoWebsockets

bool _ws_connected=false;
WebsocketsClient _ws_client;

void ws_setup() {
	_ws_client.onMessage(onMessageCallback);
	_ws_client.onEvent(onEventsCallback);
}

void ws_loop() {
	_ws_client.poll();
}

void ws_add(int v) {
	unsigned long mil;
	if (_ws_connected) {
		char data[6];
		mil = millis();
		data[0] = ((char *)mil)[0];
		data[1] = ((char *)mil)[1];
		data[2] = ((char *)mil)[2];
		data[3] = ((char *)mil)[3];
		data[4] = ((char *)v)[0];
		data[5] = ((char *)v)[1];
		_ws_client.send(data, 6);
		Serial.print(mil);
		Serial.print(" ");
		Serial.println(v);
	}
}

void ws_net_disconnected() { // inform wifi disconnected
	_ws_connected=false; // wifi disconnected
}

void ws_net_connect() {
	bool connected = _ws_client.connect(WS_HOST, WS_PORT, WS_PATH);
	if (connected) {
		Serial.println("WS connected! (ws_net_connect(). also ensure cb works)");
		_ws_client.send("login 123456");
		_ws_connected = true;
	} else {
		Serial.println("Not Connected!");
	}
}

void onMessageCallback(WebsocketsMessage message) {
	Serial.print("Got Message: ");
	Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
	if(event == WebsocketsEvent::ConnectionOpened) {
		Serial.println("WS CB: Connnection Opened");
		_ws_connected = true;
	} else if(event == WebsocketsEvent::ConnectionClosed) {
		Serial.println("WS CB: Connnection Closed");
		_ws_connected = false;
	} else if(event == WebsocketsEvent::GotPing) {
		Serial.println("WS CB: Got a Ping!");
	} else if(event == WebsocketsEvent::GotPong) {
		Serial.println("WS CB: Got a Pong!");
	}
}
