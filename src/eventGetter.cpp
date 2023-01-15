// #include <ESPAsyncWebServer.h>
#include "eventGetter.h"

int userSelectedDirection = 100;

int handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  int direction;
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;

    Serial.println((char *)data);

    if (strcmp((char *)data, "toggle") == 0)
    {
      ///   ledState = !ledState;
      //  notifyClients();
    }
    if (strcmp((char *)data, "forward") == 0)
    {

      direction = 1;
    }
    if (strcmp((char *)data, "back") == 0)
    {

      direction = 2;
    }
    if (strcmp((char *)data, "left") == 0)
    {
      direction = 3;
    }
    if (strcmp((char *)data, "right") == 0)
    {
      direction = 4;
    }
    if (strcmp((char *)data, "stop") == 0)
    {

      direction = 0;
    }

    if (strcmp((char *)data, "ctrl") == 0)
    {
      direction = 99;
    }
  }
  return direction;
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    userSelectedDirection = handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}