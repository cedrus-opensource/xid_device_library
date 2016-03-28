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

#ifndef XID_DEVICE_T_H
#define XID_DEVICE_T_H

#include "base_device_t.h"
#include "response_mgr.h"

#include <boost/shared_ptr.hpp>
#include <string>


namespace cedrus
{
    /**
     * @class xid_device_t xid_device_t.h "xid_device_driver/xid_device_t.h"
     *
     * @brief abstraction of an XID device
     */
    class xid_device_t : public base_device_t
    {
    public:
        xid_device_t(
            boost::shared_ptr<xid_con_t> xid_con,
            boost::shared_ptr<const xid_device_config_t> dev_config
            );
        virtual ~xid_device_t(void);

        virtual void reset_rt_timer();

        virtual void reset_base_timer();

        virtual int query_base_timer();

        virtual void poll_for_response();

        virtual bool has_queued_responses();

        virtual int get_number_of_keys_down() const;

        virtual void clear_response_queue();

        virtual cedrus::response get_next_response();

        virtual void clear_responses();

        virtual int get_accessory_connector_mode();

        virtual void set_output_logic( int mode );

        virtual void set_accessory_connector_mode( int mode );

        virtual void set_device_protocol( int protocol );

        virtual xid_device_config_t get_device_config();
        virtual int  open_connection();
        virtual int  close_connection();
        virtual bool has_lost_connection();
        virtual int  get_baud_rate();
        virtual void set_baud_rate( int rate );
        virtual void get_product_and_model_id( int *product_id, int *model_id );
        virtual int  get_major_firmware_version();
        virtual int  get_minor_firmware_version();
        virtual std::string get_internal_product_name();
        virtual void clear_lines();
        virtual int get_trigger_default();
        virtual void set_trigger_default( bool default_on );
        virtual int get_trigger_debounce_time();
        virtual void set_trigger_debounce_time( int time );
        virtual int get_button_debounce_time();
        virtual void set_button_debounce_time( int time );
        virtual void restore_factory_defaults();

        virtual void raise_lines
        ( unsigned int lines_bitmask,
          bool leave_remaining_lines = false);

    protected:
        boost::shared_ptr<xid_con_t> xid_con_;

    private:
        char lines_state_;
        const boost::shared_ptr<const cedrus::xid_device_config_t> config_;
        const boost::shared_ptr<cedrus::response_mgr> m_response_mgr;
    };
} // namespace cedrus

#endif // XID_DEVICE_T_H
