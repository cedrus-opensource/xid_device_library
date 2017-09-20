/*
To whom it may concern, here is how this maps to the old .devconfig files:

std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-530", 50, 49, 1, devicePorts));
[DeviceInfo]
MajorFirmwareVersion = 1
DeviceName = RB-530
XidProductID = 50
XidModelID = 49

std::vector<int> keyMap = { -1, 0, -1, 1, 2, 3, 4, -1 };
Cedrus::DevicePort port("Key", 0, 5, keyMap, true);
[Port0]
PortName = Key
NumberOfLines = 5
UseableAsResponse = Yes
XidDeviceKeyMap0 = -1
XidDeviceKeyMap1 =  0
XidDeviceKeyMap2 = -1
XidDeviceKeyMap3 =  1
XidDeviceKeyMap4 =  2
XidDeviceKeyMap5 =  3
XidDeviceKeyMap6 =  4
XidDeviceKeyMap7 = -1

port = Cedrus::DevicePort("Accessory Connector", 1, 6, false);
[Port1]
PortName = Accessory Connector
NumberOfLines = 6
UseableAsResponse = No
*/

#pragma once

#include "DeviceConfig.h"
#include <memory>

namespace
{
    void CreateRB530Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, -1, 1, 2, 3, 4, -1 };
        Cedrus::DevicePort port("Key", 0, 5, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 6, false);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-530", 50, 49, 1, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB540Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, -1, 1, 2, 3, 4, -1 };
        Cedrus::DevicePort port("Key", 0, 5, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 8, false);
        devicePorts.push_back(port);

        keyMap = { -1, -1, -1, 0, -1, -1, -1, -1 };
        port = Cedrus::DevicePort("Light Sensor", 2, 1, keyMap, true);
        devicePorts.push_back(port);

        keyMap = { 0, 1, 2, 3, 4, 5, 6, 7 };
        port = Cedrus::DevicePort("External Input", 3, 8, keyMap, true);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-540", 50, 49, 2, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB730Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 7, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 6, false);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-730", 50, 50, 1, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB740Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 7, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 8, false);
        devicePorts.push_back(port);

        keyMap = { -1, -1, -1, 0, -1, -1, -1, -1 };
        port = Cedrus::DevicePort("Light Sensor", 2, 1, keyMap, true);
        devicePorts.push_back(port);

        keyMap = { 0, 1, 2, 3, 4, 5, 6, 7 };
        port = Cedrus::DevicePort("External Input", 3, 8, keyMap, true);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-740", 50, 50, 2, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB830Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 3, 4, 1, 2, 5, 6, 0 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 6, false);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-830", 50, 51, 1, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB840Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 3, 4, 1, 2, 5, 6, 0 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 8, false);
        devicePorts.push_back(port);

        keyMap = { -1, -1, -1, 0, -1, -1, -1, -1 };
        port = Cedrus::DevicePort("Light Sensor", 2, 1, keyMap, true);
        devicePorts.push_back(port);

        keyMap = { 0, 1, 2, 3, 4, 5, 6, 7 };
        port = Cedrus::DevicePort("External Input", 3, 8, keyMap, true);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-840", 50, 51, 2, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB834Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 6, false);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-834", 50, 52, 1, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB844Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 8, false);
        devicePorts.push_back(port);

        keyMap = { -1, -1, -1, 0, -1, -1, -1, -1 };
        port = Cedrus::DevicePort("Light Sensor", 2, 1, keyMap, true);
        devicePorts.push_back(port);

        keyMap = { 0, 1, 2, 3, 4, 5, 6, 7 };
        port = Cedrus::DevicePort("External Input", 3, 8, keyMap, true);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-844", 50, 52, 2, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateLuminaLP400Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, -1, -1 };
        Cedrus::DevicePort port("Key + Scanner Trigger", 0, 5, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 6, false);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("Lumina LP-400", 48, 69, 1, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateLumina3GConfig(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, -1, -1 };
        Cedrus::DevicePort port("Lumina Pad 1, Button", 0, 5, keyMap, true);
        devicePorts.push_back(port);

        keyMap = { -1, 0, 1, 2, 3, 4, -1, -1 };
        port = Cedrus::DevicePort("Lumina Pad 2, Button", 1, 5, keyMap, true);
        devicePorts.push_back(port);

        keyMap = { -1, -1, -1, 0, 1, -1, -1, -1 };
        port = Cedrus::DevicePort("Light Sensor + Scanner Trigger", 2, 2, keyMap, true);
        devicePorts.push_back(port);

        keyMap = { 0, 1, 2, 3, 4, 5, 6, 7 };
        port = Cedrus::DevicePort("External Input", 3, 8, keyMap, true);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 4, 8, false);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("Lumina 3G", 48, 65, 2, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateSV1Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        Cedrus::DevicePort port ("UNUSED PORT", 0, 0, false);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Accessory Connector", 1, 6, false);
        devicePorts.push_back(port);

        std::vector<int> keyMap = { -1, 0, -1, -1, -1, -1, -1, -1 };
        port = Cedrus::DevicePort("Voice Key", 2, 1, keyMap, true);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("SV-1 Voice Key", 49, 66, 1, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateST100Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        Cedrus::DevicePort port("UNUSED PORT", 0, 0, false);
        devicePorts.push_back(port);

        port = Cedrus::DevicePort("Event Marker", 1, 8, false);
        devicePorts.push_back(port);

        // 'S' and 'C'
        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("StimTracker ST-100", 83, 67, 1, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void AddPodConfig(std::string deviceName,
        int productID,
        int modelID,
        int majorFirmwareVer,
        std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::vector<Cedrus::DevicePort> devicePorts;

        Cedrus::DevicePort port("Event Marker", 0, 8, false);
        devicePorts.push_back(port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig(deviceName, productID, modelID, majorFirmwareVer, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateAllmpodConfigs(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        AddPodConfig("m-pod for ADI", 51, 65, 2, listOfAllConfigs); // 'A'
        AddPodConfig("m-pod for Brain Products DB-26", 51, 66, 2, listOfAllConfigs); // 'B'
        AddPodConfig("m-pod for ANT Neuro", 51, 67, 2, listOfAllConfigs); // 'C'
        AddPodConfig("m-pod for Biopac MP35/36", 51, 68, 2, listOfAllConfigs); // 'D'
        AddPodConfig("m-pod for Biopac MP150(STP100C module)", 51, 69, 2, listOfAllConfigs); // 'E'
        AddPodConfig("m-pod for Biosemi", 51, 70, 2, listOfAllConfigs); // 'F'
        AddPodConfig("m-pod for MindWare", 51, 71, 2, listOfAllConfigs); // 'G'
        AddPodConfig("m-pod for Neuroscan", 51, 72, 2, listOfAllConfigs); // 'H'
        AddPodConfig("m-pod for SMI", 51, 74, 2, listOfAllConfigs); // 'J'
        AddPodConfig("m-pod for Brain Products actiCHamp", 51, 77, 2, listOfAllConfigs); // 'M'
        AddPodConfig("m-pod for NIRx", 51, 78, 2, listOfAllConfigs); // 'N'
        AddPodConfig("m-pod for Tobii", 51, 84, 2, listOfAllConfigs); // 'T'
        AddPodConfig("m-pod no model set", 51, 48, 2, listOfAllConfigs); // '0'
        AddPodConfig("Generic m-pod", 51, 103, 2, listOfAllConfigs); // 'g'
    }

    void CreateAllcpodConfigs(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        AddPodConfig("c-pod for ADI", 51, 65, 2, listOfAllConfigs); // 'A'
        AddPodConfig("c-pod for Brain Products DB-26", 51, 66, 2, listOfAllConfigs); // 'B'
        AddPodConfig("c-pod for ANT Neuro", 51, 67, 2, listOfAllConfigs); // 'C'
        AddPodConfig("c-pod for Biopac MP35/36", 51, 68, 2, listOfAllConfigs); // 'D'
        AddPodConfig("c-pod for Biopac MP150(STP100C module)", 51, 69, 2, listOfAllConfigs); // 'E'
        AddPodConfig("c-pod for Biosemi", 51, 70, 2, listOfAllConfigs); // 'F'
        AddPodConfig("c-pod for MindWare", 51, 71, 2, listOfAllConfigs); // 'G'
        AddPodConfig("c-pod for Neuroscan", 51, 72, 2, listOfAllConfigs); // 'H'
        AddPodConfig("c-pod for SMI", 51, 74, 2, listOfAllConfigs); // 'J'
        AddPodConfig("c-pod for Brain Products actiCHamp", 51, 77, 2, listOfAllConfigs); // 'M'
        AddPodConfig("c-pod for NIRx", 51, 78, 2, listOfAllConfigs); // 'N'
        AddPodConfig("c-pod for Tobii", 51, 84, 2, listOfAllConfigs); // 'T'
        AddPodConfig("c-pod no model set", 51, 48, 2, listOfAllConfigs); // '0'
        AddPodConfig("Generic c-pod", 51, 103, 2, listOfAllConfigs); // 'g'
    }
}
