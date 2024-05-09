#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<time.h> 
#include<sys/wait.h> 

int main(int argc, char *argv[]) 
{ 
    char *jenis = argv[1];
    char pengali1[100];
    char pengali2[100];
	int parentsfd[2];
	int childfd[2];
	pid_t p; 

    time_t current_time;
    struct tm* time_info;
    time(&current_time);
    time_info = localtime(&current_time);

    pipe(parentsfd);
    pipe(childfd);

	scanf("%s", pengali1); 
    scanf("%s", pengali2); 
    
	p = fork(); 
    
    if (p > 0) 
	{   
        close(parentsfd[0]);
        int pengubah, pengubah2;
        char tulishasil[100];

        //pengali1
        switch (pengali1[0]) {
        case 's':
        case 'S':
            if (pengali1[1] == 'e' || pengali1[1] == 'E') {
            pengubah = 9;
            } 
            else if (pengali1[1] == 'a' || pengali1[1] == 'A') {
            pengubah = 1;
            }
            break;

        case 'd':
        case 'D':
            if (pengali1[1] == 'e' || pengali1[1] == 'E') {
            pengubah = 8;
            } 
            else if (pengali1[1] == 'u' || pengali1[1] == 'U') {
            pengubah = 2;
            }
            break;

        case 't':
        case 'T':
            if (pengali1[1] == 'i' || pengali1[1] == 'I') {
            pengubah = 3;
            } 
            else if (pengali1[1] == 'u' || pengali1[1] == 'U') {
            pengubah = 7;
            }
            break;

        case 'e':
        case 'E':
            if (pengali1[1] == 'm' || pengali1[1] == 'M') {
            pengubah = 4;
            } 
            else if (pengali1[1] == 'n' || pengali1[1] == 'N') {
            pengubah = 6;
            }
            break;

        case 'l':
        case 'L':
            pengubah = 5;
            break;
        }


        //pengali2
        switch (pengali2[0]) {
        case 's':
        case 'S':
            if (pengali2[1] == 'e' || pengali2[1] == 'E') {
            pengubah2 = 9;
            } 
            else if (pengali2[1] == 'a' || pengali2[1] == 'A') {
            pengubah2 = 1;
            }
            break;

        case 'd':
        case 'D':
            if (pengali2[1] == 'e' || pengali2[1] == 'E') {
            pengubah2 = 8;
            } 
            else if (pengali2[1] == 'u' || pengali2[1] == 'U') {
            pengubah2 = 2;
            }
            break;

        case 't':
        case 'T':
            if (pengali2[1] == 'i' || pengali2[1] == 'I') {
            pengubah2 = 3;
            } 
            else if (pengali2[1] == 'u' || pengali2[1] == 'U') {
            pengubah2 = 7;
            }
            break;

        case 'e':
        case 'E':
            if (pengali2[1] == 'm' || pengali2[1] == 'M') {
            pengubah2 = 4;
            } 
            else if (pengali2[1] == 'n' || pengali2[1] == 'N') {
            pengubah2 = 6;
            }
            break;

        case 'l':
        case 'L':
            pengubah2 = 5;
            break;
        }
        
        int hasil;
        if(strcmp(argv[1], "-kali") == 0){
            hasil = pengubah*pengubah2;
        }
        else if(strcmp(argv[1], "-tambah") == 0){
            hasil = pengubah+pengubah2;
        }
        else if(strcmp(argv[1], "-kurang") == 0){
            hasil = pengubah-pengubah2;
        }
        else if(strcmp(argv[1], "-bagi") == 0){
            hasil = pengubah/pengubah2;
        }
        
        write(parentsfd[1], &hasil, strlen(pengali1)+1);
        close(parentsfd[1]); 

        wait(NULL);

        close(childfd[1]); 

        read(childfd[0], tulishasil, 100);
        FILE *file_ptr;
        if (fopen("histori.log", "r") == 0)
        {
            file_ptr = fopen("histori.log", "w");
        }
        else{
            file_ptr = fopen("histori.log", "a");
        }


        if(strcmp(argv[1], "-kali") == 0){
            fprintf(file_ptr, "[%02d/%02d/%02d %02d:%02d:%02d] [KALI] %s kali %s sama dengan %s.\n",
           time_info->tm_mday,
           time_info->tm_mon + 1,
           time_info->tm_year-100,
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec,
           pengali1, 
           pengali2, 
           tulishasil);
        }
        else if(strcmp(argv[1], "-tambah") == 0){
        fprintf(file_ptr, "[%02d/%02d/%02d %02d:%02d:%02d] [TAMBAH] %s tambah %s sama dengan %s.\n",
           time_info->tm_mday,
           time_info->tm_mon + 1,
           time_info->tm_year-100,
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec,
           pengali1, 
           pengali2, 
           tulishasil);

        }
        else if(strcmp(argv[1], "-kurang") == 0){
            if(hasil<0){
                fprintf(file_ptr, "[%02d/%02d/%02d %02d:%02d:%02d] [KURANG] %s pada pengurangan.\n",
           time_info->tm_mday,
           time_info->tm_mon + 1,
           time_info->tm_year-100,
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec, tulishasil);
            }
            else{
                fprintf(file_ptr, "[%02d/%02d/%02d %02d:%02d:%02d] [KURANG] %s kurang %s sama dengan %s.\n",
           time_info->tm_mday,
           time_info->tm_mon + 1,
           time_info->tm_year-100,
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec,
           pengali1, 
           pengali2, 
           tulishasil);
            }
        }
        else if(strcmp(argv[1], "-bagi") == 0){
            fprintf(file_ptr, "[%02d/%02d/%02d %02d:%02d:%02d] [BAGI] %s bagi %s sama dengan %s.\n",
           time_info->tm_mday,
           time_info->tm_mon + 1,
           time_info->tm_year-100,
           time_info->tm_hour,
           time_info->tm_min,
           time_info->tm_sec,
           pengali1, 
           pengali2, 
           tulishasil);
        }
        close(childfd[0]);

	} 

	// child process 
	else
	{   
        close(parentsfd[1]);
        int coba;
        read(parentsfd[0], &coba, strlen(pengali1)+1);
        int angka1,angka2;
        char tulisan1[20],tulisan2[20], hasil[100];
        angka1 = coba/10;
        angka2 = coba%10;
        //satu angka
        if(coba<0){
            sprintf(hasil, "ERROR");
        }
        else if(coba < 10){
        switch (angka2) {
        case 0:
            strcpy(tulisan2, "nol");
            break;
        case 1:
            strcpy(tulisan2, "satu");
            break;
        case 2:
            strcpy(tulisan2, "dua");
            break;
        case 3:
            strcpy(tulisan2, "tiga");
            break;
        case 4:
            strcpy(tulisan2, "empat");
            break;
        case 5:
            strcpy(tulisan2, "lima");
            break;
        case 6:
            strcpy(tulisan2, "enam");
            break;
        case 7:
            strcpy(tulisan2, "tujuh");
            break;
        case 8:
            strcpy(tulisan2, "delapan");
            break;
        case 9:
            strcpy(tulisan2, "sembilan");
            break;
        default:
            strcpy(tulisan2, "");
            break;
        }
        sprintf(hasil, "%s", tulisan2);
        }

        //lebih dari 20
        else if(coba < 20){
        switch (angka2) {
        case 0:
            strcpy(tulisan2, "sepuluh");
            sprintf(hasil, "%s", tulisan2);
            break;
        case 1:
            strcpy(tulisan2, "sebelas");
            sprintf(hasil, "%s", tulisan2);
            break;
        case 2:
            strcpy(tulisan2, "dua");
            break;
        case 3:
            strcpy(tulisan2, "tiga");
            break;
        case 4:
            strcpy(tulisan2, "empat");
            break;
        case 5:
            strcpy(tulisan2, "lima");
            break;
        case 6:
            strcpy(tulisan2, "enam");
            break;
        case 7:
            strcpy(tulisan2, "tujuh");
            break;
        case 8:
            strcpy(tulisan2, "delapan");
            break;
        case 9:
            strcpy(tulisan2, "sembilan");
            break;
        default:
            break;
        }

        if(angka2 >=2){
            sprintf(hasil, "%s belas", tulisan2);
        }

        }

        //lebih dari 20
        else if (coba > 19 ){
             switch (angka1) {
        case 1:
            strcpy(tulisan1, "satu");
            break;
        case 2:
            strcpy(tulisan1, "dua");
            break;
        case 3:
            strcpy(tulisan1, "tiga");
            break;
        case 4:
            strcpy(tulisan1, "empat");
            break;
        case 5:
            strcpy(tulisan1, "lima");
            break;
        case 6:
            strcpy(tulisan1, "enam");
            break;
        case 7:
            strcpy(tulisan1, "tujuh");
            break;
        case 8:
            strcpy(tulisan1, "delapan");
            break;
        case 9:
            strcpy(tulisan1, "sembilan");
            break;
        default:
            strcpy(tulisan1, "");
            break;
        }

        switch (angka2) {
        case 1:
            strcpy(tulisan2, "satu");
            break;
        case 2:
            strcpy(tulisan2, "dua");
            break;
        case 3:
            strcpy(tulisan2, "tiga");
            break;
        case 4:
            strcpy(tulisan2, "empat");
            break;
        case 5:
            strcpy(tulisan2, "lima");
            break;
        case 6:
            strcpy(tulisan2, "enam");
            break;
        case 7:
            strcpy(tulisan2, "tujuh");
            break;
        case 8:
            strcpy(tulisan2, "delapan");
            break;
        case 9:
            strcpy(tulisan2, "sembilan");
            break;
        default:
            strcpy(tulisan2, "");
            break;
        }
        sprintf(hasil, "%s puluh %s", tulisan1, tulisan2);
        }
        else{
            sprintf(hasil, "ERROR");
        }

        close(parentsfd[0]);
        close(childfd[0]);

        if(strcmp(argv[1], "-kali") == 0){

            printf("hasil perkalian %s dan %s adalah %s.\n" ,pengali1, pengali2, hasil);
        }
        else if(strcmp(argv[1], "-tambah") == 0){
            printf("hasil penjumlahan %s dan %s adalah %s.\n" ,pengali1, pengali2, hasil);
        }
        else if(strcmp(argv[1], "-kurang") == 0){
            printf("hasil pengurangan %s dan %s adalah %s.\n" ,pengali1, pengali2, hasil);
        }
        else if(strcmp(argv[1], "-bagi") == 0){
            printf("hasil pembagian %s dan %s adalah %s.\n" ,pengali1, pengali2, hasil);
        }

        write(childfd[1], hasil, 100);
        close(childfd[1]);
	} 
} 
