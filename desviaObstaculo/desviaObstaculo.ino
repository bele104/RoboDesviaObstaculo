#include <AFMotor.h>

// --- Motores ---
// Direita -> M1 (frente) e M4 (trás)
AF_DCMotor motorDir(1);     
AF_DCMotor motorDirTraz(4); 

// Esquerda -> M2 (frente) e M3 (trás)
AF_DCMotor motorEsq(2);     
AF_DCMotor motorEsqTraz(3); 

// --- Sensores ultrassônicos ---
// Sensor DIREITO -> A0 e A1
#define TRIG_DIR A0
#define ECHO_DIR A1

// Sensor ESQUERDO -> A2 e A3
#define TRIG_ESQ A2
#define ECHO_ESQ A3

// Sensor FRONTAL -> A4 e A5
#define TRIG_FRT A4
#define ECHO_FRT A5

// --- Configuração --- em cm
#define DISTANCIA_CRITICA 40 
#define DISTANCIA_LATERAL 20 
#define DISTANCIA_LIMITE 10
#define DISTANCIA_LIMITE_LATERAL 5

// Velocidade dos motores
#define VEL_ESQ 200
#define VEL_DIR 200

void setup() {
  Serial.begin(9600);

  motorEsq.setSpeed(VEL_ESQ);  
  motorEsqTraz.setSpeed(VEL_ESQ); 
  motorDir.setSpeed(VEL_DIR);  
  motorDirTraz.setSpeed(VEL_DIR); 

  pararMotores();

  pinMode(TRIG_ESQ, OUTPUT);
  pinMode(ECHO_ESQ, INPUT);
  pinMode(TRIG_DIR, OUTPUT);
  pinMode(ECHO_DIR, INPUT);
  pinMode(TRIG_FRT, OUTPUT);
  pinMode(ECHO_FRT, INPUT);

  Serial.println("Setup concluído!");
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

  if(distancia == 0 || distancia > 200) return 200;

  return distancia;
}

// ----------------------
// Funções de controle motores
// ----------------------

// --- Motores da ESQUERDA ---
void esquerdaFrente() {
  motorEsq.run(FORWARD);
  motorEsqTraz.run(FORWARD);
}
void esquerdaTras() {
  motorEsq.run(BACKWARD);
  motorEsqTraz.run(BACKWARD);
}
void esquerdaParar() {
  motorEsq.run(RELEASE);
  motorEsqTraz.run(RELEASE);
}

// --- Motores da DIREITA ---
void direitaFrente() {
  motorDir.run(FORWARD);
  motorDirTraz.run(FORWARD);
}
void direitaTras() {
  motorDir.run(BACKWARD);
  motorDirTraz.run(BACKWARD);
}
void direitaParar() {
  motorDir.run(RELEASE);
  motorDirTraz.run(RELEASE);
}

// --- Controle geral ---
void pararMotores() {
  esquerdaParar();
  direitaParar();
  Serial.println("Motores parados");
}
void andarFrente() {
  esquerdaFrente();
  direitaFrente();
  Serial.println("Andando para FRENTE");
}
void andarTras() {
  esquerdaTras();
  direitaTras();
  Serial.println("Re quando necessário");
}
void virarEsq() {
  esquerdaTras();
  direitaFrente();
  Serial.println("Virando ESQUERDA");
}
void virarDir() {
  esquerdaFrente();
  direitaTras();
  Serial.println("Virando DIREITA");
}

// ----------------------
// Função procurar caminho
// ----------------------
void procurarCaminho(){
  Serial.println("Todos bloqueados! Procurando caminho...");

  andarTras();
  delay(400);
  pararMotores();

  virarDir();
  delay(500);
  pararMotores();
  long distDirTest = medirDistancia(TRIG_DIR, ECHO_DIR);

  virarEsq();
  delay(1000);
  pararMotores();
  long distEsqTest = medirDistancia(TRIG_ESQ, ECHO_ESQ);

  if(distDirTest > distEsqTest){
    Serial.println("Direita tem mais espaço!");
    virarDir();
    delay(600);
  } else {
    Serial.println("Esquerda tem mais espaço!");
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

  if(distFrt < DISTANCIA_LIMITE && distEsq < DISTANCIA_LIMITE && distDir < DISTANCIA_LIMITE){
    procurarCaminho();
  }
  else if( (distFrt < DISTANCIA_LIMITE_LATERAL && distEsq < DISTANCIA_LIMITE_LATERAL) || 
           (distFrt < DISTANCIA_LIMITE_LATERAL && distDir < DISTANCIA_LIMITE_LATERAL) ){
    procurarCaminho();
  }
  else if(distEsq < DISTANCIA_LATERAL){
    virarDir();
    delay(700);
    pararMotores();
  }
  else if(distDir < DISTANCIA_LATERAL){
    virarEsq();
    delay(700);
    pararMotores();
  }
  else if(distFrt >= DISTANCIA_CRITICA){
    andarFrente();
  }
  else{
    if(distEsq > distDir){
      virarEsq();
      delay(500);
    }
    else if(distDir >= distEsq){
      virarDir();
      delay(500);
    }
    else{
      andarTras();
      delay(500);
    }
  }

  delay(200);
}
