// Webserver

String getRequestMsg(int status);
void handleNotFound();
void handleRoot();
void handleEffect();
void ServerLoop(void *pvParameters);