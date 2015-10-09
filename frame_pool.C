#include "frame_pool.H"
#define MB * (0x1 << 20)
#define KB * (0x1 << 10)
#define PROCESS_POOL_START_FRAME ((4 MB) / (4 KB))
FramePool* FramePool::kernel;
FramePool* FramePool::process;
    FramePool::FramePool(unsigned long _base_frame_no,
             unsigned long _nframes,
             unsigned long _info_frame_no){
      base_frame_no = _base_frame_no;
      nframes = _nframes;
      info_frame_no = _info_frame_no;
      if(_info_frame_no == 0){
        info_frame_no = base_frame_no;
        kernel = this;
      }
      else{
        process = this;
      }
      bitmap = (unsigned int *) (info_frame_no * FRAME_SIZE);
      for (int i = 0; i < 512; ++i){
        bitmap[i] = 0;
      }
      mark_inaccessible(info_frame_no, 1);

    }
   /* Initializes the data structures needed for the management of this
      frame pool. This function must be called before the paging system
      is initialized.
      _base_frame_no is the frame number at the start of the physical memory
      region that this frame pool manages.
      _nframes is the number of frames in the physical memory region that this
      frame pool manages.
      e.g. If _base_frame_no is 16 and _nframes is 4, this frame pool manages
      physical frames numbered 16, 17, 18 and 19
      _info_frame_no is the frame number (within the directly mapped region) of
      the frame that should be used to store the management information of the
      frame pool. However, if _info_frame_no is 0, the frame pool is free to
      choose any frame from the pool to store management information.
      */

   unsigned long FramePool::get_frame(){
    unsigned int val;
    int i;
    int j;
    //search for region of bitmap with open spot
    for (i = 0; i < 512; ++i)
    {
      val = bitmap[i];
      if(val != 0xFFFF)
        break;
    }

    if(val == 0xFFFF)
      return 0;//no open region found

    for (j = 0; j < 32; ++j)
    {
      unsigned int temp  = val & (1 << j);
      if(temp == 0){
        //found an open frame!
        unsigned long frame = i*32 + j + base_frame_no;
        mark_inaccessible(frame,1);
        return frame;
      }
    }

   }
   /* Allocates a frame from the frame pool. If successful, returns the frame
    * number of the frame. If fails, returns 0. */

   void FramePool::mark_inaccessible(unsigned long _base_frame_no,
                          unsigned long _nframes){
      _base_frame_no -= base_frame_no;
      for (int i = 0; i < _nframes; ++i)
      {
        unsigned long j = _base_frame_no + i;
        unsigned long s = j/32;
        unsigned long r = _base_frame_no % 32;
        bitmap[s] |= 1 << r;
      }
   }
   /* Mark the area of physical memory as inaccessible. The arguments have the
    * same semanticas as in the constructor.
    */

   void FramePool::release_frame(unsigned long _frame_no){
    if (_frame_no < PROCESS_POOL_START_FRAME ){
      _frame_no -= kernel->base_frame_no;
      unsigned long s = _frame_no/32;
      unsigned long r = _frame_no % 32;
       kernel->bitmap[s] &= 0 << r;
    }
    else{
      _frame_no -= process->base_frame_no;
      unsigned long s = _frame_no/32;
      unsigned long r = _frame_no % 32;
      process->bitmap[s] &= 0 << r;
    }
   }
   /* Releases frame back to the given frame pool.
      The frame is identified by the frame number. 
      NOTE: This function is static because there may be more than one frame pool
      defined in the system, and it is unclear which one this frame belongs to.
      This function must first identify the correct frame pool and then call the frame
      pool's release_frame function. */