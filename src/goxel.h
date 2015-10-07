/* Goxel 3D voxels editor
 *
 * copyright (c) 2015 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.

 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.

 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GOXEL_H
#define GOXEL_H

#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#include "vec.h"
#include "utlist.h"
#include "uthash.h"
#include "utarray.h"
#include "ivec.h"
#include <float.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GOXEL_VERSION_STR "0.1.0"

// #### Set the DEBUG macro ####
#ifndef DEBUG
#   if !defined(NDEBUG)
#       define DEBUG 1
#   else
#       define DEBUG 0
#   endif
#endif
// #############################



// #### DEFINED macro ##########
// DEFINE(NAME) returns 1 if NAME is defined to 1, 0 otherwise.
#define DEFINED(macro) DEFINED_(macro)
#define macrotest_1 ,
#define DEFINED_(value) DEFINED__(macrotest_##value)
#define DEFINED__(comma) DEFINED___(comma 1, 0)
#define DEFINED___(_, v, ...) v
// #############################


// #### Logging macros #########

enum {
    LOG_VERBOSE = 2,
    LOG_DEBUG   = 3,
    LOG_INFO    = 4,
    LOG_WARN    = 5,
    LOG_ERROR   = 6,
};

#ifndef LOG_LEVEL
#   if DEBUG
#       define LOG_LEVEL LOG_DEBUG
#   else
#       define LOG_LEVEL LOG_INFO
#   endif
#endif

#define LOG(level, msg, ...) do { \
    if (level >= LOG_LEVEL) \
        dolog(level, msg, __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define LOG_V(msg, ...) LOG(LOG_VERBOSE, msg, ##__VA_ARGS__)
#define LOG_D(msg, ...) LOG(LOG_DEBUG,   msg, ##__VA_ARGS__)
#define LOG_I(msg, ...) LOG(LOG_INFO,    msg, ##__VA_ARGS__)
#define LOG_W(msg, ...) LOG(LOG_WARN,    msg, ##__VA_ARGS__)
#define LOG_E(msg, ...) LOG(LOG_ERROR,   msg, ##__VA_ARGS__)
// #############################



// #### Include OpenGL #########
#define GL_GLEXT_PROTOTYPES
#ifdef WIN32
#    include <windows.h>
#    include "GL/glew.h"
#endif
#ifdef __APPLE__
#   include "TargetConditionals.h"
#   if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#       define GLES2 1
#       include <OPenGLES/ES2/gl.h>
#       include <OpenGLES/ES2/glext.h>
#   else
#       include <OpenGL/gl.h>
#   endif
#else
#   ifdef GLES2
#       include <GLES2/gl2.h>
#       include <GLES2/gl2ext.h>
#   else
#       include <GL/gl.h>
#   endif
#endif
// #############################



// #### GL macro ###############
#ifdef DEBUG
#  define GL(line) do {                                                 \
       line;                                                            \
       if (check_gl_errors(__FILE__, __LINE__)) assert(false);      \
   } while(0)
#else
#  define GL(line) line
#endif
// #############################



// ### Some useful inline functions / macros.

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define SWAP(x0, x) {typeof(x0) tmp = x0; x0 = x; x = tmp;}

static inline uvec4b_t HEXCOLOR(uint32_t v)
{
    return uvec4b((v >> 24) & 0xff,
                  (v >> 16) & 0xff,
                  (v >>  8) & 0xff,
                  (v >>  0) & 0xff);
}

static inline vec4_t uvec4b_to_vec4(uvec4b_t v)
{
    return vec4(v.x / 255., v.y / 255., v.z / 255., v.w / 255.);
}

#define min(a, b) ({ \
      __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      _a < _b ? _a : _b; \
      })

#define max(a, b) ({ \
      __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
      _a > _b ? _a : _b; \
      })

#define clamp(x, a, b) (min(max(x, a), b))

#define sign(x) ({ \
      __typeof__ (x) _x = (x); \
      (_x > 0) ? +1 : (_x < 0)? -1 : 0; \
      })

static inline float smoothstep(float edge0, float edge1, float x)
{
    x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

static inline float mix(float x, float y, float t)
{
    return (1.0 - t) * x + t * y;
}

// #############################

// XXX: I should clean up a but the code of vec.h so that I can put those on
// top.
#include "box.h"
#include "plane.h"


// #### Utils ##################

// Used internally by the LOG macro
void dolog(int level, const char *msg,
           const char *func, const char *file, int line, ...);
int check_gl_errors(const char *file, int line);
int create_program(const char *vertex_shader, const char *fragment_shader,
                   const char *include);
void delete_program(int prog);
unsigned int gox_rand(void);
float gox_frand(float min, float max);
uint8_t *img_read(const char *path, int *width, int *height, int *bpp);
uint8_t *img_read_from_mem(const char *data, int size,
                           int *w, int *h, int *bpp);
void img_write(const uint8_t *img, int w, int h, int bpp, const char *path);
uint8_t *img_write_to_mem(const uint8_t *img, int w, int h, int bpp,
                          int *size);
bool str_endswith(const char *str, const char *end);

// #############################



// #### System #################
void sys_log(const char *msg);
const char *sys_get_data_dir(void);
bool sys_asset_exists(const char *path);
char *sys_read_asset(const char *path, int *size);
GLuint sys_get_screen_framebuffer(void);
bool sys_save_dialog(const char *type, char **path);
bool sys_open_dialog(const char *type, char **path);
// #############################


// #### Texture ################
enum {
    TF_DEPTH    = 1 << 0,
    TF_STENCIL  = 1 << 1,
    TF_MIPMAP   = 1 << 2,
    TF_KEEP     = 1 << 3,
    TF_RGB      = 1 << 4,
    TF_RGB_565  = 1 << 5,
    TF_HAS_TEX  = 1 << 6,
    TF_HAS_FB   = 1 << 7,
    TF_IS_DEFAULT_FB = 1 << 8,
};

typedef struct texture {
    struct texture *next;
    int ref;
    char *key;
    float last_used_time;
    float life;
    // Used to regenerate the tex after a suspend.
    void (*regenerate_func)(struct texture *tex);
    void *data; // Destroyed with the texture.

#ifdef DEBUG
    struct {
        const char *tag;
    } debug;
#endif

    GLuint tex;
    int format;
    struct texture *parent; // Used for sub-textures.
    int tex_w, tex_h;       // The actual OpenGL texture size.
    int x, y, w, h;         // Position of the sub texture.
    int flags;
    // This is only used for buffer textures.
    GLuint framebuffer, depth, stencil;
} texture_t;

texture_t *texture_create_from_image(const char *path);
texture_t *texture_create_sub(texture_t *tex, int x, int y, int w, int h);
texture_t *texture_create_surface(int w, int h, int flags);
texture_t *texture_create_buffer(int w, int h, int flags);
texture_t *texture_default_framebuffer(int w, int h);
#ifdef DEBUG
    void texture_debug_set_tag(const char *tag);
#else
#   define texture_debug_set_tag(t)
#endif

void texture_clear(texture_t *tex);
void texture_discard(texture_t *tex);
texture_t *texture_compress(const texture_t *tex, int compression);
void texture_set_key(texture_t *tex, const char *key);
texture_t *texture_search(const char *key);
void texture_get_quad(const texture_t *tex, vec2_t quad[4]);

void texture_set_timer(texture_t *tex, float timer);
void texture_inc_ref(texture_t *tex);
void texture_dec_ref(texture_t *tex);
void texture_save_to_file(const texture_t *tex, const char *path, int flags);

void textures_iter(float dt);
void textures_vacuum();
void textures_full_vacuum();
int textures_count();

void textures_release_all();
void textures_regenerate_all();
// #############################

// #### Tool/Operation/Painter #
enum {
    OP_NULL,
    OP_ADD,
    OP_SUB,
    OP_PAINT,
};

enum {
    TOOL_BRUSH,
    TOOL_CUBE,
    TOOL_LASER,
    TOOL_SET_PLANE,
    TOOL_MOVE,
    TOOL_PICK_COLOR,
};

typedef struct shape {
    float (*func)(const vec3_t *p, const vec3_t *s);
} shape_t;

void shapes_init(void);
extern shape_t shape_sphere;
extern shape_t shape_cube;
extern shape_t shape_cylinder;


// The painting context, including the tool, brush, operation, radius,
// color, etc...
typedef struct painter {
    int         op;
    shape_t     *shape;
    uvec4b_t    color;
} painter_t;

// #### Block ##################
// The block size can only be 16.
#define BLOCK_SIZE 16
#define VOXEL_TEXTURE_SIZE 8

// Structure used for the OpenGL array data of blocks.
// XXX: we can probably make it smaller.
typedef struct voxel_vertex
{
    vec3b_t  pos        __attribute__((aligned(4)));
    vec3b_t  normal     __attribute__((aligned(4)));
    uvec4b_t color      __attribute__((aligned(4)));
    uvec2b_t pos_data   __attribute__((aligned(4)));
    uvec2b_t bshadow_uv __attribute__((aligned(4)));
    uvec2b_t bump_uv    __attribute__((aligned(4)));
} voxel_vertex_t;

// We use copy on write for the block data, so that it is cheap to copy
// blocks.
typedef struct block_data block_data_t;
struct block_data
{
    int         ref;
    int         id;
    uvec4b_t    voxels[BLOCK_SIZE * BLOCK_SIZE * BLOCK_SIZE]; // RGBA voxels.
};

typedef struct block block_t;
struct block
{
    UT_hash_handle  hh;     // The hash table of pos -> blocks in a mesh.
    block_data_t    *data;
    vec3_t          pos;
    int             id;
};
block_t *block_new(const vec3_t *pos, block_data_t *data);
void block_delete(block_t *block);
block_t *block_copy(const block_t *other);
box_t block_get_box(const block_t *block, bool exact);
void block_fill(block_t *block,
                uvec4b_t (*get_color)(const vec3_t *pos, void *user_data),
                void *user_data);
int block_generate_vertices(const block_data_t *data, int effects,
                            voxel_vertex_t *out);
void block_op(block_t *block, painter_t *painter, const box_t *box);
bool block_is_empty(const block_t *block, bool fast);
void block_merge(block_t *block, const block_t *other);
uvec4b_t block_get_at(const block_t *block, const vec3_t *pos);
// #############################



// #### Mesh ###################
typedef struct mesh mesh_t;
struct mesh
{
    block_t *blocks;
    int next_block_id;
    int *ref;   // Used to implement copy on write of the blocks.
};
mesh_t *mesh_new(void);
void mesh_clear(mesh_t *mesh);
void mesh_delete(mesh_t *mesh);
mesh_t *mesh_copy(const mesh_t *mesh);
void mesh_set(mesh_t **mesh, const mesh_t *other);
box_t mesh_get_box(const mesh_t *mesh, bool exact);
void mesh_fill(mesh_t *mesh,
               uvec4b_t (*get_color)(const vec3_t *pos, void *user_data),
               void *user_data);
void mesh_op(mesh_t *mesh, painter_t *painter, const box_t *box);
void mesh_merge(mesh_t *mesh, const mesh_t *other);
void mesh_add_block(mesh_t *mesh, block_data_t *data, const vec3_t *pos);
void mesh_move(mesh_t *mesh, const mat4_t *mat);
uvec4b_t mesh_get_at(const mesh_t *mesh, const vec3_t *pos);

#define MESH_ITER_BLOCKS(m, b) for (b = m->blocks; b; b = b->hh.next)

// #############################



// #### Renderer ###############

enum {
    EFFECT_RENDER_POS       = 1 << 1,
    EFFECT_SMOOTH           = 1 << 2,
    EFFECT_BORDERS          = 1 << 3,
    EFFECT_BORDERS_ALL      = 1 << 4,
    EFFECT_SEMI_TRANSPARENT = 1 << 5,
    EFFECT_SEE_BACK         = 1 << 6,
};

typedef struct renderer renderer_t;
typedef struct render_item_t render_item_t;
struct renderer
{
    struct {
        vec3_t direction;
        bool   fixed;
        float  intensity;
    } light;

    // For the moment there is just one global material.
    struct {
        float ambient;
        float diffuse;
        float specular;
        float shininess;
        float smoothness;
        int   effects;
    } material;

    float            border_shadow;
    render_item_t    *items;
};

void render_init(void);
void render_deinit(void);
void render_mesh(renderer_t *rend, const mesh_t *mesh, int effects);
void render_plane(renderer_t *rend, const plane_t *plane,
                  const uvec4b_t *color);
void render_line(renderer_t *rend, const vec3_t *a, const vec3_t *b);
void render_box(renderer_t *rend, const box_t *box, bool solid,
                const uvec4b_t *color);
void render_sphere(renderer_t *rend, const mat4_t *mat);
void render_render(renderer_t *rend, const mat4_t *view, const mat4_t *proj);

// #############################




// #### Model3d ################

typedef struct {
     vec3_t   pos       __attribute__((aligned(4)));
     vec3_t   normal    __attribute__((aligned(4)));
     uvec4b_t color     __attribute__((aligned(4)));
} model_vertex_t;

typedef struct {
    int              nb_vertices;
    model_vertex_t   *vertices;
    bool             solid;

    // Rendering buffers.
    // XXX: move this into the renderer, like for block_t
    GLuint  vertex_buffer;
    int     nb_lines;
    bool    dirty;
} model3d_t;

void model3d_init(void);
model3d_t *model3d_cube(void);
model3d_t *model3d_wire_cube(void);
model3d_t *model3d_sphere(int slices, int stacks);
model3d_t *model3d_grid(int nx, int ny);
void model3d_render(model3d_t *model3d,
                    const mat4_t *model, const mat4_t *proj,
                    const uvec4b_t *color,
                    float fade, const vec3_t *fade_center);


// #### Palette ################

typedef struct palette {
   int      size;
   uvec4b_t *values;
} palette_t;

palette_t *palette_get();

// #############################


// #### Goxel : core object ####

enum {
    KEY_RIGHT       = 262,
    KEY_LEFT        = 263,
    KEY_DOWN        = 264,
    KEY_UP          = 265,
    KEY_ENTER       = 257,
    KEY_BACKSPACE   = 259,
    KEY_SHIFT       = 340,
    KEY_CONTROL     = 341,
};

// Flags to set where the mouse snap.  This might change in the future.
enum {
    SNAP_MESH  = 1 << 0,
    SNAP_PLANE = 1 << 1,
};

typedef struct inputs
{
    int         window_size[2];
    bool        keys[512]; // Table of all the pressed keys.
    uint32_t    chars[16];
    vec2_t      mouse_pos;
    bool        mouse_down[3];
    float       mouse_wheel;
} inputs_t;

typedef struct history history_t;

typedef struct layer layer_t;
struct layer {
    layer_t *next, *prev;
    mesh_t  *mesh;
    bool    visible;
    char    name[128]; // 127 chars max.
};

typedef struct image image_t;
struct image {
    layer_t *layers;
    layer_t *active_layer;

    // For saving.
    char    *path;
    int     export_width;
    int     export_height;

    image_t *history;
    image_t *history_next, *history_prev, *history_current;
};

image_t *image_new(void);
image_t *image_copy(image_t *img);
void image_delete(image_t *img);
void image_add_layer(image_t *img);
void image_delete_layer(image_t *img, layer_t *layer);
void image_move_layer(image_t *img, layer_t *layer, int d);
void image_duplicate_layer(image_t *img, layer_t *layer);
void image_merge_visible_layers(image_t *img);
void image_history_push(image_t *img);
void image_undo(image_t *img);
void image_redo(image_t *img);

typedef struct goxel
{
    vec2i_t    screen_size;
    image_t    *image;

    mesh_t     *layers_mesh; // All the layers combined.
    mesh_t     *pick_mesh;

    history_t  *history;     // Undo/redo history.
    int        snap;
    float      snap_offset;  // Only for brush tool.

    plane_t    plane;         // The snapping plane.
    bool       plane_hidden;  // Set to true to hide the plane.

    // For the moment we only support a single view, so the matrix and
    // projection are here.  At some point I need to move this out in a
    // separate structure.
    struct {
        vec3_t ofs;
        quat_t rot;
        float  dist;
        float  zoom;
        vec4_t view;

        // Auto computed from other values:
        mat4_t view_mat;    // Model to view transformation.
        mat4_t proj_mat;    // Proj transform from camera coordinates.
    } camera;

    uvec4b_t   back_color;
    uvec4b_t   grid_color;

    texture_t  *pick_fbo;
    painter_t  painter;
    renderer_t rend;

    int        tool;
    float      tool_radius;

    // Some state for the tool iter functions.
    int        tool_state;
    int        tool_t;
    mesh_t     *tool_origin_mesh;
    // Structure used to skip rendering when don't move the mouse.
    struct     {
        vec3_t     pos;
        bool       pressed;
        int        op;
    }          tool_last_op;
    vec3_t     tool_start_pos;
    plane_t    tool_plane;

    struct {
        quat_t rotation;
        vec2_t pos;
        vec3_t camera_ofs;
    } move_origin;

    bool       painting;    // Set to true when we are in a painting operation.
    bool       moving;      // Set to true while in a movement operation.

    palette_t  *palette;    // The current color palette
    char       *help_text;  // Seen in the bottom of the screen.

    int        frame_count;       // Global frames counter.
    int        block_next_id;

    int        block_count; // Counter for the number of block data.
} goxel_t;
goxel_t *goxel(void);
void goxel_init(goxel_t *goxel);
void goxel_iter(goxel_t *goxel, inputs_t *inputs);
void goxel_render(goxel_t *goxel);
// Called by the gui when the mouse hover a 3D view.
// XXX: change the name since we also call it when the mouse get out of
// the view.
void goxel_mouse_in_view(goxel_t *goxel, const vec2_t *view_size,
                         const inputs_t *inputs, bool inside);
int goxel_unproject(goxel_t *goxel, const vec2_t *view_size,
                    const vec2_t *pos, vec3_t *out, vec3_t *normal);
bool goxel_unproject_on_screen(goxel_t *goxel, const vec2_t *view_size,
                               const vec2_t *pos,
                               vec3_t *out, vec3_t *normal);
bool goxel_unproject_on_mesh(goxel_t *goxel, const vec2_t *view_size,
                     const vec2_t *pos, mesh_t *mesh,
                     vec3_t *out, vec3_t *normal);
bool goxel_unproject_on_plane(goxel_t *goxel, const vec2_t *view_size,
                     const vec2_t *pos, const plane_t *plane,
                     vec3_t *out, vec3_t *normal);
void goxel_update_meshes(goxel_t *goxel, bool pick);
void goxel_export_as_png(goxel_t *goxel, const char *path);
void goxel_export_as_obj(goxel_t *goxel, const char *path);
void goxel_export_as_ply(goxel_t *goxel, const char *path);
void goxel_set_help_text(goxel_t *goxel, const char *msg, ...);
void goxel_undo(goxel_t *goxel);
void goxel_redo(goxel_t *goxel);
// #############################

void save_to_file(goxel_t *goxel, const char *path);
void load_from_file(goxel_t *goxel, const char *path);


int tool_iter(goxel_t *goxel, int tool, const inputs_t *inputs, int state,
              const vec2_t *view_size, bool inside);
void tool_cancel(goxel_t *goxel, int tool, int state);

// #### Colors functions #######
uvec3b_t hsl_to_rgb(uvec3b_t hsl);
uvec3b_t rgb_to_hsl(uvec3b_t rgb);

// #### Gui ####################

void gui_init(void);
void gui_iter(goxel_t *goxel, const inputs_t *inputs);
void gui_render(void);

// #############################

void wavefront_export(const mesh_t *mesh, const char *path);
void ply_export(const mesh_t *mesh, const char *path);



// #### Profiler ###############

typedef struct profiler_block profiler_block_t;
struct profiler_block
{
    const char          *name;
    profiler_block_t    *next;   // All the blocks are put in a list.
    profiler_block_t    *parent; // The block that called this block.
    int                 depth;   // How many time are we inside this block.
    int                 count;

    // In nanoseconds
    int64_t             tot_time;
    int64_t             self_time;
    int64_t             enter_time;
};
void profiler_start(void);
void profiler_stop(void);
void profiler_tick(void);
profiler_block_t *profiler_get_blocks();

void profiler_enter_(profiler_block_t *block);
void profiler_exit_(profiler_block_t *block);
static inline void profiler_cleanup_(profiler_block_t **p)
{
    profiler_exit_(*p);
}

#ifndef PROFILER
    #define PROFILER DEBUG
#endif
#if PROFILER
    #define PROFILED2(name_) \
        static profiler_block_t block_ = {name_}; \
        profiler_enter_(&block_); \
        profiler_block_t *block_ref_ \
                __attribute__((cleanup(profiler_cleanup_))); \
        block_ref_ = &block_;
#else
    #define PROFILED2(name_)
#endif
#define PROFILED PROFILED2(__func__)

// #############################

#endif // GOXEL_H
