#include <iostream>
#include <vector>

extern int fill_buffer(int *buf, int len){
    int out = 0;
    for(int i=0; i<len; i++){
        out = out + i;
        std::cout << buf[i] << std::endl;
    }
    return out;
}