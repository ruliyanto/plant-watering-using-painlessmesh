/* 
   Hardware : 
        - ESP32 DevKit V4 atau NodeMCU V3
        - Pompa celup mini
        - Transistor BC547
        - Resistor 
        - Dioda 
   Wiring :
        - Transistor BC547 : Collector ke 5V ESP32 melalui diode, Base ke Resistor lalu diode lalu ke Pin26,  Emitter ke kabel Positif Pompa
        - Pompa celup ke ESP32 : GND ke GND
*/

// Library untuk Jaringan
#include <painlessMesh.h>
#include <Arduino_JSON.h>

// Definisi Pin
#define PinPompa   26 // GPIO 26 yang terhubung dengan kabel positif pompa celup

// Definisi Jaringan Mesh
#define   MESH_PREFIX     "Watering"
#define   MESH_PASSWORD   "0011EEFF"
#define   MESH_PORT       5555

painlessMesh  mesh;
Scheduler userScheduler; 

int Penghitung = 0; // Untuk menghitung berapa kali proses telah berjalan

#define Node "P1";
String NodeSensor;
double Tanah;

// Untuk menggantikan delay()
unsigned long StartMillis;
bool StatusPompa = false;

void KirimPesan(); // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &KirimPesan);

void KirimPesan()
{
  String msg = "Pompa1"; // ini diganti dengan data sensor
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg) // Terima update dari jaringan
{
  Serial.printf("startHere: Terima dari %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  String node = myObject["node"];
  double lembab = myObject["lembab"];
  double suhu = myObject["suhu"];
  double tanah = myObject["tanah"];
  NodeSensor = node;
  Tanah = tanah;
  Serial.print("Node: ");
  Serial.println(node);
  Serial.print("Suhu : ");
  Serial.print(suhu);
  Serial.println(" C");
  Serial.print("Kelembaban Udara : ");
  Serial.print(lembab);
  Serial.println(" %");
  Serial.print("Kelembaban Tanah : ");
  Serial.print(tanah);
  Serial.println(" hpa");
}

void newConnectionCallback(uint32_t nodeId)
{
    Serial.printf("--> Koneksi Baru, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() 
{
  Serial.printf("Jaringan berubah\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Inisialisasi perangkat");
  
  // Setting untuk Pin
  pinMode(PinPompa, OUTPUT); 
  digitalWrite(PinPompa, LOW); 
  
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

}

void loop() 
{
  mesh.update();
/*
  if(NodeSensor == "A1" && Tanah >= 50)
  {
    delay(1000);
    Serial.println("Mulai Menyiram");
    digitalWrite(PinPompa, HIGH);
    delay(15000);
    digitalWrite(PinPompa, LOW);
    Serial.println("Penyiraman selesai");
    Penghitung++;
    Serial.printf("Penyiraman ke %d\n", Penghitung);
  }
*/
  if(NodeSensor == "A1" && Tanah <= 50 && StatusPompa == false)
  {
    Serial.printf("Menghidupkan Pompa, Status Pompa : %d\n", StatusPompa);
    digitalWrite(PinPompa, HIGH);
    StatusPompa = true;
    StartMillis = millis();
    Serial.printf("Start : %ld, StatusPompa : %d\n", StartMillis, StatusPompa);
  }

  if(millis() - StartMillis >= 15000)  // jika timing terbaru - waktu awal >= 15 detik
  {
    Serial.printf("Milis : %ld\n", millis());
    digitalWrite(PinPompa, LOW);   // Matikan Pompa
    StatusPompa = false;
    Serial.println("Penyiraman selesai");
    Penghitung++;
    Serial.printf("Penyiraman ke %d\n", Penghitung);
  }
  
}

