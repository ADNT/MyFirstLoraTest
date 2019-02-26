
#include <Arduino_LoRaWAN.h>
#include <Arduino_LoRaWAN_ttn.h>

class cMyLoRaWAN : public Arduino_LoRaWAN_ttn {
public:
    cMyLoRaWAN() {};

protected:
    // you'll need to provide implementations for each of the following.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    virtual void NetSaveFCntUp(uint32_t uFCntUp) override;
    virtual void NetSaveFCntDown(uint32_t uFCntDown) override;
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
};

// set up the data structures.
cMyLoRaWAN myLoRaWAN {};

void setup() {
    myLoRaWAN.begin();
}

void loop() {
    myLoRaWAN.loop();
}

// this method is called when the LMIC needs OTAA info.
// return false to indicate "no provisioning", otherwise
// fill in the data and return true.
bool
cMyLoRaWAN::GetOtaaProvisioningInfo(
    OtaaProvisioningInfo *pInfo
    ) {
    return false;
}

void
cMyLoRaWAN::NetSaveFCntDown(uint32_t uFCntDown) {
    // save uFcntDown somwwhere
}

void
cMyLoRaWAN::NetSaveFCntUp(uint32_t uFCntUp) {
    // save uFCntUp somewhere
}

void
cMyLoRaWAN::NetSaveSessionInfo(
    const SessionInfo &Info,
    const uint8_t *pExtraInfo,
    size_t nExtraInfo
    ) {
    // save Info somewhere.
}