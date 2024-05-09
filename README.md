# Soal-Shift-Sisop-Modul-3-IT15-2024

Anggota:

1. Michael Kenneth Salim (5027231008)
2. Nicholas Emanuel Fade (5027231070)
3. Veri Rahman (5027231088)

## Soal no 1
Dikerjakan oleh **Nicholas Emanuel Fade (5027231070)**

## Soal no 2
Dikerjakan oleh **Michael Kenneth Salim (5027231008)**

Pada nomer 2, ini kita pada dasarnya diminta untuk membuat sebuah kalkulator dengan menggunakan sistem fork dan pipe. Program ini memungkinkan kita untuk melakukan operasi aritmatika pada dua bilangan yang direpresentasikan dalam bentuk kata-kata, dan menyimpan hasil operasi beserta informasi waktu ke dalam file histori.log. Program menggunakan konsep pipes dan fork untuk komunikasi antara parent process dan child process.

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

## Soal no 4
Dikerjakan oleh **Veri Rahman (5027231088)**
