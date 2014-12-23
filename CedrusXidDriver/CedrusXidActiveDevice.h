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

// CedrusXidActiveDevice.h : Declaration of the CCedrusXidActiveDevice

#pragma once
#include "CedrusXidDriver_i.h"
#include "resource.h"       // main symbols
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class base_device_t;
}

using namespace ATL;



// CCedrusXidActiveDevice

class ATL_NO_VTABLE CCedrusXidActiveDevice :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CCedrusXidActiveDevice, 
                       &CLSID_CedrusXidActiveDevice>,
    public IDispatchImpl<ICedrusXidActiveDevice, 
                         &IID_ICedrusXidActiveDevice, 
                         &LIBID_CedrusXidDriverLib, 
                         /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IDispatchImpl<IActiveResponseDevice, 
                         &__uuidof(IActiveResponseDevice), 
                         &LIBID_ResponseExtension>
{
public:
    CCedrusXidActiveDevice();

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_CEDRUSXIDACTIVEDEVICE)

    DECLARE_NOT_AGGREGATABLE(CCedrusXidActiveDevice)

    BEGIN_COM_MAP(CCedrusXidActiveDevice)
        COM_INTERFACE_ENTRY(ICedrusXidActiveDevice)
        COM_INTERFACE_ENTRY2(IDispatch, IActiveResponseDevice)
        COM_INTERFACE_ENTRY(IActiveResponseDevice)
    END_COM_MAP()




    // ICedrusXidActiveDevice
public:
    STDMETHOD(setButtonCount)(unsigned long count);
    void set_xid_device(boost::shared_ptr<cedrus::base_device_t> xid_device);

    // IActiveResponseDevice Methods
public:
    STDMETHOD(getName)(BSTR * name);
    STDMETHOD(getXYIndices)(long * xIndex, long * yIndex);
    STDMETHOD(processMessage)(BSTR message, BSTR * result);
    STDMETHOD(getResponseCount)(unsigned long * count);
    STDMETHOD(getResponseData)(tagResponseInfo * data, unsigned long * count);
    STDMETHOD(clearResponseData)();
    STDMETHOD(pollAxisPositions)(double * positions, unsigned long * count);
    STDMETHOD(pollAxisChanges)(double * changes, unsigned long * count);
    STDMETHOD(getLastError)(BSTR * error);

private:
    unsigned long m_button_count;
    boost::shared_ptr<cedrus::base_device_t> m_xid_device;
    std::wstring m_last_error;
    std::vector<tagResponseInfo> m_key_press_events;
    long long m_hpc_freq;
    long m_timer_uncertainty;
};

OBJECT_ENTRY_AUTO(__uuidof(CedrusXidActiveDevice), CCedrusXidActiveDevice)
