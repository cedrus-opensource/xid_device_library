

#ifndef XID_GLOSSARY_H
#define XID_GLOSSARY_H

#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class Connection;

    class XIDGlossaryPSTProof
    {
    public:
        static int GetMajorFirmwareVersion( boost::shared_ptr<Connection> xidCon );

        static std::string GetProtocol( boost::shared_ptr<Connection> xidCon );

        static void GetProductAndModelID(boost::shared_ptr<Connection> xidCon, int *productID, int *modelID);
    };

    class CommandGlossary
    {
    public:
        // e1
        static void ResetBaseTimer(boost::shared_ptr<Connection> xidCon);

        // e3
        static unsigned int QueryBaseTimer(boost::shared_ptr<Connection> xidCon);

        // e5
        static void ResetRtTimer(boost::shared_ptr<Connection> xidCon);

        static unsigned int AdjustEndiannessCharsToUint
        ( const char c1,
          const char c2,
          const char c3,
          const char c4 );

        // _d1
        static std::string GetInternalProductName(boost::shared_ptr<Connection> xidCon);

        // _d2 and _d3
        static void GetProductAndModelID( boost::shared_ptr<Connection> xidCon, int *productID, int *modelID);

        // d3
        static void SetModelID(boost::shared_ptr<Connection> xidCon, unsigned char model);

        // _d4
        static int GetMajorFirmwareVersion( boost::shared_ptr<Connection> xidCon );

        // _d5
        static int GetMinorFirmwareVersion( boost::shared_ptr<Connection> xidCon );

        // _d6 XID 2.0 only.
        static int GetOutpostModel( boost::shared_ptr<Connection> xidCon );

        // _d7 XID 2.0 only
        static int GetHardwareGeneration( boost::shared_ptr<Connection> xidCon );

        // _lr XID 2.0 only
        static int GetLightSensorMode( boost::shared_ptr<Connection> xidCon );

        // lr XID 2.0 only
        static void SetLightSensorMode( boost::shared_ptr<Connection> xidCon, unsigned char mode );

        // _lt XID 2.0 only
        static int GetLightSensorThreshold( boost::shared_ptr<Connection> xidCon );

        // lt XID 2.0 only
        static void SetLightSensorThreshold( boost::shared_ptr<Connection> xidCon, unsigned char threshold );

        // ts XID 2.0 only
        static void SetScannerTriggerFilter( boost::shared_ptr<Connection> xidCon, unsigned char mode );

        // ah
        static void SetDigitalOutputLines_RB( boost::shared_ptr<Connection> xidCon, unsigned int lines );

        // mh
        static void SetDigitalOutputLines_ST( boost::shared_ptr<Connection> xidCon, unsigned int lines );

        // _ml
        static int GetNumberOfLines(boost::shared_ptr<Connection> xidCon);

        // ml
        static void SetNumberOfLines(boost::shared_ptr<Connection> xidCon, unsigned int lines);

        // _c1
        static std::string GetProtocol( boost::shared_ptr<Connection> xidCon );

        // c1
        static void SetProtocol( boost::shared_ptr<Connection> xidCon, unsigned char protocol );

        // f1
        static void SetBaudRate( boost::shared_ptr<Connection> xidCon, unsigned char rate );

        // _mp
        static unsigned int GetPulseDuration( boost::shared_ptr<Connection> xidCon );

        // mp
        static void SetPulseDuration( boost::shared_ptr<Connection> xidCon, unsigned int duration );

        // _a1 deprecated in XID 2.0
        static int GetAccessoryConnectorMode( boost::shared_ptr<Connection> xidCon );

        // _aa XID 2.0 only.
        static int GetAccessoryConnectorDevice( boost::shared_ptr<Connection> xidCon );

        // _a0 deprecated in XID 2.0
        static int GetOutputLogic( boost::shared_ptr<Connection> xidCon );

        // a0 deprecated in XID 2.0
        static void SetOutputLogic( boost::shared_ptr<Connection> xidCon, unsigned char mode );

        // a1 deprecated in XID 2.0
        static void SetAccessoryConnectorMode( boost::shared_ptr<Connection> xidCon, unsigned char mode );

        // aq
        static void ConnectToMpod(boost::shared_ptr<Connection> xidCon, unsigned int mpod, unsigned int action);

        // _a6
        static int GetACDebouncingTime( boost::shared_ptr<Connection> xidCon );

        // a6
        static void SetACDebouncingTime( boost::shared_ptr<Connection> xidCon, unsigned char time );

        //_b3
        static int GetVKDropDelay( boost::shared_ptr<Connection> xidCon );

        // b3
        static void SetVKDropDelay( boost::shared_ptr<Connection> xidCon, unsigned char delay );

        // f3
        static void ReprogramFlash(boost::shared_ptr<Connection> xidCon);

        // _f4
        static int GetTriggerDefault( boost::shared_ptr<Connection> xidCon );

        // f4
        static void SetTriggerDefault( boost::shared_ptr<Connection> xidCon, bool defaultOn );

        // _f5
        static int GetTriggerDebounceTime( boost::shared_ptr<Connection> xidCon );

        // f5
        static void SetTriggerDebounceTime( boost::shared_ptr<Connection> xidCon, unsigned char time );

        // _f6
        static int GetButtonDebounceTime( boost::shared_ptr<Connection> xidCon );

        // f6
        static void SetButtonDebounceTime( boost::shared_ptr<Connection> xidCon, unsigned char time );

        // f7
        static void RestoreFactoryDefaults( boost::shared_ptr<Connection> xidCon );

private:
        // _d4
        static int GetMajorFirmwareVersion( boost::shared_ptr<Connection> xidCon, bool pstProof );

        // _c1
        static std::string GetProtocol( boost::shared_ptr<Connection> xidCon, bool pstProof );

        // _d2 and _d3
        static void GetProductAndModelID(boost::shared_ptr<Connection> xidCon, int *productID, int *modelID, bool pstProof );

        // A small subset of commands differs slightly between "true" xid devices and StimTracker
        static const char XID_COMMAND_PREFIX = 'a';
        static const char ST_COMMAND_PREFIX = 'm';

        // For commands that have pst-proof implementations.
        friend class XIDGlossaryPSTProof;
    };
}
#endif // XID_GLOSSARY_H
