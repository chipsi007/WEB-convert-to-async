//
//
//
#define DBG_OUTPUT_PORT Serial

#define DEBUG_WEBSERVER

#include "FSWebServer.h"
#include "DynamicData.h"
#include "Config.h"
#include <StreamString.h>

//ESP8266WebServer server(80);

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

const char* http_username = "admin";
const char* http_password = "admin";


File fsUploadFile;
String browserMD5 = "";
static uint32_t updateSize = 0;

void sendTimeData() {
  String time = "T" + ntp->getTimeStr();

  ws.textAll(time);

  String date = "D" + ntp->getDateStr();
  ws.textAll(date);
  String sync = "S" + ntp->getTimeString(ntp->getLastNTPSync());
  ws.textAll(sync);

}
String GetMacAddress()
{
  uint8_t mac[6];
  char macStr[18] = { 0 };
  WiFi.macAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return  String(macStr);
}





//void updateFirmware () {
//  // handler for the file upload, get's the sketch bytes, and writes
//  // them through the Update object
//  HTTPUpload& upload = server.upload();
//  if (upload.status == UPLOAD_FILE_START) {
//    WiFiUDP::stopAll();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.printf("Update: %s\n", upload.filename.c_str());
//#endif // DEBUG_WEBSERVER
//    //uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
//    uint32_t maxSketchSpace = ESP.getSketchSize();
//#ifdef DEBUG_WEBSERVER
//    //uint32_t oldValue = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
//    DBG_OUTPUT_PORT.printf("Max free scketch space: %u\n", maxSketchSpace);
//    DBG_OUTPUT_PORT.printf("New scketch size: %u\n", updateSize);
//    //DBG_OUTPUT_PORT.printf("Old value: %u\n", oldValue);
//#endif // DEBUG_WEBSERVER
//    if (browserMD5 != NULL && browserMD5 != "") {
//      Update.setMD5(browserMD5.c_str());
//#ifdef DEBUG_WEBSERVER
//      DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
//#endif // DEBUG_WEBSERVER
//    }
//    if (!Update.begin(updateSize)) {//start with max available size
//#ifdef DEBUG_WEBSERVER
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_WRITE) {
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.print(".");
//#endif // DEBUG_WEBSERVER
//    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//#ifdef DEBUG_WEBSERVER
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_END) {
//    String updateHash;
//    if (Update.end(true)) { //true to set the size to the current progress
//#ifdef DEBUG_WEBSERVER
//      updateHash = Update.md5String();
//      DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
//      DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//#endif // DEBUG_WEBSERVER
//    }
//    else {
//#ifdef DEBUG_WEBSERVER
//      updateHash = Update.md5String();
//      DBG_OUTPUT_PORT.printf("Upload failed. Calculated MD5: %s\n", updateHash.c_str());
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_ABORTED) {
//    Update.end();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.println("Update was aborted");
//#endif // DEBUG_WEBSERVER
//  }
//  delay(2);
//}


void updateFirmware () {
  // handler for the file upload, get's the sketch bytes, and writes
  // them through the Update object
//  HTTPUpload& upload = server.upload();
//  if (upload.status == UPLOAD_FILE_START) {
//    WiFiUDP::stopAll();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.printf("Update: %s\n", upload.filename.c_str());
//#endif // DEBUG_WEBSERVER
    //uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
    uint32_t maxSketchSpace = ESP.getSketchSize();
#ifdef DEBUG_WEBSERVER
    //uint32_t oldValue = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
    DBG_OUTPUT_PORT.printf("Max free scketch space: %u\n", maxSketchSpace);
    DBG_OUTPUT_PORT.printf("New scketch size: %u\n", updateSize);
    //DBG_OUTPUT_PORT.printf("Old value: %u\n", oldValue);
#endif // DEBUG_WEBSERVER
    if (browserMD5 != NULL && browserMD5 != "") {
      Update.setMD5(browserMD5.c_str());
#ifdef DEBUG_WEBSERVER
      DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
#endif // DEBUG_WEBSERVER
    }
    if (!Update.begin(updateSize)) {//start with max available size
#ifdef DEBUG_WEBSERVER
      Update.printError(DBG_OUTPUT_PORT);
#endif // DEBUG_WEBSERVER
    }
  
//  else if (upload.status == UPLOAD_FILE_WRITE) {
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.print(".");
//#endif // DEBUG_WEBSERVER
//    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//#ifdef DEBUG_WEBSERVER
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
    
  
 // else if (upload.status == UPLOAD_FILE_END) {
    String updateHash;
    if (Update.end(true)) { //true to set the size to the current progress
#ifdef DEBUG_WEBSERVER
      updateHash = Update.md5String();
      DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
     // DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
#endif // DEBUG_WEBSERVER
   }
    else {
#ifdef DEBUG_WEBSERVER
      updateHash = Update.md5String();
      DBG_OUTPUT_PORT.printf("Upload failed. Calculated MD5: %s\n", updateHash.c_str());
      Update.printError(DBG_OUTPUT_PORT);
#endif // DEBUG_WEBSERVER
    }
  };


void handleUploadme(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    Serial.printf("UploadStart: %s\n", filename.c_str());

      uint32_t maxSketchSpace = ESP.getSketchSize();
#ifdef DEBUG_WEBSERVER
    //uint32_t oldValue = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
    DBG_OUTPUT_PORT.printf("Max free scketch space: %u\n", maxSketchSpace);
    DBG_OUTPUT_PORT.printf("New scketch size: %u\n", updateSize);
    //DBG_OUTPUT_PORT.printf("Old value: %u\n", oldValue);
#endif // DEBUG_WEBSERVER
    if (browserMD5 != NULL && browserMD5 != "") {
      Update.setMD5(browserMD5.c_str());
#ifdef DEBUG_WEBSERVER
      DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
#endif // DEBUG_WEBSERVER
    }
    if (!Update.begin(updateSize)) {//start with max available size
#ifdef DEBUG_WEBSERVER
      Update.printError(DBG_OUTPUT_PORT);
#endif // DEBUG_WEBSERVER
    }
    
  }
  for(size_t i=0; i<len; i++){
    Serial.write(data[i]);
  }
  if(final){
    Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index+len);

      String updateHash;
    if (Update.end(true)) { //true to set the size to the current progress
#ifdef DEBUG_WEBSERVER
      updateHash = Update.md5String();
      DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
     // DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
#endif // DEBUG_WEBSERVER
   }
    else {
#ifdef DEBUG_WEBSERVER
      updateHash = Update.md5String();
      DBG_OUTPUT_PORT.printf("Upload failed. Calculated MD5: %s\n", updateHash.c_str());
      Update.printError(DBG_OUTPUT_PORT);
#endif // DEBUG_WEBSERVER
    }
    
  }
}
 

//void handleUploadme(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
//  if (!index) {
//    Serial.printf("UploadStartUploadStart: %s\n", filename.c_str());
//  }
//    for(size_t i=0; i<len; i++){
////       Serial.printf(data[i]);
//         DBG_OUTPUT_PORT.printf("printf printf printf: %s\n", data[i]);
// }
//  if (final) {
//    Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index + len);
//
////    
////#ifdef DEBUG_WEBSERVER
////      DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
////#endif // DEBUG_WEBSERVER
////  
////    if (!Update.begin(updateSize)) {//start with max available size
////#ifdef DEBUG_WEBSERVER
////      Update.printError(DBG_OUTPUT_PORT);
////#endif // DEBUG_WEBSERVER
////    }
//if (browserMD5 != NULL && browserMD5 != "") {
//      Update.setMD5(browserMD5.c_str());
//  }
//
//    String updateHash;
//    if (Update.end(true)) { //true to set the size to the current progress
//#ifdef DEBUG_WEBSERVER
//      updateHash = Update.md5String();
//      DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
//      DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", len);
//#endif // DEBUG_WEBSERVER
// 
//    }
 
//  }
//} ;


class SPIFFSEditor: public AsyncWebHandler {
  private:
    String _username;
    String _password;
    bool _authenticated;
    uint32_t _startTime;
  public:
    SPIFFSEditor(String username = String(), String password = String()): _username(username), _password(password), _authenticated(false), _startTime(0) {}
    bool canHandle(AsyncWebServerRequest *request) {
      if (request->method() == HTTP_GET && request->url() == "/edit")
        return true;
      else if (request->method() == HTTP_GET && request->url() == "/list")
        return true;
      else if (request->method() == HTTP_GET && !(request->url().endsWith("/")) && request->hasParam("download"))
        return true;
      else if (request->method() == HTTP_POST && request->url() == "/edit")
        return true;
      else if (request->method() == HTTP_DELETE && request->url() == "/edit")
        return true;
      else if (request->method() == HTTP_PUT && request->url() == "/edit")
        return true;
      return false;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      if (_username.length() && _password.length() && !request->authenticate(_username.c_str(), _password.c_str()))
        return request->requestAuthentication();

      if (request->method() == HTTP_GET && request->url() == "/edit") {
        server.serveStatic("/edit.html", SPIFFS, "/www/edit.html.gz");
        //        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/edit.html.gz");
        //
        //        //    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", edit_htm_gz, edit_htm_gz_len);
        //        response->addHeader("Content-Encoding", "gzip");
        //        request->send(response);
      } else if (request->method() == HTTP_GET && request->url() == "/list") {
        if (request->hasParam("dir")) {
          String path = request->getParam("dir")->value();
          Dir dir = SPIFFS.openDir(path);
          path = String();
          String output = "[";
          while (dir.next()) {
            fs::File entry = dir.openFile("r");
            if (output != "[") output += ',';
            bool isDir = false;
            output += "{\"type\":\"";
            output += (isDir) ? "dir" : "file";
            output += "\",\"name\":\"";
            output += String(entry.name()).substring(1);
            output += "\"}";
            entry.close();
          }
          output += "]";
          request->send(200, "text/json", output);
          output = String();
        }
        else
          request->send(400);
      } else if (request->method() == HTTP_GET) {
        request->send(SPIFFS, request->url(), String(), true);
      } else if (request->method() == HTTP_DELETE) {
        if (request->hasParam("path", true)) {
          SPIFFS.remove(request->getParam("path", true)->value());
          request->send(200, "", "DELETE: " + request->getParam("path", true)->value());
        } else
          request->send(404);
      } else if (request->method() == HTTP_POST) {
        if (request->hasParam("data", true, true) && SPIFFS.exists(request->getParam("data", true, true)->value()))
          request->send(200, "", "UPLOADED: " + request->getParam("data", true, true)->value());
        else
          request->send(500);
      } else if (request->method() == HTTP_PUT) {
        if (request->hasParam("path", true)) {
          String filename = request->getParam("path", true)->value();
          if (SPIFFS.exists(filename)) {
            request->send(200);
          } else {
            fs::File f = SPIFFS.open(filename, "w");
            if (f) {
              f.write((uint8_t)0x00);
              f.close();
              request->send(200, "", "CREATE: " + filename);
            } else {
              request->send(500);
            }
          }
        } else
          request->send(400);
      }
    }

    void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
      if (!index) {
        if (!_username.length() || request->authenticate(_username.c_str(), _password.c_str())) {
          _authenticated = true;
          request->_tempFile = SPIFFS.open(filename, "w");
          _startTime = millis();
        }
      }
      if (_authenticated && request->_tempFile) {
        if (len) {
          request->_tempFile.write(data, len);
        }
        if (final) {
          request->_tempFile.close();
          uint32_t uploadTime = millis() - _startTime;
          os_printf("upload: %s, %u B, %u ms\n", filename.c_str(), index + len, uploadTime);
        }
      }
    }
};


//format bytes
//String formatBytes(size_t bytes) {
//  if (bytes < 1024) {
//    return String(bytes) + "B";
//  }
//  else if (bytes < (1024 * 1024)) {
//    return String(bytes / 1024.0) + "KB";
//  }
//  else if (bytes < (1024 * 1024 * 1024)) {
//    return String(bytes / 1024.0 / 1024.0) + "MB";
//  }
//  else {
//    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
//  }
//}
//
//String getContentType(String filename) {
//  if (server.hasArg("download")) return "application/octet-stream";
//  else if (filename.endsWith(".htm")) return "text/html";
//  else if (filename.endsWith(".html")) return "text/html";
//  else if (filename.endsWith(".css")) return "text/css";
//  else if (filename.endsWith(".js")) return "application/javascript";
//  else if (filename.endsWith(".json")) return "application/json";
//  else if (filename.endsWith(".png")) return "image/png";
//  else if (filename.endsWith(".gif")) return "image/gif";
//  else if (filename.endsWith(".jpg")) return "image/jpeg";
//  else if (filename.endsWith(".ico")) return "image/x-icon";
//  else if (filename.endsWith(".xml")) return "text/xml";
//  else if (filename.endsWith(".pdf")) return "application/x-pdf";
//  else if (filename.endsWith(".zip")) return "application/x-zip";
//  else if (filename.endsWith(".gz")) return "application/x-gzip";
//  return "text/plain";
//}
//
//bool handleFileRead(String path) {
//#ifdef DEBUG_WEBSERVER
//  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
//#endif // DEBUG_WEBSERVER
//  if (CONNECTION_LED >= 0) {
//    flashLED(CONNECTION_LED, 1, 25); // Show activity on LED
//  }
//  if (path.endsWith("/"))
//    path += "index.htm";
//  String contentType = getContentType(path);
//  String pathWithGz = path + ".gz";
//  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
//    if (SPIFFS.exists(pathWithGz))
//      path += ".gz";
//    File file = SPIFFS.open(path, "r");
//    size_t sent = server.streamFile(file, contentType);
//    //DBG_OUTPUT_PORT.printf("File %s exist\n", file.name());
//    file.close();
//    return true;
//  }
//#ifdef DEBUG_WEBSERVER
//  else
//    DBG_OUTPUT_PORT.printf("Cannot find %s\n", path.c_str());
//#endif // DEBUG_WEBSERVER
//  return false;
//}
//
//void handleFileUpload() {
//  if (server.uri() != "/edit") return;
//  HTTPUpload& upload = server.upload();
//  if (upload.status == UPLOAD_FILE_START) {
//    String filename = upload.filename;
//    if (!filename.startsWith("/")) filename = "/" + filename;
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.printf("handleFileUpload Name: %s\n", filename.c_str());
//#endif // DEBUG_WEBSERVER
//    fsUploadFile = SPIFFS.open(filename, "w");
//    filename = String();
//  }
//  else if (upload.status == UPLOAD_FILE_WRITE) {
//    //DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
//    if (fsUploadFile)
//      fsUploadFile.write(upload.buf, upload.currentSize);
//  }
//  else if (upload.status == UPLOAD_FILE_END) {
//    if (fsUploadFile)
//      fsUploadFile.close();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.printf("handleFileUpload Size: %u\n", upload.totalSize);
//#endif // DEBUG_WEBSERVER
//  }
//}
//
//void handleFileDelete() {
//  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
//  String path = server.arg(0);
//#ifdef DEBUG_WEBSERVER
//  DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
//#endif // DEBUG_WEBSERVER
//  if (path == "/")
//    return server.send(500, "text/plain", "BAD PATH");
//  if (!SPIFFS.exists(path))
//    return server.send(404, "text/plain", "FileNotFound");
//  SPIFFS.remove(path);
//  server.send(200, "text/plain", "");
//  path = String();
//}
//
//void handleFileCreate() {
//  if (server.args() == 0)
//    return server.send(500, "text/plain", "BAD ARGS");
//  String path = server.arg(0);
//#ifdef DEBUG_WEBSERVER
//  DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
//#endif // DEBUG_WEBSERVER
//  if (path == "/")
//    return server.send(500, "text/plain", "BAD PATH");
//  if (SPIFFS.exists(path))
//    return server.send(500, "text/plain", "FILE EXISTS");
//  File file = SPIFFS.open(path, "w");
//  if (file)
//    file.close();
//  else
//    return server.send(500, "text/plain", "CREATE FAILED");
//  server.send(200, "text/plain", "");
//  path = String();
//}

//void handleFileList() {
//  if (!server.hasArg("dir")) {
//    server.send(500, "text/plain", "BAD ARGS");
//    return;
//  }

//  String path = server.arg("dir");
//#ifdef DEBUG_WEBSERVER
//  DBG_OUTPUT_PORT.println("handleFileList: " + path);
//#endif // DEBUG_WEBSERVER
//  Dir dir = SPIFFS.openDir(path);
//  path = String();
//
//  String output = "[";
//  while (dir.next()) {
//    File entry = dir.openFile("r");
//    if (true)//entry.name()!="secret.json") // Do not show secrets
//    {
//      if (output != "[")
//        output += ',';
//      bool isDir = false;
//      output += "{\"type\":\"";
//      output += (isDir) ? "dir" : "file";
//      output += "\",\"name\":\"";
//      output += String(entry.name()).substring(1);
//      output += "\"}";
//    }
//    entry.close();
//  }
//
//  output += "]";
//#ifdef DEBUG_WEBSERVER
//  DBG_OUTPUT_PORT.println(output);
//#endif // DEBUG_WEBSERVER
//  server.send(200, "text/json", output);
//}

//void setUpdateMD5() {
//  browserMD5 = "";
//#ifdef DEBUG_WEBSERVER
//  DBG_OUTPUT_PORT.printf("Arg number: %d\n", server.args());
//#endif // DEBUG_WEBSERVER
//  if (server.args() > 0)  // Read hash
//  {
//    //String temp = "";
//    for (uint8_t i = 0; i < server.args(); i++) {
//#ifdef DEBUG_WEBSERVER
//      DBG_OUTPUT_PORT.printf("Arg %s: %s\n", server.argName(i).c_str(), server.arg(i).c_str());
//#endif // DEBUG_WEBSERVER
//      if (server.argName(i) == "md5") {
//        browserMD5 = urldecode(server.arg(i));
//        Update.setMD5(browserMD5.c_str());
//        continue;
//      } if (server.argName(i) == "size") {
//        updateSize = server.arg(i).toInt();
//#ifdef DEBUG_WEBSERVER
//        DBG_OUTPUT_PORT.printf("Update size: %u\n", server.arg(i).toInt());
//#endif // DEBUG_WEBSERVER
//        continue;
//      }
//    }
//    server.send(200, "text/html", "OK --> MD5: " + browserMD5);
//  }
//
//}
//
//void updateFirmware () {
//  // handler for the file upload, get's the sketch bytes, and writes
//  // them through the Update object
//  HTTPUpload& upload = server.upload();
//  if (upload.status == UPLOAD_FILE_START) {
//    WiFiUDP::stopAll();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.printf("Update: %s\n", upload.filename.c_str());
//#endif // DEBUG_WEBSERVER
//    //uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
//    uint32_t maxSketchSpace = ESP.getSketchSize();
//#ifdef DEBUG_WEBSERVER
//    //uint32_t oldValue = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
//    DBG_OUTPUT_PORT.printf("Max free scketch space: %u\n", maxSketchSpace);
//    DBG_OUTPUT_PORT.printf("New scketch size: %u\n", updateSize);
//    //DBG_OUTPUT_PORT.printf("Old value: %u\n", oldValue);
//#endif // DEBUG_WEBSERVER
//    if (browserMD5 != NULL && browserMD5 != "") {
//      Update.setMD5(browserMD5.c_str());
//#ifdef DEBUG_WEBSERVER
//      DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
//#endif // DEBUG_WEBSERVER
//    }
//    if (!Update.begin(updateSize)) {//start with max available size
//#ifdef DEBUG_WEBSERVER
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_WRITE) {
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.print(".");
//#endif // DEBUG_WEBSERVER
//    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//#ifdef DEBUG_WEBSERVER
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_END) {
//    String updateHash;
//    if (Update.end(true)) { //true to set the size to the current progress
//#ifdef DEBUG_WEBSERVER
//      updateHash = Update.md5String();
//      DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
//      DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//#endif // DEBUG_WEBSERVER
//    }
//    else {
//#ifdef DEBUG_WEBSERVER
//      updateHash = Update.md5String();
//      DBG_OUTPUT_PORT.printf("Upload failed. Calculated MD5: %s\n", updateHash.c_str());
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_ABORTED) {
//    Update.end();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.println("Update was aborted");
//#endif // DEBUG_WEBSERVER
//  }
//  delay(2);
//}

void serverInit() {

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient * client) {
    client->send("hello!", NULL, millis(), 1000);
  });
  server.addHandler(&events);

  server.addHandler(new SPIFFSEditor(http_username, http_password));
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });

  server.onNotFound([](AsyncWebServerRequest * request) {
    os_printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
      os_printf("GET");
    else if (request->method() == HTTP_POST)
      os_printf("POST");
    else if (request->method() == HTTP_DELETE)
      os_printf("DELETE");
    else if (request->method() == HTTP_PUT)
      os_printf("PUT");
    else if (request->method() == HTTP_PATCH)
      os_printf("PATCH");
    else if (request->method() == HTTP_HEAD)
      os_printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      os_printf("OPTIONS");
    else
      os_printf("UNKNOWN");
    os_printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if (request->contentLength()) {
      os_printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      os_printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for (i = 0; i < headers; i++) {
      AsyncWebHeader* h = request->getHeader(i);
      os_printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for (i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile()) {
        os_printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if (p->isPost()) {
        os_printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        os_printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index)
      os_printf("UploadStart: %s\n", filename.c_str());
    os_printf("%s", (const char*)data);
    if (final)
      os_printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
  });
  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    if (!index)
      os_printf("BodyStart: %u\n", total);
    os_printf("%s", (const char*)data);
    if (index + len == total)
      os_printf("BodyEnd: %u\n", total);
  });


  //SERVER INIT
  //list directory
  //  server.on("/list", HTTP_GET, []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    handleFileList();
  //  });
  //  //load editor
  //  server.on("/edit", HTTP_GET, []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    if (!handleFileRead("/edit.html"))
  //      server.send(404, "text/plain", "FileNotFound");
  //  });
  //  //create file
  //  server.on("/edit", HTTP_PUT, []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    handleFileCreate();
  //  });
  //  //delete file
  //  server.on("/edit", HTTP_DELETE, []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    handleFileDelete();
  //  });
  //  //first callback is called after the request has ended with all parsed arguments
  //  //second callback handles file uploads at that location
  //  server.on("/edit", HTTP_POST, []() {
  //    server.send(200, "text/plain", "");
  //  }, handleFileUpload);
  //

  //server.on("/admin/generalvalues", send_general_configuration_values_html);

  server.on("/admin/generalvalues", HTTP_GET, [](AsyncWebServerRequest * request) {

    String values = "";
    values += "devicename|" + (String)config.DeviceName + "|input\n";
    //  server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

    request->send(200, "text/plain", values);
  });



  //  server.on("/admin/values", send_network_configuration_values_html);
  server.on("/admin/connectionstate", HTTP_GET, [](AsyncWebServerRequest * request) {

    String values = "";

    values += "ssid|" + (String)config.ssid + "|input\n";
    values += "password|" + (String)config.password + "|input\n";
    values += "ip_0|" + (String)config.IP[0] + "|input\n";
    values += "ip_1|" + (String)config.IP[1] + "|input\n";
    values += "ip_2|" + (String)config.IP[2] + "|input\n";
    values += "ip_3|" + (String)config.IP[3] + "|input\n";
    values += "nm_0|" + (String)config.Netmask[0] + "|input\n";
    values += "nm_1|" + (String)config.Netmask[1] + "|input\n";
    values += "nm_2|" + (String)config.Netmask[2] + "|input\n";
    values += "nm_3|" + (String)config.Netmask[3] + "|input\n";
    values += "gw_0|" + (String)config.Gateway[0] + "|input\n";
    values += "gw_1|" + (String)config.Gateway[1] + "|input\n";
    values += "gw_2|" + (String)config.Gateway[2] + "|input\n";
    values += "gw_3|" + (String)config.Gateway[3] + "|input\n";
    values += "dns_0|" + (String)config.DNS[0] + "|input\n";
    values += "dns_1|" + (String)config.DNS[1] + "|input\n";
    values += "dns_2|" + (String)config.DNS[2] + "|input\n";
    values += "dns_3|" + (String)config.DNS[3] + "|input\n";
    values += "dhcp|" + (String)(config.dhcp ? "checked" : "") + "|chk\n";

    //  server.send(200, "text/plain", values);
    request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

    request->send(200, "text/plain", values);
  });

  //  server.on("/admin/connectionstate", send_connection_state_values_html);
  server.on("/admin/connectionstate", HTTP_GET, [](AsyncWebServerRequest * request) {
    String state = "N/A";
    String Networks = "";
    if (WiFi.status() == 0) state = "Idle";
    else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
    else if (WiFi.status() == 2) state = "SCAN COMPLETED";
    else if (WiFi.status() == 3) state = "CONNECTED";
    else if (WiFi.status() == 4) state = "CONNECT FAILED";
    else if (WiFi.status() == 5) state = "CONNECTION LOST";
    else if (WiFi.status() == 6) state = "DISCONNECTED";



    int n = WiFi.scanNetworks();

    if (n == 0)
    {
      Networks = "<font color='#FF0000'>No networks found!</font>";
    }
    else
    {


      Networks = "Found " + String(n) + " Networks<br>";
      Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
      Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
      for (int i = 0; i < n; ++i)
      {
        int quality = 0;
        if (WiFi.RSSI(i) <= -100)
        {
          quality = 0;
        }
        else if (WiFi.RSSI(i) >= -50)
        {
          quality = 100;
        }
        else
        {
          quality = 2 * (WiFi.RSSI(i) + 100);
        }


        Networks += "<tr><td><a href='javascript:selssid(\"" + String(WiFi.SSID(i)) + "\")'>" + String(WiFi.SSID(i)) + "</a></td><td>" + String(quality) + "%</td><td>" + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*") + "</td></tr>";
      }
      Networks += "</table>";
    }

    String values = "";
    values += "connectionstate|" + state + "|div\n";
    values += "networks|" + Networks + "|div\n";
    //  server.send(200, "text/plain", values);
    request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
  });


  //  server.on("/admin/infovalues", send_information_values_html);
  server.on("/admin/infovalues", HTTP_GET, [](AsyncWebServerRequest * request) {
    String values = "";
    //
    values += "x_ssid|" + (String)WiFi.SSID() + "|div\n";
    values += "x_ip|" + (String)WiFi.localIP()[0] + "." + (String)WiFi.localIP()[1] + "." + (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3] + "|div\n";
    values += "x_gateway|" + (String)WiFi.gatewayIP()[0] + "." + (String)WiFi.gatewayIP()[1] + "." + (String)WiFi.gatewayIP()[2] + "." + (String)WiFi.gatewayIP()[3] + "|div\n";
    values += "x_netmask|" + (String)WiFi.subnetMask()[0] + "." + (String)WiFi.subnetMask()[1] + "." + (String)WiFi.subnetMask()[2] + "." + (String)WiFi.subnetMask()[3] + "|div\n";
    values += "x_mac|" + GetMacAddress() + "|div\n";
    values += "x_dns|" + (String)WiFi.dnsIP()[0] + "." + (String)WiFi.dnsIP()[1] + "." + (String)WiFi.dnsIP()[2] + "." + (String)WiFi.dnsIP()[3] + "|div\n";
    values += "x_ntp_sync|" + ntp->getTimeString(ntp->getLastNTPSync()) + "|div\n";
    values += "x_ntp_time|" + ntp->getTimeStr() + "|div\n";
    values += "x_ntp_date|" + ntp->getDateStr() + "|div\n";

    //  server.send(200, "text/plain", values);
    request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
  });



  //  server.on("/admin/ntpvalues", send_NTP_configuration_values_html);

  server.on("/admin/ntpvalues", HTTP_GET, [](AsyncWebServerRequest * request) {
    String values = "";
    values += "ntpserver|" + (String)config.ntpServerName + "|input\n";
    values += "update|" + (String)config.Update_Time_Via_NTP_Every + "|input\n";
    values += "tz|" + (String)config.timezone + "|input\n";
    values += "dst|" + (String)(config.daylight ? "checked" : "") + "|chk\n";
    //server.send(200, "text/plain", values);
    request->send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
  });

  server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest * request) {

    if (!checkAuth())
      return checkAuth();


#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA


    int args = request->args();
    if (args > 0)  // Save Settings
    {
      //String temp = "";
      bool oldDHCP = config.dhcp; // Save status to avoid general.html cleares it
      config.dhcp = false;
      // for (uint8_t i = 0; i < server.args(); i++) {

      for (int i = 0; i < args; i++) {
        // Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());



#ifdef DEBUG_DYNAMICDATA
        DBG_OUTPUT_PORT.printf("Arg %d: %s\n", i, args(i).c_str());
#endif // DEBUG_DYNAMICDATA

        if (request->hasArg("devicename"))
          config.DeviceName = request->arg("devicename");
        config.dhcp = oldDHCP;


        if (request->hasArg( "ssid")) {
          config.ssid = request->arg("ssid");
        }
        if (request->hasArg( "password")) {
          config.password = request->arg("devicename");
        }
        // if (request->hasArg( "ip_0")) {  if (checkRange(server.arg(i)))  config.IP[0] = server.arg(i).toInt();  }
        if (request->hasArg( "ip_0")) {
          config.IP[0] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "ip_1")) {
          config.IP[1] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "ip_2")) {
          config.IP[2] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "ip_3")) {
          config.IP[3] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "nm_0")) {
          config.Netmask[0] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "nm_1")) {
          config.Netmask[1] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "nm_2")) {
          config.Netmask[2] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "nm_3")) {
          config.Netmask[3] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "gw_0")) {
          config.Gateway[0] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "gw_1")) {
          config.Gateway[1] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "gw_2")) {
          config.Gateway[2] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "gw_3")) {
          config.Gateway[3] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "dns_0")) {
          config.DNS[0] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "dns_1")) {
          config.DNS[1] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "dns_2")) {
          config.DNS[2] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "dns_3")) {
          config.DNS[3] = request->arg("devicename").toInt();
        }
        if (request->hasArg( "dhcp")) {
          config.dhcp = true;
        }
      }
      save_config();
      //server.send(200, "text/html", Page_WaitAndReload);

      //server.send(200, "text/html", (Update.hasError()) ? "FAIL" : "<META http-equiv=\"refresh\" content=\"15;URL=/update\">Update correct. Restarting...");

      Serial.print("OOHHHHAAAAAAAAAAAAAA");
      //String gonderabi = String("http://")+ config.DeviceName.c_str()+String(".local");
      //server.send(200, "text/html", (Update.hasError()) ? "FAIL" : request->redirect("/"));
      //server.sendHeader("Location", gonderabi, true);
      //server.send ( 301, "text/plain", "");
      //     request->send(301, "text/plain", "");


      //request->send(200, "text/plain",  (Update.hasError()) ? "FAIL" : request->redirect("/"));

      request->redirect("/");
      yield();
      delay(3000);



      ESP.restart();
      //ConfigureWifi();
      //AdminTimeOutCounter = 0;
    }
    else
    {
      //   DBG_OUTPUT_PORT.println(server.uri());
      //   handleFileRead(server.uri());
    }
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

  });
  //  server.on("/general.html", []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    send_general_configuration_html();
  //  });

  server.on("/general.html", HTTP_GET, [](AsyncWebServerRequest * request) {
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
    int args = request->args();
    if (args > 0)  // Save Settings
    {
      for (int i = 0; i < args; i++) {
#ifdef DEBUG_DYNAMICDATA
        DBG_OUTPUT_PORT.printf("Arg %d: %s\n", i, server.arg(i).c_str());
#endif // DEBUG_DYNAMICDATA
        if (request->hasArg("devicename"))
          config.DeviceName = request->arg("devicename");
        continue;
      }
      request->send(200, "text/plain", "Yeniden başlıyor. . .");
      save_config();
      ESP.restart();
    }
    else
    {
      request->send(SPIFFS, "/general.html");  // handleFileRead(server.uri());
    }
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
  });

  //  server.on("/ntp.html", []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    send_NTP_configuration_html();
  //  });

  server.on("/ntp.html", HTTP_GET, [](AsyncWebServerRequest * request) {
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
    int args = request->args();
    if (args > 0)  // Save Settings
    { config.daylight = false;

      for (int i = 0; i < args; i++) {
#ifdef DEBUG_DYNAMICDATA
        DBG_OUTPUT_PORT.printf("Arg %d: %s\n", i, server.arg(i).c_str());
#endif // DEBUG_DYNAMICDATA
        if (request->hasArg("ntpserver")) {
          config.ntpServerName = request->arg("ntpserver");
        }

        if (request->hasArg("update")) {
          config.Update_Time_Via_NTP_Every = request->arg("update").toInt();;
        }
        if (request->hasArg("tz")) {
          config.timezone = request->arg("tz").toInt();;
          ntp->setTimeZone(config.timezone / 10);
        }
        if (request->hasArg("dst"))
          config.daylight = true;
#ifdef DEBUG_DYNAMICDATA
        DBG_OUTPUT_PORT.printf("Daylight Saving: %d\n", config.daylight);
#endif // DEBUG_DYNAMICDATA
      }

      ntp->setDayLight(config.daylight);
      save_config();
      //firstStart = true;

      setTime(ntp->getTime()); //set time
    }

#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

    request->send(SPIFFS, "/ntp.html");  //   handleFileRead("/ntp.html");
  });

  //  server.on("/admin", HTTP_GET, []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    if (!handleFileRead("/admin.html"))
  //      server.send(404, "text/plain", "FileNotFound");
  //  });

  server.on("/admin", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
    request->send(SPIFFS, "/admin.html");
  });
  //  server.on("/admin/restart", []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    restart_esp();
  //  });
  server.on("/admin/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
    restart_esp();
  });

  //  server.on("/system.html", []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    send_wwwauth_configuration_html();
  //  });
  server.on("/system.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.printf("%s %d\n", __FUNCTION__, server.args());
#endif // DEBUG_DYNAMICDATA
    int args = request->args();
    if (args > 0)  {// Save Settings
      httpAuth.auth = false;
      //String temp = "";

      config.DeviceName = request->arg("devicename");


      if (request->arg("wwwuser") == "wwwuser") {
        httpAuth.wwwUsername = request->arg("wwwuser") ;
#ifdef DEBUG_DYNAMICDATA
        DBG_OUTPUT_PORT.printf("User: %s\n", httpAuth.wwwUsername.c_str());
#endif // DEBUG_DYNAMICDATA
      }
      if (request->arg("wwwpass")  == "wwwpass") {
        httpAuth.wwwPassword = request->arg("wwwpass") ;
#ifdef DEBUG_DYNAMICDATA
        DBG_OUTPUT_PORT.printf("Pass: %s\n", httpAuth.wwwPassword.c_str());
#endif // DEBUG_DYNAMICDATA
      }
      if (request->arg("wwwauth")  == "wwwauth") {
        httpAuth.auth = true;
#ifdef DEBUG_DYNAMICDATA
        DBG_OUTPUT_PORT.printf("HTTP Auth enabled\n");
#endif // DEBUG_DYNAMICDATA
      }

      saveHTTPAuth();
    }
    request->send(SPIFFS, "/system.html"); //handleFileRead("/system.html");
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

  });

  //  server.on("/admin/wwwauth", []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    send_wwwauth_configuration_values_html();
  //  });
  //  server.on("/update/updatepossible", []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    send_update_firmware_values_html();
  //  });
  server.on("/admin/wwwauth", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
    String values = "";

    values += "wwwauth|" + (String)(httpAuth.auth ? "checked" : "") + "|chk\n";
    values += "wwwuser|" + (String)httpAuth.wwwUsername + "|input\n";
    values += "wwwpass|" + (String)httpAuth.wwwPassword + "|input\n";

    //server.send(200, "text/plain", values);
    request->send(200, "text/plain", values);

#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

  });
  server.on("/update/updatepossible", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
    String values = "";
    uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
    //bool updateOK = Update.begin(maxSketchSpace);
    bool updateOK = maxSketchSpace < ESP.getFreeSketchSpace();
    StreamString result;
    Update.printError(result);
#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.printf("--MaxSketchSpace: %d\n", maxSketchSpace);
    DBG_OUTPUT_PORT.printf("--Update error = %s\n", result.c_str());
#endif // DEBUG_DYNAMICDATA
    values += "remupd|" + (String)((updateOK) ? "OK" : "ERROR") + "|div\n";

    if (Update.hasError()) {
      result.trim();
      values += "remupdResult|" + result + "|div\n";
    }
    else {
      values += "remupdResult||div\n";
    }

    //server.send(200, "text/plain", values);
    request->send(200, "text/plain", values);


#ifdef DEBUG_DYNAMICDATA
    DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
  });


  //  server.on("/setmd5", [&]() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    //DBG_OUTPUT_PORT.println("md5?");
  //    setUpdateMD5();
  //  });

  server.on("/setmd5", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
    browserMD5 = "";


    if (request->arg("md5") == "md5") {

      browserMD5 = urldecode(request->arg("md5"));
      Update.setMD5(browserMD5.c_str());
    } if (request->arg("size")  == "size") {
      updateSize = request->arg("size").toInt();

      Serial.print("ahahahahha bok " + updateSize);
#ifdef DEBUG_WEBSERVER
      DBG_OUTPUT_PORT.printf("Update size: %u\n", request->arg("size").toInt());
#endif // DEBUG_WEBSERVER
    }
    // server.send(200, "text/html", "OK --> MD5: " + browserMD5);
    request->send(200, "text/html", "OK --> MD5: " + browserMD5);


  });
  //  server.on("/update", HTTP_GET, []() {
  //    if (!checkAuth())
  //      return server.requestAuthentication();
  //    if (!handleFileRead("/update.html"))
  //      server.send(404, "text/plain", "FileNotFound");
  //  });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
    request->send(SPIFFS, "/update.html");
  });


  //server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
  //    request->send(200);
  //  }, handleUpload);


//
//  server.on("/update", HTTP_POST, [&]() {
//    if (!checkAuth())
//      return server.requestAuthentication();
//    server.sendHeader("Connection", "close");
//    server.sendHeader("Access-Control-Allow-Origin", "*");
//    String gonderabi = String("http://") + config.DeviceName.c_str() + String(".local");
//    server.sendHeader("Location", gonderabi, true);
//    server.send ( 301, "text/plain", "");
//    server.send(200, "text/html", (Update.hasError()) ? "FAIL" : String("<META http-equiv=\"refresh\" content=\"15;URL=/\">Ayarlar kaydediliyor... http://") + gonderabi );
//
//    delay(50);
//    //  server.send(200, "text/html", (Update.hasError()) ? "FAIL" : "<META http-equiv=\"refresh\" content=\"15;URL=/update\">Update correct. Restarting...");
//    ESP.restart();
//  }, updateFirmware);





  
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {

     if (!checkAuth())
      return checkAuth();


    AsyncWebServerResponse *response = request->beginResponse(200); //Sends 404 File Not Found
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);


    request->redirect("/");

    //  server.send(200, "text/html", (Update.hasError()) ? "FAIL" : "<META http-equiv=\"refresh\" content=\"15;URL=/update\">Update correct. Restarting...");
  ESP.restart();
  }, handleUploadme);

  //called when the url is not defined here
  //use it to load content from SPIFFS
  //    server.onNotFound( []() {
  //      if (!checkAuth())
  //        return server.requestAuthentication();
  //      server.sendHeader("Connection", "close");
  //      server.sendHeader("Access-Control-Allow-Origin", "*");
  //      if (!handleFileRead(server.uri()))
  //        server.send(404, "text/plain", "FileNotFound");
  //    });

#ifndef HIDE_SECRET
  server.on(SECRET_FILE, HTTP_GET, [](AsyncWebServerRequest * request) {
    //server.on(SECRET_FILE, HTTP_GET, []() {
    if (!checkAuth())
      return checkAuth();
    AsyncWebServerResponse *response = request->beginResponse(403); //Sends 404 File Not Found
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(200, "text/plain", "Yasak!");
    request->send(response);
  });
#endif // HIDE_SECRET

  //  #ifndef HIDE_CONFIG
  //    server.on(CONFIG_FILE, HTTP_GET, []() {
  //      if (!checkAuth())
  //        return server.requestAuthentication();
  //      server.sendHeader("Connection", "close");
  //      server.sendHeader("Access-Control-Allow-Origin", "*");
  //      server.send(403, "text/plain", "Forbidden");
  //    });
  //  #endif // HIDE_CONFIG

#ifndef HIDE_CONFIG
  server.on(CONFIG_FILE, HTTP_GET, [](AsyncWebServerRequest * request) {
    if (!checkAuth())
      return checkAuth();
    AsyncWebServerResponse *response = request->beginResponse(403); //Sends 404 File Not Found
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(200, "text/plain", "Yasak!");
    request->send(response);
  });
#endif // HIDE_CONFIG




  server.begin();


};



//  server.on("/update", HTTP_POST, [&]() {
//    if (!checkAuth())
//      return server.requestAuthentication();
//    server.sendHeader("Connection", "close");
//    server.sendHeader("Access-Control-Allow-Origin", "*");
//    String gonderabi = String("http://")+ config.DeviceName.c_str()+String(".local");
//    server.sendHeader("Location", gonderabi, true);
//    server.send ( 301, "text/plain", "");
//    server.send(200, "text/html", (Update.hasError()) ? "FAIL" : String("<META http-equiv=\"refresh\" content=\"15;URL=/\">Ayarlar kaydediliyor... http://")+gonderabi );
//
//    delay(50);
//    //	server.send(200, "text/html", (Update.hasError()) ? "FAIL" : "<META http-equiv=\"refresh\" content=\"15;URL=/update\">Update correct. Restarting...");
//    ESP.restart();
//  }, updateFirmware);
//
//  //called when the url is not defined here
//  //use it to load content from SPIFFS
//  server.onNotFound( []() {
//    if (!checkAuth())
//      return server.requestAuthentication();
//    server.sendHeader("Connection", "close");
//    server.sendHeader("Access-Control-Allow-Origin", "*");
//    if (!handleFileRead(server.uri()))
//      server.send(404, "text/plain", "FileNotFound");
//  });
//
//#ifndef HIDE_SECRET
//  server.on(SECRET_FILE, HTTP_GET, []() {
//    if (!checkAuth())
//      return server.requestAuthentication();
//    server.sendHeader("Connection", "close");
//    server.sendHeader("Access-Control-Allow-Origin", "*");
//    server.send(403, "text/plain", "Forbidden");
//  });
//#endif // HIDE_SECRET
//
//#ifndef HIDE_CONFIG
//  server.on(CONFIG_FILE, HTTP_GET, []() {
//    if (!checkAuth())
//      return server.requestAuthentication();
//    server.sendHeader("Connection", "close");
//    server.sendHeader("Access-Control-Allow-Origin", "*");
//    server.send(403, "text/plain", "Forbidden");
//  });
//#endif // HIDE_CONFIG
//
//  //get heap status, analog input value and all GPIO statuses in one json call
//  server.on("/all", HTTP_GET, []() {
//    String json = "{";
//    json += "\"heap\":" + String(ESP.getFreeHeap());
//    json += ", \"analog\":" + String(analogRead(A0));
//    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
//    json += "}";
//    server.send(200, "text/json", json);
//    json = String();
//  });
//  server.begin();
//  //httpUpdater.setup(&server,httpAuth.wwwUsername,httpAuth.wwwPassword);
//#ifdef DEBUG_WEBSERVER
//  DBG_OUTPUT_PORT.println("HTTP server started");
//#endif // DEBUG_WEBSERVER




//void updateFirmware () {
//  // handler for the file upload, get's the sketch bytes, and writes
//  // them through the Update object
//  HTTPUpload& upload = server.upload();
//  if (upload.status == UPLOAD_FILE_START) {
//    WiFiUDP::stopAll();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.printf("Update: %s\n", upload.filename.c_str());
//#endif // DEBUG_WEBSERVER
//    //uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
//    uint32_t maxSketchSpace = ESP.getSketchSize();
//#ifdef DEBUG_WEBSERVER
//    //uint32_t oldValue = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
//    DBG_OUTPUT_PORT.printf("Max free scketch space: %u\n", maxSketchSpace);
//    DBG_OUTPUT_PORT.printf("New scketch size: %u\n", updateSize);
//    //DBG_OUTPUT_PORT.printf("Old value: %u\n", oldValue);
//#endif // DEBUG_WEBSERVER
//    if (browserMD5 != NULL && browserMD5 != "") {
//      Update.setMD5(browserMD5.c_str());
//#ifdef DEBUG_WEBSERVER
//      DBG_OUTPUT_PORT.printf("Hash from client: %s\n", browserMD5.c_str());
//#endif // DEBUG_WEBSERVER
//    }
//    if (!Update.begin(updateSize)) {//start with max available size
//#ifdef DEBUG_WEBSERVER
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_WRITE) {
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.print(".");
//#endif // DEBUG_WEBSERVER
//    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//#ifdef DEBUG_WEBSERVER
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_END) {
//    String updateHash;
//    if (Update.end(true)) { //true to set the size to the current progress
//#ifdef DEBUG_WEBSERVER
//      updateHash = Update.md5String();
//      DBG_OUTPUT_PORT.printf("Upload finished. Calculated MD5: %s\n", updateHash.c_str());
//      DBG_OUTPUT_PORT.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//#endif // DEBUG_WEBSERVER
//    }
//    else {
//#ifdef DEBUG_WEBSERVER
//      updateHash = Update.md5String();
//      DBG_OUTPUT_PORT.printf("Upload failed. Calculated MD5: %s\n", updateHash.c_str());
//      Update.printError(DBG_OUTPUT_PORT);
//#endif // DEBUG_WEBSERVER
//    }
//  }
//  else if (upload.status == UPLOAD_FILE_ABORTED) {
//    Update.end();
//#ifdef DEBUG_WEBSERVER
//    DBG_OUTPUT_PORT.println("Update was aborted");
//#endif // DEBUG_WEBSERVER
//  }
//  delay(2);
//}


boolean checkAuth() {
  if (!httpAuth.auth) {
    return true;
  }
  else
  {
    server.on("/login", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (!request->authenticate(http_username, http_password))
        return request->requestAuthentication();
      request->send(200, "text/plain", "Login Success!");
    });
    //  return server.authenticate(httpAuth.wwwUsername.c_str(), httpAuth.wwwPassword.c_str());
  }




  //  if (!httpAuth.auth) {
  //    return true;
  //  }
  //  else
  //  {
  //    return server.authenticate(httpAuth.wwwUsername.c_str(), httpAuth.wwwPassword.c_str());
  //  }

}
