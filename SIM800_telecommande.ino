//http://a110a.free.fr/SPIP172/IMG/txt/SIM800_telecommande_de__Led_par_SMS_070717.txt
//SIM800_telecommande_de_ Led_par_SMS 070717
//D�riv� du programme de Jacques Guiblais
//Allumer et eteindre une LED par SMS.
//Ce programme qui permet a� un Arduino de recevoir un ordre par SMS : "led on" ou "led off".
//Il analyse le message recu , recupere le numero du telephone a� l'origine du SMS
//et renvoie un SMS de confirmation de l'ordre
//Plus de d�tails en fin de listing

//La fonction boolean ecouteGSM(String strAttendue, unsigned int timeout, boolean affiche)
//est tr�s interessante: elle "renifle" et assemble les caract�res �mis par le module
//On lui fournit la chaine  strAttendue : si elle est trouv�e elle renvoie True, sinon False.
//En outre elle se limite � timeout ms d'attente et peut afficher la position de la chaine trouv�e

#include <SoftwareSerial.h>
SoftwareSerial gsm(8, 7); //  il faut relier D10 de l'Arduino au Tx du SIM800
//et D11 de l'Arduino au Rx du SIM800
//*********************Les variables**********************
int led = 13;//Led de l'Arduino
int posOK = 0; //Position de OK dans strCumul
String strCumul, strAttendue, numeroSMS, textSMS;
//strCumul accumule les caract�res re�us du SIM800
unsigned long t0;//Temps d'attente maxi en ms pour une r�pose du SIM800

//********************Les fonctions***************************

boolean ecouteGSM(String strAttendue, unsigned int timeout, boolean affiche) {
  //R�cup�re dans "strCumul" tous les caract emis par le SIM800
  //Si "strAttendue" s'y trouve, renvoie true et sa position, sinon false
  //"timeout": Dure maxi en  ms.  "affiche" ou non le resultat

  t0 = millis();//Demarre le chrono pour attente maxi
  strCumul = ""; //String vierge, elle va recevoir les caract �mis par le S800
  while ((millis() - t0) < timeout) {      //Tant que timeout non atteint
    while (gsm.available()) strCumul.concat(char(gsm.read())); //Lire et cumuler les caract
    //Il n'arrive plus de caract, On recherche strAttendue dans  strCumul
    if (strCumul.indexOf(strAttendue) > 0) {     //  Oui elle est trouv�e,  lire encore 100 ms le port serie
      delay (100);
      while (gsm.available()) strCumul.concat(char(gsm.read())); // pour etre sur de ne rien louper...
      break;        //Sortir, strCumul est prete
    }
  }
  if (affiche || strCumul.indexOf(strAttendue) == -1 ) {  //Si affichage demand� ou strAttendue non trouv�e
    if (strCumul.indexOf(strAttendue) == -1 )Serial.print("En attente de  " + strAttendue + " "  + " duree ");
    else Serial.print("Recu   " + strAttendue + " en position " + strCumul.indexOf(strAttendue) + " duree ");
    Serial.print(millis() - t0); Serial.println(" ms");
    Serial.println(strCumul);
  }

  if (strCumul.indexOf(strAttendue) > 0) return true;
  else return false;
}

//*************************************
void LireSMS() {
  gsm.println("AT+CMGR=1");  // Lit le premier message disponible sur la carte SIM
  ecouteGSM("OK", 2000, 1);

  // Recupere le N�  de telephone emetteur pour lui repondre

  int test = strCumul.indexOf("+33");// Recherche du code du pays
  numeroSMS = strCumul.substring(test, test + 12);
  Serial.println("SMS recu depuis : " + numeroSMS);
  if (strCumul.indexOf("led on") > 0) {
    textSMS = "Ordre recu : Allumer LED !";
    digitalWrite(led, 1);
  }
  else {
    if (strCumul.indexOf("led off") > 0) {
      textSMS = "Ordre recu : Eteindre LED !";
      digitalWrite(led, 0);
    }
    else textSMS = "Ordre non compris !";
  }
  gsm.println("AT+CMGD=1,4"); // effacer le SMS de la Carte SIM
  ecouteGSM("OK", 1000, 0);
  Serial.println(textSMS);
  Serial.println("");
  // Envoyer la confirmation de l'ordre par SMS
  Serial.println("Envoi message confirmation");
  gsm.println("AT+CMGS=\"" + numeroSMS + "\"");
  ecouteGSM(">", 1000, 0);
  gsm.println(textSMS);
  gsm.write(26); // Caractere de fin 26 <Ctrl-Z>
  gsm.println("");
  ecouteGSM("+CMGS:", 10000, 1);
}
//*************************************************************************
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);//Vers l'ecran de l'IDE
  gsm.begin(9600);//Vers le module SIM800
  delay(2000);

  Serial.println("Initialisation...");
  gsm.println("AT");

  while (!ecouteGSM("OK", 1000, 0)) gsm.println("AT"); //Tant que "OK" non re�u, envoyer l'ordre AT

  gsm.println("AT+CNUM");            // Affiche N� de la carte SIM utilisee
  ecouteGSM("OK", 20000, 1); //Attendre OK, au plus 2000ms, et imprimer le resultat

  Serial.println ("Qualite reseau . Probleme si le premier chiffre est 0...!");
  gsm.println("AT+CSQ");            // Qualité du réseau, pb si CSQ = 0
  ecouteGSM("OK", 10000, 1);  //Attendre OK, au plus 10 s, et imprimer le resultat

  gsm.println("AT+CMGF=1");         // Mode Texte pour les SMS
  ecouteGSM("OK", 1000, 0);     //Attendre OK, au plus 1 s, ne pas imprimer le resultat
    //On efface pas le SMS au cas o� il serait arriv� avant le demarrage de ce programme
//  gsm.println("AT+CMGD = 1,4");       // effacer les SMS en mémoire dans la carte SIM
//  ecouteGSM("OK", 2000, 0);           // car on lit toujours le message N�1 de la carte SIM...
  //Attendre OK, au plus 2 s, ne pas imprimer le resultat
}
//****************************************************
void loop() {
  gsm.println("AT+CMGR=1");  // Lit le premier message disponible sur la carte SIM
  if ( ecouteGSM("REC", 20000, 1)) LireSMS(); // Si REC est trouv�, un SMS est arriv�

}
//********************************************************

/*
  Les commandes AT

  Pour programmer ce module et envoyer des SMS utiliser les commandes AT suivantes :

  AT+CMGF=1 <CR>  : permet d'activer le mode texte (ASCII) pour l'envoi du SMS.
  Cette commande renvoie le code OK en cas de réussite.
  Le temps d'attente maximal est de 1 secondes.

  AT+CMGS=\"+336xxxxxxxx\"<CR>TEXTE du SMS<CTRL-Z>

  <CR> = code ASCII 13 contenu dans println
  <CTRL-Z> = code ASCII 26, signale la fin du message.


  La première étape consiste en l'envoi du numéro de téléphone du destinataire suivi d'un retour charriot <CR>.
  La variable numéro représente le numéro de téléphone pris avec l'indicatif téléphonique (+33 pour un numéro de téléphone français).
  Après quelques ms, le SIM800 répond par une invite > à poursuivre par le contenu du message.
  Envoi ensuite du message au module SIM800 (attendre au moins 200 ms avant d'envoyer le corps du message).
  La saisie du texte se termine par un CTR+Z. Pour ce faire le programme envoie simplement la valeur 0x1A (code ASCII de CTR+Z) au module GSM.

  Le module SIM800L  renvoi un code d'erreur à l'utilisateur si le module GSM ne donne pas de réponse pendant plus de 20 secondes après l'envoi du message.
*/


