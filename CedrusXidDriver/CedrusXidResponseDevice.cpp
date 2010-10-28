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

// CedrusXidResponseDevice.cpp : Implementation of CCedrusXidResponseDevice

#include "stdafx.h"
#include "CedrusXidResponseDevice.h"
#include "CedrusXidActiveDevice.h"
#include "xid_device_t.h"
#include "string_utilities.h"
#include "com_ptr.h"

using namespace public_nbs;

// CCedrusXidResponseDevice

CCedrusXidResponseDevice::CCedrusXidResponseDevice()
    : xid_device_(),
      button_count_(0)
{
}

void CCedrusXidResponseDevice::set_button_names()
{
    button_names_.clear();

    if(!xid_device_)
    {
        return;
    }

    std::wstring prefix = xid_device_->input_name_prefix();

    for(unsigned int i = 0; i < button_count_; ++i)
    {
        if(button_count_ == 1)
        {
            button_names_.push_back(prefix);
        }
        else
        {
            button_names_.push_back(
                prefix + L" " + to_wstr(i+1));
        }
    }
}

void CCedrusXidResponseDevice::set_xid_device(
    boost::shared_ptr<cedrus::xid_device_t> xid_device)
{
    xid_device_ = xid_device;
    button_count_ = xid_device_->get_button_count();
    set_button_names();
}

STDMETHODIMP CCedrusXidResponseDevice::getName(BSTR *name)
{
    if(!xid_device_)
    {
        last_error_ = L"Xid device has not been properly initialized.";
        return E_FAIL;
    }

    std::string dev_name = xid_device_->get_device_name();
    std::wstring wname;
    wname.assign(dev_name.begin(), dev_name.end());

    *name = SysAllocString(wname.c_str());
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getPropertiesViewable(
    unsigned char *viewable)
{
    *viewable = 0; // no property page yet.
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::showProperties(HWND parentWindow)
{
    (void)parentWindow;
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::setDefaults()
{
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getProperties(BSTR *parameters)
{
    *parameters = SysAllocString(to_wstr(button_count_).c_str());
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::setProperties(BSTR parameters)
{
    (void)parameters;
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getButtonCount(unsigned long *count)
{
    *count = button_count_;
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getButtonNames(
    BSTR *names, unsigned long *count)
{
#undef min
    *count = std::min<unsigned long>(*count, button_count_);

    for(unsigned int i = 0; i < *count; ++i)
    {
        names[i] = SysAllocString(button_names_[i].c_str());
    }
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::acquire(
    unsigned char useButtons,
    unsigned char useAxes,
    IActiveResponseDevice **device)
{
    HRESULT rval = S_OK;

    try
    {
        COM_ptr<IActiveResponseDevice> active_device;
        active_device = active_device.create(
            CLSID_CedrusXidActiveDevice, IID_IActiveResponseDevice);


        if(active_device.get() == 0)
        {
            last_error_ = L"Failed to create response device";
            rval = E_FAIL;
        }
        else
        {
            *device = active_device.get();
            (*device)->AddRef();

            CCedrusXidActiveDevice *dev = static_cast<
                CCedrusXidActiveDevice*>(*device);

            dev->set_xid_device(xid_device_);
            dev->setButtonCount(button_count_);
        }
    }
    catch(...)
    {
        last_error_ = L"Failed to create response device";
        rval = E_FAIL;
    }

    return rval;
}

STDMETHODIMP CCedrusXidResponseDevice::getAxisCount(
    unsigned long *count, AxisType *type)
{
    *count = 0;
    *type = static_cast<AxisType>(NULL);
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getAxisNames(
    BSTR *names, unsigned long *count)
{
    *count = 0;
    names = NULL;
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getLastError(BSTR *error)
{
    *error = SysAllocString(last_error_.c_str());
    return S_OK;
}
