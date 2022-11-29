#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#define DIR_MAX 20
#define FILE_MAX 80

void ls_R(const char *name)
{
    DIR *dir;
    struct dirent *pde;
    char temp[DIR_MAX][FILE_MAX];
    char *dir_path[DIR_MAX];
    int index = 0, p_index = 0;

    for (int i = 0; i < DIR_MAX; i++)
    {
        dir_path[i] = NULL;
        for (int j = 0; j < FILE_MAX; j++)
            temp[i][j] = NULL;
    }

    dir = opendir(name);
    if (dir == NULL)
        return;

    printf("%s: \n", name);
    while ((pde = readdir(dir)) != NULL)
    {
        if (pde->d_type == DT_DIR)
        {
            char path[1024];
           if (strcmp(pde->d_name, ".") == 0 || strcmp(pde->d_name, "..") == 0 || strcmp(pde->d_name, ".git") == 0){
                continue;
           }
            snprintf(path, sizeof(path), "%s/%s", name, pde->d_name);

            for (int k = 0; k < strlen(path); k++){
                temp[index][k] = path[k];
            }
            dir_path[index] = temp[index];
            index++;

            printf("%s\t", dir_path[index - 1]);
        }
        else{
            printf("%s\t", pde->d_name);
        }
    }

    printf("\n\n");

    while (dir_path[p_index] != NULL)
    {
        if (dir_path[p_index] == NULL){
            break;
        }
        ls_R(dir_path[p_index]);
        p_index++;
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    char dir[1024];

    if (argc == 1)
    {
        strcpy(dir, ".");
        ls_R(dir);
    }
    else
        for (int i = 1; i < argc; i++)
        {
            strcpy(dir, argv[i]);
            ls_R(dir);
        }
    return 0;
}

