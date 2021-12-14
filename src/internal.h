#ifndef INTERNAL_H
#define INTERNAL_H


#define FM_MATERIALS_FILE_SIZE MB(1)


extern void
fm_parse_materials_file(u8* buffer, u32 buffer_length);

extern Model
fm_dme_load_from_file(char const* file_name);


#endif // INTERNAL_H
