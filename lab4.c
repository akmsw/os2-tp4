#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <limits.h>

long total_threads;

typedef struct PGMStruct
{
    char pgmType[3];
    unsigned char **data;
    unsigned int width;
    unsigned int height;
    unsigned int maxValue;
} PGMStruct;

void ignoreComments(FILE *fp)
{
    int ch;

    while ((ch = fgetc(fp)) != EOF && isspace(ch));

    if (ch == '#')
    {
        char line[100];

        if (fgets(line, sizeof(line), fp) == NULL)
        {
            printf("could not read pgm file data\n");
            exit(EXIT_FAILURE);
        }

        ignoreComments(fp);
    }
    else
        fseek(fp, -1, SEEK_CUR);
}

bool openPGM(PGMStruct *pgm, const char *filename)
{
    FILE *pgmfile = fopen(filename, "rb");

    if (pgmfile == NULL)
    {
        printf("file does not exist\n");
        return false;
    }

    if (fscanf(pgmfile, "%3s", pgm->pgmType) == EOF)
    {
        printf("could not read pgm file data\n");
        fclose(pgmfile);
        return false;
    }

    ignoreComments(pgmfile);

    if ((fscanf(pgmfile, "%u %u", &(pgm->width), &(pgm->height)) == EOF) ||
        (fscanf(pgmfile, "%u", &(pgm->maxValue)) == EOF))
    {
        printf("could not read pgm file data\n");
        fclose(pgmfile);
        return false;
    }

    pgm->data = malloc(pgm->height * sizeof(unsigned char *));

    if (!pgm->data)
    {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    fgetc(pgmfile);

    for (unsigned int i = 0; i < pgm->height; i++)
    {
        pgm->data[i] = malloc(pgm->width * sizeof(unsigned char));

        if (!pgm->data[i])
        {
            fprintf(stderr, "malloc failed\n");
            return false;
        }

        if (fread(pgm->data[i], sizeof(unsigned char), pgm->width, pgmfile) == 0)
        {
            fprintf(stderr, "fread failed\n");
            return false;
        }
    }

    if (fclose(pgmfile) != 0)
    {
        fprintf(stderr, "fclose failed\n");
        return false;
    }

    return true;
}

void saveCoords(unsigned int x, unsigned int y, unsigned int target_width, unsigned int target_height)
{
    FILE *f = fopen("match_coords.txt", "w");

    if (!f)
    {
        printf("fopen failed\n");
        exit(1);
    }

    fprintf(f, "%u,%u,%u,%u", x, y, target_width, target_height);

    if (fclose(f) != 0)
    {
        fprintf(stderr, "fclose failed\n");
        exit(1);
    }
}

void printPGMData(PGMStruct *pgm)
{
    fprintf(stdout, "pgm file type: %s\n", pgm->pgmType);
    fprintf(stdout, "pgm file width: %u\n", pgm->width);
    fprintf(stdout, "pgm file height: %u\n", pgm->height);
    fprintf(stdout, "pgm file maximum gray value: %u\n", pgm->maxValue);
}

void getMatch(PGMStruct *main, PGMStruct *target)
{
    unsigned long acc;

    unsigned int LIM_HEIGHT = main->height - target->height + 1;
    unsigned int LIM_WIDTH = main->width - target->width + 1;

    unsigned int i, j, k, l;

    #pragma omp parallel for private(i, j, k, l, acc) num_threads(total_threads) schedule(dynamic)
    for (i = 0; i < LIM_HEIGHT; i++)
        for (j = 0; j < LIM_WIDTH; j++)
        {
            acc = 0;

            for (k = 0; k < target->height; k++)
                for (l = 0; l < target->width; l++)
                {
                    int temp = target->data[k][l] - main->data[k + i][l + j];
                    acc += (unsigned long)(temp * temp);
                }

            if (acc == 0)
            {
                printf("se halló un 0 en (%u, %u)\n", j, i);
                saveCoords(j, i, target->width, target->height);
            }
        }
}

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("ingrese el nombre de los archivos main y target (deben ser imágenes png), y la cantidad de threads a utilizar\n");
        return EXIT_FAILURE;
    }

    total_threads = strtol(argv[3], NULL, 10);

    if ((total_threads == LONG_MAX) || (total_threads == LONG_MIN))
    {
        fprintf(stderr, "strtol failed\n");
        return EXIT_FAILURE;
    }

    if (total_threads < 1)
    {
        printf("no es posible correr el programa con menos de 1 hilo\n");
        return EXIT_FAILURE;
    }

    PGMStruct *mainPGM = malloc(sizeof(PGMStruct));
    PGMStruct *targetPGM = malloc(sizeof(PGMStruct));

    char *mainFile = malloc(sizeof(char) * (strlen(argv[1]) + 16));
    char *targetFile = malloc(sizeof(char) * (strlen(argv[2]) + 16));

    if (!mainPGM || !targetPGM || !mainFile || !targetFile)
    {
        fprintf(stderr, "malloc failed\n");
        free(mainPGM);
        free(mainFile);
        free(targetPGM);
        free(targetFile);
        return EXIT_FAILURE;
    }

    strncpy(mainFile, "./img/", 11);
    strcat(mainFile, argv[1]);
    strcat(mainFile, ".pgm");

    strncpy(targetFile, "./img/", 11);
    strcat(targetFile, argv[2]);
    strcat(targetFile, ".pgm");

    openPGM(mainPGM, mainFile);
    printf("---main picture pgm info---\n");
    printPGMData(mainPGM);

    openPGM(targetPGM, targetFile);
    printf("\n---target picture pgm info---\n");
    printPGMData(targetPGM);

    printf("\n");

    getMatch(mainPGM, targetPGM);

    free(mainPGM);
    free(mainFile);
    free(targetPGM);
    free(targetFile);

    return EXIT_SUCCESS;
}