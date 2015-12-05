#include "pebble_process_info.h"
#include "src/resource_ids.auto.h"

const PebbleProcessInfo __pbl_app_info __attribute__ ((section (".pbl_header"))) = {
  .header = "PBLAPP",
  .struct_version = { PROCESS_INFO_CURRENT_STRUCT_VERSION_MAJOR, PROCESS_INFO_CURRENT_STRUCT_VERSION_MINOR },
  .sdk_version = { PROCESS_INFO_CURRENT_SDK_VERSION_MAJOR, PROCESS_INFO_CURRENT_SDK_VERSION_MINOR },
  .process_version = { 1, 0 },
  .load_size = 0xb6b6,
  .offset = 0xb6b6b6b6,
  .crc = 0xb6b6b6b6,
  .name = "Big Time+Date Colour",
  .company = "Kevin Grant",
  .icon_resource_id = RESOURCE_ID_big_timedate_menu,
  .sym_table_addr = 0xA7A7A7A7,
  .flags = PROCESS_INFO_WATCH_FACE,
  .num_reloc_entries = 0xdeadcafe,
  .uuid = { 0x0A, 0xB4, 0x47, 0x72, 0x4F, 0xC2, 0x45, 0xF5, 0x98, 0x1D, 0x00, 0x14, 0xF2, 0xF5, 0xF7, 0xB7 },
  .virtual_size = 0xb6b6
};
