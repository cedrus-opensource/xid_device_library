#include "ResponseManager.h"

#include "Connection.h"
#include "DeviceConfig.h"
#include "constants.h"

#include "CedrusAssert.h"

#include <boost/bind.hpp>

namespace
{
    bool device_is_7byte_lumina3g_21
    (
        const int minorFirmwareVer,
        std::shared_ptr<const Cedrus::DeviceConfig> devConfig
    )
    {
        const bool prod_is_lumina3g_21 = (devConfig->GetProductID() == Cedrus::PRODUCT_ID_LUMINA);
        const bool maj_fw_is_lumina3g_21 = (devConfig->GetMajorVersion() == 2);
        const bool min_fw_is_lumina3g_21 = (minorFirmwareVer == 1);

        return (prod_is_lumina3g_21 && maj_fw_is_lumina3g_21 && min_fw_is_lumina3g_21);
    }
}

Cedrus::ResponseManager::ResponseManager( const int minorFirmwareVer, std::shared_ptr<const DeviceConfig> devConfig )
    : m_BytesInBuffer(0),
      m_XIDPacketIndex(INVALID_PACKET_INDEX),
      m_numKeysDown(0),
      m_packetSize(XID_PACKET_SIZE),
      m_ResponseParsingFunction( boost::bind( &Cedrus::ResponseManager::XidInputFound, this, _1 ) ),
      m_respDevConfig(devConfig)
{
    m_packetSize = devConfig->IsStimTracker2() ? ST2_PACKET_SIZE : XID_PACKET_SIZE;
    for(int i = 0; i < m_packetSize; ++i)
    {
        m_InputBuffer[i] = '\0';
    }

    /*
     This is a sort of a rough outline of a way to maintain bugwards compatibility
     going forward. At the time of writing, the Lumina 3G firmware version 2.1 is
     bugged and is producing 7-byte xid response packets, which is weird and bad
     for a variety of reasons. Our parsing can handle snipping off the extra byte,
     but since the 7th byte isn't at the end of the packet, this messes with the
     timestamp, making the problem described in the comment at the top of
     Cedrus::ResponseManager::XidInputFound much, much worse.
    */
    if ( devConfig && device_is_7byte_lumina3g_21( minorFirmwareVer, devConfig ) )
    {
        m_ResponseParsingFunction = boost::bind( &Cedrus::ResponseManager::XIDInputFoundLumina3G_21, this, _1 );
    }
    else if (devConfig->IsStimTracker2())
    {
        m_ResponseParsingFunction = boost::bind(&Cedrus::ResponseManager::ST2InputFound, this, _1);
    }
}

Cedrus::ResponseManager::~ResponseManager()
{
}

bool Cedrus::ResponseManager::XidInputFound(Response &res)
{
    bool input_found = false;
    m_XIDPacketIndex = INVALID_PACKET_INDEX;

    if (m_InputBuffer[0] == 'k' && ((m_InputBuffer[1] & INVALID_PORT_BITS) == 0))
    {
        m_XIDPacketIndex = 0;

        // If this is false, all is not yet lost. Looks like we have a partial XID
        // packet for whatever reason. We don't need to adjust the buffer in any way,
        // we'll just read in fewer bytes on the next pass in an attempt to finish it.
        if (m_BytesInBuffer == XID_PACKET_SIZE)
        {
            res.wasPressed = (m_InputBuffer[1] & KEY_RELEASE_BITMASK) == KEY_RELEASE_BITMASK;
            res.port = m_InputBuffer[1] & 0x0F;
            res.key = (m_InputBuffer[1] & 0xE0) >> 5;

            res.reactionTime = AdjustEndiannessCharsToUint
            (m_InputBuffer[2], m_InputBuffer[3], m_InputBuffer[4], m_InputBuffer[5]);

            input_found = true;
        }
    }

    if (m_XIDPacketIndex == INVALID_PACKET_INDEX)
    {
        // Something is amiss, but perhaps the packet is later in the buffer?
        for (int i = 1; i < XID_PACKET_SIZE; ++i)
        {
            if (m_InputBuffer[i] == 'k')
            {
                // We have found a k later in the buffer. Setting the index will
                // allow AdjustBufferForPacketRecovery() to adjust the buffer
                // accordingly in preparation for recovery.
                m_XIDPacketIndex = i;
                break;
            }
        }

        // We never want to be here. This means that we either have random junk
        // in the buffer, or that we just ate some other valid output from the
        // device that was not meant for us. That can have mystifying consequences
        // elsewhere, so take note.
        CEDRUS_ASSERT(m_XIDPacketIndex == INVALID_PACKET_INDEX, "ResponseManager just read something inappropriate from the device buffer!");
    }

    // If m_XIDPacketIndex == INVALID_PACKET_INDEX, recovery is impossible, as
    // we have no point of reference to attempt to reconstruct a response packet.
    // If m_XIDPacketIndex is 0, there is no need to adjust the buffer for recovery.
    // Otherwise, we need to flush some unwanted bytes from the buffer.
    if (m_XIDPacketIndex != INVALID_PACKET_INDEX && m_XIDPacketIndex != 0)
        AdjustBufferForPacketRecovery();

    if (m_BytesInBuffer == XID_PACKET_SIZE)
    {
        /*
        This is the end of our journey. We have either successfully cobbled together an XID packet
        or we have failed utterly. Enjoy the complimentary assert if that was the case, and keep in
        mind that we cannot process responses from XID devices that have been on for 4.66 hours.
        If you're fairly certain 4.66 hours haven't passed and/or if you're getting SOME responses,
        there's probably something terribly wrong!
        */

        CEDRUS_ASSERT(input_found, "We failed to get a response from an XID device! See comments for why it may have failed.");

        // Either way, we're starting fresh.
        m_BytesInBuffer = 0;

        // We need to clean out the buffer. If the next read doesn't grab a full
        // XID_PACKET_SIZE bytes, things can get REALLY weird.
        memset(m_InputBuffer, 0x00, XID_PACKET_SIZE);
    }

    return input_found;
}

bool Cedrus::ResponseManager::ST2InputFound(Response &res)
{
    bool input_found = false;
    m_XIDPacketIndex = INVALID_PACKET_INDEX;

    if (m_BytesInBuffer == ST2_PACKET_SIZE && m_InputBuffer[0] == 'o' && m_InputBuffer[ST2_PACKET_SIZE - 1] == 0)
    {
        m_XIDPacketIndex = 0;

        res.port = m_InputBuffer[1];
        res.key = m_InputBuffer[2];
        res.wasPressed = m_InputBuffer[3] == '1';

        res.reactionTime = AdjustEndiannessCharsToUint
        (m_InputBuffer[4], m_InputBuffer[5], m_InputBuffer[6], m_InputBuffer[7]);

        input_found = true;
    }
    else
    {
        for (int i = 1; i < m_BytesInBuffer; ++i)
        {
            if (m_InputBuffer[i] == 'o')
            {
                m_XIDPacketIndex = i;
                break;
            }
        }
    }

    if (m_XIDPacketIndex != INVALID_PACKET_INDEX && m_XIDPacketIndex != 0)
        AdjustBufferForPacketRecovery();

    if (m_BytesInBuffer == ST2_PACKET_SIZE)
    {
        m_BytesInBuffer = 0;
        memset(m_InputBuffer, 0x00, ST2_PACKET_SIZE);
    }

    return input_found;
}

bool Cedrus::ResponseManager::XIDInputFoundLumina3G_21(Response &res)
{
    bool input_found = false;
    m_XIDPacketIndex = INVALID_PACKET_INDEX;

    if (m_InputBuffer[0] == 'k' && ((m_InputBuffer[1] & INVALID_PORT_BITS) == 0))
    {
        m_XIDPacketIndex = 0;

        if (m_BytesInBuffer == XID_PACKET_SIZE)
        {
            res.wasPressed = (m_InputBuffer[1] & KEY_RELEASE_BITMASK) == KEY_RELEASE_BITMASK;
            res.port = m_InputBuffer[1] & 0x0F;
            res.key = (m_InputBuffer[1] & 0xE0) >> 5;

            CEDRUS_ASSERT(res.port < 6, "As of March 2015, no devices should be able to receive input from more than 5 ports!");

            res.reactionTime = AdjustEndiannessCharsToUint
            (m_InputBuffer[2], m_InputBuffer[3], m_InputBuffer[4], m_InputBuffer[5]);

            input_found = true;
        }
    }

    if (m_XIDPacketIndex == INVALID_PACKET_INDEX)
    {
        for (int i = 1; i < XID_PACKET_SIZE; ++i)
        {
            if (m_InputBuffer[i] == 'k')
            {
                m_XIDPacketIndex = i;
                break;
            }
        }
    }

    if (m_XIDPacketIndex != INVALID_PACKET_INDEX && m_XIDPacketIndex != 0)
        AdjustBufferForPacketRecovery();

    if (m_BytesInBuffer == XID_PACKET_SIZE)
    {
        CEDRUS_ASSERT(input_found, "We failed to get a response from an XID device! See comments for why it may have failed.");

        m_BytesInBuffer = 0;

        // We need to clean out the buffer. If the next read doesn't grab a full
        // XID_PACKET_SIZE bytes, things can get REALLY weird.
        memset(m_InputBuffer, 0x00, XID_PACKET_SIZE);
    }

    return input_found;
}

void Cedrus::ResponseManager::CheckForKeypress(std::shared_ptr<Connection> portConnection)
{
    DWORD bytes_read = 0;
    Response res;
    bool response_found = false;

    // The amount of bytes read is variable as a part of a process that attempts to recover
    // malformed xid packets. The process will not work 100% reliably, but it's the best we
    // can do given the protocol.
    portConnection->Read(&m_InputBuffer[m_BytesInBuffer], (m_packetSize - m_BytesInBuffer), &bytes_read);

    if(bytes_read > 0)
    {
        m_BytesInBuffer += bytes_read;
        response_found = m_ResponseParsingFunction(res);
    }

    if(response_found)
    {
        res.key = m_respDevConfig->GetMappedKey(res.port, res.key);

        m_responseQueue.push(res);

        if (res.wasPressed)
            ++m_numKeysDown;
        else
            --m_numKeysDown;
    }
}

void Cedrus::ResponseManager::AdjustBufferForPacketRecovery()
{
    CEDRUS_ASSERT( m_XIDPacketIndex != INVALID_PACKET_INDEX, "We're about to crash, because someone misused this function!" );

    unsigned char *dest_char = m_InputBuffer;
    unsigned char *src_char  = &m_InputBuffer[m_XIDPacketIndex];

    // Move the k and everything after it to the beginning of the buffer.
    for(int i = m_XIDPacketIndex; i < ST2_PACKET_SIZE; ++i, ++dest_char, ++src_char)
    {
        *dest_char = *src_char;
    }

    // CheckForKeypress() uses m_BytesInBuffer to know how many bytes
    // to read, so we're setting it appropriately here.
    m_BytesInBuffer = m_BytesInBuffer - m_XIDPacketIndex;

    // Clean out the rest of the buffer past the partial packet
    memset( &m_InputBuffer[m_BytesInBuffer], 0x00, (ST2_PACKET_SIZE - m_BytesInBuffer) );
}

bool Cedrus::ResponseManager::HasQueuedResponses() const
{
    return !m_responseQueue.empty();
}

// If there are no processed responses, this will return a default response
// with most of the properties resolving to -1 and such. To avoid that, verify
// that responses exist with HasQueuedResponses()
Cedrus::Response Cedrus::ResponseManager::GetNextResponse()
{
    Response res;
    if ( HasQueuedResponses() )
    {
        res = m_responseQueue.front();
        m_responseQueue.pop();
    }

    return res;
}

unsigned int Cedrus::ResponseManager::GetNumberOfKeysDown() const
{
    return m_numKeysDown;
}

void Cedrus::ResponseManager::ClearResponseQueue()
{
    // This is how you clear a queue, evidently.
    m_responseQueue = std::queue<Response>();

    // We probably want to zero out the keypress counter.
    m_numKeysDown = 0;
}
