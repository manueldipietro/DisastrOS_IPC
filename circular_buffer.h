#pragma once

void Circular_buffer_read(const char *src, char* dest, int to_read, int size_max, int* read_pos);
void Circular_buffer_write(const char *src, char* dest, int to_write, int size_max, int* write_pos);

