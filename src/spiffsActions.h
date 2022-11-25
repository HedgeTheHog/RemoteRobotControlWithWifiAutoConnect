#include "SPIFFS.h"

/* const char *ssidPath = "/ssid.txt";
const char *passPath = "/pass.txt";
const char *ipPath = "/ip.txt";
const char *gatewayPath = "/gateway.txt"; */
const char* const  ssidPath ="/ssid.txt";
const char* const  passPath ="/pass.txt";
const char* const  ipPath  = "/ip.txt" ;
const char* const  gatewayPath = "/gateway.txt" ;

const char* const hostname = "{{ cookiecutter.device_hostname }}";


void initSPIFFS();

String readFile(fs::FS &fs, const char *path);

void writeFile(fs::FS &fs, const char *path, const char *message);