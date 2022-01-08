#ifndef _WEBSOCKET_H
#define _WEBSOCKET_H
#include <ArduinoWebsockets.h>

using namespace websockets;

void ws_setup(); // call once from setup()
void ws_loop(); // call each loop()
void ws_net_disconnected(); // inform wifi disconnected
void ws_net_connect();      // inform wifi reconnected

void ws_add(int v); // call to add value to send

void onMessageCallback(WebsocketsMessage message);
void onEventsCallback(WebsocketsEvent event, String data);

#ifndef _IN_WEBSOCKET_C
#endif

#endif // _WEBSOCKET_H
