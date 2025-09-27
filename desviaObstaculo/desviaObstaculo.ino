#include <AFMotor.h>

// --- Motores ---
AF_DCMotor motorEsq(1);  // M1
AF_DCMotor motorDir(2);  // M2
AF_DCMotor motorEsq2(3); // M3
AF_DCMotor motorDir2(4); // M4

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

void setup() {
  Serial.begin(9600);

  // Define velocidade para todos os motores
  motorEsq.setSpeed(200);
  motorDir.setSpeed(200);
  motorEsq2.setSpeed(200);
  motorDir2.setSpeed(200);

  pararMotores();

  pinMode(TRIG_ESQ, OUTPUT);
  pinMode(ECHO_ESQ, INPUT);
  pinMode(TRIG_DIR, OUTPUT);
  pinMode(ECHO_DIR, INPUT);
  pinMode(TRIG_FRT, OUTPUT);
  pinMode(ECHO_FRT, INPUT);
}

// --- Função para medir distância ---
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

// --- Funções de movimento ---
void pararMotores(){
  motorEsq.run(RELEASE);
  motorDir.run(RELEASE);
  motorEsq2.run(RELEASE);
  motorDir2.run(RELEASE);
}

void andarFrente(){
  motorEsq.run(FORWARD);
  motorDir.run(FORWARD);
  motorEsq2.run(FORWARD);
  motorDir2.run(FORWARD);
  Serial.println("Andando para FRENTE");
}

void andarTras(){
  motorEsq.run(BACKWARD);
  motorDir.run(BACKWARD);
  motorEsq2.run(BACKWARD);
  motorDir2.run(BACKWARD);
  Serial.println("Re quando necessário");
}

void virarEsq(){
  motorEsq.run(FORWARD);
  motorDir.run(BACKWARD);
  motorEsq2.run(FORWARD);
  motorDir2.run(BACKWARD);
  Serial.println("Virando ESQUERDA");
}

void virarDir(){
  motorEsq.run(BACKWARD);
  motorDir.run(FORWARD);
  motorEsq2.run(BACKWARD);
  motorDir2.run(FORWARD);
  Serial.println("Virando DIREITA");
}

// --- Nova função procurar caminho ---
void procurarCaminho(){
  Serial.println("Todos bloqueados! Procurando caminho...");

  // 1. Dá ré por um tempo curto
  andarTras();
  delay(400);
  pararMotores();

  // 2. Mede virando para a direita
  virarDir();
  delay(500);
  pararMotores();
  long distDirTest = medirDistancia(TRIG_DIR, ECHO_DIR);

  // 3. Mede virando para a esquerda
  virarEsq();
  delay(1000);
  pararMotores();
  long distEsqTest = medirDistancia(TRIG_ESQ, ECHO_ESQ);

  // 4. Decide para onde ir
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

// --- Loop principal ---
void loop() {
  long distEsq = medirDistancia(TRIG_ESQ, ECHO_ESQ);
  long distDir = medirDistancia(TRIG_DIR, ECHO_DIR);
  long distFrt = medirDistancia(TRIG_FRT, ECHO_FRT);

  Serial.print("Esq: "); Serial.print(distEsq); 
  Serial.print(" | Dir: "); Serial.print(distDir);
  Serial.print(" | Frt: "); Serial.println(distFrt);

  // --- Situação crítica: todos bloqueados ---
  if(distFrt < DISTANCIA_LIMITE && distEsq < DISTANCIA_LIMITE && distDir < DISTANCIA_LIMITE){
    procurarCaminho();
  }
  // --- Situação crítica parcial: frente e um lado bloqueados ---
  else if( (distFrt < DISTANCIA_LIMITE_LATERAL && distEsq < DISTANCIA_LIMITE_LATERAL) || 
           (distFrt < DISTANCIA_LIMITE_LATERAL && distDir < DISTANCIA_LIMITE_LATERAL) ){
    procurarCaminho();
  }
  // --- Sensor lateral independente ---
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
  // --- Lógica normal ---
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
