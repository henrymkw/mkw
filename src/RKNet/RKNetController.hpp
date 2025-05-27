#pragma once

#include <egg/core/eggHeap.hpp>
#include <dwc/common/dwc_memfunc.h>
#include <rvl/os/osMutex.h>

#include <rk_types.h>
#include <rk_common.h>

#include <decomp.h>

namespace RKNet {

class RKNetController {
public:
  static void createStaticInstance(EGG::Heap *heap);

  RKNetController(EGG::Heap *heap);

  static void* alloc(DWCAllocType name, u32 size, int align);

  static void free(DWCAllocType name, void* ptr, u32 size);

private:
  static RKNetController* spInstance;

  u8 _0000[0x0008 - 0x0000];
  OSMutex m_mutex;
  EGG::Heap* m_heap;
  u8 _0024[0x0028 - 0x0024];
  u32 m_connectionState; // TODO: Make enum
  u8 _002c[0x29c8 - 0x002c];
};
static_assert(sizeof(RKNetController) == 0x29c8);

}
