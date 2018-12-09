/** 
  FallDetector - Implementación del Sketch
  -----------------------------------------
  Desarrollado por:
  Grupo 2:
    - Sergio Laso Mangas
    - José Manuel Romero Muelas
    - Sergio Zabala Mesonero
  Asignatura: Descripción de Sistemas Hardware
  Máster en Ingeniería Informática - Curso 2018/19 **/
  
#include <FallDetector.h>
FallDetector f;
void setup() {
  
  f.configuracionInicial();

}

void loop() {
  f.ejecucion();
}
