#ifndef GLSL_BACKEND_H
#define GLSL_BACKEND_H

extern bool glsl_backend_create_shaders(
   slang_program *program,
   Dataflow *vertex_x,
   Dataflow *vertex_y,
   Dataflow *vertex_z,
   Dataflow *vertex_w,
   Dataflow *vertex_point_size,
   Dataflow **vertex_vary,
   uint32_t vary_count,
   Dataflow *frag_r,
   Dataflow *frag_g,
   Dataflow *frag_b,
   Dataflow *frag_a,
   Dataflow *frag_discard);

extern uint32_t glsl_backend_get_schedule_type(Dataflow *dataflow);

extern bool glsl_backend_schedule(Dataflow *root, uint32_t type, bool *allow_thread, bool fb_rb_swap);

#define SCHEDULE_TYPE_INPUT (1<<0)
#define SCHEDULE_TYPE_OUTPUT (1<<1)
#define SCHEDULE_TYPE_ALU (1<<2)
#define SCHEDULE_TYPE_SIG (1<<4)

#endif