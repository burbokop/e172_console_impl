#include "eventprovider.h"

#include <cstring>
#include <curses.h>
#include <e172/net/linux/socket.h>
#include <termios.h>

#include <fstream>
namespace e172::impl::console {

namespace {

namespace escapeseq {

constexpr Byte BeginByte = 0x1B;
constexpr Byte SecondByte = 0x5b;

std::optional<std::size_t> findFirstTerminatingByte(Byte *arr,
                                                    std::size_t len,
                                                    std::size_t startIndex)
{
    for (std::size_t i = startIndex; i < len; ++i) {
        /// final byte (F) from the range 0x30–0x7E.
        if (arr[i] >= 0x30 && arr[i] <= 0x7E) {
            return i;
        }
    }
    return std::nullopt;
}

} // namespace escapeseq

e172::Scancode scancodeFromSym(std::uint8_t sym, bool esc)
{
    switch (sym) {
    case 0x00:
        return e172::ScancodeUnknown;
    case 0x01:
        todo(); //SOH;
    case 0x02:
        todo(); //STX;
    case 0x03:
        todo(); //ETX;
    case 0x04:
        todo(); //EOT;
    case 0x05:
        todo(); //ENQ;
    case 0x06:
        todo(); //ACK;
    case 0x07:
        todo(); //BEL;
    case 0x08:
        todo(); //BS;
    case 0x09:
        todo(); //HT;
    case 0x0A:
        return esc ? todo() : e172::ScancodeReturn; //LF;
    case 0x0B:
        todo(); //VT;
    case 0x0C:
        todo(); //FF;
    case 0x0D:
        todo(); //CR;
    case 0x0E:
        todo(); //SO;
    case 0x0F:
        todo(); //SI;
    case 0x10:
        todo(); //DLE;
    case 0x11:
        todo(); //DC1;
    case 0x12:
        todo(); //DC2;
    case 0x13:
        todo(); //DC3;
    case 0x14:
        todo(); //DC4;
    case 0x15:
        todo(); //NAK;
    case 0x16:
        todo(); //SYN;
    case 0x17:
        todo(); //ETB;
    case 0x18:
        todo(); //CAN;
    case 0x19:
        todo(); //EM;
    case 0x1A:
        todo(); //SUB;
    case escapeseq::BeginByte:
        return esc ? throw std::logic_error("already escaped") : e172::ScancodeEscape;
    case 0x1C:
        todo(); //FS;
    case 0x1D:
        todo(); //GS;
    case 0x1E:
        todo(); //RS;
    case 0x1F:
        todo(); //US;
    case 0x20:
        return esc ? todo() : e172::ScancodeSpace;
    case 0x21:
        return esc ? todo() : e172::Scancode1; //'!';
    case 0x22:
        return esc ? todo() : e172::ScancodeApostrophe; //'"';
    case 0x23:
        return esc ? todo() : e172::Scancode3; //'#';
    case 0x24:
        return esc ? todo() : e172::Scancode4; //'$';
    case 0x25:
        return esc ? todo() : e172::Scancode5; //'%';
    case 0x26:
        return esc ? todo() : e172::Scancode7; //'&';
    case 0x27:
        return esc ? todo() : e172::ScancodeApostrophe; //'\'';
    case 0x28:
        return esc ? todo() : e172::Scancode9; //'(';
    case 0x29:
        return esc ? todo() : e172::Scancode0; //')';
    case 0x2A:
        return esc ? todo() : e172::Scancode8; //'*';
    case 0x2B:
        return esc ? todo() : e172::ScancodeEquals; //'+';
    case 0x2C:
        return esc ? todo() : e172::ScancodeComma; //',';
    case 0x2D:
        return esc ? todo() : e172::ScancodeMinus; //'-';
    case 0x2E:
        todo(); // '.';
    case 0x2F:
        todo(); //'/';
    case 0x30:
        return esc ? todo() : e172::Scancode0; //'0';
    case 0x31:
        return esc ? todo() : e172::Scancode1; //'1';
    case 0x32:
        return esc ? todo() : e172::Scancode2; //'2';
    case 0x33:
        return esc ? todo() : e172::Scancode3; //'3';
    case 0x34:
        return esc ? todo() : e172::Scancode4; //'4';
    case 0x35:
        return esc ? todo() : e172::Scancode5; //'5';
    case 0x36:
        return esc ? todo() : e172::Scancode6; //'6';
    case 0x37:
        return esc ? todo() : e172::Scancode7; //'7';
    case 0x38:
        return esc ? todo() : e172::Scancode8; //'8';
    case 0x39:
        return esc ? todo() : e172::Scancode9; //'9';
    case 0x3A:
        todo(); //':';
    case 0x3B:
        todo(); //';';
    case 0x3C:
        todo(); //'<';
    case 0x3D:
        return esc ? todo() : e172::ScancodeEquals; //'=';
    case 0x3E:
        todo(); //'>';
    case 0x3F:
        todo(); //'?';
    case 0x40:
        todo(); //'@';
    case 0x41:
        return esc ? e172::ScancodeUp : e172::ScancodeA;
    case 0x42:
        return esc ? e172::ScancodeDown : e172::ScancodeB;
    case 0x43:
        return esc ? e172::ScancodeRight : e172::ScancodeC;
    case 0x44:
        return esc ? e172::ScancodeLeft : e172::ScancodeD;
    case 0x45:
        todo(); //'E';
    case 0x46:
        todo(); //'F';
    case 0x47:
        todo(); //'G';
    case 0x48:
        todo(); //'H';
    case 0x49:
        todo(); //'I';
    case 0x4A:
        todo(); //'J';
    case 0x4B:
        todo(); //'K';
    case 0x4C:
        todo(); //'L';
    case 0x4D:
        todo(); //'M';
    case 0x4E:
        todo(); //'N';
    case 0x4F:
        todo(); //'O';
    case 0x50:
        todo(); //'P';
    case 0x51:
        todo(); //'Q';
    case 0x52:
        todo(); //'R';
    case 0x53:
        todo(); //'S';
    case 0x54:
        todo(); //'T';
    case 0x55:
        todo(); //'U';
    case 0x56:
        todo(); //'V';
    case 0x57:
        todo(); //'W';
    case 0x58:
        todo(); //'X';
    case 0x59:
        todo(); //'Y';
    case 0x5A:
        todo(); //'Z';
    case 0x5B:
        todo(); //'[';
    case 0x5C:
        todo(); //'\\';
    case 0x5D:
        todo(); //']';
    case 0x5E:
        todo(); //'^';
    case 0x5F:
        todo(); //'_';
    case 0x60:
        return esc ? todo() : e172::ScancodeGrave; //'`';
    case 0x61:
        return esc ? todo() : e172::ScancodeA; //'a';
    case 0x62:
        todo(); //'b';
    case 0x63:
        todo(); //'c';
    case 0x64:
        return esc ? todo() : e172::ScancodeD; //'d';
    case 0x65:
        todo(); //'e';
    case 0x66:
        return esc ? todo() : e172::ScancodeF; //'f';
    case 0x67:
        todo(); //'g';
    case 0x68:
        todo(); //'h';
    case 0x69:
        todo(); //'i';
    case 0x6A:
        todo(); //'j';
    case 0x6B:
        todo(); //'k';
    case 0x6C:
        todo(); //'l';
    case 0x6D:
        todo(); //'m';
    case 0x6E:
        todo(); //'n';
    case 0x6F:
        todo(); //'o';
    case 0x70:
        todo(); //'p';
    case 0x71:
        return esc ? todo() : e172::ScancodeQ; //'q';
    case 0x72:
        todo(); //'r';
    case 0x73:
        return esc ? todo() : e172::ScancodeS; //'s';
    case 0x74:
        todo(); //'t';
    case 0x75:
        todo(); //'u';
    case 0x76:
        todo(); //'v';
    case 0x77:
        return esc ? todo() : e172::ScancodeW; //'w';
    case 0x78:
        todo(); //'x';
    case 0x79:
        todo(); //'y';
    case 0x7A:
        todo(); //'z';
    case 0x7B:
        todo(); //'{';
    case 0x7C:
        todo(); //'|';
    case 0x7D:
        todo(); //'}';
    case 0x7E:
        todo(); //'~';
    case 0x7F:
        todo(); //DEL;
    }
    return e172::ScancodeUnknown;
}

} // namespace

class StdinSocket : e172::LinuxSocket
{
public:
    StdinSocket()
        : e172::LinuxSocket(STDIN_FILENO)
    {
        setFdNonBlockingFlag(fd(), false);
    }

    ~StdinSocket() {}

    std::size_t bufferize() override
    {
        setFdNonBlockingFlag(fd(), true);

        termios orgAttr, newAttr;

        //-----  store old settings -----------
        auto res = tcgetattr(fd(), &orgAttr);
        assert(res == 0);
        //---- set new terminal parms --------
        memcpy(&newAttr, &orgAttr, sizeof(newAttr));
        newAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
        newAttr.c_cc[VTIME] = 10;
        newAttr.c_cc[VMIN] = 1;
        tcsetattr(fd(), TCSANOW, &newAttr);

        const auto size = e172::LinuxSocket::bufferize();

        //------  restore old settings ---------
        res = tcsetattr(fd(), TCSANOW, &orgAttr);
        assert(res == 0);

        setFdNonBlockingFlag(fd(), false);

        return size;
    }

    using e172::LinuxSocket::bytesAvailable;
    using e172::LinuxSocket::isConnected;
    using e172::LinuxSocket::peek;
    using e172::LinuxSocket::read;
};

EventProvider::EventProvider(std::ostream &log)
    : m_stdinSocket(std::make_unique<StdinSocket>())
    , m_log(log)
{}

EventProvider::~EventProvider() = default;

std::optional<e172::Event> EventProvider::pullEvent()
{
    bufferize();
    if (!m_events.empty()) {
        const auto e = m_events.front();
        m_events.pop();
        if (m_log.good()) {
            m_log << "event pulled -> " << e << std::endl;
        }

        return e;
    }
    return std::nullopt;
}

void EventProvider::bufferize()
{
    if (m_events.empty()) {
        m_stdinSocket->bufferize();
        if (m_stdinSocket->isConnected() && m_stdinSocket->bytesAvailable() > 0) {
            if (m_stdinSocket->bytesAvailable() > 2) {
                Byte syms[2];
                auto bytesRead = m_stdinSocket->peek(syms, sizeof(syms) / sizeof(syms[0]));
                assert(bytesRead == sizeof(syms) / sizeof(syms[0]));

                /// see 'General syntax of escape sequences' in https://en.wikipedia.org/wiki/ISO/IEC_2022
                if (syms[0] == escapeseq::BeginByte && syms[1] == escapeseq::SecondByte) {
                    e172::Byte seq[32];
                    bytesRead = m_stdinSocket->peek(seq, sizeof(seq) / sizeof(seq[0]));

                    if (m_log.good()) {
                        m_log << "ecs -> " << e172::Bytes(seq, seq + bytesRead) << std::endl;
                    }

                    if (const std::optional<std::size_t> terminatingByteIndex
                        = escapeseq::findFirstTerminatingByte(seq, bytesRead, 2)) {
                        const auto seqlen = *terminatingByteIndex + 1;
                        bytesRead = m_stdinSocket->read(seq, seqlen);
                        assert(bytesRead == seqlen);

                        if (m_log.good()) {
                            m_log << "term: " << *terminatingByteIndex << " (" << std::hex
                                  << static_cast<std::size_t>(seq[*terminatingByteIndex])
                                  << std::dec << ")" << std::endl;
                            m_log << "ecs: "
                                  << e172::Bytes(seq + 1, seq + *terminatingByteIndex + 1)
                                  << std::endl;
                        }

                        if (seqlen == 3) {
                            const auto scancode = scancodeFromSym(seq[2], true);
                            m_events.push(e172::Event::keyDown(scancode));
                            m_events.push(e172::Event::keyUp(scancode));
                            return;
                        }
                    }
                }
            }

            {
                Byte sym;
                m_stdinSocket->read(&sym, sizeof(sym));
                if (m_log.good()) {
                    m_log << "no ecs -> " << static_cast<std::size_t>(sym) << " ("
                          << static_cast<char>(sym) << ")" << std::endl;
                }
                const auto scancode = scancodeFromSym(sym, false);
                m_events.push(e172::Event::keyDown(scancode));
                m_events.push(e172::Event::keyUp(scancode));
            }
        }
    }
}

} // namespace e172::impl::console
