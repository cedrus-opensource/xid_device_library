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

// CedrusXidActiveDevice.cpp : Implementation of CCedrusXidActiveDevice

#include "stdafx.h"
#include "CedrusXidActiveDevice.h"
#include "../xid_device_driver/xid_device_t.h"

// CCedrusXidActiveDevice

CCedrusXidActiveDevice::CCedrusXidActiveDevice()
    : button_count_(0),
      xid_device_(),
      timer_uncertainty_(0)
{
    LARGE_INTEGER hpc_freq;
    QueryPerformanceFrequency(&hpc_freq);
    hpc_freq_ = hpc_freq.QuadPart;
}

void CCedrusXidActiveDevice::set_xid_device(
    boost::shared_ptr<cedrus::xid_device_t> xid_device)
{
    // LARGE_INTEGER begin;
    // LARGE_INTEGER end;
    xid_device_ = xid_device;
    xid_device_->reset_base_timer();

    // QueryPerformanceCounter(&begin);
    xid_device_->reset_rt_timer();
    // QueryPerformanceCounter(&end);
    // timer_uncertainty_ = end.QuadPart - begin.QuadPart;
}

STDMETHODIMP CCedrusXidActiveDevice::getName(BSTR *name)
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

STDMETHODIMP CCedrusXidActiveDevice::getXYIndices(long *xIndex, long *yIndex)
{
    // we have no axes on XID devices as of yet.  Return 0 for both xIndex and
    // yIndex
    *xIndex = 0;
    *yIndex = 0;
    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::processMessage(BSTR message, BSTR *result)
{
    *result = SysAllocString(L"processsMessage()");
    return S_OK;
}

// According to the Presentation SDK documentation, this method is polled
// at approximately 1000Hz.  Thus we use this method to poll the XID device
// for responses.
STDMETHODIMP CCedrusXidActiveDevice::getResponseCount(unsigned long *count)
{
    if(!xid_device_)
    {
        last_error_ = L"Xid device has not been properly initialized.";
        return E_FAIL;
    }

    xid_device_->poll_for_response();
    std::size_t queue_size = xid_device_->response_queue_size();

    for(std::size_t i = 0; i < queue_size; ++i)
    {
        cedrus::response res = xid_device_->get_next_response();

        // as far as I can tell right now, there's no way in Presentation to
        // differentiate between a key press and release.  Here, we only grab
        // key press events from the xid device and ignre the release events.
        if(res.key_state == cedrus::FOUND_KEY_DOWN)
        {
#if 0
            LARGE_INTEGER begin_hpc;
            QueryPerformanceCounter(&begin_hpc);
            long long base_timer = xid_device_->query_base_timer();
            LARGE_INTEGER end_hpc;
            QueryPerformanceCounter(&end_hpc);
#endif

            // translate the cedrus::response struct into a tagResponseInfo
            // struct and place it in the key press events list.
            tagResponseInfo info;
            info.buttonIndex = res.button;
            info.useTime = 0;
#if 0
            info.useTime = 1;
            info.time = begin_hpc.QuadPart - 
                ((hpc_freq_*(base_timer-res.reaction_time))/1000);
            info.uncertainty = end_hpc.QuadPart - begin_hpc.QuadPart + 
                timer_uncertainty_;
#endif
            key_press_events_.push_back(info);
        }
    }

    *count = key_press_events_.size();

    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::getResponseData(
    tagResponseInfo *data, unsigned long *count)
{
#undef min
    *count = std::min<unsigned long>(*count, key_press_events_.size());

    std::copy(key_press_events_.begin(),
              key_press_events_.begin() + *count, data);

    key_press_events_.erase(key_press_events_.begin(),
                            key_press_events_.begin() + *count);

    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::clearResponseData()
{
    if(!xid_device_)
    {
        last_error_ = L"Xid device has not been properly initialized.";
        return E_FAIL;
    }

    xid_device_->clear_response_queue();
    key_press_events_.clear();
    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::pollAxisPositions(
    double *positions, unsigned long *count)
{
    *positions = 0;
    *count = 0;
    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::pollAxisChanges(
    double *changes, unsigned long *count)
{
    *changes = 0;
    *count = 0;
    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::getLastError(BSTR *error)
{
    *error = SysAllocString(last_error_.c_str());
    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::setButtonCount(unsigned long buttonCount)
{
    button_count_ = buttonCount;
    return S_OK;
}
