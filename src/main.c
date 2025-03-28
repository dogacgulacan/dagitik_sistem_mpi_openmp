#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>
#include "data_processor.h"

/* Sabit değerlerin tanımlanması */
#define MAX_LINE_LENGTH 1024  // Dosyadaki bir satırın maksimum uzunluğu
#define MASTER 0              // Ana düğüm (master) için tanımlanan rank değeri

int main(int argc, char *argv[]) {
    int rank, size, i;
    double start_time, end_time;
    char *input_file = "data/input_data.txt";
    
    /* MPI ortamını başlat */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Her düğümün kendi rank değerini alması
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Toplam düğüm sayısını alması

    /* Ana düğüm için başlangıç mesajları ve zamanlama başlatma */
    if (rank == MASTER) {
        printf("Dağıtık veri işleme başlatılıyor. Toplam %d düğüm çalışıyor.\n", size);
        start_time = MPI_Wtime();  // Zaman ölçümünü başlat
    }

    /* Dosya okuma işlemleri (sadece ana düğüm tarafından yapılır) */
    int total_lines = 0;
    char **all_data = NULL;
    
    if (rank == MASTER) {
        FILE *file = fopen(input_file, "r");
        if (!file) {
            printf("Dosya açılamadı: %s\n", input_file);
            MPI_Abort(MPI_COMM_WORLD, 1);  // Hata durumunda MPI işlemini sonlandır
        }
        
        /* Dosyadaki toplam satır sayısını hesapla */
        char line[MAX_LINE_LENGTH];
        while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
            total_lines++;
        }
        
        rewind(file);  // Dosya göstergesini başa al
        
        /* Tüm dosya içeriğini belleğe al */
        all_data = (char **)malloc(total_lines * sizeof(char *));
        for (i = 0; i < total_lines; i++) {
            all_data[i] = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
            fgets(all_data[i], MAX_LINE_LENGTH, file);
        }
        
        fclose(file);
        printf("Toplam %d satır veri okundu.\n", total_lines);
    }

    /* Toplam satır sayısını tüm düğümlere yayınla */
    MPI_Bcast(&total_lines, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    
 
    int lines_per_process = total_lines / size;  // Her düğüme düşen temel satır sayısı
    int remainder = total_lines % size;          // Artık kalan satırlar
    int my_start, my_end;
    
    /* Satırların dengeli dağıtımı için - kalan satırlar ilk düğümlere ekstra olarak verilir */
    if (rank < remainder) {
        my_start = rank * (lines_per_process + 1);
        my_end = my_start + lines_per_process + 1;
    } else {
        my_start = rank * lines_per_process + remainder;
        my_end = my_start + lines_per_process;
    }
    
    int my_lines = my_end - my_start;  // Bu düğümün işleyeceği toplam satır sayısı

    /* Her düğüm kendi veri payı için bellek ayırır */
    char **my_data = (char **)malloc(my_lines * sizeof(char *));
    for (i = 0; i < my_lines; i++) {
        my_data[i] = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    }
    
    /* VERİ DAĞITIMI - Ana düğümden diğer tüm düğümlere verilerin gönderilmesi */
    if (rank == MASTER) {
        /* Ana düğüm kendi verisini kopyalar */
        for (i = 0; i < my_lines; i++) {
            strcpy(my_data[i], all_data[i + my_start]);
        }
        
        /* Ana düğüm diğer düğümlere veri gönderir */
        for (i = 1; i < size; i++) {
            int start, end, lines;
            
            /* Her düğümün başlangıç ve bitiş indekslerini hesapla */
            if (i < remainder) {
                start = i * (lines_per_process + 1);
                end = start + lines_per_process + 1;
            } else {
                start = i * lines_per_process + remainder;
                end = start + lines_per_process;
            }
            
            lines = end - start;
            
            /* Her satırı ilgili düğüme gönder */
            for (int j = 0; j < lines; j++) {
                MPI_Send(all_data[start + j], MAX_LINE_LENGTH, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        /* Diğer düğümler veriyi alır */
        for (i = 0; i < my_lines; i++) {
            MPI_Recv(my_data[i], MAX_LINE_LENGTH, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    /* VERİ İŞLEME - OpenMP kullanarak düğüm içinde çok iş parçacıklı paralel işleme */
    double local_sum = 0.0;
    
    #pragma omp parallel
    {
        #pragma omp single
        printf("Düğüm %d, %d thread ile OpenMP işlemi başlatıyor\n", rank, omp_get_num_threads());
        
        /* İş parçacıkları arasında dinamik iş dağılımı ve sonuçların toplanması için reduction kullanımı */
        #pragma omp for reduction(+:local_sum) schedule(dynamic)
        for (i = 0; i < my_lines; i++) {
            local_sum += process_data_line(my_data[i]);  // Her satırı işle ve sonucu topla
        }
    }
    
    printf("Düğüm %d, işlem sonucu: %f\n", rank, local_sum);

    /* SONUÇLARI TOPLAMA - Tüm düğümlerden gelen sonuçları birleştirme */
    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);
    
    /* Ana düğüm tarafından sonuçların raporlanması ve zamanlamanın bitirilmesi */
    if (rank == MASTER) {
        end_time = MPI_Wtime();
        printf("Toplam işlem sonucu: %f\n", global_sum);
        printf("Toplam çalışma süresi: %f saniye\n", end_time - start_time);
    }

    /* Bellek temizleme işlemleri */
    for (i = 0; i < my_lines; i++) {
        free(my_data[i]);
    }
    free(my_data);
    
    if (rank == MASTER && all_data) {
        for (i = 0; i < total_lines; i++) {
            free(all_data[i]);
        }
        free(all_data);
    }
    
    /* MPI ortamını sonlandır */
    MPI_Finalize();
    return 0;
}