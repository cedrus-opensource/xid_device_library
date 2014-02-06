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

#ifndef XID_CON_T_H
#define XID_CON_T_H

#include "port_settings_t.h"
#include "constants.h"

#include <boost/shared_ptr.hpp>
#include "XidDriverImpExpDefs.h"

namespace cedrus
{
    /**
     * @class xid_con_t xid_con_t.h "xid_device_driver/xid_con_t.h"
     * 
     * @brief Implementation of communication with an XID device
     *
     * This class implements the nuts and bolts of communicating with an
     * XID device.  Inter-byte delays are optionally added if it's
     * communicating with a 4 MHz XID device.  It also handles the parsing
     * of the XID protocol 6-byte packats.
     */
    class CEDRUS_XIDDRIVER_IMPORTEXPORT xid_con_t
    {
    public:
        
        /**
         * xid_con_t constructor
         *
         * @param[in] port_name COM port name the device is connected to.
         * @param[in] port_speed speed of the port. This defaults to 115200bps
         * @param[in] delay_ms Inter-byte delay time in miliseconds.  Defaults to 1ms.
         * @param[in] byte_size Byte size used by the device
         * @param[in] bit_parity bit parity of the device.  Defaults to none.
         * @param[in] stop_bits stop bits of the device. Defaults to 0
         */
        xid_con_t(
                  const std::string &port_name,
                  int port_speed = 115200,
                  int delay_ms = 1,
                  port_settings_t::bytesize byte_size = port_settings_t::BYTESIZE_8,
                  port_settings_t::bitparity bit_parity = port_settings_t::BITPARITY_NONE,
                  port_settings_t::stopbits stop_bits = port_settings_t::STOP_BIT_1
                  );
        
        virtual ~xid_con_t();

        /**
         * Closes the device
         * 
         * @returns NO_ERR if closing the device was successful, or 
         * ERROR_CLOSING_PORT if it was unsuccessful. Enum values are defined
         * in constants.h
         */ 
        int close();

        /**
         * Flush the device's input buffer.
         * 
         * @returns NO_ERR if flushing the device was successful, or 
         * ERROR_FLUSHING_PORT if it was unsuccessful.  Enum values are defined
         * in constants.h
         */
        int flush_input();

        /**
         * Flush the device's output buffer.
         * 
         * @returns NO_ERR if flushing the device was successful, or 
         * ERROR_FLUSHING_PORT if it was unsuccessful.  Enum values are defined
         * in constants.h
         */
        int flush_output();

        /**
         * Opens the COM port for use.
         *
         * @returns NO_ERR if opening the device for communication was 
         * successful. If the operation was unsuccessful, it can return one of
         * the following:  PORT_NOT_AVAILABLE, ERROR_SETTING_UP_PORT,
         * ERROR_FLUSHING_PORT.  Enum values are defined in constants.h.
         */
        int open();

        /**
         * Read data from the device.
         *
         * @param[out] in_buffer character array to be used for reading data
         * from the device into.  It is the user's responsibility to ensure
         * bytes_to_read is less than the size of in_buffer
         * @param[in] bytes_to_read specifies the number of bytes to expect
         * to be returned from the device
         * @param[out] bytes_read number of bytes actually read
         *
         * @returns NO_ERR if the read was successful, ERROR_READING_PORT otherwise.
         */
        int read(
            unsigned char *in_buffer,
            int bytes_to_read,
            int &bytes_read);

        /**
         * Write data to the device.
         *
         * If the device is a 4MHz XID unit, an inter-byte delay is used between
         * each byte sent to the device.
         *
         * @param[in] in_buffer command to be sent to the device
         * @param[in] bytes_to_write number of bytes in the command to be sent
         * @param[out] bytes_written number of bytes written to the device
         */
        int write(
            unsigned char * const in_buffer,
            int bytes_to_write,
            int &bytes_written);

        /**
         * Checks the device to see if an event response has been sent.
         * 
         * @returns key_state if no event was found, NO_KEY_DETECTED is returned.
         * Otherwise, it responds with FOUND_KEY_UP, or FOUND_KEY_DOWN.
         */
        key_state check_for_keypress();

        /**
         * Removes the current response from the internal buffer
         */
        void remove_current_response();

        /**
         * Gets the last response from the device
         *
         * @param[out] port device port the response is from.  usually 0
         * @param[out] key Which key was pressed.
         * @param[out] was_pressed true if a key was pressed, false otherwise
         * @param[out] reaction_time reaction time read from the internal
         * device timer.
         */
        void get_current_response(
            int &port, int &key, bool &was_pressed, int &reaction_time);

        /**
         * Number of keys currently being held down
         */
        int get_number_of_keys_down();

        /**
         * Send an XID command to the device.
         *
         * XID commands are documented at http://www.cedrus.com/xid/
         *
         * @param[in] in_command command to send to the device
         * @param[in] num_bytes number of bytes in the command
         * @param[out] out_response response from the device
         * @param[in] max_out_response_size maximum allowed response size
         * @param[in] expected_bytes_rec number of bytes expected
         * @param[in] timeout command timeout.  Defaults to 100ms
         * @param[in] command_delay Some devices need an additional delay
         * between receiving a request and sending a response. Set the delay
         * here.  It defaults to 0.
         */
        int send_xid_command(
            const char in_command[],
            int num_bytes,
            char out_response[],
            int max_out_response_size,
            int expected_bytes_rec,
            int timeout = 100,
            int command_delay = 0);

        /**
         * Sets whether or not the device needs an inter-byte delay.
         */
        void set_needs_interbyte_delay(bool need_delay = true);

        /**
         * Sets the digital output prefix
         * 
         * @param[in] prefix A single character.  This should be 'a' for XID
         * response devices, or 'm' for StimTracker devices.
         */
        void set_digital_out_prefix(char prefix);

        /**
         * Raise digital output lines on the StimTracker device.
         * 
         * @param[in] lines This is a bitmask used to specify the lines
         * to be raised on the device. Each of the 8 bits in the integer
         * specifies a line.  If bits 0 and 7 are 1, lines 1 and 8 are raised.
         * @param[in] leave_remaining_lines boolean value of whether or not to
         * keep the current line state when applying the new bitmask.
         */
        void set_digital_output_lines(
            unsigned int lines,
            bool leave_remaining_lines = false);

        /**
         * Clear digital output lines on the StimTracker device.
         *
         * @param[in] lines This is a bitmask used to specify the lines
         * to be raised on the device. Each of the 8 bits in the integer
         * specifies a line.  If bits 0 and 7 are 1, lines 1 and 8 are raised.
         * @param[in] leave_remaining_lines boolean value of whether or not to
         * keep the current line state when applying the new bitmask.
         */ 
        void clear_digital_output_lines(
            unsigned int lines,
            bool leave_remaining_lines = false);

    private:

    	enum { OS_FILE_ERROR = -1 };
        int setup_com_port();
        //void setup_dcb(DCB &dcb) const;
        //void setup_timeouts(COMMTIMEOUTS &ct) const;

        key_state xid_input_found();
        unsigned long GetTickCount();

        port_settings_t port_settings_;
        int in_buffer_size_;
        int out_buffer_size_;
        int model_id_;
        char port_name_[40];
        //HANDLE device_id_;
      
        int delay_;

        enum {INPUT_BUFFER_SIZE = 1000};
        enum {XID_PACKET_SIZE = 6};
        enum {INVALID_PACKET_INDEX = -1};
        enum {KEY_RELEASE_BITMASK = 0x10};
        
        int bytes_in_buffer_;
        unsigned char input_buffer_[INPUT_BUFFER_SIZE];
        unsigned char invalid_port_bits_;

        const double timer_rate_;

        int first_valid_xid_packet_;
        int num_keys_down_;
        int last_resp_port_;
        int last_resp_key_;
        bool last_resp_pressed_;
        int last_resp_rt_;

        char lines_state_;
        char set_lines_cmd_[4];

        bool needs_interbyte_delay_;

        struct DarwinConnPimpl;
        boost::shared_ptr<DarwinConnPimpl> m_darwinPimpl;

        struct WindowsConnPimpl;
        boost::shared_ptr<WindowsConnPimpl> m_winPimpl;
    };
} // namespace cedrus

#endif // XID_CON_T_H
