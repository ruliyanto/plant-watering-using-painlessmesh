Proyek ini untuk mengairi tanaman menggunakan jaringan mesh
Terdiri dari :
- 1 buah esp32 yang terhubung ke sensor dht22 dan capacitive soil moisture sensor
- 1 buah esp32 yang terhubung dengan pompa celup mini
- Arduino IDE
- Diode
- Resistor
- BC547

Jaringan Mesh digunakan untuk memudahkan penambahan board

Flow program
Esp32 yang terhubung dengan sensor akan melakukan pembacaan
jika capacitive soil moisture sensor menunjukkan bahwa kelembaban air kurang
maka pompa akan menyala untuk mengairi tanaman selama beberapa detik lalu pompa akan berhenti
jika pada pembacaan berikutnya ternyata masih kurang lembab maka pompa akan menyala kembali.

Untuk skema dan diagram bisa dilihat di :
https://ruliyanto.wordpress.com/2024/10/13/membuat-pengairan-tanaman-otomatis-menggunakan-microcontroller/
