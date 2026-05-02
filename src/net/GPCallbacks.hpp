#pragma once

#include <gamespy/GP/gp.h>

namespace Net {

void GPReversBuddiesListCallback(GPConnection* connection, void* arg,
                                 void* param);

void SetGPError(GPResult result);
} // namespace Net
