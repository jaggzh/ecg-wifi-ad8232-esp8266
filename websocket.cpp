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

struct ecg_packet {
	uint32_t mil;
	uint16_t val;
};

void ws_add(int v) {
	unsigned long mil;
	volatile int dowss=0;
	if (_ws_connected) {
		struct ecg_packet packy;
		packy.mil = millis();
		packy.val = v;
		//Serial.print("_ws_client: "); Serial.println((unsigned int)_ws_client); Serial.flush();
		//_ws_client.sendBinary((char *)&packy, sizeof(packy));
		/* Serial.print(_ws_client.sendBinary(data, 6)); */
		/* Serial.println(_ws_client.sendBinary(data, 6)); */
		/* Serial.print(mil); */
		/* Serial.print(" "); */
		/* Serial.println(v); */
	}
}

void ws_net_disconnected() { // inform wifi disconnected
	_ws_connected=false; // wifi disconnected
}

void ws_net_connect() {
	if (!_ws_connected) {
		bool connected = _ws_client.connect(WS_HOST, WS_PORT, WS_PATH);
		if (connected) {
			Serial.println("WS connected! (ws_net_connect(). also ensure cb works)");
			_ws_client.send("login 123456");
			_ws_connected = true;
		}
	} else {
		// Serial.println("Not Connected!");
	}
}

void onMessageCallback(WebsocketsMessage message) {
	Serial.print("Got Message: ");
	Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
	if(event == WebsocketsEvent::ConnectionOpened) {
		if (_ws_connected) {
			Serial.println("WS CB: Already connected. Why are we receiving this CB?");
		} else {
			Serial.println("WS CB: Connnection Opened");
			_ws_connected = true;
		}
	} else if(event == WebsocketsEvent::ConnectionClosed) {
		Serial.println("WS CB: Connnection Closed");
		_ws_connected = false;
	} else if(event == WebsocketsEvent::GotPing) {
		Serial.println("WS CB: Got a Ping!");
	} else if(event == WebsocketsEvent::GotPong) {
		Serial.println("WS CB: Got a Pong!");
	}
}
