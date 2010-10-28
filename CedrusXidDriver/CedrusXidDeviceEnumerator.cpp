/* Copyright (c) 2010, Cedrus Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
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

// CedrusXidDeviceEnumerator.cpp : Implementation of CCedrusXidDeviceEnumerator

#include "stdafx.h"
#include "CedrusXidDeviceEnumerator.h"
#include "CedrusXidResponseDevice.h"
#include "xid_device_scanner_t.h"
#include "xid_con_t.h"
#include "xid_device_t.h"
#include <boost/shared_ptr.hpp>

using namespace public_nbs;

// CCedrusXidDeviceEnumerator

CCedrusXidDeviceEnumerator::CCedrusXidDeviceEnumerator()
{
    // lookup the path to this DLL.  The devconfig files are stored along 
    // with it.
    HMODULE module = GetModuleHandle(L"CedrusXidDriver.dll");
    if(module)
    {
        wchar_t result[MAX_PATH];

        if(GetModuleFileName(module, result, sizeof(result)) != 0)
        {
            devconfig_location_ = std::wstring(result);
            std::size_t pos = devconfig_location_.find_last_of(L"\\") + 1;
            devconfig_location_.erase(pos);
        }
    }

    devices_.resize(port_info_.detect_valid_xid_devices());
}

STDMETHODIMP CCedrusXidDeviceEnumerator::getDeviceCount(unsigned long *count)
{
    *count = devices_.size();
    return S_OK;
}

STDMETHODIMP CCedrusXidDeviceEnumerator::getDevice(
    unsigned long index, IResponseDevice **device)
{
    HRESULT rval = S_OK;
    
    if(index >= devices_.size())
    {
        last_error_ = 
            L"Invalid index passed to CedrusXidDeviceEnumerator::getDevice";
        rval = E_FAIL;
    }
    else
    {
        try
        {
            if(!devices_[index].get())
            {
                // device not yet created
                // create it here.
                devices_[index] = devices_[index].create(
                    CLSID_CedrusXidResponseDevice, IID_IResponseDevice);

                CCedrusXidResponseDevice *dev = 
                    static_cast<CCedrusXidResponseDevice*>(
                        devices_[index].get());

                if(dev)
                {
                    boost::shared_ptr<cedrus::xid_con_t> xid_con = 
                        port_info_.connection_at_index(index);
                    xid_con->open();

                    boost::shared_ptr<cedrus::xid_device_t> xid_device(
                        new cedrus::xid_device_t(xid_con,
                                                 devconfig_location_));

                    dev->set_xid_device(xid_device);
                }
            }

            if(!devices_[index].get())
            {
                last_error_ = L"Failed to create IResponseDevice";
                rval = E_FAIL;
            }
            else
            {
                *device = devices_[index].get();
                (*device)->AddRef();
            }
        }
        catch(Exception &e)
        {
            last_error_ = e.message();
            rval = E_FAIL;
        }
        catch(...)
        {
            last_error_ = L"Unknown exception creating IResponseDevice";
            rval = E_FAIL;
        }
    }

    return rval;
}

STDMETHODIMP CCedrusXidDeviceEnumerator::getLastError(BSTR *error)
{
    *error = SysAllocString(last_error_.c_str());
    return S_OK;
}
