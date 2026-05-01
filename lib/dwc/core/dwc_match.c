#pragma once

#include <rk_types.h>

#include <dwc/core/dwci_match.h>
#include <gamespy/GP/gp.h>

// again splits are wrong, just a very rough guess.
// documenting types and functions is more important than having 100% correct
// splits atp.

#ifdef __cplusplus
extern "C" {
#endif

u32 DWC_GetGroupId() {
  if (s_MatchContext == 0) {
    return 0;
  }
  return s_MatchContext->groupId;
}

#ifdef __cplusplus
}
#endif
