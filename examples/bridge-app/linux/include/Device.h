/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include <app/util/attribute-storage.h>

#include <stdbool.h>
#include <stdint.h>

#include <functional>
#include <vector>

using namespace chip::app::Clusters;
using namespace chip::app::Clusters::BridgedActions;

class Device
{
public:
    static const int kDeviceNameSize = 32;

    enum Changed_t
    {
        kChanged_Reachable = 1u << 0,
        kChanged_Location  = 1u << 1,
        kChanged_Name      = 1u << 2,
        kChanged_Last      = kChanged_Name,
    } Changed;

    Device(const char * szDeviceName, std::string szLocation);
    virtual ~Device() {}

    bool IsReachable();
    void SetReachable(bool aReachable);
    void SetName(const char * szDeviceName);
    void SetLocation(std::string szLocation);
    inline void SetEndpointId(chip::EndpointId id) { mEndpointId = id; };
    inline chip::EndpointId GetEndpointId() { return mEndpointId; };
    inline void SetParentEndpointId(chip::EndpointId id) { mParentEndpointId = id; };
    inline chip::EndpointId GetParentEndpointId() { return mParentEndpointId; };
    inline char * GetName() { return mName; };
    inline std::string GetLocation() { return mLocation; };
    inline std::string GetZone() { return mZone; };
    inline void SetZone(std::string zone) { mZone = zone; };

private:
    virtual void HandleDeviceChange(Device * device, Device::Changed_t changeMask) = 0;

protected:
    bool mReachable;
    char mName[kDeviceNameSize];
    std::string mLocation;
    chip::EndpointId mEndpointId;
    chip::EndpointId mParentEndpointId;
    std::string mZone;
};

class DeviceOnOff : public Device
{
public:
    enum Changed_t
    {
        kChanged_OnOff = kChanged_Last << 1,
    } Changed;

    DeviceOnOff(const char * szDeviceName, std::string szLocation);

    bool IsOn();
    void SetOnOff(bool aOn);
    void Toggle();

    using DeviceCallback_fn = std::function<void(DeviceOnOff *, DeviceOnOff::Changed_t)>;
    void SetChangeCallback(DeviceCallback_fn aChanged_CB);

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    bool mOn;
    DeviceCallback_fn mChanged_CB;
};

class DeviceSwitch : public Device
{
public:
    enum Changed_t
    {
        kChanged_NumberOfPositions = kChanged_Last << 1,
        kChanged_CurrentPosition   = kChanged_Last << 2,
        kChanged_MultiPressMax     = kChanged_Last << 3,
    } Changed;

    DeviceSwitch(const char * szDeviceName, std::string szLocation, uint32_t aFeatureMap);

    void SetNumberOfPositions(uint8_t aNumberOfPositions);
    void SetCurrentPosition(uint8_t aCurrentPosition);
    void SetMultiPressMax(uint8_t aMultiPressMax);

    inline uint8_t GetNumberOfPositions() { return mNumberOfPositions; };
    inline uint8_t GetCurrentPosition() { return mCurrentPosition; };
    inline uint8_t GetMultiPressMax() { return mMultiPressMax; };
    inline uint32_t GetFeatureMap() { return mFeatureMap; };

    using DeviceCallback_fn = std::function<void(DeviceSwitch *, DeviceSwitch::Changed_t)>;
    void SetChangeCallback(DeviceCallback_fn aChanged_CB);

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    uint8_t mNumberOfPositions;
    uint8_t mCurrentPosition;
    uint8_t mMultiPressMax;
    uint32_t mFeatureMap;
    DeviceCallback_fn mChanged_CB;
};

class ComposedDevice : public Device
{
public:
    ComposedDevice(const char * szDeviceName, std::string szLocation) : Device(szDeviceName, szLocation){};

    using DeviceCallback_fn = std::function<void(ComposedDevice *, ComposedDevice::Changed_t)>;

    void SetChangeCallback(DeviceCallback_fn aChanged_CB);

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    DeviceCallback_fn mChanged_CB;
};

class DevicePowerSource : public Device
{
public:
    enum Changed_t
    {
        kChanged_BatLevel    = kChanged_Last << 1,
        kChanged_Description = kChanged_Last << 2,
    } Changed;

    DevicePowerSource(const char * szDeviceName, std::string szLocation, uint32_t aFeatureMap) :
        Device(szDeviceName, szLocation), mFeatureMap(aFeatureMap){};

    using DeviceCallback_fn = std::function<void(DevicePowerSource *, DevicePowerSource::Changed_t)>;
    void SetChangeCallback(DeviceCallback_fn aChanged_CB) { mChanged_CB = aChanged_CB; }

    void SetBatChargeLevel(uint8_t aBatChargeLevel);
    void SetDescription(std::string aDescription);

    inline uint32_t GetFeatureMap() { return mFeatureMap; };
    inline uint8_t GetBatChargeLevel() { return mBatChargeLevel; };
    inline uint8_t GetOrder() { return mOrder; };
    inline uint8_t GetStatus() { return mStatus; };
    inline std::string GetDescription() { return mDescription; };

private:
    void HandleDeviceChange(Device * device, Device::Changed_t changeMask);

private:
    uint8_t mBatChargeLevel  = 0;
    uint8_t mOrder           = 0;
    uint8_t mStatus          = 0;
    std::string mDescription = "Primary Battery";
    uint32_t mFeatureMap;
    DeviceCallback_fn mChanged_CB;
};

class EndpointListInfo
{
public:
    EndpointListInfo(uint16_t endpointListId, std::string name, EndpointListTypeEnum type);
    EndpointListInfo(uint16_t endpointListId, std::string name, EndpointListTypeEnum type, chip::EndpointId endpointId);
    void AddEndpointId(chip::EndpointId endpointId);
    inline uint16_t GetEndpointListId() { return mEndpointListId; };
    std::string GetName() { return mName; };
    inline EndpointListTypeEnum GetType() { return mType; };
    inline chip::EndpointId * GetEndpointListData() { return mEndpoints.data(); };
    inline size_t GetEndpointListSize() { return mEndpoints.size(); };

private:
    uint16_t mEndpointListId = static_cast<uint16_t>(0);
    std::string mName;
    EndpointListTypeEnum mType = static_cast<EndpointListTypeEnum>(0);
    std::vector<chip::EndpointId> mEndpoints;
};

class Room
{
public:
    Room(std::string name, uint16_t endpointListId, EndpointListTypeEnum type, bool isVisible);
    inline void setIsVisible(bool isVisible) { mIsVisible = isVisible; };
    inline bool getIsVisible() { return mIsVisible; };
    inline void setName(std::string name) { mName = name; };
    inline std::string getName() { return mName; };
    inline EndpointListTypeEnum getType() { return mType; };
    inline uint16_t getEndpointListId() { return mEndpointListId; };

private:
    bool mIsVisible;
    std::string mName;
    uint16_t mEndpointListId;
    EndpointListTypeEnum mType;
};
