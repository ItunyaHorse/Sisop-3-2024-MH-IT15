# Soal-Shift-Sisop-Modul-3-IT15-2024

Anggota:

1. Michael Kenneth Salim (5027231008)
2. Nicholas Emanuel Fade (5027231070)
3. Veri Rahman (5027231088)

## Soal no 1
Dikerjakan oleh **Nicholas Emanuel Fade (5027231070)**

## Soal no 2
Dikerjakan oleh **Michael Kenneth Salim (5027231008)**

Pada nomer 2 ini, kita pada dasarnya diminta untuk membuat sebuah kalkulator dengan menggunakan sistem fork dan pipe. Program ini memungkinkan kita untuk melakukan operasi aritmatika pada dua bilangan yang direpresentasikan dalam bentuk kata-kata, dan menyimpan hasil operasi beserta informasi waktu ke dalam file histori.log. Program menggunakan konsep pipes dan fork untuk komunikasi antara parent process dan child process.

```bash
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
```

  Pada bagian ini, program melakukan inisialisasi variabel-variabel yang akan digunakan dalam proses utama. Pertama, jenis adalah sebuah pointer ke karakter yang menyimpan jenis operasi aritmatika yang akan dilakukan (misalnya -kali, -tambah, -kurang, atau -bagi) yang diambil dari argumen baris perintah setelah kalkulator. Kemudian, pengali1 dan pengali2 adalah dua array karakter dengan ukuran 100 yang akan digunakan untuk menyimpan dua bilangan dalam bentuk kata-kata yang dimasukkan oleh pengguna. Program juga membuat dua pipa (pipe), yaitu parentsfd dan childfd, yang akan digunakan untuk komunikasi antara proses induk dan proses anak. Variabel p dari tipe pid_t akan menyimpan ID proses anak setelah proses fork dilakukan. Selanjutnya, program mengambil waktu saat ini menggunakan fungsi time dan menyimpannya dalam variabel current_time dan time_info yang berisi informasi waktu dalam format yang lebih terstruktur. Setelah itu, program membuat dua pipa dengan memanggil fungsi pipe untuk parentsfd dan childfd. Terakhir, program membaca dua bilangan dalam bentuk kata-kata dari pengguna menggunakan scanf dan menyimpannya dalam pengali1 dan pengali2.

```bash
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
```
  Pada bagian kode ini, proses induk (parent process) dieksekusi setelah berhasil melakukan fork. Pertama, proses induk menutup ujung pembacaan pipa parentsfd karena hanya akan digunakan untuk menulis. Kemudian, program mendeklarasikan variabel pengubah, pengubah2, dan tulishasil. Variabel pengubah dan pengubah2 digunakan untuk menyimpan representasi angka dari pengali1 dan pengali2 yang sebelumnya berupa kata-kata. Program mengonversi pengali1 dan pengali2 menjadi angka menggunakan struktur switch case berdasarkan dua karakter pertama dari setiap kata. Setelah itu, program melakukan operasi aritmatika yang sesuai (perkalian, penjumlahan, pengurangan, atau pembagian) berdasarkan argumen argv[1] dan menyimpan hasilnya dalam variabel hasil. 
  Hasil tersebut kemudian dikirimkan ke proses anak melalui pipa parentsfd menggunakan fungsi write. Lalu, proses induk menunggu proses anak selesai dengan menggunakan fungsi wait(NULL). Setelah itu, proses induk membaca hasil operasi dalam bentuk kata-kata dari proses anak melalui pipa childfd dan menyimpannya dalam tulishasil. Selanjutnya, program membuka atau membuat file histori.log dan menulis hasil operasi beserta informasi waktu ke dalam file tersebut dengan format yang ditentukan. Terakhir, proses induk menutup ujung penulisan pipa childfd.

```bash
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
```
  Pada bagian kode ini, proses anak (child process) dieksekusi. Pertama, proses anak menutup ujung penulisan pipa parentsfd karena hanya akan digunakan untuk membaca. Kemudian, proses anak membaca hasil operasi dari proses induk melalui pipa parentsfd dan menyimpannya dalam variabel coba. Selanjutnya, program mengonversi nilai coba menjadi representasi kata-kata dengan menggunakan beberapa kondisi dan struktur switch case.
  Jika nilai coba kurang dari 0, maka program akan menyimpan "ERROR" dalam variabel hasil. Jika nilai coba kurang dari 10, program akan mengonversi angka terakhir (angka satuan) menjadi kata-kata. Jika nilai coba antara 10 hingga 19, program akan mengonversi angka terakhir menjadi kata-kata dan menambahkan "belas" jika angka terakhir lebih besar dari 1. Jika nilai coba lebih dari 19, program akan mengonversi angka puluhan dan satuan menjadi kata-kata secara terpisah, kemudian menggabungkannya. Setelah itu, proses anak menutup ujung pembacaan pipa parentsfd dan childfd, mencetak hasil operasi dalam bentuk kalimat, dan mengirimkan hasil operasi dalam bentuk kata-kata ke proses induk melalui pipa childfd sebelum menutup ujung penulisan pipa childfd.
  
### Revisi soal no 2
1. Penambahan pengubah string menjadi integer untuk angka 0

## Soal no 3
Dikerjakan oleh **Michael Kenneth Salim (5027231008)**

Pada nomer 3 ini, kita pada dasarnya diminta untuk membuat sebuah alat komunikasi antara driver dan paddock menggunakan RPC. Untuk melakukan hal tersebut, kita diminta untuk membuat 3 program, yaitu action.c, driver.c, and paddock.c.

```bash
const char *gap(float jarak) {
    if (jarak <= 3.5)
    {
        return "Gogogo";
    }
    else if (jarak <= 10){
        return "Push";
    }
    else{
        return "Stay out of Trouble";
    }
}

const char *fuel(int bensin) {
    if (bensin < 50)
    {
        return "Conserve fuel";
    }
    else if (bensin <= 80){
        return "You can go";
    }
    else{
        return "Push Push Push";
    }
}

const char *ban(int sisa) {
    if (sisa < 30)
    {
        return "Box Box Box";
    }
    else if (sisa <= 50){
        return "Conserve Your Tire";
    }
    else if (sisa <= 80){
        return "Good Tire Wear";
    }
    else{
        return "Push Go Push";
    }
}

const char *tire(char *tipe) {
    if (strcmp("Soft", tipe) == 0)
    {
        return "Mediums Ready";
    }
    else if(strcmp("Medium", tipe) == 0){
        return "Box for Softs";
    }
    else {
        return "Invalid tire type";
    }
}
```
Pada program actions.c ini, terdapat empat fungsi utama yang masing-masing berfungsi untuk memberikan instruksi atau saran kepada driver berdasarkan kondisi tertentu yang dihadapi, yang mana akan digunakan oleh program paddock.c nantinya. Fungsi gap menerima parameter jarak dengan pembalap di depan dalam satuan detik, dan akan memberikan instruksi "Gogogo" jika jarak kurang dari atau sama dengan 3,5 detik, "Push" jika jarak antara 3,5 hingga 10 detik, dan "Stay out of Trouble" jika jarak lebih dari 10 detik. Fungsi fuel menerima parameter sisa bahan bakar dalam persen, dan akan memberikan instruksi "Conserve fuel" jika sisa bahan bakar kurang dari 50%, "You can go" jika sisa bahan bakar antara 50% hingga 80%, dan "Push Push Push" jika sisa bahan bakar lebih dari 80%. Fungsi ban menerima parameter sisa ban dalam satuan tertentu, dan akan memberikan instruksi "Box Box Box" jika sisa ban kurang dari 30, "Conserve Your Tire" jika sisa ban antara 30 hingga 50, "Good Tire Wear" jika sisa ban antara 50 hingga 80, dan "Push Go Push" jika sisa ban lebih dari 80. Terakhir, fungsi tire menerima parameter tipe ban saat ini, dan akan memberikan instruksi "Mediums Ready" jika tipe ban saat ini adalah "Soft", "Box for Softs" jika tipe ban saat ini adalah "Medium", dan "Invalid tire type" jika tipe ban tidak valid.

```bash
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define IP "127.0.0.1"

int main(int argc, char *argv[]) {
    char *jenis = argv[1];
    char *jumlah = argv[2];

    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, IP, &serv_addr.sin_addr);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    bzero(buffer, 1024);
    strcpy(buffer, jenis);
    send(sock, buffer, strlen(buffer), 0);

    FILE* file_ptr = fopen("race.log", "r");
    if (file_ptr == NULL) {
        file_ptr = fopen("race.log", "w");
    }
    fclose(file_ptr);

    if (strcmp("Gap", jenis)== 0)
    {
        float angka = atof(jumlah);
        send(sock, &angka, sizeof(angka), 0);
    }
    else if (strcmp("Fuel", jenis)== 0)
    {
        int angka = atoi(jumlah);
        send(sock, &angka, sizeof(angka), 0);
    }
    else if (strcmp("Tire", jenis)== 0)
    {
        int angka = atoi(jumlah);
        send(sock, &angka, sizeof(angka), 0);
    }
    else if (strcmp("Type", jenis)== 0)
    {
        if(strcmp("Medium", jumlah)== 0){
            bzero(buffer, 1024);
            strcpy(buffer, jumlah);
            send(sock, buffer, strlen(buffer), 0);
        }
        else if (strcmp("Soft", jumlah)== 0){
            bzero(buffer, 1024);
            strcpy(buffer, jumlah);
            send(sock, buffer, strlen(buffer), 0);
        }
    }
    
    printf("[Driver]: [%s][%s]\n", jenis, jumlah);
    bzero(buffer, 1024);
    recv(sock, buffer, sizeof(buffer), 0);
    printf("[Paddock]: [%s]\n", buffer);
}
```
File driver.c merupakan program klien yang berfungsi untuk terhubung dengan server paddock.c dengan menggunakan socket, dimana program ini menerima dua argumen dari baris perintah, yaitu "Gap", "Fuel", "Tire", atau "Type" dan string angka yang mepresentasikan kondisi yang dialami driver. Program membuat koneksi soket dengan alamat IP dan port yang telah ditentukan, kemudian mengirimkan jenis dan jumlah ke server melalui soket. Tergantung pada command yang dikirimkan, program akan mengirimkan data tambahan seperti nilai float untuk "Gap", nilai integer untuk "Fuel" dan "Tire", atau string untuk "Type" (misalnya "Soft" atau "Medium"). Program ini juga berfungsi untuk mendeteksi apakah race.log sudah dibuat atau belum. Apabila belum, maka file race.log akan dibuat menggunakan fungsi fopen dengan inisial w, yaitu write. Setelah itu, program akan mencetak ke layar pesan yang dikirimkan dalam format "[Driver]: [Jenis Perintah][Kondisi]". Kemudian, program akan menerima balasan dari server dan mencetak dalam format "[Paddock]: [balasan]". Program ini memungkinkan driver untuk berinteraksi dengan server paddock.c melalui baris perintah atau argumen program untuk mendapatkan balasan.

```bash
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include "actions.c"

#define PORT 8080
#define LOG_FILE "/home/mken/SISOPraktikum/SISOP3/soal_3/race.log"

void penulis(const char *orang, const char *jenis, const char *jumlah) {
    
    FILE* file_ptr = fopen(LOG_FILE, "a");
    if (file_ptr == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);

    fprintf(file_ptr, "%s [%02d/%02d/%04d %02d:%02d:%02d]: [%s] [%s]\n",
            orang,
            local_time->tm_mday, 
            local_time->tm_mon + 1, 
            local_time->tm_year + 1900,
            local_time->tm_hour, 
            local_time->tm_min, 
            local_time->tm_sec,
            jenis, 
            jumlah);
    printf("it works\n");
    fclose(file_ptr);
}

void daemonize() {
    pid_t pid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    umask(0);

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


int main() {

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }


    daemonize();
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        bzero(buffer, sizeof(buffer));

        if (recv(new_socket, buffer, sizeof(buffer), 0) < 0) {
            perror("Receive failed");
            close(new_socket);
            continue;
        }

        const char *result;

        if (strcmp("Gap", buffer) == 0) {
            float distance;
            if (recv(new_socket, &distance, sizeof(distance), 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char str_distance[100];
            sprintf(str_distance, "%.3f", distance);
            result = gap(distance);
            penulis("[Driver]", buffer, str_distance);

        } 
        
        else if (strcmp("Fuel", buffer) == 0) {
            int bensin;
            if (recv(new_socket, &bensin, sizeof(bensin), 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char str_fuel[100];
            sprintf(str_fuel, "%d", bensin);
            result = fuel(bensin);
            penulis("[Driver]", buffer, str_fuel);

        } 
        
        else if (strcmp("Tire", buffer) == 0) {
            int sisa;
            if (recv(new_socket, &sisa, sizeof(sisa), 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char str_sisa[100];
            sprintf(str_sisa, "%d", sisa);
            result = ban(sisa);
            penulis("[Driver]", buffer, str_sisa);

        } 
        
        else if (strcmp("Type", buffer) == 0) {
            char tire_type[256];
            if (recv(new_socket, tire_type, sizeof(tire_type) - 1, 0) < 0) {
                perror("Receive failed");
                close(new_socket);
                continue;
            }
            char jenis_ban[100];
            strncpy(jenis_ban, tire_type, 6);
            jenis_ban[4] = '\0';
            if (strcmp(jenis_ban, "Medi") == 0) {
                strcpy(jenis_ban, "Medium");
            }
            result = tire(jenis_ban);
            penulis("[Driver]", buffer, jenis_ban);

        } 
        
        else {
            result = "Wrongggg Command";
            penulis("[Driver]", buffer, "ERROR");
            
        }

        penulis("[Paddock]", buffer, result);

        if (send(new_socket, result, strlen(result), 0) < 0) {
            perror("Send failed");
        }

        close(new_socket);
    }

    return 0;
}
```
Dalam program paddock.c ini, program akan menerima permintaan dari klien driver.c melalui soket TCP/IP. Program ini dijalankan sebagai daemon di latar belakang. Pertama, program membuat sebuah soket server dan mengikatnya (bind) dengan alamat IP dan port yang telah ditentukan (dalam kasus ini, PORT 8080). Setelah itu, program mendengarkan (listen) koneksi masuk dari klien. Ketika ada koneksi yang masuk, program menerima (accept) koneksi tersebut dan menerima data yang dikirimkan oleh klien melalui soket. Data yang diterima adalah jenis perintah (seperti "Gap", "Fuel", "Tire", atau "Type") dan nilai terkait (jarak, sisa bahan bakar, sisa ban, atau jenis ban). Program akan memanggil fungsi yang sesuai dari file actions.c berdasarkan jenis perintah dan nilai yang diterima, misalnya fungsi gap untuk perintah "Gap" dengan nilai jarak.
## Soal no 4
Dikerjakan oleh **Veri Rahman (5027231088)**
