/* 
   Hardware : 
        - ESP32 DevKit V4 atau NodeMCU V3
        - Capacitive Soil Moisture Sensor V1.2
        - DHT22 Sensor suhu dan kelembaban udara
   Wiring :
        - Sensor kelembaban ke ESP32 : AUOT ke pin VP, GND ke pin GND, VCC ke pin 25
    Nilai Sensor berdasarkan pengetesan :
    - 720  : Sensor terendam dalam tanah penuh air dalam gelas   
    - 800  : Sensor terendam air di gelas tanpa media lain sampai batas garis sensor.
    - 1650 : Sensor terpasang di tanah kering di gelas
    - 2450 : Sensor kering tidak terpasang
*/

#include <painlessMesh.h> // Library untuk MESH Network
#include <DHT22.h> // Library untuk DHT22
#include <Arduino_JSON.h> // Library JSON untuk mengirim data

// Definisi Pin
#define PinSoil    36 // GPIO 36 (VP) untuk membaca sensor kelembaban tanah
#define PinPositif 25 // GPIO 25 yang terhubung dengan kabel positif sensor kelembaban
#define PinDHT     27 // GPIO 27 untuk membaca temperatur dan kelembaban udara

// Nilai untuk mapping
int NilaiSoilMoisture;
int KeadaanTanah; // Hasil mapping dari nilai pembacaan Sensor kelembaban tanah

double Baca; // Untuk millis

// Definisi untuk DHT 22
DHT22 dht22(PinDHT); 
float KelembabanUdara;
float SuhuUdara;

// Definisi Jaringan Mesh
#define   MESH_PREFIX     "Watering"
#define   MESH_PASSWORD   "0011EEFF"
#define   MESH_PORT       5555
painlessMesh  mesh;

Scheduler userScheduler; 

#define Node "A1"
String readings;

void KirimPesan();

Task taskSendMessage( TASK_SECOND * 30 , TASK_FOREVER, &KirimPesan ); // broadcast ke jaringan tiap 30 detik

void receivedCallback( uint32_t from, String &msg)
{
  Serial.printf("startHere: Terima dari %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double temp = myObject["lembab"];
  double hum = myObject["suhu"];
  double pres = myObject["tanah"];
  Serial.print("Node: ");
  Serial.println(node);
  Serial.print("Kelembaban Udara : ");
  Serial.print(KelembabanUdara);
  Serial.println(" C");
  Serial.print("Suhu Udara: ");
  Serial.print(SuhuUdara);
  Serial.println(" %");
  Serial.print("Kelembaban Tanah : ");
  Serial.print(KeadaanTanah);
}

void newConnectionCallback(uint32_t nodeId)
{
    Serial.printf("--> startHere: Koneksi Baru, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() 
{
  Serial.printf("Jaringan Berubah\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup()
{
  Serial.begin(115200); 
  Serial.println("Inisialisasi perangkat");
  Baca = millis();

  // Setting untuk Pin
  pinMode(PinSoil, INPUT);        // Assign Pin gpio sebagai Input untuk membaca sensor kelembaban tanah
  pinMode(PinPositif, OUTPUT);    // Assign PinPositif sebagai output untuk mematikan dan menghidupkan sensor kelembaban tanah
  pinMode(PinDHT, INPUT);        // Assign Pin sebagai input untuk membaca sensor suhu dan kelembaban udara
  digitalWrite(PinPositif, LOW);  // Matikan sensor kelembaban
  
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

}

void checkDHT22()
{
  if (dht22.getLastError() != dht22.OK) 
  {
    Serial.print("dht last error :");
    Serial.println(dht22.getLastError());
  }
  KelembabanUdara = dht22.getHumidity();
  SuhuUdara = dht22.getTemperature();

}

void BacaSensor()
{
  NilaiSoilMoisture = 0;           // Inisialisasi Nilai Sensor Kelembaban 
  KeadaanTanah = 0;                // Inisialisasi Persentasi Nilai Sensor Kelembaban 
  digitalWrite(PinPositif, HIGH);  // Hidupkan sensor
  NilaiSoilMoisture = analogRead(PinSoil);  // Baca nilai dari sensor
  digitalWrite(PinPositif, LOW);  // Matikan sensor
  KeadaanTanah = map(NilaiSoilMoisture, 800, 1800, 100, 1);  // mapping nilai sensor dengan kondisi sensor terpasang di tanah
  Serial.printf("Nilai Sensor : %d\n", NilaiSoilMoisture);
  Serial.printf("Hasil mapping : %d\n", KeadaanTanah);
}

String BacaHasil() 
{
  JSONVar jsonReadings;
  jsonReadings["node"] = Node;
  jsonReadings["lembab"] = KelembabanUdara;
  jsonReadings["suhu"] = SuhuUdara;
  jsonReadings["tanah"] = KeadaanTanah;
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void KirimPesan() 
{
  String Pesan = BacaHasil();
  mesh.sendBroadcast(Pesan);
  Serial.println("Broadcast ke jaringan");
}

void loop() 
{
  
  mesh.update();

  if(millis() - Baca >= 30000)  // jika timing terbaru - waktu awal >= 30 detik
  {
    BacaSensor(); // Baca data dari Capacitive Soil Moisture Sensor
    Baca = millis();
  }
  
}
