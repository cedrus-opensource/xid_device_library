/*
To whom it may concern, here is how this maps to the old .devconfig files:

std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-530", 50, 49, 1, 6, devicePorts));
[DeviceInfo]
MajorFirmwareVersion = 1
DeviceName = RB-530
XidProductID = 50
XidModelID = 49

std::vector<int> keyMap = { -1, 0, -1, 1, 2, 3, 4, -1 };
Cedrus::DevicePort port("Key", 0, 5, keyMap);
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

// This has been reduced to a single constructor argument (the 6 in this example)
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
    void CreateEmptyConfig(std::shared_ptr<Cedrus::DeviceConfig> & invalidCfgPtr)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("Invalid Device", -1, -1, -1, 0, devicePorts));

        invalidCfgPtr = config;
    }

    void CreateRB530Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, -1, 1, 2, 3, 4, -1 };
        Cedrus::DevicePort port("Key", 0, 5, keyMap);
        devicePorts.emplace(0, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-530", 50, 49, 1, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB540Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, -1, 1, 2, 3, 4, -1 };
        Cedrus::DevicePort port("Key", 0, 5, keyMap);
        devicePorts.emplace(0, port);

        port = Cedrus::DevicePort("Light Sensor", 2, 1);
        devicePorts.emplace(2, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-540", 50, 49, 2, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB730Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 7, keyMap);
        devicePorts.emplace(0, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-730", 50, 50, 1, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB740Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 7, keyMap);
        devicePorts.emplace(0, port);

        port = Cedrus::DevicePort("Light Sensor", 2, 1);
        devicePorts.emplace(2, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-740", 50, 50, 2, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB830Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 3, 4, 1, 2, 5, 6, 0 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap);
        devicePorts.emplace(0, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-830", 50, 51, 1, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB840Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 3, 4, 1, 2, 5, 6, 0 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap);
        devicePorts.emplace(0, port);

        port = Cedrus::DevicePort("Light Sensor", 2, 1);
        devicePorts.emplace(2, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-840", 50, 51, 2, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB834Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap);
        devicePorts.emplace(0, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-834", 50, 52, 1, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateRB844Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { 7, 0, 1, 2, 3, 4, 5, 6 };
        Cedrus::DevicePort port("Key", 0, 8, keyMap);
        devicePorts.emplace(0, port);

        port = Cedrus::DevicePort("Light Sensor", 2, 1);
        devicePorts.emplace(2, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("RB-844", 50, 52, 2, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateLuminaLP400Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, -1, -1 };
        Cedrus::DevicePort port("Key + Scanner Trigger", 0, 5, keyMap);
        devicePorts.emplace(0, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("Lumina LP-400", 48, 69, 1, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateLumina3GConfig(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::vector<int> keyMap = { -1, 0, 1, 2, 3, 4, -1, -1 };
        Cedrus::DevicePort port("Lumina Pad 1", 0, 5, keyMap);
        devicePorts.emplace(0, port);

        keyMap = { -1, 0, 1, 2, 3, 4, -1, -1 };
        port = Cedrus::DevicePort("Lumina Pad 2", 1, 5, keyMap);
        devicePorts.emplace(1, port);

        port = Cedrus::DevicePort("Light Sensor + Scanner Trigger", 2, 2);
        devicePorts.emplace(2, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("Lumina 3G", 48, 65, 2, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateSV1Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        Cedrus::DevicePort port("Voice Key", 2, 1);
        devicePorts.emplace(2, port);

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("SV-1 Voice Key", 49, 66, 1, 8, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateST100Config(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        // 'S' and 'C'
        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("StimTracker ST-100", 83, 67, 0, 8));

        listOfAllConfigs.push_back(config);
    }

    void CreateST2DuoConfig(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        // 'S' and '1'
        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("StimTracker Duo", 83, 49, 2, 16));

        listOfAllConfigs.push_back(config);
    }

    void CreateST2QuadConfig(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        // 'S' and '2'
        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("StimTracker Quad", 83, 50, 2, 16));

        listOfAllConfigs.push_back(config);
    }

    void CreateST2QuadWithMpod4(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        // 'S' and '3'
        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("StimTracker Quad with m-pod 4", 83, 51, 2, 16));

        listOfAllConfigs.push_back(config);
    }

    void CreateST2StimTrigger(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        // 'S' and '4'
        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig("StimTrigger", 83, 52, 2, 16));

        listOfAllConfigs.push_back(config);
    }

    void AddPodConfig(std::string deviceName,
        int productID,
        int modelID,
        int majorFirmwareVer,
        unsigned int num_lines,
        std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        std::map<unsigned int, Cedrus::DevicePort> devicePorts;

        std::shared_ptr<Cedrus::DeviceConfig> config(new Cedrus::DeviceConfig(deviceName, productID, modelID, majorFirmwareVer, num_lines, devicePorts));

        listOfAllConfigs.push_back(config);
    }

    void CreateAllmpodConfigs(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        AddPodConfig("m-pod for ABM", 51, 97, 2, 8, listOfAllConfigs); // 'a'
        AddPodConfig("m-pod for ADI", 51, 65, 2, 8, listOfAllConfigs); // 'A'
        AddPodConfig("m-pod for ANT Neuro", 51, 67, 2, 8, listOfAllConfigs); // 'C'
        AddPodConfig("m-pod for Biopac MP35/36", 51, 68, 2, 8, listOfAllConfigs); // 'D'
        AddPodConfig("m-pod for Biopac MP150(STP100C module)", 51, 69, 2, 8, listOfAllConfigs); // 'E'
        AddPodConfig("m-pod for Biosemi", 51, 70, 2, 16, listOfAllConfigs); // 'F'
        AddPodConfig("m-pod for Brain Products actiCHamp", 51, 77, 2, 8, listOfAllConfigs); // 'M'
        AddPodConfig("m-pod for Brain Products DB-26", 51, 66, 2, 16, listOfAllConfigs); // 'B'
        AddPodConfig("m-pod for Bittium NeurOne", 51, 110, 2, 8, listOfAllConfigs); // 'n'
        AddPodConfig("m-pod for CGX Systems", 51, 88, 2, 16, listOfAllConfigs); // 'X'
        AddPodConfig("m-pod for EGI", 51, 79, 2, 8, listOfAllConfigs); // 'O'
        AddPodConfig("m-pod for iWorx", 51, 105, 2, 8, listOfAllConfigs); // 'i'
        AddPodConfig("m-pod for MindWare", 51, 71, 2, 8, listOfAllConfigs); // 'G'
        AddPodConfig("m-pod for NeuraLynx", 51, 82, 2, 16, listOfAllConfigs); // 'R'
        AddPodConfig("m-pod for Neuroscan 16-bit", 51, 72, 2, 16, listOfAllConfigs); // 'H'
        AddPodConfig("m-pod for Neuroscan Grael", 51, 104, 2, 16, listOfAllConfigs); // 'h'
        AddPodConfig("m-pod for NIRx", 51, 78, 2, 8, listOfAllConfigs); // 'N'
        AddPodConfig("m-pod for Parallel port", 51, 80, 2, 13, listOfAllConfigs); // 'P'
        AddPodConfig("m-pod for SMI", 51, 74, 2, 8, listOfAllConfigs); // 'J'
        AddPodConfig("m-pod for SR Research", 51, 115, 2, 8, listOfAllConfigs); // 's'
        AddPodConfig("m-pod for TMSi", 51, 116, 2, 8, listOfAllConfigs); // 't'
        AddPodConfig("m-pod for Tobii", 51, 84, 2, 8, listOfAllConfigs); // 'T'
        AddPodConfig("m-pod no model set", 51, 48, 2, 16, listOfAllConfigs); // '0'
        AddPodConfig("Generic m-pod", 51, 103, 2, 8, listOfAllConfigs); // 'g'
    }

    void CreateAllcpodConfigs(std::vector<std::shared_ptr<Cedrus::DeviceConfig> > & listOfAllConfigs)
    {
        AddPodConfig("c-pod for ABM", 52, 97, 2, 8, listOfAllConfigs); // 'a'
        AddPodConfig("c-pod for ADI", 52, 65, 2, 8, listOfAllConfigs); // 'A'
        AddPodConfig("c-pod for ANT Neuro", 52, 67, 2, 8, listOfAllConfigs); // 'C'
        AddPodConfig("c-pod for Biopac MP35/36", 52, 68, 2, 8, listOfAllConfigs); // 'D'
        AddPodConfig("c-pod for Biopac MP150(STP100C module)", 52, 69, 2, 8, listOfAllConfigs); // 'E'
        AddPodConfig("c-pod for Biosemi", 52, 70, 2, 16, listOfAllConfigs); // 'F'
        AddPodConfig("c-pod for Brain Products actiCHamp", 52, 77, 2, 8, listOfAllConfigs); // 'M'
        AddPodConfig("c-pod for Brain Products DB-26", 52, 66, 2, 16, listOfAllConfigs); // 'B'
        AddPodConfig("c-pod for Bittium NeurOne", 52, 110, 2, 8, listOfAllConfigs); // 'n'
        AddPodConfig("c-pod for CGX Systems", 52, 88, 2, 16, listOfAllConfigs); // 'X'
        AddPodConfig("c-pod for EGI", 52, 79, 2, 8, listOfAllConfigs); // 'O'
        AddPodConfig("c-pod for iWorx", 52, 105, 2, 8, listOfAllConfigs); // 'i'
        AddPodConfig("c-pod for MindWare", 52, 71, 2, 8, listOfAllConfigs); // 'G'
        AddPodConfig("c-pod for NeuraLynx", 52, 82, 2, 16, listOfAllConfigs); // 'R'
        AddPodConfig("c-pod for Neuroscan 16-bit", 52, 72, 2, 16, listOfAllConfigs); // 'H'
        AddPodConfig("c-pod for Neuroscan Grael", 52, 104, 2, 16, listOfAllConfigs); // 'h'
        AddPodConfig("c-pod for NIRx", 52, 78, 2, 8, listOfAllConfigs); // 'N'
        AddPodConfig("c-pod for Parallel port", 52, 80, 2, 13, listOfAllConfigs); // 'P'
        AddPodConfig("c-pod for SMI", 52, 74, 2, 8, listOfAllConfigs); // 'J'
        AddPodConfig("c-pod for SR Research", 52, 115, 2, 8, listOfAllConfigs); // 's'
        AddPodConfig("c-pod for TMSi", 52, 116, 2, 8, listOfAllConfigs); // 't'
        AddPodConfig("c-pod for Tobii", 52, 84, 2, 8, listOfAllConfigs); // 'T'
        AddPodConfig("c-pod no model set", 52, 48, 2, 16, listOfAllConfigs); // '0'
        AddPodConfig("Generic c-pod", 52, 103, 2, 8, listOfAllConfigs); // 'g'
    }
}
