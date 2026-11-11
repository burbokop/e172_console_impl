// Copyright 2023 Borys Boiko

#pragma once

#include <e172/abstracteventprovider.h>
#include <e172/net/socket.h>
#include <memory>
#include <queue>

namespace e172::impl::console {

class EventProvider : public e172::AbstractEventProvider {
public:
    EventProvider(std::ostream& log);
    ~EventProvider();

    // AbstractEventHandler interface
public:
    std::optional<e172::Event> pullEvent() override;

private:
    void bufferize();

private:
    std::unique_ptr<Socket> m_stdinSocket;
    std::queue<Event> m_events;
    std::ostream& m_log;
};

} // namespace e172::impl::console
