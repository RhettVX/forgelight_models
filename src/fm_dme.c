#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "forgelight_lib.h"
#include "internal.h"


typedef enum input_layout_type
    {
    INPUT_LAYOUT_TYPE_UNKNOWN,
    INPUT_LAYOUT_TYPE_FLOAT1,
    INPUT_LAYOUT_TYPE_FLOAT2,
    INPUT_LAYOUT_TYPE_FLOAT3,
    INPUT_LAYOUT_TYPE_FLOAT4,
    INPUT_LAYOUT_TYPE_HFLOAT2,
    INPUT_LAYOUT_TYPE_SHORT2,
    INPUT_LAYOUT_TYPE_SHORT4,
    INPUT_LAYOUT_TYPE_UBYTE4N,
    INPUT_LAYOUT_TYPE_D3DCOLOR,
    } input_layout_type;

typedef enum input_layout_usage
    {
    INPUT_LAYOUT_USAGE_UNKNOWN,
    INPUT_LAYOUT_USAGE_POSITION,
    INPUT_LAYOUT_USAGE_TEXCOORD,
    INPUT_LAYOUT_USAGE_BINORMAL,
    INPUT_LAYOUT_USAGE_BLENDWEIGHT,
    INPUT_LAYOUT_USAGE_BLENDINDICES,
    INPUT_LAYOUT_USAGE_COLOR,
    INPUT_LAYOUT_USAGE_NORMAL,
    INPUT_LAYOUT_USAGE_TANGENT,
    } input_layout_usage;


#define LAYOUT_ENTRY_COUNT 16
// TODO(rhett): Should I use an array of entry structs or just arrays of each field?
typedef struct input_layout input_layout;
struct input_layout
    {
    u32                name_hash;
    u32                entry_count;
    u32                streams[LAYOUT_ENTRY_COUNT];
    input_layout_type  types[LAYOUT_ENTRY_COUNT];
    input_layout_usage usages[LAYOUT_ENTRY_COUNT];
    };


#define MAX_FIELDS 8

typedef enum node_type
    {
    NODE_TYPE_OBJECT,
    NODE_TYPE_ARRAY,
    NODE_TYPE_STRING,
    } node_type;

typedef struct string_ref string_ref;
struct string_ref
    {
    u32 begin;
    u32 length;
    };

typedef struct node node;
struct node
    {
    string_ref strings[8];
    };

#define PEEK_U8(ptr, n) *(u8*)((uptr)ptr + n)


void
fm_parse_materials_file(u8* buffer, u32 buffer_length)
    {
    os_load_entire_file("../resources/materials_3.xml", buffer, buffer_length);
    input_layout layout = { .name_hash   = 1000,
                            .entry_count = 1,
                            .types[0]    = INPUT_LAYOUT_TYPE_FLOAT3,
                            .usages[0]   = INPUT_LAYOUT_USAGE_POSITION };

    // NOTE(rhett): Scan tokens
    u32 pos = 0;
    u32 line = 1;
    u32 token_count = 0;
    node nodes[8192] = {0};
    EVAL_PRINT_U64(sizeof(node) * 8192);

    u8* output_buffer, * base;
    output_buffer = base = calloc(MB(1), 1);


    u8* c = buffer;
    do
        {
        switch(*c++)
            {
        case '<':
            // // TODO(rhett): 
            // if (PEEK_U8(c, 0) == '!' && PEEK_U8(c, 1) == '-' && PEEK_U8(c, 2) == '-')
            //     {
            //     c += 3;
            //     for (;;)
            //         {
            //         if (PEEK_U8(c, 0) == '-' && PEEK_U8(c, 1) == '-' && PEEK_U8(c, 2) == '>')
            //             {
            //             break;
            //             }
            //         c++;
            //         }
            //     c += 3;
            //     // tokens[token_count++] = XML_TOKEN_TYPE_COMMENT;
            //     break;
            //     }
            // // tokens[token_count++] = XML_TOKEN_TYPE_TAG_BEGIN;
            node this_node = {0};
            // node this_node = {.begin = (u32)((uptr)c - (uptr)buffer)};
            // EVAL_PRINT_U32(this_node.begin);
            for (;;)
                {
                if (PEEK_U8(c, 0) == '>')
                    {
                    c++;
                    break;
                    }

                if (PEEK_U8(c, 0))

                this_node.length++;
                c++;
                }
            nodes[token_count++] = this_node;
            for (u32 i = 0; i < nodes[token_count - 1].length; i++)
                {
                sprintf(output_buffer++, "%c", *(char*)(buffer+nodes[token_count - 1].begin+i));
                }
            sprintf(output_buffer++, "\n");
            // EVAL_PRINT_U32(this_node.length);
            break;

        // NOTE(rhett): Whitespace
        case ' ':
        case '\r':
        case '\t':
            break;

        case '\n':
            // tokens[token_count++] = XML_TOKEN_TYPE_NEWLINE;
            line++;
            break;

        default:
            printf("Unexpected character on line %d: \'%c\'\n", line, *c);
            }
        } while (*c != 0);

    os_write_buffer_to_file("output_buffer_test.txt", base, MB(1));
    return;
    }


Model
fm_dme_load_from_file(char const* file_name)
    {
    Model model = {0};

    u32 bytes_read;
    u8* file_data = LoadFileData(file_name, &bytes_read);

    if (file_data != NULL)
        {
        u32 offset = 0;

        u8 dmod_magic[4]; 
        memcpy(file_data, dmod_magic, 4);
            offset += 4;
        u32 version = endian_get_u32_le(offset+file_data);
            offset += 4;
        u32 dmat_length = endian_get_u32_le(offset+file_data);
            offset += 4;

        // TODO(rhett): Skip DMAT for now
        offset += dmat_length;

        // TODO(rhett): Skip AABB
        offset += 24;

        model.meshCount = endian_get_u32_le(offset+file_data);
            offset += 4;

        model.materialCount = 1;

        model.meshes = RL_CALLOC(model.meshCount, sizeof(Mesh));
        model.materials = RL_CALLOC(model.materialCount, sizeof(Material));
        model.meshMaterial = RL_CALLOC(model.meshCount, sizeof(int));


        for (int i = 0; i < model.meshCount; i++)
            {
            // u32 drawcall_offset = endian_get_u32_le(offset+file_data);
                offset += 4;
            // u32 drawcall_count = endian_get_u32_le(offset+file_data);
                offset += 4;
            // u32 bone_transform_count = endian_get_u32_le(offset+file_data);
                offset += 4;
            // u32 unk0 = endian_get_u32_le(offset+file_data);
                offset += 4;
            u32 vertex_stream_count = endian_get_u32_le(offset+file_data);
                offset += 4;
            // u32 index_length = endian_get_u32_le(offset+file_data);
                offset += 4;
            u32 index_count = endian_get_u32_le(offset+file_data);
                offset += 4;
            u32 vertex_count = endian_get_u32_le(offset+file_data);
                offset += 4;

            model.meshes[i].vertexCount = index_count;
            model.meshes[i].triangleCount = index_count/3;

            model.meshes[i].vertices   = RL_CALLOC(model.meshes[i].vertexCount*3, sizeof(float));
            model.meshes[i].texcoords  = RL_CALLOC(model.meshes[i].vertexCount*2, sizeof(float));
            // model.meshes[i].normals    = RL_CALLOC(model.meshes[i].vertexCount*3, sizeof(float));
            // model.meshes[i].tangents   = RL_CALLOC(model.meshes[i].vertexCount*4, sizeof(float));
            // f32* tangents              = RL_CALLOC(model.meshes[i].vertexCount*4, sizeof(f32));
            // f32* binormals             = RL_CALLOC(model.meshes[i].vertexCount*4, sizeof(f32));

            model.meshMaterial[i] = i;

            // TODO(rhett): HARDCODED FOR MODELSKINNED
            // NOTE(rhett): Vertex stream 1
            u32 bytes_per_vertex = endian_get_u32_le(offset+file_data);
                offset += 4;

            // TODO(rhett): We will need to load materials_3.xml for this
            for (uint j = 0; j < vertex_count; j++)
                {
                f32* ptr_vertices = &model.meshes[i].vertices[j * 3];

                ptr_vertices[0] = endian_get_f32_le(offset+file_data);
                ptr_vertices[1] = endian_get_f32_le(offset+file_data+4);
                ptr_vertices[2] = endian_get_f32_le(offset+file_data+8);
                    offset += bytes_per_vertex;
                }

            // NOTE(rhett): Vertex stream 2
            bytes_per_vertex = endian_get_u32_le(offset+file_data);
                offset += 4;

            for (uint j = 0; j < vertex_count; j++)
                {
                    // NOTE(rhett): tangents and binormals aren't quite stored as floats, so u32 is used
                    // f32* ptr_tangents  = &tangents[j * sizeof(u32)];
                    // f32* ptr_tangents  = &model.meshes[i].tangents[j * sizeof(u32)];
                    // f32* ptr_binormals = &binormals[j * sizeof(u32)];
                    f32* ptr_texcoords = &model.meshes[i].texcoords[j * sizeof(u16)];

                    // ptr_tangents[0] = ((f32)*(u8*)(offset+file_data)   / 255.0f);
                    // ptr_tangents[1] = ((f32)*(u8*)(offset+file_data+1) / 255.0f);
                    // ptr_tangents[2] = ((f32)*(u8*)(offset+file_data+2) / 255.0f);
                    // ptr_tangents[3] = ((f32)*(u8*)(offset+file_data+3) / 255.0f);

                    // binormals[0] = ((f32)*(u8*)(offset+file_data+4) / 255.0f);
                    // binormals[1] = ((f32)*(u8*)(offset+file_data+5) / 255.0f);
                    // binormals[2] = ((f32)*(u8*)(offset+file_data+6) / 255.0f);
                    // binormals[3] = ((f32)*(u8*)(offset+file_data+7) / 255.0f);

                    // TODO(rhett): Skipping colors for now
                    ptr_texcoords[0] = endian_get_f16_as_f32_le(offset+file_data+12);
                    ptr_texcoords[1] = endian_get_f16_as_f32_le(offset+file_data+14);

                        offset += bytes_per_vertex;
                }

            // NOTE(rhett): Calculate normals
            // for (uint j = 0; j < vertex_count; j++)
            //     {
            //     f32* ptr_tangents  = &tangents[j*4];
            //     f32* ptr_tangents  = &model.meshes[i].tangents[j*4];
            //     f32* ptr_normals   = &model.meshes[i].normals[j*3];
            //     f32* ptr_binormals = &binormals[j*4];
                
            //     i32 sign = 1;
            //     if (ptr_tangents[3] == 1)
            //         {
            //         sign = -1;
            //         }

            //     ptr_normals[0] = (ptr_binormals[1] * ptr_tangents[2] - ptr_binormals[2] * ptr_tangents[1]) * sign;
            //     ptr_normals[1] = (ptr_binormals[2] * ptr_tangents[0] - ptr_binormals[0] * ptr_tangents[2]) * sign;
            //     ptr_normals[2] = (ptr_binormals[0] * ptr_tangents[1] - ptr_binormals[1] * ptr_tangents[0]) * sign;
            //     }

            // RL_FREE(binormals);

            model.meshes[i].indices = RL_CALLOC(index_count, sizeof(u16));
            for (uint j = 0; j < index_count; j++, offset+=2)
                {
                model.meshes[i].indices[j] = endian_get_u16_le(offset+file_data);
                }
            }

        for (int i = 0; i < model.materialCount; i++)
            {
            model.materials[i] = LoadMaterialDefault();
            }

        }

    UnloadFileData(file_data);

    // NOTE(rhett): Prepare for use. Stealing from raylib's LoadModel()
    model.transform = MatrixIdentity();

    for (int i = 0; i < model.meshCount; i++)
        {
        UploadMesh(&model.meshes[i], false);
        }

    return model;
    }
