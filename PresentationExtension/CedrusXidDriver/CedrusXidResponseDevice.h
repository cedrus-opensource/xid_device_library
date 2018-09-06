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

// CedrusXidResponseDevice.h : Declaration of the CCedrusXidResponseDevice

#pragma once
#include "CedrusXidDriver_i.h"
#include "resource.h"       // main symbols
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace cedrus
{
    class base_device_t;
    struct device_port;
}

using namespace ATL;

// CCedrusXidResponseDevice

class ATL_NO_VTABLE CCedrusXidResponseDevice :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CCedrusXidResponseDevice,
                       &CLSID_CedrusXidResponseDevice>,
    public IDispatchImpl<ICedrusXidResponseDevice,
                         &IID_ICedrusXidResponseDevice,
                         &LIBID_CedrusXidDriverLib,
                         /*wMajor =*/ 1, /*wMinor =*/ 0>,
    public IDispatchImpl<IResponseDevice,
                         &__uuidof(IResponseDevice),
                         &LIBID_ResponseExtension>
{
public:
    CCedrusXidResponseDevice();

    DECLARE_PROTECT_FINAL_CONSTRUCT()

    HRESULT FinalConstruct()
    {
        return S_OK;
    }

    void FinalRelease()
    {
    }

    DECLARE_REGISTRY_RESOURCEID(IDR_CEDRUSXIDRESPONSEDEVICE)

    DECLARE_NOT_AGGREGATABLE(CCedrusXidResponseDevice)

    BEGIN_COM_MAP(CCedrusXidResponseDevice)
        COM_INTERFACE_ENTRY(ICedrusXidResponseDevice)
        COM_INTERFACE_ENTRY2(IDispatch, IResponseDevice)
        COM_INTERFACE_ENTRY(IResponseDevice)
    END_COM_MAP()


    // ICedrusXidResponseDevice
public:
    void set_xid_device(boost::shared_ptr<cedrus::base_device_t> xid_device);

private:
    void set_button_names();
    void populate_response_collection( std::vector<cedrus::device_port> * portVector, bool isPressPass );

    // IResponseDevice Methods
public:
    STDMETHOD(getName)(BSTR * name);
    STDMETHOD(getPropertiesViewable)(unsigned char * viewable);
    STDMETHOD(showProperties)(HWND parentWindow);
    STDMETHOD(setDefaults)();
    STDMETHOD(getProperties)(BSTR * parameters);
    STDMETHOD(setProperties)(BSTR parameters);
    STDMETHOD(getButtonCount)(unsigned long * count);
    STDMETHOD(getButtonNames)(BSTR * names, unsigned long * size);
    // IMPORTANT: This function resets the device timer
    STDMETHOD(acquire)(unsigned char useButtons,
                       unsigned char useAxes,
                       IActiveResponseDevice * * device);
    STDMETHOD(getAxisCount)(unsigned long * count, AxisType * type);
    STDMETHOD(getAxisNames)(BSTR * names, unsigned long * size);
    STDMETHOD(getLastError)(BSTR * error);

private:
    std::wstring m_last_error;
    boost::shared_ptr<cedrus::base_device_t> m_xid_device;
    std::vector<std::wstring> m_button_names;
};

OBJECT_ENTRY_AUTO(__uuidof(CedrusXidResponseDevice), CCedrusXidResponseDevice)
