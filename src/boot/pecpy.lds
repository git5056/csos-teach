ENTRY(_start)

SECTIONS {
    . = 0x1400;
    .text ALIGN(0x200): {
        *(*.text)
    }

    .data  ALIGN(0x200): {
        *(*.data)
    }
    
    .rdata  ALIGN(0x200): {
        *(*.rdata)
    }

    .reloc  ALIGN(0x200): {
        *(*.reloc)
    }

    .idata ALIGN(0x200): {
        *(*.idata)
    }

    .bss  ALIGN(0x200): {
        *(*.bss)
    }

    .debug_info  ALIGN(0x200): {
        *(*.debug_info)
    }

    .debug_ranges  ALIGN(0x200): {
        *(*.debug_ranges)
    }

    .debug_loc  ALIGN(0x200): {
        *(*.debug_loc)
    }

    .debug_str  ALIGN(0x200): {
        *(*.debug_str)
    }
  
    .debug_aranges  ALIGN(0x200): {
        *(*.debug_aranges)
    }
      
    .debug_line  ALIGN(0x200): {
        *(*.debug_line)
    }

    .debug_frame  ALIGN(0x200): {
        *(*.debug_frame)
    }

    .debug_abbrev  ALIGN(0x200): {
        *(*.debug_abbrev)
    }
}