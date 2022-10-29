#include <EEPROM.h>
#include <ESP32Time.h>
#include <totime.h>
#include <otadrive_esp.h>
#include <toconfig.h>
#include <toflash.h>
#include <toupdate.h>
#include <toconnect.h>
#include <WiFi.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

bool auto_update;

int last_question;
extern ESP32Time  rtc;

//WebServer webserver(60);

char url_update[MAX_URL_KEY_LEN];
char key_update[MAX_URL_KEY_LEN];

EEPROMClass f_url_update("urlupdate", MAX_URL_KEY_LEN);
EEPROMClass f_key_update("keyupdate", MAX_URL_KEY_LEN);

extern int connect_mode;

// const char* host = "bloom";

// const char* loginIndex =
//  "<form name='loginForm'>"
//     "<table width='20%' bgcolor='A09F9F' align='center'>"
//         "<tr>"
//             "<td colspan=2>"
//                 "<center><font size=4><b>BLOOM Login Page</b></font></center>"
//                 "<br>"
//             "</td>"
//             "<br>"
//             "<br>"
//         "</tr>"
//         "<tr>"
//              "<td>Username:</td>"
//              "<td><input type='text' size=25 name='userid'><br></td>"
//         "</tr>"
//         "<br>"
//         "<br>"
//         "<tr>"
//             "<td>Password:</td>"
//             "<td><input type='Password' size=25 name='pwd'><br></td>"
//             "<br>"
//             "<br>"
//         "</tr>"
//         "<tr>"
//             "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
//         "</tr>"
//     "</table>"
// "</form>"
// "<script>"
//     "function check(form)"
//     "{"
//     "if(form.userid.value=='admin' && form.pwd.value=='admin')"
//     "{"
//     "window.open('/serverIndex')"
//     "}"
//     "else"
//     "{"
//     " alert('Error Password or Username')/*displays error message*/"
//     "}"
//     "}"
// "</script>";

// const char* serverIndex =
// "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
// "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
//    "<input type='file' name='update'>"
//         "<input type='submit' value='Update'>"
//     "</form>"
//  "<div id='prg'>progress: 0%</div>"
//  "<script>"
//   "$('form').submit(function(e){"
//   "e.preventDefault();"
//   "var form = $('#upload_form')[0];"
//   "var data = new FormData(form);"
//   " $.ajax({"
//   "url: '/update',"
//   "type: 'POST',"
//   "data: data,"
//   "contentType: false,"
//   "processData:false,"
//   "xhr: function() {"
//   "var xhr = new window.XMLHttpRequest();"
//   "xhr.upload.addEventListener('progress', function(evt) {"
//   "if (evt.lengthComputable) {"
//   "var per = evt.loaded / evt.total;"
//   "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
//   "}"
//   "}, false);"
//   "return xhr;"
//   "},"
//   "success:function(d, s) {"
//   "console.log('success!')"
//  "},"
//  "error: function (a, b, c) {"
//  "}"
//  "});"
//  "});"
//  "</script>";


void toUpdate_Init(void)
{
    f_url_update.get(0, url_update);
    f_key_update.get(0, key_update);
    auto_update = EEPROM.read(ADDRESS_AUTO_UPDATE);
    Serial.print("url  update: "); Serial.println(url_update);
    Serial.print("ket  update: "); Serial.println(key_update);
    Serial.print("auto update: "); Serial.println(auto_update?"enable":"disable");
    OTADRIVE.setInfo(String(key_update), DEFAULT_DEVICE_VERSION);
}
void toUpdate_Update(unsigned int a_toalarm)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if(a_toalarm > 15)
        {
            int n_min_time = toTime_SecToMin(rtc.getEpoch());
            if (n_min_time - last_question > 120)
            {
                last_question = n_min_time;
                updateInfo inf = OTADRIVE.updateFirmwareInfo();
                if (inf.available)
                {
                    Serial.printf("\nNew version available: %s, %dBytes\n",inf.version.c_str(), inf.size);
                    OTADRIVE.updateFirmware();
                }
                else
                {
                    Serial.println("No newer version");
                }
                //OTADRIVE.syncResources();
                //String c = OTADRIVE.getConfigs();
                //Serial.printf("\nconfiguration: %s\n", c.c_str());
            }
        }
    }
}
void toUpdate_NewKey(const char * keyurl)
{
    memset(key_update, '\0', sizeof(key_update));
    strcpy(key_update, keyurl);
    f_key_update.writeString(0, key_update); f_key_update.commit();
}

void toUpdate_OtaServer_Init(void)
{
  // if (!MDNS.begin(host)) {
  //   Serial.println("Error setting up MDNS responder!");
  //   while (1) {
  //     delay(1000);
  //   }
  // }
  // Serial.println("mDNS responder started");
  // webserver.on("/", HTTP_GET, []() {
  //   webserver.sendHeader("Connection", "close");
  //   webserver.send(200, "text/html", loginIndex);
  // });
  // webserver.on("/serverIndex", HTTP_GET, []() {
  //   webserver.sendHeader("Connection", "close");
  //   webserver.send(200, "text/html", serverIndex);
  // });
  // webserver.on("/update", HTTP_POST, []() {
  //   webserver.sendHeader("Connection", "close");
  //   webserver.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  //   ESP.restart();
  // }, []() {
  //   HTTPUpload& upload = webserver.upload();
  //   if (upload.status == UPLOAD_FILE_START) {
  //     Serial.printf("Update: %s\n", upload.filename.c_str());
  //     if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
  //       Update.printError(Serial);
  //     }
  //   } else if (upload.status == UPLOAD_FILE_WRITE) {
  //     if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
  //       Update.printError(Serial);
  //     }
  //   } else if (upload.status == UPLOAD_FILE_END) {
  //     if (Update.end(true)) {
  //       Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
  //     } else {
  //       Update.printError(Serial);
  //     }
  //   }
  // });
  // webserver.begin();
}
void toUpdate_OtoServer_loop(void)
{
  // webserver.handleClient();
  // delay(1);
}
void toUpdate_Oto_Init(void)
{
  // ArduinoOTA
  //   .onStart([]() {
  //     String type;
  //     if (ArduinoOTA.getCommand() == U_FLASH)
  //       type = "sketch";
  //     else // U_SPIFFS
  //       type = "filesystem";

  //     // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
  //     Serial.println("Start updating " + type);
  //   })
  //   .onEnd([]() {
  //     Serial.println("\nEnd");
  //   })
  //   .onProgress([](unsigned int progress, unsigned int total) {
  //     Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  //   })
  //   .onError([](ota_error_t error) {
  //     Serial.printf("Error[%u]: ", error);
  //     if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
  //     else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
  //     else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
  //     else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
  //     else if (error == OTA_END_ERROR) Serial.println("End Failed");
  //   });
  //   ArduinoOTA.begin();
}
void toUpdate_Oto_loop(void)
{
    // ArduinoOTA.handle();
}