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

// CedrusXidDeviceEnumerator.h : Declaration of the CCedrusXidDeviceEnumerator

#pragma once
#include "CedrusXidDriver_i.h"
#include "resource.h"       // main symbols
#include "com_ptr.h"
#include <vector>
#include <string>
#include "../xid_device_driver/xid_device_scanner_t.h"

using namespace ATL;

namespace cedrus
{
    class xid_device_config_t;
}

// CCedrusXidDeviceEnumerator

class ATL_NO_VTABLE CCedrusXidDeviceEnumerator :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CCedrusXidDeviceEnumerator,
                       &CLSID_CedrusXidDeviceEnumerator>,
    public IDispatchImpl<ICedrusXidDeviceEnumerator,
                         &IID_ICedrusXidDeviceEnumerator,
                         &LIBID_CedrusXidDriverLib,
                         /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IDispatchImpl<IResponseDeviceEnumerator,
                         &__uuidof(IResponseDeviceEnumerator),
                         &LIBID_ResponseExtension>
{
public:
    CCedrusXidDeviceEnumerator();

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_CEDRUSXIDDEVICEENUMERATOR)

    DECLARE_NOT_AGGREGATABLE(CCedrusXidDeviceEnumerator)

    BEGIN_COM_MAP(CCedrusXidDeviceEnumerator)
        COM_INTERFACE_ENTRY(ICedrusXidDeviceEnumerator)
        COM_INTERFACE_ENTRY2(IDispatch, IResponseDeviceEnumerator)
        COM_INTERFACE_ENTRY(IResponseDeviceEnumerator)
    END_COM_MAP()




    // ICedrusXidDeviceEnumerator
public:

    // IResponseDeviceEnumerator Methods
public:
    STDMETHOD(getDeviceCount)(unsigned long * count);
    STDMETHOD(getDevice)(unsigned long index, IResponseDevice * * device);
    STDMETHOD(getLastError)(BSTR * error);

private:
    std::wstring m_last_error;
    std::vector<public_nbs::COM_ptr<IResponseDevice> > m_devices;

    cedrus::xid_device_scanner_t m_device_scanner;
};

OBJECT_ENTRY_AUTO(__uuidof(CedrusXidDeviceEnumerator), CCedrusXidDeviceEnumerator)
