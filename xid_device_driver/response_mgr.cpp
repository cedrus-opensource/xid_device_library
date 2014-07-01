#include "response_mgr.h"
#include "xid_con_t.h"
#include "xid_device_config_t.h"
#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

cedrus::response_mgr::response_mgr( void )
    : m_bytes_in_buffer(0),
      m_xid_packet_index(INVALID_PACKET_INDEX),
      num_keys_down_(0)
{
    for(int i = 0; i < XID_PACKET_SIZE; ++i)
    {
        m_input_buffer[i] = '\0';
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
    m_xid_packet_index = 0;

    // We will not give a partial packet the time of day.
    if(m_bytes_in_buffer == XID_PACKET_SIZE)
    {
        // A valid XID packet will meet all of these requirements.
        if( m_input_buffer[0] == 'k' &&
            (m_input_buffer[1] & INVALID_PORT_BITS) == 0 &&
            m_input_buffer[5] == '\0')
        {
            res.was_pressed = (m_input_buffer[1] & KEY_RELEASE_BITMASK) == KEY_RELEASE_BITMASK;
            res.port = m_input_buffer[1] & 0x0F;
            res.key = (m_input_buffer[1] & 0xE0) >> 5;

            // get reaction time
            union {
                int as_int;
                char as_char[4];// what if it were a 64 bit int? would this still work?
            } rt;

            for(int n = 0; n < 4; ++n)
            {
                // would this work on PPC? due to flipped endianness?
                rt.as_char[n] = m_input_buffer[2+n];
            }

            res.reaction_time = rt.as_int;

            input_found = static_cast<key_state>(FOUND_KEY_DOWN + !res.was_pressed);

            if(input_found == FOUND_KEY_DOWN)
                num_keys_down_++;
            else
                num_keys_down_--;
        }
        else // O-oh... Well, maybe it's a part of a packet? Look for a k later in the packet.
        {
            for(int i = 1; i < XID_PACKET_SIZE; ++i)
            {
                if ( m_input_buffer[i] == 'k' )
                {
                    m_xid_packet_index = i;
                    break;
                }
            }
        }
    }

    /*
        Three scenarios at this point and why we're claiming that we now have 0 bytes in buffer:

        1) We were satisfied with the packet. We have a fully formed response and everything is great.
        We're pretending the buffer is now empty.

        2) The packet was complete garbage and didn't even have a k in it! We're pretending that didn't
        happen and moving on with our lives.

        3) The packet was malformed, but had a k in it. At this point we cannot guarantee that we haven't
        missed any responses or that this one will ultimately register, but we can attempt to salvage it.
        adjust_buffer_for_packet_recovery() will do some math and set m_bytes_in_buffer to a different
        value to try and keep us on track.
    */
    m_bytes_in_buffer = 0;

    // True: that the packet was malformed, but can possibly still be salvaged. Proceed with recovery.
    // False: the packet was either valid or garbled beyond all hope. Don't worry about it.
    if( m_xid_packet_index != 0 )
        adjust_buffer_for_packet_recovery();

    return input_found;
}

void cedrus::response_mgr::check_for_keypress(boost::shared_ptr<xid_con_t> port_connection, boost::shared_ptr<xid_device_config_t> dev_config)
{
    int bytes_read = 0;
    int status = NO_ERR;
    response res;
    key_state response_found = NO_KEY_DETECTED;

    // The amount of bytes read is variable as a part of a process that attempts to recover
    // malformed xid packets. The process will not work 100% reliably, but it's the best we
    // can do given the protocol.
    status = port_connection->read(&m_input_buffer[m_bytes_in_buffer], (XID_PACKET_SIZE - m_bytes_in_buffer), bytes_read);

    if(bytes_read > 0)
    {
        m_bytes_in_buffer += bytes_read;
        response_found = xid_input_found(res);
    }

    if(response_found != cedrus::NO_KEY_DETECTED)
    {
        res.key = dev_config->get_mapped_key(res.port, res.key);
 
        response_queue_.push(res);
    }
}

void cedrus::response_mgr::adjust_buffer_for_packet_recovery()
{
    unsigned char *dest_char = m_input_buffer;
    unsigned char *src_char  = &m_input_buffer[m_xid_packet_index];

    // Move the k and everything after it to the beginning of the buffer.
    for(int i = m_xid_packet_index; i < XID_PACKET_SIZE; ++i, ++dest_char, ++src_char)
    {
        *dest_char = *src_char;
    }

    // check_for_keypress() uses m_bytes_in_buffer to know how many bytes
    // to read, so we're setting it to "however many it would take to
    // potentially finish an xid packet."
    m_bytes_in_buffer = XID_PACKET_SIZE - m_xid_packet_index;
    m_xid_packet_index = 0;
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
