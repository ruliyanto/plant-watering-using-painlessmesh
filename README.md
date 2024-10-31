Program ini sebagai contoh untuk mengairi tanaman menggunakan jaringan painlessmesh, perangkat terdiri dari :
- 1 buah esp32 devkitc v4 atau nodemcu v3 yang terhubung ke sensor
- DHT22 sensor
- Capacitive soil moisture sensor
- 1 buah esp32 devkitc v4 atau nodemcu v3 yang terhubung dengan pompa
- Mini submersible water pump
- Diode
- Resistor
- BC547
- Arduino IDE on Ubuntu

Flow program
Esp32 yang terhubung dengan sensor akan melakukan pembacaan menggunakan capacitive soil moisture sensor dan menyebar (broadcast) hasil pembacaan melalui jaringan painlessmesh yang akan dibaca oleh seluruh node yang terhubung ke dalam jaringan baik node yang terhubung ke sensor maupun node yang terhubung dengan pompa, jika hasil pembacaan menunjukkan bahwa kelembaban air kurang maka node yang terhubung ke pompa akan menyalakan pompa untuk mulai mengairi tanaman selama beberapa detik supaya tidak banjir lalu pompa akan berhenti dan menunggu pembacaan sensor berikutnya yang akan dikirim melalui jaringan. Jika pada pembacaan berikutnya ternyata kelembaban masih kurang maka pompa akan menyala kembali.

Painlessmesh digunakan untuk memudahkan penambahan node baik untuk pembacaan sensor maupun untuk penambahan pompa pada lokasi yang tidak memiliki jaringan WiFi.
Millis() digunakan untuk menggantikan delay() yang sebaiknya tidak digunakan pada painlessmesh.

Untuk referensi contoh skema dan program bisa dilihat di :
https://ruliyanto.wordpress.com/2024/10/13/membuat-pengairan-tanaman-otomatis-menggunakan-microcontroller/
