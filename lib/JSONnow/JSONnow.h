#ifndef JSONNOW_H_
#define JSONNOW_H_

#include <main.h>
// #include <iomanip>
// #include <vector>
// #include <map>
#include <map>

class JSONnow
{
private:
    JsonDocument jsondoc;

public:
    void addDictionary(const string &key, const string &value)
    {
        jsondoc[key] = value;
    }
    void addDictionary(const string &key, const int &value)
    {
        jsondoc[key] = value;
    }

    JsonDocument parseJSON(string jsonvar)
    {
        deserializeJson(jsondoc, jsonvar);
        return jsondoc;
    }

    void printJSON() const
    {
        serializeJson(jsondoc, Serial);
    }
};

#endif



