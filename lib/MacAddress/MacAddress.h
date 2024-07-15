#ifndef MACADDRESS_H_
#define MACADDRESS_H_

#include <main.h>

string mac_address_str;

class MacAddress {
private:
    uint8_t mac_address_arr[6];
    vector<uint8_t> macAddr;

public:
    MacAddress() {}

    MacAddress(const uint8_t *addr) : macAddr(addr, addr + 6) {
        setAddress(addr);
    }

    MacAddress(string address) {
        setAddress(address);
    }

    string getStrAddress() {
        char src[6];
        mac_address_str = "";
        for (int i = 0; i < 6; i++) {
            sprintf(src, "%02X", mac_address_arr[i]);
            mac_address_str += string(src);
            if (i < 5) {
                mac_address_str += ":";
            }
        }
        return mac_address_str;
    }

    const uint8_t *getMacAddress() const {
        return this->mac_address_arr;
    }

    void copyConstantUint(const uint8_t *copy) {
        setAddress(copy);
    }

    void printMacAddress() const {
        for (size_t i = 0; i < 6; ++i) {
            Serial.printf("%02X", mac_address_arr[i]);
            if (i < 5) Serial.printf(":");
        }
        Serial.printf("\n");
    }

    int CharToNum(char c) {
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        else if (c >= '0' && c <= '9') return c - '0';
        return -1;
    }

    int HexstringToNumber(string hex) {
        int number = 0;
        int multiple = 1;
        for (int i = hex.length() - 1; i >= 0; i--) {
            int temp = CharToNum(hex[i]) * multiple;
            if (temp == -1) return -1;
            number += temp;
            multiple *= 16;
        }
        return number;
    }

    bool stringToHex(string adr) {
        if (adr.length() != 17) return false; // Ensure correct length with separators
        int cnt = 0;
        string t = "";
        for (int i = 0; i < adr.length(); i++) {
            if (adr[i] == ':') {
                if (cnt >= 6) return false;
                int temp = HexstringToNumber(t);
                if (temp == -1) return false;
                mac_address_arr[cnt++] = temp;
                t = "";
            } else {
                t += adr[i];
            }
        }
        return cnt == 6;
    }

    void setAddress(uint8_t addr[]) {
        for (int i = 0; i < 6; i++) {
            this->mac_address_arr[i] = addr[i];
        }
    }

    void setAddress(string address) {
        if (!stringToHex(address)) {
            Serial.println("Invalid MAC address format");
        }
    }

    void setAddress(const uint8_t *copy) {
        for (int i = 0; i < 6; i++) {
            mac_address_arr[i] = copy[i];
        }
    }

    void parseName(string name) {
        if (name.length() > 6) {
            name = name.substr(0, 6);
        }
        while (name.length() < 6) {
            name = "." + name;
        }

        for (int i = 0; i < 6; i++) {
            mac_address_arr[i] = static_cast<uint8_t>(name[i]);
        }
    }
};

#endif
