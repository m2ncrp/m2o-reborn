#define settings_default                \
    "{\n"                               \
    "   hostname: \"my m2o server\",\n" \
    "   password: \"\",\n"              \
    "   port: 27010,\n\n"               \
                                        \
    "   /* settings for amounts */\n"   \
    "   max_entities: 16384,\n"         \
    "   max_connections: 128,\n"        \
    "   resources: [],\n"               \
    "}\n"

zplj_object_t *settings_read_value(zplj_object_t *obj, char *name) {
    for (i32 i = 0; i < zpl_array_count(obj->nodes); ++i) {
        zplj_object_t *ptr = (obj->nodes + i);
        if (!strcmp(ptr->name, name)) {
            return ptr;
        }
    }

    return NULL;
}

#define settings_readto(handle, name, type, dest) do { \
        zplj_object_t *element = settings_read_value(handle, (char *)name); \
        if (element) { dest = element->type; } \
    } while(0)

void settings_read(librg_config_t *config, librg_address_t *address, mod_settings_t *settings) {
    zpl_file_t file;
    zpl_file_error_e error = zpl_file_open(&file, "server.json");

    // create file if didnt exist
    if (error != 0) {
        zpl_file_create(&file, "server.json");
        zpl_file_write(&file, settings_default, zpl_strlen(settings_default));
        zpl_file_seek(&file, 0);
    }

    // read and copy the data
    isize file_size = zpl_file_size(&file);
    char *content = (char *)zpl_malloc(file_size+1);
    zpl_file_read(&file, content, file_size);
    content[file_size] = 0;
    zpl_file_close(&file);

    // parse json
    zplj_object_t root = {0}; u8 error2;
    zplj_parse(&root, file_size, content, zpl_heap_allocator(), true, &error2);

    // read up the data
    settings_readto(&root, "port", integer, address->port);
    settings_readto(&root, "max_entities", integer, config->max_entities);
    settings_readto(&root, "max_connections", integer, config->max_connections);

    settings_readto(&root, "hostname", string, settings->hostname);
    settings_readto(&root, "password", string, settings->password);

    // free
    zplj_free(&root);
    zpl_mfree(content);
}
