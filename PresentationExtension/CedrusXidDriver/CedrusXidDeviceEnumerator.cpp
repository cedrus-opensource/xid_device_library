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
#include "../xid_device_driver/xid_device_scanner_t.h"
#include "../xid_device_driver/base_device_t.h"
#include <boost/shared_ptr.hpp>

using namespace public_nbs;

CCedrusXidDeviceEnumerator::CCedrusXidDeviceEnumerator()
{
    // Look up the path to this DLL. The devconfig files are stored along with it.
    HMODULE module = GetModuleHandle(L"CedrusXidDriver.dll");
    std::wstring devconfig_location;
    if(module)
    {
        wchar_t result[MAX_PATH];

        if(GetModuleFileName(module, result, sizeof(result)) != 0)
        {
            devconfig_location = std::wstring(result);
            std::size_t pos = devconfig_location.find_last_of(L"\\") + 1;
            devconfig_location.erase(pos);
        }
    }
    m_device_scanner.detect_valid_xid_devices(std::string(devconfig_location.begin(), devconfig_location.end()));

    m_devices.resize(m_device_scanner.device_count());
}

STDMETHODIMP CCedrusXidDeviceEnumerator::getDeviceCount(unsigned long *count)
{
    *count = m_devices.size();
    return S_OK;
}

STDMETHODIMP CCedrusXidDeviceEnumerator::getDevice(
    unsigned long index, IResponseDevice **device)
{
    HRESULT rval = S_OK;

    if(index >= m_devices.size())
    {
        m_last_error = L"Invalid index passed to CedrusXidDeviceEnumerator::getDevice";
        rval = E_FAIL;
    }
    else
    {
        try
        {
            if(!m_devices[index].get())
            {
                // Device not yet created.
                m_devices[index] = m_devices[index].create(
                    CLSID_CedrusXidResponseDevice, IID_IResponseDevice);

                CCedrusXidResponseDevice *dev =
                    static_cast<CCedrusXidResponseDevice*>(m_devices[index].get());

                if(dev)
                {
                    boost::shared_ptr<cedrus::base_device_t> xid_device =
                        m_device_scanner.device_connection_at_index(index);
                    xid_device->open_connection();

                    dev->set_xid_device(xid_device);
                }
            }

            if(!m_devices[index].get())
            {
                m_last_error = L"Failed to create IResponseDevice";
                rval = E_FAIL;
            }
            else
            {
                *device = m_devices[index].get();
                // The assumption is that Presentation will call Release on this when the time comes.
                (*device)->AddRef();
            }
        }
        catch(Exception &e)
        {
            m_last_error = e.message();
            rval = E_FAIL;
        }
        catch(...)
        {
            m_last_error = L"Unknown exception creating IResponseDevice";
            rval = E_FAIL;
        }
    }

    return rval;
}

STDMETHODIMP CCedrusXidDeviceEnumerator::getLastError(BSTR *error)
{
    *error = SysAllocString(m_last_error.c_str());
    return S_OK;
}
