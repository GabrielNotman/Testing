 /**
 * @example TCPClientSingle.ino
 * @brief The TCPClientSingle demo of library WeeESP8266. 
 * @author Wu Pengfei<pengfei.wu@itead.cc> 
 * @date 2015.02
 * 
 * @par Copyright:
 * Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version. \n\n
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "ESP8266.h"

#define SSID        "SKY2CA87"
#define PASSWORD    "YRERBXRP"

#define HOST_NAME   "api.thingspeak.com"
#define HOST_PORT   80

#define API_KEY "V4IK25491GSHJJTP"
#define NEW_LINE "\r\n"

ESP8266 wifi(Serial1);

void setup(void)
{
    Serial.begin(9600);
    Serial.println("setup begin");
    
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
    if (wifi.setOprToStationSoftAP()) {
        Serial.println("to station + softap ok");
    } else {
        Serial.println("to station + softap err");
    }
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.println("Join AP success");
        Serial.print("IP:");
        Serial.println( wifi.getLocalIP().c_str());       
    } else {
        Serial.println("Join AP failure");
    }
    
    if (wifi.disableMUX()) {
        Serial.println("single ok");
    } else {
        Serial.println("single err");
    }
    
    Serial.println("setup end");
}
 
void loop(void)
{
    uint8_t buffer[128] = {0};
    
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }
    
    String requestLine = String("GET /update?key=") + API_KEY + "&field1=73" + NEW_LINE + NEW_LINE;
    wifi.send((const uint8_t*)requestLine.c_str(), requestLine.length());
     
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.println("]");
    }
    
    if (wifi.releaseTCP()) {
        Serial.println("release tcp ok");
    } else {
        Serial.println("release tcp err");
    }
    delay(15000);
}

