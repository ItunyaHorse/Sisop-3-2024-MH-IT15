# Soal-Shift-Sisop-Modul-3-IT15-2024

Anggota:

1. Michael Kenneth Salim (5027231008)
2. Nicholas Emanuel Fade (5027231070)
3. Veri Rahman (5027231088)

## Soal no 1
Dikerjakan oleh **Nicholas Emanuel Fade (5027231070)**

Pada nomor 1 ini, kita pada dasarnya diminta untuk membuat 3 program, auth.c itu untuk mengecheck bawha csv file di direktori newdata memiliki keyword trashcan atau parkinglot dan menaruhnya di shared memory, db.c itu untuk mengambil file csv dari shared memory ke direktori microservices/database dan membuat file db.log di direktori yang sama, rate.c itu untuk mengambil csv file di shared memory dan mengoutput nama dan rating paling tinggi di filenya.

Pertama kita liat dulu auth.c

Bagian ini untuk menauthenticate file csvnya:

```bash
void authenticate_files() {
    DIR *dir;
    struct dirent *entry;
    dir = opendir("new-data");
    if (!dir) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *filename = entry->d_name;
            if (strstr(filename, "trashcan") == NULL && strstr(filename, "parkinglot") == NULL) {
                // File name doesn't contain "trashcan" or "parkinglot", delete the file
                char filepath[MAX_FILENAME_LENGTH];
                snprintf(filepath, MAX_FILENAME_LENGTH, "new-data/%s", filename);
                remove(filepath);
                printf("Deleted file: %s\n", filename);
            }
        }
    }
    closedir(dir);
}
```

Bagian ini untuk mengambil file ke shared memory

```bash
void authenticate_and_move_to_shared_memory() {
    // Create or attach to shared memory segment
    int shmid = shmget(SHM_KEY, MAX_FILENAME_LENGTH * sizeof(char), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    DIR *dir;
    struct dirent *entry;
    dir = opendir("new-data");
    if (!dir) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }


    char *current_position = shared_memory;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *filename = entry->d_name;
            if (strstr(filename, ".csv") != NULL) { 
                char filepath[MAX_FILENAME_LENGTH];
                snprintf(filepath, MAX_FILENAME_LENGTH, "new-data/%s", filename);

                FILE *file = fopen(filepath, "r");
                if (file == NULL) {
                    perror("Failed to open file");
                    continue; // Skip to the next file
                }

                strncpy(current_position, filename, MAX_FILENAME_LENGTH - (current_position - shared_memory));
                current_position += strlen(filename);
                *current_position++ = '\n'; // Add newline after filename

                char line[MAX_FILENAME_LENGTH];
                while (fgets(line, MAX_FILENAME_LENGTH, file) != NULL) {
                    strncpy(current_position, line, MAX_FILENAME_LENGTH - (current_position - shared_memory));
                    current_position += strlen(line);
                }

                *current_position++ = '\n';

                fclose(file);

                printf("Moved file content to shared memory: %s\n", filename);
            }
        }
    }
    closedir(dir);
```

output:

![image](https://drive.google.com/uc?export=view&id=1q_PDNcTiqW_mhgwWbadcM0slsp6zUa5-)

Kedua kita liat db.c:

Bagian ini untuk mengambil file dari shared memory ke direktori database dan mengambil timestamp untuk file db.log yang akan dibuat juga

```bash
void move_files_from_shared_memory() {
    int shmid = shmget(SHM_KEY, MAX_FILENAME_LENGTH * sizeof(char), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
  
    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    FILE *log_file = fopen("microservices/database/db.log", "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    char timestamp[20];
    struct tm *tm_info;
    tm_info = localtime(&tv.tv_sec);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_info);

    char *current_position = shared_memory;
    while (*current_position != '\0') {
        // Find the end of the filename
        char *end_of_filename = strchr(current_position, '\n');
        if (end_of_filename == NULL) {
            fprintf(stderr, "Invalid format in shared memory: missing newline character\n");
            break;
        }

        *end_of_filename = '\0'; // Null-terminate the filename
        char *filename = current_position;

        // Move to the next filename (if any)
        current_position = end_of_filename + 1;

        char source_path[MAX_FILENAME_LENGTH];
        snprintf(source_path, sizeof(source_path), "new-data/%s", filename);
        char dest_path[MAX_FILENAME_LENGTH];
        snprintf(dest_path, sizeof(dest_path), "microservices/database/%s", filename);
      
        if (access(source_path, F_OK) != 0) {
            fprintf(stderr, "Source file '%s' does not exist\n", source_path);
            continue;
        }

        char* dest_directory = "microservices/database";
        if (access(dest_directory, F_OK) != 0) {
            fprintf(stderr, "Destination directory '%s' does not exist\n", dest_directory);
            continue;
        }

        if (rename(source_path, dest_path) == -1) {
            perror("Failed to move file");
            continue;
        }
        printf("Moved file from shared memory: %s\n", filename);

        char file_type[20];
        if (strstr(filename, "trashcan") != NULL) {
            strcpy(file_type, "[Trash Can]");
        } else if (strstr(filename, "parkinglot") != NULL) {
            strcpy(file_type, "[Parking Lot]");
        } else {
            strcpy(file_type, "[Unknown]");
        }

        char log_entry[MAX_LOG_LENGTH];
        snprintf(log_entry, sizeof(log_entry), "%s %s %s\n", timestamp, file_type, filename);
        fprintf(log_file, "%s", log_entry);
    }

    fclose(log_file);

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}
```

output di terminal:

![image](https://drive.google.com/uc?export=view&id=19Xo9Fshg3NR1DPjBx2IrmMlqSc8REdSo)

output ls di direktori database:

![image](https://drive.google.com/uc?export=view&id=1tC4mEqpLFAnpMFo84ZVw-s3ig-YPpJj-)

dalam file db.log:

![image](https://drive.google.com/uc?export=view&id=143VTrQnsT_vMClgtRotL-8eysByxj7sO)

Terakhir kita liat rate.c:

Bagian ini untuk mengambil file dari shared memory dan mengoutput rating tertinggi dari kedua file

```bash
typedef struct {
    char name[MAX_FILENAME_LENGTH];
    float rating;
} Place;

void get_highest_rating(const char *type) {
    // Variables to store the highest rated place for each type
    float highest_rating = 0;
    char highest_name[MAX_FILENAME_LENGTH];
    char highest_filename[MAX_FILENAME_LENGTH];
  
    int shmid = shmget(SHM_KEY, MAX_FILENAME_LENGTH * sizeof(char), 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    char *current_position = shared_memory;
    while (*current_position != '\0') {
        char filename[MAX_FILENAME_LENGTH];
        strcpy(filename, current_position);
        current_position += strlen(filename) + 1; // Move to the next filename

        FILE *file = fopen(filename, "r");
        if (file == NULL) {
            perror("Failed to open file");
            continue; // Skip to the next file
        }

        Place place;
        while (fscanf(file, "%[^,], %f\n", place.name, &place.rating) != EOF) {
            if (place.rating > highest_rating) {
                highest_rating = place.rating;
                strcpy(highest_name, place.name);
                strcpy(highest_filename, filename);
            }
        }
        fclose(file);
    }

    printf("Type: %s\n", type);
    printf("Filename: %s\n", highest_filename);
    printf("--------------------\n");
    printf("Name: %s\n", highest_name);
    printf("Rating: %.1f\n", highest_rating);

    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed");
        exit(EXIT_FAILURE);
    }
}
```

output:

![image](https://drive.google.com/uc?export=view&id=1Mk_jmbe4rxOKdENk-cxy7ou5mHHswKG2)

### Revisi soal no 1

rate.c tidak bsia membuka file csv yang berada di shared memory dan ada kesalahan kecil di file db.lognya

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

![image](https://drive.google.com/uc?export=view&id=1Wlb4uPGb_KC3LTApTkULMthiYfb8WXHr)

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

Pada soal no 4 ini, Kita diminta untuk membuat program yang menampilkan list anime dari file yang disediakan. Program ini menggunakan socket dimana client dan server terhubung, Client berfungsi sebagai pengirim pesan dan dapat menerima pesan dari server. Sedangkan, server berfungsi sebagai penerima pesan dari client dan hanya menampilkan pesan perintah client saja. Server juga digunakan untuk membaca myanimelist.csv dan mencatat perubahan seperti add, edit, dan delete pada sebuah log.

```bash
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char command[2048] = {0};
    char response[4096] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection gagal \n");
        return -1;
    }

    while (1) {
        printf("You: ");
        fgets(command, 2048, stdin);
        command[strcspn(command, "\n")] = 0;

        send(sock, command, strlen(command), 0);

        read(sock, response, 4096);
        printf("Server: \n%s\n", response);

        if (strcmp(command, "exit") == 0) {
            break;
        }
    }

    return 0;
}
```
Kode diatas merupakan Client yang berfungsi sebagai pengirim pesan dan dapat menerima pesan dari server. **stdio.h, stdlib.h, string.h, sys/socket.h, netinet/in.h, unistd.h, dan arpa/inet.h** pustaka-pustaka ini memberikan fungsi-fungsi yang digunakan dalam program untuk melakukan operasi input-output, alokasi memori, manipulasi string, dan interaksi dengan jaringan. Konstanta **'PORT'** didefinisikan sebagai nomor port yang akan digunakan untuk koneksi ke server. Program membuat socket dengan memanggil fungsi **'socket()'** dengan parameter AF_INET yang menandakan penggunaan protokol IPv4, SOCK_STREAM yang menandakan jenis socket TCP, dan 0 yang menandakan bahwa protokol yang digunakan akan dipilih secara otomatis oleh sistem. Program mengatur struktur **'serv_addr'** yang berisi informasi tentang alamat server, yaitu alamat IP dan nomor port. Program mencoba membuat koneksi ke server dengan memanggil fungsi **'connect()'** yang mengambil socket, alamat server, dan ukuran alamat sebagai argumen. Jika koneksi berhasil, program akan lanjut ke langkah selanjutnya. Kemudian program memasuki loop tak terbatas di mana client dapat mengirimkan perintah ke server dan menerima respons dari server. Di dalam loop, client menampilkan prompt "You: ", kemudian membaca input dari pengguna menggunakan **fgets()** dan mengirimkan perintah tersebut ke server menggunakan fungsi **send()**. Setelah itu, client membaca respons dari server menggunakan **read()** dan menampilkannya kepada pengguna dengan prompt "Server: ". Jika pengguna memasukkan perintah **"exit"**, program keluar dari loop dan menutup koneksi dengan server sebelum mengakhiri eksekusi.

```bash
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080

void log_change(const char *type, const char *message) {
    FILE *fp = fopen("/home/rhmnn/sisop/prak3/soal_4/change.log", "a");
    if (fp != NULL) {
        time_t now;
        time(&now);
        struct tm *timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%d/%m/%y %T", timeinfo);
        fprintf(fp, "[%s] [%s] %s\n", timestamp, type, message);
        fclose(fp);
    }
}

void process_command(int client_socket, const char *command) {
    FILE *anime_file = fopen("/home/rhmnn/sisop/prak3/soal_4/myanimelist.csv", "r+");
    if (anime_file == NULL) {
        perror("Gagal membuka file myanimelist.csv");
        return;
    }

    char response[4096] = {0};
    char line[1024];
    char *token;

    if (strncmp(command, "tampilkan", 9) == 0) {
        while (fgets(line, 1024, anime_file) != NULL) {
            strcat(response, line);
            continue;
        }
    } else if (strncmp(command, "genre", 5) == 0 || strncmp(command, "hari", 4) == 0 || strncmp(command, "status", 6) == 0) {
        char keyword[512];
        sscanf(command, "%*s %[^\n]", keyword);
        while (fgets(line, 1024, anime_file) != NULL) {
            if (strstr(line, keyword) != NULL) {
                strcat(response, line);
                continue;
            }
        }
    } else if (strncmp(command, "add", 3) == 0) {
        char new_anime[1024];
        sscanf(command, "%*s %[^\n]", new_anime);
        fprintf(anime_file, "%s\n", new_anime);
        strcpy(response, "Anime berhasil ditambahkan.");
        log_change("ADD", new_anime);
    } else if (strncmp(command, "edit", 4) == 0) {
        char old_title[100];
        sscanf(command, "%*s %[^\n]", old_title);
        long int pos = ftell(anime_file);
        int found = 0;
        while (fgets(line, 1024, anime_file) != NULL) {
            if (strstr(line, old_title) != NULL) {
                found = 1;
                fseek(anime_file, pos, SEEK_SET);
                fprintf(anime_file, "%s\n", command + 5); // Skip "edit " part
                strcpy(response, "Anime berhasil diedit.");
                log_change("EDIT", command + 5); // Skip "edit " part
                break;
            }
            pos = ftell(anime_file);
        }
        if (!found) {
            strcpy(response, "Anime tidak ditemukan.");
        }
    } else if (strncmp(command, "delete", 6) == 0) {
        char delete_title[100];
        sscanf(command, "%*s %[^\n]", delete_title);
        long int pos = ftell(anime_file);
        int found = 0;
        while (fgets(line, 1024, anime_file) != NULL) {
            if (strstr(line, delete_title) != NULL) {
                found = 1;
                fseek(anime_file, pos, SEEK_SET);
                fprintf(anime_file, " ");
                strcpy(response, "Anime berhasil dihapus.");
                log_change("DEL", delete_title);
                break;
            }
            pos = ftell(anime_file);
        }
        if (!found) {
            strcpy(response, "Anime tidak ditemukan.");
        }
    } else if (strncmp(command, "exit", 4) == 0) {
        strcpy(response, "Exiting the client");
    } else {
        strcpy(response, "Invalid Command");
    }

    send(client_socket, response, strlen(response), 0);
    fclose(anime_file);
}

int main(int argc, char const *argv[]) {
    int server_socket, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char command[2048] = {0};

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
         perror("accept");
         exit(EXIT_FAILURE);
    }
    while(1) {
        recv(client_socket, command, 2048, 0);

        printf("Received: %s\n", command);

        process_command(client_socket, command);

    }

    close(server_socket);
    close(client_socket);
    return 0;
}
```
Kode diatas merupakan Server berfungsi sebagai penerima pesan dari client dan hanya menampilkan pesan perintah client saja. Pustaka **stdio.h, stdlib.h, string.h, sys/socket.h, netinet/in.h, unistd.h, dan time.h** ini menyediakan fungsi-fungsi yang digunakan dalam program untuk melakukan operasi input-output, alokasi memori, manipulasi string, operasi socket, dan manipulasi waktu. Lalu konstanta **PORT** didefinisikan sebagai nomor port yang akan digunakan untuk koneksi ke server. Fungsi **log_change** digunakan untuk mencatat perubahan pada file myanimelist.csv ke dalam file change.log. Fungsi ini menerima dua parameter, yaitu tipe perubahan (type) dan pesan perubahan (message). Fungsi ini membuka file change.log, menuliskan timestamp, tipe perubahan, dan pesan perubahan, kemudian menutup file. Fungsi **process_command** digunakan untuk memproses perintah yang diterima dari client. Fungsi ini menerima dua parameter, yaitu socket client dan perintah yang diterima (command). Fungsi ini membuka file myanimelist.csv untuk menampilkan seluruh judul, menampilkan judul berdasarkan genre, menampilkan judul berdasarkan hari, menampilkan status berdasarkan berdasarkan judul, menambahkan anime ke dalam file myanimelist.csv, melakukan edit anime berdasarkan judul, melakukan delete berdasarkan judul, selain command yang diberikan akan menampilkan tulisan “Invalid Command”, lalu memproses perintah yang diterima, menulis respons, dan mengirimkan respons tersebut kembali ke client. Respons yang dikirimkan berisi hasil dari pemrosesan perintah. Fungsi **main** melakukan inisialisasi socket, membuat koneksi, menerima perintah dari client, dan memanggil fungsi process_command untuk memproses perintah tersebut. Program akan terus berjalan dalam loop yang tak terbatas hingga menerima perintah "exit" dari client. Program membuat socket dengan memanggil fungsi **socket()** dan mengatur opsi socket dengan **setsockopt()**. Kemudian, program mengatur alamat server menggunakan **bind()**. Program ini juga menggunakan **listen()** untuk mendengarkan koneksi dari client, dan **accept()** untuk menerima koneksi. Setelah koneksi diterima, program akan masuk ke dalam loop dengan menggunakan **recv()** untuk menerima perintah dari client, kemudian memanggil fungsi **process_command** untuk memproses perintah tersebut. Setelah loop selesai, program akan menutup socket dan koneksi dengan client.

## REVISI SOAL 4
1. Fungsi edit yang tidak dapat menemukan anime pada file myanimelist.csv sehingga tidak bisa menampilkan outputnya.
2. Fungsi delete yang tidak dapat menghapus anime yang baru ditambahkan.
3. Anime yang baru ditambahkan ada di file myanimelist.csv, tetapi tidak membuat line sendiri melainkan mengambil line anime yang sudah ada.
