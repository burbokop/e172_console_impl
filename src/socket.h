// Copyright 2023 Borys Boiko

#pragma once

#include <e172/net/socket.h>
#include <memory>

namespace e172::impl::console {

std::unique_ptr<Socket> createStdinSocket() noexcept;

}
