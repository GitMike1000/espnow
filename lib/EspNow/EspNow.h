#ifndef ESPNOW_H_
#define ESPNOW_H_

#define MAX_PEERS 20

#include <main.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
// #include <string.h>
#include "MacAddress.h"
#include "JSONnow.h"

int peerIndex = 0;
MacAddress macAddrHelper;

string default_keys[] = {"carID", "command", "startNode", "endNode", "ctd_status", "ctd"};

typedef struct struct_message
{
    // send to peer
    int carID;
    char *command;
    char *startNode;
    char *endNode;

    // receive from peer
    char *ctdStatus;
    int ctd;

    // analysis
    int ctdAverage;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len);

void defaultPrintHandlerESPNow(JsonDocument msg)
{
    Serial.println("Data found");
    // Serial.print(msg["data"]);
    // if(msg["newLine"])   Serial.println();
}

class Peer
{
private:
    MacAddress mac;
    uint8_t channel;
    uint8_t encrypt;
    JSONnow json;
    JsonDocument parseJson;
    int handlerIndex = 0;
    // JsonDocument jsondoc;

    // esp_now_peer_info_t peerInfo;
    struct_message peerExeData; // peer execution data

public:
    JsonDocument peerHandleType;
    MacAddress peerAddress;
    void (*dataReceiveFunc[15])(JsonDocument);

    Peer() {}

    Peer(const uint8_t *_macAddr, uint8_t _channel, uint8_t _encrypt)
        : mac(_macAddr), channel(_channel), encrypt(_encrypt)
    {
        // memcpy(peerInfo.peer_addr, _macAddr, 6);
        // peerInfo.channel = _channel;
        // peerInfo.encrypt = _encrypt;
    }

    void setOnReceive(void (*func)(JsonDocument), string type = "default")
    {
        // peerHandleType[type] = handlerIndex;
        addDictionaryToJSON(type, handlerIndex);
        this->dataReceiveFunc[handlerIndex++] = func;
    }

    void ESPNowInit()
    {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        if (esp_now_init() == ESP_OK)
        {
            Serial.println("ESPNow Init Success");
        }
        else
        {
            Serial.println("ESPNow Init Failed");
            // Retry InitESPNow, add a counte and then restart?
            // InitESPNow();
            // or Simply Restart
            // ESP.restart();
        }
    }

    void peerInit(string peerId)
    {
        this->peerAddress.parseName(peerId);
        if (peerIndex == 0)
            ESPNowInit();
        addPeer();
        createPeer();
        esp_now_register_send_cb(OnDataSent);
        esp_now_register_recv_cb(onDataReceive);
        // setOnReceive(defaultPrintHandlerESPNow1,"print");
        setOnReceive(defaultPrintHandlerESPNow, "check");
    }

    void addPeer();

    void createPeer()
    {
        // esp_now_peer_info_t peerinfo;
        // memcpy(peerinfo.peer_addr, this->peerAddress.getMacAddress(), 6);
        // peerinfo.channel = 7;
        // peerinfo.encrypt = false;
        // Serial.printf("peer: %s\n", this->peerAddress.getMacAddress());
        // // Register the peer
        // if (esp_now_add_peer(&peerinfo) != ESP_OK)
        // {
        //     // Serial.println("Peer added");
        //     Serial.printf("Failed to add peer\n");
        //     return;
        // }
        // else
        // {
        //     Serial.printf("peer added successfully\n");
        // }

        esp_now_peer_info_t peerinfo;
        
        uint8_t *macAddress = const_cast<uint8_t *>(this->peerAddress.getMacAddress());
        if (macAddress[0] & 0x01)
        {
            macAddress[0] &= 0xFE; // Clear the least significant bit of the first byte
        }

        memcpy(peerinfo.peer_addr, macAddress, 6);
        peerinfo.channel = 0;     // Set to your desired channel, ensure it matches the sender
        peerinfo.encrypt = true; // Set to true if encryption is required

        // Print the MAC address for debugging
        Serial.printf("str mac: %s\n", macAddress);
        Serial.printf("Peer MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                      peerinfo.peer_addr[0], peerinfo.peer_addr[1], peerinfo.peer_addr[2],
                      peerinfo.peer_addr[3], peerinfo.peer_addr[4], peerinfo.peer_addr[5]);

        // Register the peer
        esp_err_t addPeerResult = esp_now_add_peer(&peerinfo);
        if (addPeerResult == ESP_OK)
        {
            Serial.println("Peer added successfully");
        }
        else if (addPeerResult == ESP_ERR_ESPNOW_NOT_INIT)
        {
            Serial.println("ESPNOW is not initialized");
        }
        else if (addPeerResult == ESP_ERR_ESPNOW_ARG)
        {
            Serial.println("Invalid argument");
        }
        else if (addPeerResult == ESP_ERR_ESPNOW_FULL)
        {
            Serial.println("Peer list is full");
        }
        else if (addPeerResult == ESP_ERR_ESPNOW_NO_MEM)
        {
            Serial.println("Out of memory");
        }
        else if (addPeerResult == ESP_ERR_ESPNOW_EXIST)
        {
            Serial.println("Peer already exists in the peer list");
        }
        else
        {
            Serial.println("Failed to add peer");
        }
    }

    void send(JsonDocument data)
    {
        if (!data.containsKey("type"))
            data["type"] = "default";
        // String dataString = JSON.stringify(data);
        string str_data;
        serializeJson(data, str_data);
        sendString(str_data);
    }

    void sendString(string dataString)
    {
        const char *dataConst = dataString.c_str();
        Serial.printf("c1 %s\n", dataConst);
        int dataSize = dataString.length() + 1;
        Serial.printf("c2 %d\n", dataSize);
        char dataArray[dataSize];
        memcpy(dataArray, dataConst, dataSize);
        Serial.printf("c3 %s\n", dataArray);
        Serial.printf("c4 %s\n", this->peerAddress.getMacAddress());
        esp_now_send(this->peerAddress.getMacAddress(), (uint8_t *)dataArray, dataSize);
    }

    void addDictionaryToJSON(const string &key, const string &value)
    {
        json.addDictionary(key, value);
    }
    void addDictionaryToJSON(const string &key, const int &value)
    {
        json.addDictionary(key, value);
    }

    void parseJSONToDictionary(const char *jsonvar)
    {
        parseJson = json.parseJSON(jsonvar);
        // peerExeData.carID = parseJson[default_keys[0]];
        // peerExeData.command = parseJson[default_keys[1]];
        // peerExeData.startNode = parseJson[default_keys[2]];
        // peerExeData.endNode = parseJson[default_keys[3]];

        // peerExeData.ctdStatus = parseJson[default_keys[4]];
        // peerExeData.ctd = parseJson[default_keys[5]];
    }

    void getPeerInfo() const
    {
        mac.printMacAddress();
        json.printJSON();
    }

    const uint8_t *getPeerMacAddress() const
    {
        return mac.getMacAddress();
    }
};

Peer *peerList[MAX_PEERS];
void Peer::addPeer()
{
    peerList[peerIndex++] = this;
}

Peer findPeer(string targetAddress)
{
    for (int i = 0; i < peerIndex; i++)
    {
        if (targetAddress == peerList[i]->peerAddress.getStrAddress())
        {
            return *peerList[i];
        }
    }
}

void setId(string id)
{
    // macAddrHelper.parseName(id);
    // const uint8_t* unicast_macAddress = macAddrHelper.getMacAddress();
    // Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
    //                   unicast_macAddress[0], unicast_macAddress[1], unicast_macAddress[2],
    //                   unicast_macAddress[3], unicast_macAddress[4], unicast_macAddress[5]);
    // Serial.println("i1");
    // esp_base_mac_addr_set(unicast_macAddress);
    // Serial.println("i1");

    macAddrHelper.parseName(id);
    uint8_t *macAddress = const_cast<uint8_t *>(macAddrHelper.getMacAddress());

    // Print the original MAC address
    Serial.printf("Original MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  macAddress[0], macAddress[1], macAddress[2],
                  macAddress[3], macAddress[4], macAddress[5]);

    // Ensure the MAC address is a unicast address
    if (macAddress[0] & 0x01)
    {
        macAddress[0] &= 0xFE; // Clear the least significant bit of the first byte
    }

    // Print the modified MAC address
    Serial.printf("Modified MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  macAddress[0], macAddress[1], macAddress[2],
                  macAddress[3], macAddress[4], macAddress[5]);

    Serial.println("i1");
    esp_base_mac_addr_set(macAddress);
    Serial.println("i2");
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    Serial.printf("\r\nLast Packet Send Status:\t");
    Serial.printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\n" : "Delivery Fail\n");
}

JsonDocument receivedJsondoc;
JSONnow json;
string receivedData;
Peer getDataOf;
void onDataReceive(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    // parseJson = json.parseJSON(incomingData);
    macAddrHelper.copyConstantUint(mac);
    receivedData = "";
    string type;

    for (int i = 0; i < len; i++)
    {
        receivedData += char(incomingData[i]);
    }

    receivedJsondoc = json.parseJSON(receivedData);

    // if (JSON.typeof(receivedJson) == "undefined")
    // {
    //     receivedJson = JSONVar();
    //     receivedJson["type"] = "string";
    //     receivedJson["value"] = receivedData;
    //     type = "string";
    // }

    type = receivedJsondoc["type"].as<const char *>();
    getDataOf = findPeer(macAddrHelper.getStrAddress());
    int typeIndex = getDataOf.peerHandleType.containsKey(type) ? getDataOf.peerHandleType[type].as<const int>() : getDataOf.peerHandleType["default"].as<const int>();

    getDataOf.dataReceiveFunc[typeIndex](receivedJsondoc);
}

#endif
