#include "DHT.h"
#include <EEPROM.h>
DHT dht(2, DHT11);

#define RELAIS 7
const int TAILLE_NB_VAL_MOYENNE_HUMIDITE = 60;
const int SEUIL_ACTIVATION_VENTIL = 75;

float tableauHistoMoyenne[TAILLE_NB_VAL_MOYENNE_HUMIDITE];

long heureDerniereSave = 0;

void setup() {
  dht.begin();
  Serial.begin(9600);

  Serial.println("===================================================================");
  Serial.println("\tLancement du programme \"VMC V1.0.0\" By Harold CONAN");
  Serial.println("===================================================================");
  Serial.println("\tAttendre " + String(TAILLE_NB_VAL_MOYENNE_HUMIDITE) + " secondes avant le lancement réel");
  Serial.println("\tLe seuil d'activation de la ventilation est fixé à " + String(SEUIL_ACTIVATION_VENTIL) + "% d'humidité");
  Serial.println("===================================================================");

  //Initialisation du tableau avec une valeur moyenne artificielle
  for(int index = 0; index < TAILLE_NB_VAL_MOYENNE_HUMIDITE ; index++)
  {
    tableauHistoMoyenne[index] = 50.0;
  }

  // partie interupteur
  pinMode(RELAIS,OUTPUT);
}

void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // on commence la boucle au debut +1 pour laisser la place à la nouvelle valeur
  // On décale toutes les valeurs de 1
  double valtotale = 0;
  for(int index = (TAILLE_NB_VAL_MOYENNE_HUMIDITE-1); index > 0 ; index--)
  {
    tableauHistoMoyenne[index] = tableauHistoMoyenne[index-1];
    valtotale += tableauHistoMoyenne[index];
  }

  //Stat courrante
  String stat = "Humidité actuelle : " + padLeft(String(h,0),3) + "% pour une température de " + padLeft(String(t,1),4) + "°C";

  //Stat historique
  // je redefini la première valeur
  tableauHistoMoyenne[0] = h;
  valtotale += h;
  stat += " - valeur moyenne : " + padLeft(String(valtotale/TAILLE_NB_VAL_MOYENNE_HUMIDITE,0),3);

  if( valtotale/TAILLE_NB_VAL_MOYENNE_HUMIDITE > SEUIL_ACTIVATION_VENTIL)
  {
    digitalWrite(RELAIS, HIGH);
    stat += " - Ventilation ON";
  }
  else
  {
    digitalWrite(RELAIS, LOW);
    stat += " - Ventilation OFF";
  }
  
  Serial.println("");
  Serial.println(stat);
    
  delay(1000);
}

String padLeft(String str, int length) {
  // Ajouter des espaces à gauche pour compléter la longueur
  while (str.length() < length) {
    str = "0" + str; // Ajouter un espace à gauche
  }
  return str;
}
