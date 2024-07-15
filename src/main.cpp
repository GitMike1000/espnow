#include <Arduino.h>
#include "main.h"
#include "EspNow.h"

// uint8_t addr1_data[] = {0x34, 0x85, 0x18, 0x42, 0xe4, 0x7c};
// uint8_t addr2_data[] = {0x34, 0x85, 0x18, 0x43, 0x06, 0x60};

// MacAddress addr1(addr1_data);
// MacAddress addr2(addr2_data);

Peer peer;
JsonDocument jd;

void message_serialize(JsonDocument msg);

void getThisMacAddress()
{
    uint8_t baseMac[6];
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
    if (ret == ESP_OK)
    {
        Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                      baseMac[0], baseMac[1], baseMac[2],
                      baseMac[3], baseMac[4], baseMac[5]);
    }
    else
    {
        Serial.printf("Failed to read MAC address\n");
    }
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    // peer[0].addPeer("11:11:11:11:11:11", 0, false);
    // peer[1].addPeer("22:22:22:22:22:22", 0, false);

    // peer[1].getPeerInfo();
    // peer[2].getPeerInfo();

    // esp_now_send()
    // esp_now_register_send_cb(OnDataSent1);

    // WiFi.mode(WIFI_STA);
    setId("esp001");
    Serial.println("s1");
    peer.peerInit("esp002");
    Serial.println("s2");
    // peer.peerInit("esp003");
    peer.setOnReceive(&message_serialize, "success");


    // Set device as a Wi-Fi Station
    // WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    // if (esp_now_init() != ESP_OK)
    // {
    //     Serial.println("Error initializing ESP-NOW");
    //     return;
    // }

    // peer[0] = new Peer(addr1.getMacAddress(), 0, 0);
    // peer[1] = new Peer(addr2.getMacAddress(), 1, 0);

    // peer[0]->ESPNowInit();
    // peer[1]->ESPNowInit();

    // peer[0]->addDictionaryToJSON("carID", "Peer 0");
    // peer[0]->addDictionaryToJSON("command", "f");
    // peer[0]->addDictionaryToJSON("startNode", "0-r-0");
    // peer[0]->addDictionaryToJSON("endNode", "1-r-0");
    // peer[0]->addDictionaryToJSON("Type", "Test");

    // peer[1]->addDictionaryToJSON("Name", "Peer 1");
    // peer[1]->addDictionaryToJSON("Type", "Switch");

    // Print peer information
    // peer[1]->getPeerInfo();
    // peer[0]->getPeerInfo();
}

void loop()
{
    // peer[1]->getPeerInfo();
    // peer[0]->getPeerInfo();
    Serial.printf("%s\n", macAddrHelper.getMacAddress());
    // getThisMacAddress();
    delay(2000);

    jd["Name"] =  "from esp001";
    jd["Age"] =  22;
    jd["CPI"] =  9.7;
    jd["type"] =  "success";
    peer.send(jd);


    // strcpy(myData.a, "THIS IS A Sender 0");
    // myData.b = random(1, 20);
    // myData.c = 2.4;
    // myData.d = true;
    // esp_err_t result = esp_now_send(peer[1]->getPeerMacAddress(), (uint8_t *)&myData, sizeof(myData));
    // if (result == ESP_OK)
    // {
    //     Serial.println("Sent with success");
    // }
    // else
    // {
    //     Serial.println("Error sending the data");
    // }
    // 34:85:18:42:e4:7c
    // 34:85:18:43:06:60
}

void message_serialize(JsonDocument msg)
{
  //Message is the myData recieved from Peer it contains myData insidee it.
//   Serial.println("myData is : " + JSON.stringify(msg));
    string str_msg;
    // serializeJson(msg, str_msg);
    serializeJson(msg, Serial);
}



