


uint32_t read_vfs(char* file_path, char* buffer, uint32_t offset, uint32_t size);
uint32_t write_vfs(char* file_path, char* buffer, uint32_t offset, uint32_t size);
uint32_t readdir_vfs(char* file_path, dir_entry_t* dir_entry, uint32_t index);
uint32_t mk_file_vfs(char* file_path, uint32_t file_type);
uint32_t stats_vfs(char* file_path, struct file_stats_s* file_stats);