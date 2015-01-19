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
#include "../xid_device_driver/base_device_t.h"
#include "../xid_device_driver/xid_device_config_t.h"

// CCedrusXidActiveDevice

CCedrusXidActiveDevice::CCedrusXidActiveDevice()
    : m_button_count(0),
      m_xid_device(),
      m_timer_uncertainty(0)
{
    LARGE_INTEGER hpc_freq;
    QueryPerformanceFrequency(&hpc_freq);
    m_hpc_freq = hpc_freq.QuadPart;
}

void CCedrusXidActiveDevice::set_xid_device(boost::shared_ptr<cedrus::base_device_t> xid_device)
{
    // LARGE_INTEGER begin;
    // LARGE_INTEGER end;
    m_xid_device = xid_device;
    m_xid_device->reset_base_timer();

    // QueryPerformanceCounter(&begin);
    m_xid_device->reset_rt_timer();
    // QueryPerformanceCounter(&end);
    // m_timer_uncertainty = end.QuadPart - begin.QuadPart;
}

STDMETHODIMP CCedrusXidActiveDevice::getName(BSTR *name)
{
    if(!m_xid_device)
    {
        m_last_error = L"Xid device has not been properly initialized.";
        return E_FAIL;
    }

    std::string dev_name = m_xid_device->get_device_config().get_device_name();
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
// at approximately 1000Hz. Thus we use this method to poll the XID device
// for responses.
STDMETHODIMP CCedrusXidActiveDevice::getResponseCount(unsigned long *count)
{
    if(!m_xid_device)
    {
        m_last_error = L"Xid device has not been properly initialized.";
        return E_FAIL;
    }

    m_xid_device->poll_for_response();
    cedrus::response res = m_xid_device->get_next_response();

    // as far as I can tell right now, there's no way in Presentation to
    // differentiate between a key press and release.  Here, we only grab
    // key press events from the xid device and ignre the release events.

    /*
    UPDATE: there is a way to differentiate! (From the docs):

    If the device can generate different events for the same button (such
    as press and release), each event should be listed as a seperate
    button with an appropriate name, even though those events are related
    to the same physical button.

    As we haven't done this historically and no one seems to have
    complained, we'll leave this be.
    */
    if(res.was_pressed == true)
    {
        // translate the cedrus::response struct into a tagResponseInfo
        // struct and place it in the key press events list.
        std::vector<cedrus::device_port> port_vector = m_xid_device->get_device_config().get_vector_of_ports();
        int math = 0;

        for ( unsigned int i = 0; i < res.port; i++ )
        {
            if ( port_vector[i].is_response_port )
            {
                math += port_vector[i].number_of_lines;
            }
        }

        tagResponseInfo info;
        info.buttonIndex = math+res.key;
        info.useTime = 0;

        m_key_press_events.push_back(info);
    }

    *count = m_key_press_events.size();

    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::getResponseData(
    tagResponseInfo *data, unsigned long *count)
{
#undef min
    *count = std::min<unsigned long>(*count, m_key_press_events.size());

    std::copy(m_key_press_events.begin(),
              m_key_press_events.begin() + *count, data);

    m_key_press_events.erase(m_key_press_events.begin(),
                            m_key_press_events.begin() + *count);

    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::clearResponseData()
{
    if(!m_xid_device)
    {
        m_last_error = L"Xid device has not been properly initialized.";
        return E_FAIL;
    }

    m_xid_device->clear_responses();
    m_key_press_events.clear();
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
    *error = SysAllocString(m_last_error.c_str());
    return S_OK;
}

STDMETHODIMP CCedrusXidActiveDevice::setButtonCount(unsigned long buttonCount)
{
    m_button_count = buttonCount;
    return S_OK;
}
