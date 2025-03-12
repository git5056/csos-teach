MEMORY
{
 flash (rxai!w) : ORIGIN = 0x10020000, LENGTH = 4M   
  ilm (rxai!w) : ORIGIN = 0x10080000, LENGTH = 64K
  ram (wxa!ri) : ORIGIN = 0x10090000, LENGTH = 64K
}

SECTIONS
{
	/*. = 0x400000;*/
	. = ALIGN(1,0x400000);
	zxc_q = ABSOLUTE(.);
	qwe_q = .;

	PROVIDE(b_kernel = .);
	PROVIDE(xxaasd1 = .);
	. = ALIGN(.+1,0x1000);
	PROVIDE(xxaasd2 = .);
	PROVIDE(b_text_ori = .);
	PROVIDE(b_texta_32 = .);
	PROVIDE(b_text = LOADADDR(.text));

	.text ALIGN(0x1000): {
		*(EXCLUDE_FILE(*kernel32/init/init_* *kernel32/sysboundary/syscall_*) .text)
	}
	
	. = ALIGN(.,0x1000);
	PROVIDE(e_text = LOADADDR(.text)+SIZEOF(.text));
	PROVIDE(b_data = LOADADDR(.data));

	.data  ALIGN(0x1000): {
		*(EXCLUDE_FILE(*kernel32/init/init_* *kernel32/sysboundary/syscall_*) .data)
	}

	.rdata ALIGN(0x1000): {
		*(EXCLUDE_FILE(*kernel32/init/init_*  *kernel32/sysboundary/syscall_*) .rdata)
	}

	.reloc ALIGN(0x1000): {
		*(EXCLUDE_FILE(*kernel32/init/init_*  *kernel32/sysboundary/syscall_*) .reloc)
	}

	.mysection  ALIGN(0x1000):{
 		KEEP(*(.mysection))
	}
	
	_zxcd = LOADADDR(.text);
	PROVIDE(_xxxcd = LOADADDR(.text));
	.rodata : {
		*(EXCLUDE_FILE(*kernel32/init/init_*  *kernel32/sysboundary/syscall_*) .rodata)
	}


	. = ALIGN(0x1000);
	_bbbbd = . ;

	.bss  ALIGN(0x1000): {
		*(EXCLUDE_FILE(*kernel32/init/init_* *kernel32/sysboundary/syscall_*) .bss)
	}

	. = ALIGN(.+1,0x5000);
	.sysboundary ALIGN(0x1000)  : {
		*syscall_proxy*(.text .rodata .rdata .data .bss)
		*syscall_stdio*(.text .rodata .rdata .data .bss)
	}
	
	PROVIDE(b_sysboundary = LOADADDR(.sysboundary));

	PROVIDE(e_sysboundary = LOADADDR(.sysboundary) + SIZEOF(.sysboundary));

	.debug_info  ALIGN(0x1000): {
        *(*.debug_info)
    }

    .debug_ranges  ALIGN(0x1000): {
        *(*.debug_ranges)
    }

	.debug_loc  ALIGN(0x1000): {
        *(*.debug_loc)
    }

	.debug_str  ALIGN(0x1000): {
        *(*.debug_str)
    }
		
	.debug_aranges  ALIGN(0x1000): {
        *(*.debug_aranges)
    }
      
    .debug_line  ALIGN(0x1000): {
        *(*.debug_line)
    }

	.debug_frame  ALIGN(0x1000): {
        *(*.debug_frame)
    }
    

	.debug_abbrev  ALIGN(0x1000): {
        *(*.debug_abbrev)
    }

	e_data = LOADADDR(.debug_abbrev)+SIZEOF(.debug_abbrev);
	/*e_data_align = ALIGN(e_data,0x1000);*/
	e_data_align = 0x40000000;
	/*. = 0x403F0000;*/
	/*. = 0x40000000;*/
	. = ALIGN(.+1,0x10000);
	/*AT(e_data_align)*/
	/*(NOLOAD)*/
	/*ABSOLUTE(e_data_align)*/
		.init_task ALIGN(0x1000)  : {
		*init_task_entry*(.text .rodata .rdata .data .bss)
		*init_task*(.text .rodata .rdata .data .bss)
	}

	PROVIDE(b_init_task = LOADADDR(.init_task));

	PROVIDE(e_init_task = LOADADDR(.init_task) + SIZEOF(.init_task));

	PROVIDE(memory_start = e_init_task);
}