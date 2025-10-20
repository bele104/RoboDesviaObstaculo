#include <AFMotor.h>

// --- Motores ---
// Direita -> M2 (frente) e M1 (trás)
AF_DCMotor motorDirFrente(2);
AF_DCMotor motorDirTraz(1);

// Esquerda -> M3 (frente) e M4 (trás)
AF_DCMotor motorEsqFrente(3);
AF_DCMotor motorEsqTraz(4);

// --- Sensores ultrassônicos ---
#define TRIG_DIR A0
#define ECHO_DIR A1

#define TRIG_ESQ A2
#define ECHO_ESQ A3

#define TRIG_FRT A4
#define ECHO_FRT A5

// --- Configurações ---
#define DISTANCIA       20   // distância mínima frontal
#define LIMITE          5    // distância crítica
#define INTERVALO_DELAY 20   // tempo base para sincronização
#define VEL_ESQ         200
#define VEL_DIR         200

// ----------------------
// Setup
// ----------------------
void setup() {
  Serial.begin(9600);

  motorEsqFrente.setSpeed(VEL_ESQ);  
  motorEsqTraz.setSpeed(VEL_ESQ); 
  motorDirFrente.setSpeed(VEL_DIR);  
  motorDirTraz.setSpeed(VEL_DIR); 

  pinMode(TRIG_ESQ, OUTPUT);
  pinMode(ECHO_ESQ, INPUT);
  pinMode(TRIG_DIR, OUTPUT);
  pinMode(ECHO_DIR, INPUT);
  pinMode(TRIG_FRT, OUTPUT);
  pinMode(ECHO_FRT, INPUT);

  pararMotores();
  Serial.println("✅ Setup concluído!");
}

// ----------------------
// Função para medir distância
// ----------------------
long medirDistancia(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, 20000); 
  long distancia = duracao * 0.034 / 2;

  if (distancia == 0 || distancia > 200) return 200;
  return distancia;
}

// ----------------------
// Funções de controle motores
// ----------------------
void esquerdaFrente()  { motorEsqFrente.run(FORWARD);  motorEsqTraz.run(FORWARD); }
void esquerdaTras()    { motorEsqFrente.run(BACKWARD); motorEsqTraz.run(BACKWARD); }
void direitaFrente()   { motorDirFrente.run(FORWARD);  motorDirTraz.run(FORWARD); }
void direitaTras()     { motorDirFrente.run(BACKWARD); motorDirTraz.run(BACKWARD); }

void pararMotores() {
  motorEsqFrente.run(RELEASE);
  motorEsqTraz.run(RELEASE);
  motorDirFrente.run(RELEASE);
  motorDirTraz.run(RELEASE);
}

void andarFrente() { 
  esquerdaFrente(); 
  direitaFrente(); 
}

void andarTras() { 
  esquerdaTras(); 
  direitaTras(); 
}

void virarEsq() { 
  esquerdaTras(); 
  direitaFrente(); 
}

void virarDir() { 
  esquerdaFrente(); 
  direitaTras(); 
}

// ----------------------
// Função procurar caminho
// ----------------------
void procurarCaminho(){
  Serial.println("🚧 Todos bloqueados! Procurando caminho...");

  andarTras();
  delay(400);
  pararMotores();

  virarDir();
  delay(400);
  pararMotores();
  long distDirTest = medirDistancia(TRIG_DIR, ECHO_DIR);

  virarEsq();
  delay(500);
  pararMotores();
  long distEsqTest = medirDistancia(TRIG_ESQ, ECHO_ESQ);

  if (distDirTest > distEsqTest) {
    Serial.println("↪️ Direita tem mais espaço! Virando para DIREITA...");
    virarDir();
    delay(600);
  } else {
    Serial.println("↩️ Esquerda tem mais espaço! Virando para ESQUERDA...");
    virarEsq();
    delay(600);
  }

  pararMotores();
}

// ----------------------
// Loop principal
// ----------------------
void loop() {
  long distEsq = medirDistancia(TRIG_ESQ, ECHO_ESQ);
  long distDir = medirDistancia(TRIG_DIR, ECHO_DIR);
  long distFrt = medirDistancia(TRIG_FRT, ECHO_FRT);

  Serial.print("Esq: "); Serial.print(distEsq); 
  Serial.print(" | Dir: "); Serial.print(distDir);
  Serial.print(" | Frt: "); Serial.println(distFrt);

  if (distFrt <= LIMITE) {
    Serial.println("🚨 Obstáculo muito próximo! Ativando procurarCaminho()");
    procurarCaminho();
    delay(300);
    pararMotores();
  }
  else if (distFrt <= DISTANCIA) {
    if (distEsq <= distDir) {
      Serial.println("↩️ Virando para ESQUERDA");
      virarEsq();
      delay(300);
    } else {
      Serial.println("↪️ Virando para DIREITA");
      virarDir();
      delay(300);
    }
    pararMotores();
  }
  else {
    Serial.println("⬆️ Caminho livre — Andando para FRENTE");
    andarFrente();
  }

  delay(INTERVALO_DELAY);
}
