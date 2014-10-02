#include "response_mgr.h"

#include "interface_xid_con.h"
#include "xid_device_config_t.h"
#include "xid_glossary.h"
#include "constants.h"

#include "CedrusAssert.h"

cedrus::response_mgr::response_mgr( void )
    : m_bytes_in_buffer(0),
      m_xid_packet_index(INVALID_PACKET_INDEX)
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
    m_xid_packet_index = INVALID_PACKET_INDEX;

    if( m_input_buffer[0] == 'k' && ((m_input_buffer[1] & INVALID_PORT_BITS) == 0) )
    {
        m_xid_packet_index = 0;

        // If this is false, all is not yet lost. Looks like we have a partial XID
        // packet for whatever reason. We don't need to adjust the buffer in any way,
        // we'll just read in fewer bytes on the next pass in an attempt to finish it.
        if(m_bytes_in_buffer == XID_PACKET_SIZE && m_input_buffer[5] == '\0')
        {
            res.was_pressed = (m_input_buffer[1] & KEY_RELEASE_BITMASK) == KEY_RELEASE_BITMASK;
            res.port = m_input_buffer[1] & 0x0F;
            res.key = (m_input_buffer[1] & 0xE0) >> 5;

            res.reaction_time = xid_glossary::adjust_endianness_chars_to_uint
                ( m_input_buffer[2], m_input_buffer[3], m_input_buffer[4], m_input_buffer[5] );

            input_found = static_cast<key_state>(FOUND_KEY_DOWN + !res.was_pressed);
        }
    }
    else
    {
        // Something is amiss, but perhaps the packet is later in the buffer?
        for(int i = 1; i < XID_PACKET_SIZE; ++i)
        {
            if ( m_input_buffer[i] == 'k' )
            {
                // We have found a k later in the buffer. Setting the index will
                // allow adjust_buffer_for_packet_recovery() to adjust the buffer
                // accordingly in preparation for recovery.
                m_xid_packet_index = i;
                break;
            }
        }

        // We never want to be here. This means that we either have random junk
        // in the buffer, or that we just ate some other valid output from the
        // device that was not meant for us. That can have mystifying consequences
        // elsewhere, so take note.
        CEDRUS_ASSERT( m_xid_packet_index == INVALID_PACKET_INDEX, "response_mgr just read something inappropriate from the device buffer!" );
    }

    if ( m_xid_packet_index == 0 && m_bytes_in_buffer == XID_PACKET_SIZE )
    {
        /* 
        This is the end of our journey. We have either successfully cobbled together an XID packet
        or we have failed utterly. Enjoy the complimentary assert if that was the case, and keep in
        mind that we cannot process responses from XID devices that have been on for 4.66 hours.
        If you're fairly certain 4.66 hours haven't passed and/or if you're getting SOME responses,
        there's probably something terribly wrong!
        */

        CEDRUS_ASSERT( input_found != NO_KEY_DETECTED, "We failed to get a response from an XID device! See comments for why it may have failed." );
        m_bytes_in_buffer = 0; // Either way, we're starting fresh.
    }

    // If m_xid_packet_index == INVALID_PACKET_INDEX, recovery is impossible, as
    // we have no point of reference to attempt to reconstruct a response packet.
    // If m_xid_packet_index is 0, there is no need to adjust the buffer for recovery.
    // Otherwise, we need to flush some unwanted bytes from the buffer.
    if( m_xid_packet_index != INVALID_PACKET_INDEX && m_xid_packet_index != 0 )
        adjust_buffer_for_packet_recovery();

    return input_found;
}

void cedrus::response_mgr::check_for_keypress(boost::shared_ptr<interface_xid_con> port_connection, boost::shared_ptr<const xid_device_config_t> dev_config)
{
    int bytes_read = 0;
    response res;
    key_state response_found = NO_KEY_DETECTED;

    // The amount of bytes read is variable as a part of a process that attempts to recover
    // malformed xid packets. The process will not work 100% reliably, but it's the best we
    // can do given the protocol.
    port_connection->read(&m_input_buffer[m_bytes_in_buffer], (XID_PACKET_SIZE - m_bytes_in_buffer), &bytes_read);

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
    CEDRUS_ASSERT( m_xid_packet_index != INVALID_PACKET_INDEX, "We're about to crash, because someone misused this function!" );

    unsigned char *dest_char = m_input_buffer;
    unsigned char *src_char  = &m_input_buffer[m_xid_packet_index];

    // Move the k and everything after it to the beginning of the buffer.
    for(int i = m_xid_packet_index; i < XID_PACKET_SIZE; ++i, ++dest_char, ++src_char)
    {
        *dest_char = *src_char;
    }

    // check_for_keypress() uses m_bytes_in_buffer to know how many bytes
    // to read, so we're setting it appropriately here.
    m_bytes_in_buffer = m_bytes_in_buffer - m_xid_packet_index;
}

bool cedrus::response_mgr::has_queued_responses() const
{
    return !response_queue_.empty();
}

cedrus::response cedrus::response_mgr::get_next_response()
{
    response res;
    if ( !response_queue_.empty() )
    {
        res = response_queue_.front();
        response_queue_.pop();
    }

    return res;
}
