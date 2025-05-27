#include "RKNetController.hpp"

#include <dwc/common/dwc_init.h>

// Temporary, no idea where this should live
extern const char* s_mariokartwii = "mariokartwii";

namespace RKNet {

void RKNetController::createStaticInstance(EGG::Heap *heap) {
  if (!spInstance) {
    spInstance = new (heap, 0x4) RKNetController(heap);
    DWC_Init(DWC_SVR_RELEASE, s_mariokartwii, (u32) 0x524d434a, alloc, free);
  }
}

}
