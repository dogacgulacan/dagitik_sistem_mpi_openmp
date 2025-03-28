#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "data_processor.h"

// Satır verilerini işleyen fonksiyon
double process_data_line(char *line) {
    double sum = 0.0;
    char *token;
    char *line_copy = strdup(line);
    
    // Satırı boşluklara göre parçala ve sayıları topla
    token = strtok(line_copy, " \t\n");
    while (token != NULL) {
        sum += atof(token);
        token = strtok(NULL, " \t\n");
    }
    
    // CPU yoğun işlem similasyonu
    double result = sum;
    for (int i = 0; i < 1000000; i++) {
        result = result + (sum * 0.0000001);
    }
    
    free(line_copy);
    return result;
}