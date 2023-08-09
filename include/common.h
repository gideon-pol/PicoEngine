
#ifdef PLATFORM_PICO
#define FRAME_WIDTH     120
#define FRAME_HEIGHT    120
#endif

#ifdef PLATFORM_NATIVE
#define FRAME_WIDTH     120
#define FRAME_HEIGHT    120
#endif


#define FORCE_INLINE __attribute__((always_inline)) inline