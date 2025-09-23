#ifndef hash.H
#define hash.H
struct Hash_n * create_hash(char* key,int value);
unsigned int hash_func(char* key);
struct Hash_t*init_hash();
void destroy_hashtable(struct Hash_t* table);
bool add_hash_n(struct Hash_t*table,char*key,int value);
bool destroy_hash_n(struct Hash_t * table,char *key);
int look_up_value(struct Hash_t *table,char * key);

#endif // !hash.H
