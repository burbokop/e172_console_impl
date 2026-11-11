// Copyright 2023 Borys Boiko

#include "socket.h"

#include <cstring>
#include <memory>
#include <vector>
#include <iostream>

#ifdef _WIN32

#include <e172/net/socket.h>
#include <io.h>
#include <windows.h>

#else

#include <e172/net/linux/socket.h>
#include <termios.h>

#endif

namespace e172::impl::console {

#ifdef _WIN32

class WindowsStdinSocket : public e172::Socket {
public:
    WindowsStdinSocket()
        : m_handle(GetStdHandle(STD_INPUT_HANDLE))
    {
    }

    ~WindowsStdinSocket() override = default;

    std::size_t bufferize() override
    {
        if (m_handle == INVALID_HANDLE_VALUE || m_handle == nullptr) {
            return 0;
        }

        DWORD origMode = 0;
        const bool isConsole = GetConsoleMode(m_handle, &origMode) != 0;
        if (isConsole) {
            DWORD rawMode = origMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
            SetConsoleMode(m_handle, rawMode);
        }

        std::size_t totalRead = 0;
        DWORD available = 0;

        if (isConsole) {
            // Drain pending key-down character events from the Windows console
            DWORD numEvents = 0;
            while (GetNumberOfConsoleInputEvents(m_handle, &numEvents) && numEvents > 0) {
                INPUT_RECORD record {};
                DWORD eventsRead = 0;
                if (!ReadConsoleInputA(m_handle, &record, 1, &eventsRead) || eventsRead == 0) {
                    break;
                }
                if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
                    char ch = record.Event.KeyEvent.uChar.AsciiChar;
                    if (ch != '\0') {
                        m_buffer.push_back(ch);
                        ++totalRead;
                    }
                }
            }
        } else if (PeekNamedPipe(m_handle, nullptr, 0, nullptr, &available, nullptr) && available > 0) {
            // Non-blocking read when stdin is a pipe/file
            const std::size_t offset = m_buffer.size();
            m_buffer.resize(offset + available);
            DWORD bytesRead = 0;
            if (ReadFile(m_handle, m_buffer.data() + offset, available, &bytesRead, nullptr)) {
                m_buffer.resize(offset + bytesRead);
                totalRead = bytesRead;
            } else {
                m_buffer.resize(offset);
            }
        }

        if (isConsole) {
            SetConsoleMode(m_handle, origMode);
        }

        return totalRead;
    }

    std::size_t bytesAvailable() const override { return m_buffer.size(); }

    bool isConnected() const override { return m_handle != INVALID_HANDLE_VALUE && m_handle != nullptr; }

    std::size_t peek(
        Byte* data,
        std::size_t size) const override
    {
        const std::size_t toCopy = (size < m_buffer.size()) ? size : m_buffer.size();
        if (toCopy > 0) {
            std::memcpy(data, m_buffer.data(), toCopy);
        }
        return toCopy;
    }

    std::size_t read(
        Byte* data,
        std::size_t size) override
    {
        const std::size_t toCopy = peek(data, size);
        if (toCopy > 0) {
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + static_cast<std::ptrdiff_t>(toCopy));
        }
        return toCopy;
    }

    // Write interface
public:
    std::size_t write(const Byte *bytes, std::size_t size) override {
        std::cout << "Writing not supported" << std::endl;
        std::abort();
    }

    void flush() override {
        std::cout << "Flushing not supported" << std::endl;
        std::abort();
    }

private:
    HANDLE m_handle;
    std::vector<char> m_buffer;
};

#else

class LinuxStdinSocket : public LinuxSocket {
public:
    LinuxStdinSocket()
        : e172::LinuxSocket(STDIN_FILENO)
    {
        setFdNonBlockingFlag(fd(), false);
    }

    ~LinuxStdinSocket() {}

    std::size_t bufferize() override
    {
        setFdNonBlockingFlag(fd(), true);

        termios orgAttr, newAttr;

        //-----  store old settings -----------
        auto res = tcgetattr(fd(), &orgAttr);

        if (res != 0) {
            std::cerr << "Error during bufferize: " << res;
            std::abort();
        }

        //---- set new terminal parms --------
        std::memcpy(&newAttr, &orgAttr, sizeof(newAttr));
        newAttr.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
        newAttr.c_cc[VTIME] = 10;
        newAttr.c_cc[VMIN] = 1;
        tcsetattr(fd(), TCSANOW, &newAttr);

        const auto size = e172::LinuxSocket::bufferize();

        //------  restore old settings ---------
        res = tcsetattr(fd(), TCSANOW, &orgAttr);

        if (res != 0) {
            std::cerr << "Error during bufferize: " << res;
            std::abort();
        }

        setFdNonBlockingFlag(fd(), false);

        return size;
    }
};

#endif

std::unique_ptr<Socket> createStdinSocket() noexcept
{
#ifdef _WIN32
    return std::make_unique<WindowsStdinSocket>();
#else
    return std::make_unique<LinuxStdinSocket>();
#endif
}

} // namespace e172::impl::console
