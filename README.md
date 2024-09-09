# 4100901-Final_Exam-Drill
Please fork this repo, then clone it into your computer, and place your work for the Final exam here.

## Criterio de Evaluación 

* Funcionalidad: 80% (valor equivalente para cada requerimiento)
* Repositorio: 10% (commits entendibles que describan los cambios realizados)
* Documentación: 10% (en código y readme del repositorio)


## Instrucciones

Implemente un **sistema de seguridad** según los requerimientos descritos.

### Requerimientos del sistema:

#### No funcionales:
1. Tener 1 LED para indicar el estatus del sistema de seguridad.
2. Tener un teclado hexadecimal para ingresar la clave del sistema.
3. Tener un puerto de depuración con el PC (USART2 @256000 baudios).
4. Tener un display OLED para mostrar eventos del sistema.

#### Funcionales:
5. El teclado hexadecimal debe recibir la clave en dígitos, que corresponde al documento del estudiante.
6. Se debe presionar la tecla '#' para verificar la clave, o la tecla '*' para reiniciar el ingreso de dígitos.
7. El LED debe parpadear a una frecuencia de 4 Hz por 3s en caso de que la clave ingresada sea incorrecta.
8. El LED debe parpadear a una frecuencia de 0 Hz por 3s en caso de que la clave ingresada sea correcta.
9. El display OLED debe mostrar "Success" o "Error" según el resultado cuando la clave sea verificada.
10. Debe enviar mensajes de información por la consola cuando hayan eventos en el sistema.
