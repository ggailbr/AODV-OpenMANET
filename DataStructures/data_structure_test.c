#include "data_structure.h"


int main(){
    data_header T = create_data_header();
    uint32_t test = 0x76112233;
    add_entry_to_data_structure(T, (void *)&test, test);
    entry test_e = find_entry_in_data_structure(T, test);
    printf("%08x\n", *(uint32_t*)test_e);
    return 0;
}