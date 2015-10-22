#include "page_table.H"
#include "paging_low.H"
#include "console.H"
//initialize statics
PageTable     * PageTable::current_page_table;
unsigned int    PageTable::paging_enabled;
FramePool     * PageTable::kernel_mem_pool;
FramePool     * PageTable::process_mem_pool;
unsigned long   PageTable::shared_size;
unsigned long * PageTable::page_directory;

void PageTable::init_paging(FramePool * _kernel_mem_pool,
                          FramePool * _process_mem_pool,
                          const unsigned long _shared_size){
 kernel_mem_pool = _kernel_mem_pool;
 process_mem_pool = _process_mem_pool;
 shared_size = _shared_size;
}
  /* Set the global parameters for the paging subsystem. */

  PageTable::PageTable(){
    page_directory = (unsigned long *) (kernel_mem_pool->get_frame() * PAGE_SIZE);
    unsigned long * page_table = (unsigned long *) (kernel_mem_pool->get_frame() * PAGE_SIZE);
    unsigned long address = 0;

    for (int i = 0; i < ENTRIES_PER_PAGE; ++i)
    {
      page_table[i] = address | 3;
      address = address + PAGE_SIZE;
    }
    page_directory[0] = (unsigned long) page_table;
    page_directory[0] |= 3;

    for (int i = 1; i < ENTRIES_PER_PAGE; ++i)
    {
      page_directory[i] = 0 | 2;
    }
    //page_directory[ENTRIES_PER_PAGE-1] = (long unsigned int)page_directory;
    paging_enabled = false;
  }
  /* Initializes a page table with a given location for the directory and the
     page table proper.
     NOTE: The PageTable object still needs to be stored somewhere! Probably it is best
           to have it on the stack, as there is no memory manager yet...
     NOTE2: It may also be simpler to create the first page table *before* paging
           has been enabled.
  */

  void PageTable::load(){
    current_page_table = this;
  }
  /* Makes the given page table the current table. This must be done once during
     system startup and whenever the address space is switched (e.g. during
     process switching). */

  void PageTable::enable_paging(){
    write_cr3((unsigned long) page_directory);
    write_cr0(read_cr0() | 0x80000000);
    paging_enabled = true;
  }
  /* Enable paging on the CPU. Typically, a CPU start with paging disabled, and
     memory is accessed by addressing physical memory directly. After paging is
     enabled, memory is addressed logically. */

  void PageTable::handle_fault(REGS * _r){
    // Read the address
    unsigned long address = read_cr2();
    unsigned long* page_dir = current_page_table->page_directory;
    
    //get the indices of the directory and page table
    unsigned long page_dir_index = address >> 22;
    unsigned long page_tab_index = (address >> 12) & 0x03FF;
    Console::putui(page_dir_index);
    Console::putui(page_tab_index);
    Console::putui(address);
    
    // Get some information about situation from the _regs register
  // Code used from James Malloy's post about paging found at:
  // http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html
  int present = !(_r->err_code & 0x1);
  int write = _r->err_code & 0x2;
  int us = _r->err_code & 0x4;
  int reserved = _r->err_code & 0x8;
  int id = _r->err_code & 0x10;

    unsigned long* page_entry;

    //get the page table from the directory
    unsigned long* page_tab = (unsigned long *) page_dir[page_dir_index];
    page_tab = (unsigned long *)(((unsigned long)(page_tab)) & 0xFFFFF000);
    //set up page table if none exists
    if ((page_dir[page_dir_index] & 1) == 0) {
      Console::puts("Filling page table");
      page_tab = (unsigned long *) (kernel_mem_pool->get_frame() * PAGE_SIZE);
      //fill up page table
      for (int i = 0; i < ENTRIES_PER_PAGE; ++i)
      {                                                  
            page_tab[i] = 0;
      }
      page_dir[page_dir_index] = (unsigned long) page_tab;
      page_dir[page_dir_index] |= 3;
    }

    page_entry = (unsigned long *) page_tab[page_tab_index];
    if ((page_tab[page_tab_index] & 1) == 0) {
        Console::puts("Allocated entry");
        page_entry = (unsigned long *) (process_mem_pool->get_frame() * PAGE_SIZE);                                                      
        page_tab[page_tab_index] = (unsigned long) (page_entry);
        page_tab[page_tab_index] |= 7;
    }
  }
  /* The page fault handler. */