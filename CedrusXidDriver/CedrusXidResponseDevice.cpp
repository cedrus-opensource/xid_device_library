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
#include "../xid_device_driver/base_device_t.h"
#include "../xid_device_driver/xid_device_config_t.h"
#include "string_utilities.h"
#include "com_ptr.h"

using namespace public_nbs;

CCedrusXidResponseDevice::CCedrusXidResponseDevice()
    : m_xid_device()
{
}

void CCedrusXidResponseDevice::set_button_names()
{
    m_button_names.clear();

    if(!m_xid_device)
    {
        return;
    }

    std::vector<cedrus::device_port> port_vector = m_xid_device->get_device_config().get_vector_of_ports();

    // Populating is done in two passes. The first adds the presses and the second
    // adds the releases. We're keeping the two halves separate for experiment
    // import purposes. Since the releases are new, if we intermixed them with
    // presses, old experiments would break in puzzling ways. "Key 2" would
    // become "Key 1 Release", "Key 5" would become "Key 3 Press" and so on.
    populate_response_collection( &port_vector, true/*presses*/ );
    populate_response_collection( &port_vector, false/*releases*/ );
}

void CCedrusXidResponseDevice::populate_response_collection( std::vector<cedrus::device_port> * portVector, bool isPressPass )
{
    // This part is awkward. Both Lumina models have a port that provides mixed
    // responses: keys/trigger and light sensor/trigger respectively. As such,
    // we end up with a little bit of kludgy logic to separate out the different
    // responses for clarity.
    bool is_lumina_lp400 = (m_xid_device->get_device_config().get_product_id() == 48 &&
        m_xid_device->get_device_config().m_major_firmware_ver == 1);
    bool is_lumina_3g = (m_xid_device->get_device_config().get_product_id() == 48 &&
        m_xid_device->get_device_config().m_major_firmware_ver == 2);

    std::wstring light_sensor_string = std::wstring(L"Light Sensor ") + std::wstring(isPressPass? L"On" : L"Off");
    std::wstring scanner_trigger_string = std::wstring(L"Scanner Trigger ") + std::wstring(isPressPass? L"On" : L"Off");
    std::wstring generic_suffix = isPressPass? L" Press" : L" Release";

    std::string temp = "Key";

    // Iterate through every port and add responses for each response port.
    for ( unsigned int i = 0; i < portVector->size(); i++ )
    {
        if ( (*portVector)[i].is_response_port )
        {
            for ( unsigned int button = 0; button < (*portVector)[i].number_of_lines; ++button )
            {
                if ( is_lumina_3g && i == 2 )
                {
                    if ( button == 0 )
                        m_button_names.push_back( light_sensor_string );
                    else if ( button == 1 )
                        m_button_names.push_back( scanner_trigger_string );
                }
                else if ( is_lumina_lp400 && i == 0 )
                {
                    if ( button < 4 )
                    {
                        std::wstring prefix (temp.begin(), temp.end());
                        m_button_names.push_back(
                            prefix + L" " + public_nbs::to_wstr(button+1)+ generic_suffix);
                    }
                    else if ( button == 4 )
                        m_button_names.push_back( scanner_trigger_string );
                }
                else
                {
                    temp = (*portVector)[i].port_name;
                    std::wstring prefix (temp.begin(), temp.end());
                    m_button_names.push_back(
                        prefix + L" " + public_nbs::to_wstr(button+1)+ generic_suffix);
                }
            }
        }
    }
}

void CCedrusXidResponseDevice::set_xid_device(boost::shared_ptr<cedrus::base_device_t> xid_device)
{
    m_xid_device = xid_device;

    set_button_names();
}

STDMETHODIMP CCedrusXidResponseDevice::getName(BSTR *name)
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
    *parameters = SysAllocString(to_wstr(m_button_names.size()).c_str());
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::setProperties(BSTR parameters)
{
    (void)parameters;
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getButtonCount(unsigned long *count)
{
    *count = m_button_names.size();
    return S_OK;
}

STDMETHODIMP CCedrusXidResponseDevice::getButtonNames(
    BSTR *names, unsigned long *count)
{
#undef min
    *count = std::min<unsigned long>(*count, m_button_names.size());

    for(unsigned int i = 0; i < *count; ++i)
    {
        names[i] = SysAllocString(m_button_names[i].c_str());
    }
    return S_OK;
}

// IMPORTANT: This function resets the device timer
STDMETHODIMP CCedrusXidResponseDevice::acquire(
    unsigned char useButtons,
    unsigned char useAxes,
    IActiveResponseDevice **device)
{
    HRESULT rval = S_OK;

    try
    {
        public_nbs::COM_ptr<IActiveResponseDevice> active_device;
        active_device = active_device.create(
            CLSID_CedrusXidActiveDevice, IID_IActiveResponseDevice);

        if(active_device.get() == 0)
        {
            m_last_error = L"Failed to create response device";
            rval = E_FAIL;
        }
        else
        {
            *device = active_device.get();
            (*device)->AddRef();

            CCedrusXidActiveDevice *dev = static_cast<
                CCedrusXidActiveDevice*>(*device);

            // This is the actual only time where we reset the device timer.
            dev->set_xid_device(m_xid_device);

            dev->setButtonCount(m_button_names.size());
        }
    }
    catch(...)
    {
        m_last_error = L"Failed to create response device";
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
    *error = SysAllocString(m_last_error.c_str());
    return S_OK;
}
