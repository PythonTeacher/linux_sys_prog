#include <stdio.h>

struct person {
    char name[20];
    int age;
};

int write_to_file() {
    FILE *fp;
    size_t ret;

    fp = fopen("person.dat", "wb");
    if (fp == NULL) {
        printf("fopen failed\n");
        return -1;
    }
    
    struct person p = {        
        .name = "IU",
        .age = 29
    };

    ret = fwrite(&p, sizeof(struct person), 1, fp);
    if (ret != 1) {
        printf("fwrite failed\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

int read_from_file() {
    FILE *fp;
    size_t ret;
    struct person p;

    fp = fopen("person.dat", "rb");
    if (fp == NULL) {
        printf("fopen failed\n");
        return -1;
    }

    ret = fread(&p, sizeof(struct person), 1, fp);
    if (ret != 1) {
        printf("fread failed\n");
        fclose(fp);
        return -1;
    }

    printf("name:%s, age:%d\n", p.name, p.age);

    fclose(fp);
    return 0;
}

int main(int argc, char **argv) {

    if (write_to_file()) {
        printf("write_to_file failed\n");
        return -1;
    }

    if (read_from_file()) {
        printf("read_from _file failed\n");
        return -1;
    }

    return 0;
}