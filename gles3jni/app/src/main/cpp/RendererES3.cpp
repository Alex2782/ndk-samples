/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <EGL/egl.h>

#include "gles3jni.h"

#define STR(s) #s
#define STRV(s) STR(s)

//#define POS_ATTRIB 0
//#define COLOR_ATTRIB 1
//#define SCALEROT_ATTRIB 2
//#define OFFSET_ATTRIB 3

#define POS_ATTRIB 12
#define COLOR_ATTRIB 13
#define SCALEROT_ATTRIB 14
#define OFFSET_ATTRIB 15

//ORIG Shader
/*
static const char VERTEX_SHADER[] =
    "#version 300 es\n"
    "layout(location = " STRV(POS_ATTRIB) ") in vec2 pos;\n"
    "layout(location=" STRV(COLOR_ATTRIB) ") in vec4 color;\n"
    "layout(location=" STRV(SCALEROT_ATTRIB) ") in vec4 scaleRot;\n"
    "layout(location=" STRV(OFFSET_ATTRIB) ") in vec2 offset;\n"
    "out vec4 vColor;\n"
    "void main() {\n"
    "    mat2 sr = mat2(scaleRot.xy, scaleRot.zw);\n"
    "    gl_Position = vec4(sr*pos + offset, 0.0, 1.0);\n"
    "    vColor = color;\n"
    "}\n";

static const char FRAGMENT_SHADER[] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec4 vColor;\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "    outColor = vColor;\n"
    "}\n";
*/


static const char VERTEX_SHADER[] =
		"#version 300 es\n"
		"#define DISABLE_LIGHTING \n"
		"#define USE_ATTRIBUTES \n"
		"#define USE_INSTANCING \n"
		"#define MAX_GLOBAL_SHADER_UNIFORMS 256 \n"
		"#define MAX_LIGHTS 256 \n"
		"layout(location = " STRV(POS_ATTRIB) ") in vec2 pos;\n"
		"layout(location=" STRV(COLOR_ATTRIB) ") in vec4 color;\n"
		"layout(location=" STRV(SCALEROT_ATTRIB) ") in vec4 scaleRot;\n"
		"layout(location=" STRV(OFFSET_ATTRIB) ") in vec2 offset;\n"
		"#ifdef USE_ATTRIBUTES\n"
		//-------------------------------------------------------------------
		"layout(location = 0) in vec2 vertex_attrib;\n"
		"layout(location = 3) in vec4 color_attrib;\n"
		"layout(location = 4) in vec2 uv_attrib;\n"
		"\n"
		"#ifdef USE_INSTANCING\n"
		"\n"
		"layout(location = 1) in highp vec4 instance_xform0;\n"
		"layout(location = 2) in highp vec4 instance_xform1;\n"
		//"layout(location = 5) in highp uvec4 instance_color_custom_data; // Color packed into xy, custom_data packed into zw for compatibility with 3D\n"
		"\n"
		"#endif // USE_INSTANCING\n"
		"\n"
		"#endif // USE_ATTRIBUTES\n"
		//----------------------------------------------------------------------
		"uint godot_packUnorm4x8(vec4 v) {\n"
		"	uvec4 uv = uvec4(round(clamp(v, vec4(0.0), vec4(1.0)) * 255.0));\n"
		"	return uv.x | (uv.y << uint(8)) | (uv.z << uint(16)) | (uv.w << uint(24));\n"
		"}\n"

		"vec4 godot_unpackUnorm4x8(uint p) {\n"
		"	return vec4(float(p & uint(0xff)), float((p >> uint(8)) & uint(0xff)), float((p >> uint(16)) & uint(0xff)), float(p >> uint(24))) * 0.00392156862; // 1.0 / 255.0\n"
		"}\n"

		"uint godot_packSnorm4x8(vec4 v) {\n"
		"	uvec4 uv = uvec4(round(clamp(v, vec4(-1.0), vec4(1.0)) * 127.0) + 127.0);\n"
		"	return uv.x | uv.y << uint(8) | uv.z << uint(16) | uv.w << uint(24);\n"
		"}\n"

		"vec4 godot_unpackSnorm4x8(uint p) {\n"
		"	vec4 v = vec4(float(p & uint(0xff)), float((p >> uint(8)) & uint(0xff)), float((p >> uint(16)) & uint(0xff)), float(p >> uint(24)));\n"
		"	return clamp((v - vec4(127.0)) * vec4(0.00787401574), vec4(-1.0), vec4(1.0));\n"
		"}\n"

		"#define packUnorm4x8 godot_packUnorm4x8\n"
		"#define unpackUnorm4x8 godot_unpackUnorm4x8\n"
		"#define packSnorm4x8 godot_packSnorm4x8\n"
		"#define unpackSnorm4x8 godot_unpackSnorm4x8\n"
		//----------------------------------------------------------------------
		"layout(location = 5) in highp vec4 attrib_A;\n"
		"layout(location = 6) in highp vec4 attrib_B;\n"
		"layout(location = 7) in highp vec4 attrib_C;\n"
		"layout(location = 8) in highp vec4 attrib_D;\n"
		"layout(location = 9) in highp vec4 attrib_E;\n"
		"layout(location = 10) in highp vec4 attrib_F;\n"
		"layout(location = 11) in highp uvec4 attrib_G;\n"
		"//layout(location = 12) in highp uvec4 attrib_H;\n"

		"#define read_draw_data_world_x attrib_A.xy\n"
		"#define read_draw_data_world_y attrib_A.zw\n"
		"#define read_draw_data_world_ofs attrib_B.xy\n"
		"//#define read_draw_data_color_texture_pixel_size attrib_B.zw\n"

		"#define read_draw_data_modulation attrib_C\n"
		"#define read_draw_data_dst_rect attrib_E\n"
		"#define read_draw_data_src_rect attrib_F\n"

		"//#endif\n"

		"#define read_draw_data_flags attrib_G.z\n"
		"#define read_draw_data_specular_shininess attrib_G.w\n"
		"//#define read_draw_data_lights attrib_H\n"

		"// Varyings so the per-instance info can be used in the fragment shader\n"
		"flat out vec4 varying_A;\n"
		"//flat out vec2 varying_B;\n"
		"#ifndef USE_PRIMITIVE\n"
		"flat out vec4 varying_C;\n"
		"#ifndef USE_ATTRIBUTES\n"
		"flat out vec4 varying_E;\n"
		"#endif\n"
		"#endif\n"
		"//flat out uvec2 varying_F;\n"
		"//flat out uvec4 varying_G;\n"
		//----------------------------------------------------------------------
		"#define MAX_LIGHTS_PER_ITEM uint(16)\n"

		"#define M_PI 3.14159265359\n"

		"#define SDF_MAX_LENGTH 16384.0\n"

		"//1 means enabled, 2+ means trails in use\n"
		"#define FLAGS_INSTANCING_MASK uint(0x7F)\n"
		"#define FLAGS_INSTANCING_HAS_COLORS uint(1 << 7)\n"
		"#define FLAGS_INSTANCING_HAS_CUSTOM_DATA uint(1 << 8)\n"

		"#define FLAGS_CLIP_RECT_UV uint(1 << 9)\n"
		"#define FLAGS_TRANSPOSE_RECT uint(1 << 10)\n"
		"// (1 << 11) is for FLAGS_CONVERT_ATTRIBUTES_TO_LINEAR in RD backends, unused here.\n"
		"#define FLAGS_NINEPACH_DRAW_CENTER uint(1 << 12)\n"
		"#define FLAGS_USING_PARTICLES uint(1 << 13)\n"

		"#define FLAGS_NINEPATCH_H_MODE_SHIFT 16\n"
		"#define FLAGS_NINEPATCH_V_MODE_SHIFT 18\n"

		"#define FLAGS_LIGHT_COUNT_SHIFT 20\n"

		"#define FLAGS_DEFAULT_NORMAL_MAP_USED uint(1 << 26)\n"
		"#define FLAGS_DEFAULT_SPECULAR_MAP_USED uint(1 << 27)\n"

		"#define FLAGS_USE_MSDF uint(1 << 28)\n"
		"#define FLAGS_USE_LCD uint(1 << 29)\n"

		"#define FLAGS_FLIP_H uint(1 << 30)\n"
		"#define FLAGS_FLIP_V uint(1 << 31)\n"

		"layout(std140) uniform GlobalShaderUniformData { //ubo:1\n"
		"		vec4 global_shader_uniforms[MAX_GLOBAL_SHADER_UNIFORMS];\n"
		"};\n"

		"layout(std140) uniform CanvasData { //ubo:0\n"
		"		mat4 canvas_transform;\n"
		"		mat4 screen_transform;\n"
		"		mat4 canvas_normal_transform;\n"
		"		vec4 canvas_modulation;\n"
		"		vec2 screen_pixel_size;\n"
		"		float time;\n"
		"		bool use_pixel_snap;\n"

		"		vec4 sdf_to_tex;\n"
		"		vec2 screen_to_sdf;\n"
		"		vec2 sdf_to_screen;\n"

		"		uint directional_light_count;\n"
		"		float tex_to_sdf;\n"
		"		uint pad1;\n"
		"		uint pad2;\n"
		"};\n"

		"#ifndef DISABLE_LIGHTING\n"
		"#define LIGHT_FLAGS_BLEND_MASK uint(3 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_ADD uint(0 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_SUB uint(1 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_MIX uint(2 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_MASK uint(3 << 16)\n"
		"#define LIGHT_FLAGS_HAS_SHADOW uint(1 << 20)\n"
		"#define LIGHT_FLAGS_FILTER_SHIFT 22\n"
		"#define LIGHT_FLAGS_FILTER_MASK uint(3 << 22)\n"
		"#define LIGHT_FLAGS_SHADOW_NEAREST uint(0 << 22)\n"
		"#define LIGHT_FLAGS_SHADOW_PCF5 uint(1 << 22)\n"
		"#define LIGHT_FLAGS_SHADOW_PCF13 uint(2 << 22)\n"

		"struct Light {\n"
		"	mat2x4 texture_matrix; //light to texture coordinate matrix (transposed)\n"
		"	mat2x4 shadow_matrix; //light to shadow coordinate matrix (transposed)\n"
		"	vec4 color;\n"

		"	uint shadow_color; // packed\n"
		"	uint flags; //index to light texture\n"
		"	float shadow_pixel_size;\n"
		"	float height;\n"

		"	vec2 position;\n"
		"	float shadow_zfar_inv;\n"
		"	float shadow_y_ofs;\n"

		"	vec4 atlas_rect;\n"
		"};\n"

		"layout(std140) uniform LightData { //ubo:2\n"
		"		Light light_array[MAX_LIGHTS];\n"
		"};\n"
		"#endif // DISABLE_LIGHTING		\n"
		//----------------------------------------------------------------------
		"out vec2 uv_interp;\n"
		"out vec4 color_interp;\n"
		"out vec2 vertex_interp;\n"
		//----------------------------------------------------------------------
		"out vec4 vColor;\n"
		"flat out int int_debug;\n"
		"void main() {\n"
		"    mat2 sr = mat2(scaleRot.xy, scaleRot.zw);\n"
		"    gl_Position = vec4(sr*pos + offset, 0.0, 1.0);\n"
		"    vColor = color;\n"
		//--------------------------------------------------------------------------------------------------
		//GODOT Main Vertex
		"varying_A = vec4(read_draw_data_world_x, read_draw_data_world_y);\n"
		"//varying_B = read_draw_data_color_texture_pixel_size;\n"

		"//varying_F = uvec2(read_draw_data_flags, read_draw_data_specular_shininess);\n"
		"//varying_G = read_draw_data_lights;\n"

		"#ifdef USE_ATTRIBUTES\n"

		"vec2 vertex = vertex_attrib;\n"
		"	vec4 color = color_attrib * read_draw_data_modulation;\n"
		"	vec2 uv = uv_attrib;\n"

		"#else\n"
		"vec2 vertex_base_arr[6] = vec2[](vec2(0.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 1.0));\n"
		"vec2 vertex_base = vertex_base_arr[gl_VertexID % 6];\n"

		"vec2 uv = read_draw_data_src_rect.xy + abs(read_draw_data_src_rect.zw) * ((read_draw_data_flags & FLAGS_TRANSPOSE_RECT) != uint(0) ? vertex_base.yx : vertex_base.xy);\n"
		"vec4 color = read_draw_data_modulation;\n"
		"vec2 vertex = read_draw_data_dst_rect.xy + abs(read_draw_data_dst_rect.zw) * mix(vertex_base, vec2(1.0, 1.0) - vertex_base, lessThan(read_draw_data_src_rect.zw, vec2(0.0, 0.0)));\n"

		"#endif\n"

		"mat4 model_matrix = mat4(vec4(read_draw_data_world_x, 0.0, 0.0), vec4(read_draw_data_world_y, 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(read_draw_data_world_ofs, 0.0, 1.0));\n"

		"#ifdef USE_INSTANCING\n"
		"model_matrix = model_matrix * transpose(mat4(instance_xform0, instance_xform1, vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0)));\n"
		"#endif // USE_INSTANCING\n"


		"{\n"
		"//#CODE : VERTEX\n"
		"}\n"


		"vertex = (model_matrix * vec4(vertex, 0.0, 1.0)).xy;\n"
		"color_interp = color;\n"

		"vertex = (canvas_transform * vec4(vertex, 0.0, 1.0)).xy;\n"

		"vertex_interp = vertex;\n"
		"uv_interp = uv;\n"
		//TODO: debug
		//"gl_Position = screen_transform * vec4(vertex, 0.0, 1.0);	\n"
		"int_debug = 0;\n"
		//--------------------------------------------------------------------------------------------------
		"}\n";

static const char FRAGMENT_SHADER[] =
		"#version 300 es\n"
		"#define DISABLE_LIGHTING \n"
		"#define USE_ATTRIBUTES \n"
		"#define USE_INSTANCING \n"
		"#define MAX_GLOBAL_SHADER_UNIFORMS 256 \n"
		"#define MAX_LIGHTS 256 \n"
		//------------------------------------------------------------------------------------------
		//aus #include "canvas_uniforms_inc.glsl" und #include "stdlib_inc.glsl"
		"#define MAX_LIGHTS_PER_ITEM uint(16)\n"

		"#define M_PI 3.14159265359\n"

		"#define SDF_MAX_LENGTH 16384.0\n"

		"//1 means enabled, 2+ means trails in use\n"
		"#define FLAGS_INSTANCING_MASK uint(0x7F)\n"
		"#define FLAGS_INSTANCING_HAS_COLORS uint(1 << 7)\n"
		"#define FLAGS_INSTANCING_HAS_CUSTOM_DATA uint(1 << 8)\n"

		"#define FLAGS_CLIP_RECT_UV uint(1 << 9)\n"
		"#define FLAGS_TRANSPOSE_RECT uint(1 << 10)\n"
		"// (1 << 11) is for FLAGS_CONVERT_ATTRIBUTES_TO_LINEAR in RD backends, unused here.\n"
		"#define FLAGS_NINEPACH_DRAW_CENTER uint(1 << 12)\n"
		"#define FLAGS_USING_PARTICLES uint(1 << 13)\n"

		"#define FLAGS_NINEPATCH_H_MODE_SHIFT 16\n"
		"#define FLAGS_NINEPATCH_V_MODE_SHIFT 18\n"

		"#define FLAGS_LIGHT_COUNT_SHIFT 20\n"

		"#define FLAGS_DEFAULT_NORMAL_MAP_USED uint(1 << 26)\n"
		"#define FLAGS_DEFAULT_SPECULAR_MAP_USED uint(1 << 27)\n"

		"#define FLAGS_USE_MSDF uint(1 << 28)\n"
		"#define FLAGS_USE_LCD uint(1 << 29)\n"

		"#define FLAGS_FLIP_H uint(1 << 30)\n"
		"#define FLAGS_FLIP_V uint(1 << 31)\n"

		"layout(std140) uniform GlobalShaderUniformData { //ubo:1\n"
		"		vec4 global_shader_uniforms[MAX_GLOBAL_SHADER_UNIFORMS];\n"
		"};\n"

		"layout(std140) uniform CanvasData { //ubo:0\n"
		"		mat4 canvas_transform;\n"
		"		mat4 screen_transform;\n"
		"		mat4 canvas_normal_transform;\n"
		"		vec4 canvas_modulation;\n"
		"		vec2 screen_pixel_size;\n"
		"		float time;\n"
		"		bool use_pixel_snap;\n"

		"		vec4 sdf_to_tex;\n"
		"		vec2 screen_to_sdf;\n"
		"		vec2 sdf_to_screen;\n"

		"		uint directional_light_count;\n"
		"		float tex_to_sdf;\n"
		"		uint pad1;\n"
		"		uint pad2;\n"
		"};\n"

		"#ifndef DISABLE_LIGHTING\n"
		"#define LIGHT_FLAGS_BLEND_MASK uint(3 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_ADD uint(0 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_SUB uint(1 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_MIX uint(2 << 16)\n"
		"#define LIGHT_FLAGS_BLEND_MODE_MASK uint(3 << 16)\n"
		"#define LIGHT_FLAGS_HAS_SHADOW uint(1 << 20)\n"
		"#define LIGHT_FLAGS_FILTER_SHIFT 22\n"
		"#define LIGHT_FLAGS_FILTER_MASK uint(3 << 22)\n"
		"#define LIGHT_FLAGS_SHADOW_NEAREST uint(0 << 22)\n"
		"#define LIGHT_FLAGS_SHADOW_PCF5 uint(1 << 22)\n"
		"#define LIGHT_FLAGS_SHADOW_PCF13 uint(2 << 22)\n"

		"struct Light {\n"
		"	mat2x4 texture_matrix; //light to texture coordinate matrix (transposed)\n"
		"	mat2x4 shadow_matrix; //light to shadow coordinate matrix (transposed)\n"
		"	vec4 color;\n"

		"	uint shadow_color; // packed\n"
		"	uint flags; //index to light texture\n"
		"	float shadow_pixel_size;\n"
		"	float height;\n"

		"	vec2 position;\n"
		"	float shadow_zfar_inv;\n"
		"	float shadow_y_ofs;\n"

		"	vec4 atlas_rect;\n"
		"};\n"

		"layout(std140) uniform LightData { //ubo:2\n"
		"		Light light_array[MAX_LIGHTS];\n"
		"};\n"
		"#endif // DISABLE_LIGHTING		\n"
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		"uint godot_packUnorm4x8(vec4 v) {\n"
		"	uvec4 uv = uvec4(round(clamp(v, vec4(0.0), vec4(1.0)) * 255.0));\n"
		"	return uv.x | (uv.y << uint(8)) | (uv.z << uint(16)) | (uv.w << uint(24));\n"
		"}\n"

		"vec4 godot_unpackUnorm4x8(uint p) {\n"
		"	return vec4(float(p & uint(0xff)), float((p >> uint(8)) & uint(0xff)), float((p >> uint(16)) & uint(0xff)), float(p >> uint(24))) * 0.00392156862; // 1.0 / 255.0\n"
		"}\n"

		"uint godot_packSnorm4x8(vec4 v) {\n"
		"	uvec4 uv = uvec4(round(clamp(v, vec4(-1.0), vec4(1.0)) * 127.0) + 127.0);\n"
		"	return uv.x | uv.y << uint(8) | uv.z << uint(16) | uv.w << uint(24);\n"
		"}\n"

		"vec4 godot_unpackSnorm4x8(uint p) {\n"
		"	vec4 v = vec4(float(p & uint(0xff)), float((p >> uint(8)) & uint(0xff)), float((p >> uint(16)) & uint(0xff)), float(p >> uint(24)));\n"
		"	return clamp((v - vec4(127.0)) * vec4(0.00787401574), vec4(-1.0), vec4(1.0));\n"
		"}\n"

		"#define packUnorm4x8 godot_packUnorm4x8\n"
		"#define unpackUnorm4x8 godot_unpackUnorm4x8\n"
		"#define packSnorm4x8 godot_packSnorm4x8\n"
		"#define unpackSnorm4x8 godot_unpackSnorm4x8\n"
		//------------------------------------------------------------------------------------------
		"in vec2 uv_interp;\n"
		"in vec2 vertex_interp;\n"
		"in vec4 color_interp;\n"

		"// Can all be flat as they are the same for the whole batched instance\n"
		"//flat in vec4 varying_A;\n"
		"//flat in vec2 varying_B;\n"
		"//#define read_draw_data_world_x varying_A.xy\n"
		"//#define read_draw_data_world_y varying_A.zw\n"
		"//#define read_draw_data_color_texture_pixel_size varying_B\n"


		"//flat in uvec2 varying_F;\n"
		"//flat in uvec4 varying_G;\n"
		"//#define read_draw_data_flags varying_F.x\n"
		"//#define read_draw_data_specular_shininess varying_F.y\n"
		"//#define read_draw_data_lights varying_G\n"


		"uniform sampler2D color_buffer; //texunit:-4\n"
		"uniform sampler2D sdf_texture; //texunit:-5\n"
		"uniform sampler2D normal_texture; //texunit:-6\n"
		"uniform sampler2D specular_texture; //texunit:-7\n"

		"uniform sampler2D color_texture; //texunit:0\n"

		"layout(location = 0) out vec4 frag_color;		\n"
		//------------------------------------------------------------------------------------------
		"precision mediump float;\n"
		"in vec4 vColor;\n"
		"out vec4 outColor;\n"
		"flat in int int_debug;\n"
		"void main() {\n"
		//"    outColor = vColor;\n"
		//------------------------------------------------------------------------------------------
		//GODOT Main Fragment

		"	vec4 color = color_interp;\n"
		"	vec2 uv = uv_interp;\n"
		"	vec2 vertex = vertex_interp;\n"

		"	//#CODE : FRAGMENT\n"

		"	color *= texture(color_texture, uv);\n"
		"	frag_color = color;		\n"
		"	frag_color = vColor;		\n"
		"	if (int_debug == 1) outColor = vec4(0.5, 0.5, 0.5, 0.5); \n"

		//------------------------------------------------------------------------------------------
		"}\n";


class RendererES3 : public Renderer {
 public:
  RendererES3();
  virtual ~RendererES3();
  bool init();

 private:
  enum { VB_INSTANCE, VB_SCALEROT, VB_OFFSET, VB_COUNT };

  virtual float* mapOffsetBuf();
  virtual void unmapOffsetBuf();
  virtual float* mapTransformBuf();
  virtual void unmapTransformBuf();
  virtual void draw(unsigned int numInstances);

  const EGLContext mEglContext;
  GLuint mProgram;
  GLuint mVB[VB_COUNT];
  GLuint mVBState;
};

Renderer* createES3Renderer() {
  RendererES3* renderer = new RendererES3;
  if (!renderer->init()) {
    delete renderer;
    return NULL;
  }
  return renderer;
}

RendererES3::RendererES3()
    : mEglContext(eglGetCurrentContext()), mProgram(0), mVBState(0) {
  for (int i = 0; i < VB_COUNT; i++) mVB[i] = 0;
}

bool RendererES3::init() {
  mProgram = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
  if (!mProgram) return false;

  glGenBuffers(VB_COUNT, mVB);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD), &QUAD[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
  glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * 4 * sizeof(float), NULL,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_OFFSET]);
  glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * 2 * sizeof(float), NULL,
               GL_STATIC_DRAW);


  glGenVertexArrays(1, &mVBState);
  glBindVertexArray(mVBState);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_INSTANCE]);
  glVertexAttribPointer(POS_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (const GLvoid*)offsetof(Vertex, pos));
  glVertexAttribPointer(COLOR_ATTRIB, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                        sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rgba));
  glEnableVertexAttribArray(POS_ATTRIB);
  glEnableVertexAttribArray(COLOR_ATTRIB);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
  glVertexAttribPointer(SCALEROT_ATTRIB, 4, GL_FLOAT, GL_FALSE,
                        4 * sizeof(float), 0);
  glEnableVertexAttribArray(SCALEROT_ATTRIB);
  glVertexAttribDivisor(SCALEROT_ATTRIB, 1);

  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_OFFSET]);
  glVertexAttribPointer(OFFSET_ATTRIB, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        0);
  glEnableVertexAttribArray(OFFSET_ATTRIB);
  glVertexAttribDivisor(OFFSET_ATTRIB, 1);

  ALOGV("Using OpenGL ES 3.0 renderer");
  return true;
}

RendererES3::~RendererES3() {
  /* The destructor may be called after the context has already been
   * destroyed, in which case our objects have already been destroyed.
   *
   * If the context exists, it must be current. This only happens when we're
   * cleaning up after a failed init().
   */
  if (eglGetCurrentContext() != mEglContext) return;
  glDeleteVertexArrays(1, &mVBState);
  glDeleteBuffers(VB_COUNT, mVB);
  glDeleteProgram(mProgram);
}

float* RendererES3::mapOffsetBuf() {
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_OFFSET]);
  return (float*)glMapBufferRange(
      GL_ARRAY_BUFFER, 0, MAX_INSTANCES * 2 * sizeof(float),
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

void RendererES3::unmapOffsetBuf() { glUnmapBuffer(GL_ARRAY_BUFFER); }

float* RendererES3::mapTransformBuf() {
  glBindBuffer(GL_ARRAY_BUFFER, mVB[VB_SCALEROT]);
  return (float*)glMapBufferRange(
      GL_ARRAY_BUFFER, 0, MAX_INSTANCES * 4 * sizeof(float),
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
}

void RendererES3::unmapTransformBuf() { glUnmapBuffer(GL_ARRAY_BUFFER); }

void RendererES3::draw(unsigned int numInstances) {
  glUseProgram(mProgram);
  glBindVertexArray(mVBState);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numInstances);
}
