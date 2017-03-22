/* Copyright (c) 2010, Cedrus Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions areXIDConnection
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of Cedrus Corporation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BASE_DEVICE_T_H
#define BASE_DEVICE_T_H

#include "XidDriverImpExpDefs.h"

#include "ResponseManager.h"
#include "XIDDeviceReadOnly.h"

#include <boost/utility.hpp>

namespace cedrus
{
    class Connection;
    class DeviceConfig;

    class CEDRUS_XIDDRIVER_IMPORTEXPORT XIDDevice : public XIDDeviceReadOnly, boost::noncopyable
    {
    public:
        struct ProductAndModelID
        {
            ProductAndModelID() : productID(-1), modelID(-1) { }
            int productID;
            int modelID;
        };

        virtual ~XIDDevice()
        {
        }

        virtual int GetOutpostModel()=0;

        virtual int GetHardwareGeneration()=0;

        virtual int GetLightSensorMode()=0;

        virtual void SetLightSensorMode( unsigned char mode )=0;

        virtual void SetLightSensorThreshold( unsigned char threshold )=0;

        virtual int GetLightSensorThreshold()=0;

        virtual void SetScannerTriggerFilter( unsigned char mode )=0;

        virtual void ResetRtTimer()=0;

        virtual void ResetBaseTimer()=0;

        virtual unsigned int QueryBaseTimer()=0;

        virtual unsigned int GetPulseDuration()=0;

        virtual void SetPulseDuration( unsigned int duration )=0;

        virtual int GetNumberOfLines()=0;

        virtual void SetNumberOfLines(unsigned int lines)=0;

        virtual void PollForResponse()=0;

        virtual bool HasQueuedResponses()=0;

        virtual int GetNumberOfKeysDown() const=0;

        virtual void ClearResponseQueue()=0;

        virtual cedrus::response GetNextResponse()=0;

        virtual void ClearResponses()=0;

        virtual int GetAccessoryConnectorMode()=0;

        virtual int GetAccessoryConnectorDevice()=0;

        virtual int GetOutputLogic()=0;

        virtual void SetOutputLogic( int mode )=0;

        virtual void SetAccessoryConnectorMode( int mode )=0;

        virtual int GetVKDropDelay()=0;

        virtual void SetVKDropDelay( unsigned char delay )=0;

        virtual void ReprogramFlash()=0;

        virtual int GetTriggerDefault()=0;

        virtual void SetTriggerDefault( bool defaultOn )=0;

        virtual int GetTriggerDebounceTime()=0;

        virtual void SetTriggerDebounceTime( unsigned char time )=0;

        virtual int  GetButtonDebounceTime()=0;

        virtual void SetButtonDebounceTime( unsigned char time )=0;

        virtual void SetProtocol( unsigned char protocol )=0;

        virtual void ConnectToMpod(unsigned int mpod, unsigned int action) = 0;

        // Every device needs these.
        virtual const boost::shared_ptr<const DeviceConfig> GetDeviceConfig() const = 0;
        virtual int OpenConnection() = 0;
        virtual int CloseConnection() = 0;
        virtual bool HasLostConnection() = 0;
        virtual int GetBaudRate() = 0;
        virtual void SetBaudRate( unsigned char rate ) = 0;
        virtual std::string GetProtocol()=0;
        virtual void GetProductAndModelID( int *productID, int *modelID ) = 0;
        virtual void SetModelID( unsigned char model ) = 0;
        virtual ProductAndModelID GetProductAndModelID()=0;
        virtual int GetMajorFirmwareVersion() = 0;
        virtual int GetMinorFirmwareVersion() = 0;
        virtual std::string GetInternalProductName() = 0;
        virtual void RaiseLines(unsigned int linesBitmask, bool leaveRemainingLines = false) = 0;
        virtual void LowerLines(unsigned int linesBitmask, bool leaveRemainingLines = false) = 0;
        virtual void ClearLines() = 0;
        virtual void RestoreFactoryDefaults() = 0;
    };

} // namespace cedrus

#endif
