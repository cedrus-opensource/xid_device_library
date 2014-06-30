#include "response_mgr.h"
#include "xid_con_t.h"
#include "xid_device_config_t.h"
#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

cedrus::response_mgr::response_mgr( void )
    : bytes_in_buffer_(0),
      num_keys_down_(0)
{
    for(int i = 0; i < INPUT_BUFFER_SIZE; ++i)
    {
        input_buffer_[i] = '\0';
    }
}

cedrus::response_mgr::~response_mgr( void )
{
}

cedrus::key_state cedrus::response_mgr::xid_input_found( response &res )
{
    // Wisdom from Hisham, circa Aug 13, 2005. Point C is especially relevant.
    //
    // First, we try to determine if we have a valid packet. Our options
    // are limited; here what we look for:
    //
    // a. The first byte must be the letter 'k'
    //
    // b. Bits 0-3 of the second byte indicate the port number.  Lumina
    //    and RB-x30 models use only bits 0 and 1; SV-1 uses only bits
    //    1 and 2.  We check that the two remaining bits are zero.
    //
    // c. The remaining four bytes provide the reaction time.  Here, we'll
    //    assume that the RT will never exceed 4.66 hours :-) and verify
    //    that the last byte is set to 0.

    key_state input_found = NO_KEY_DETECTED;

    if(bytes_in_buffer_ >= 6)
    {
        for(int i = 0; i <= bytes_in_buffer_; ++i)
        {
            if( input_buffer_[i] == 'k' &&
                (input_buffer_[i+1] & INVALID_PORT_BITS) == 0)
            {
                res.was_pressed = (
                    input_buffer_[i+1] & KEY_RELEASE_BITMASK) ==
                    KEY_RELEASE_BITMASK;
                res.port = input_buffer_[i+1] & 0x0F;
                res.key = (input_buffer_[i+1] & 0xE0) >> 5;

                // get reaction time
                union {
                    int as_int;
                    char as_char[4];// what if it were a 64 bit int? would this still work?
                } rt;

                for(int n = 0; n < 4; ++n)
                {
                    // would this work on PPC? due to flipped endianness?
                    rt.as_char[n] = input_buffer_[i+2+n];
                }
                
                res.reaction_time = rt.as_int;

                input_found = static_cast<key_state>(FOUND_KEY_DOWN + !res.was_pressed);

                if(input_found == FOUND_KEY_DOWN)
                    num_keys_down_++;
                else
                    num_keys_down_--;

                // We're done. There's only ever one packet.
                break;
            }
        }
    }

    return input_found;
}

void cedrus::response_mgr::check_for_keypress(boost::shared_ptr<xid_con_t> port_connection, boost::shared_ptr<xid_device_config_t> dev_config)
{
    int bytes_read = 0;
    int status = NO_ERR;
    response res;
    key_state response_found = NO_KEY_DETECTED;

    status = port_connection->read(input_buffer_, 6, bytes_in_buffer_);

    if(bytes_in_buffer_ > 0)
    {
        response_found = xid_input_found(res);
    }

    if(response_found != cedrus::NO_KEY_DETECTED)
    {
        res.key = dev_config->get_mapped_key(res.port, res.key);
 
        response_queue_.push(res);
    }
}

int cedrus::response_mgr::get_number_of_keys_down()
{
    return num_keys_down_;
}

bool cedrus::response_mgr::has_queued_responses() const
{
    return !response_queue_.empty();
}

cedrus::response cedrus::response_mgr::get_next_response()
{
    response res = response_queue_.front();
    response_queue_.pop();
    return res;
}