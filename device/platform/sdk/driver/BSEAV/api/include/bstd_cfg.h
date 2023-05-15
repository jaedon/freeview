#ifdef __cplusplus
extern "C"
{
#endif
void b_task_check(const char *file, unsigned line);
#if BDBG_DEBUG_BUILD
#define B_TASK_CHECK() b_task_check(__FILE__, __LINE__)
#else
#define B_TASK_CHECK() 
#endif

#define BSTD_INLINE extern inline __attribute__((always_inline))

#ifdef __cplusplus
}
#endif


