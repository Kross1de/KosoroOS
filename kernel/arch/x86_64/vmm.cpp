#include "vmm.hpp"

extern "C" {
    alignas(vmm::PAGE_SIZE) uint64_t pml4[512];
    alignas(vmm::PAGE_SIZE) uint64_t pdp[512];
    alignas(vmm::PAGE_SIZE) uint64_t pd[512];
    alignas(vmm::PAGE_SIZE) uint64_t first_pt[512];
}
