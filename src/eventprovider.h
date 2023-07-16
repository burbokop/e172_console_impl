#pragma once

#include <e172/abstracteventprovider.h>

namespace e172::impl::console {

class StdinSocket;

class EventProvider : public e172::AbstractEventProvider
{
public:
    EventProvider(std::ostream &log);
    ~EventProvider();

    // AbstractEventHandler interface
public:
    std::optional<e172::Event> pullEvent() override;

private:
    void bufferize();

private:
    std::unique_ptr<StdinSocket> m_stdinSocket;
    std::queue<Event> m_events;
    std::ostream &m_log;
};

} // namespace e172::impl::console
