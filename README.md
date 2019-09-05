# CineVivo 
OCS visual app

CineVivo es una app para hacer edición de videos o camaras en tiempo real que puede ser controlada por su propio mini lenguaje o usando el protocolo OSC.

El archivo help.txt tiene una pequeña guia y keywords.txt la referencia completa del minilenguaje.

Para el envio de mensajes OSC, el puerto de escucha es 5612 (este puerto puede cambiarse desde el archivo de configuración OSCconfig.xml), cada paquete debe ser construido en referencia al minilenguaje de está forma:

1 load anyVideo.mov

Esta linea en la sintaxis del minilenguaje CineVivo carga el video anyVideo.mov en la capa 1. El mensaje OSC equivalente para ser enviado por cualquier aplicación es:

/load 1 "anyVideo.mov"

Así con todos los comandos.

Los videos a manipular deben ser puestos en la carpeta videos, las mascaras en mask y se recomienda guardar los códigos (al presionar cmd + s) en code (para tener sintaxis con colores debe guardar su archivo con la extensión .cvf), cmd + o permite abrir los códigos guardados.

(Al ejecutar la palabra: espanol, el lenguaje funciona con palabras claves en español, english hace lo mismo para ingles, que es el idioma por defecto, languageES.xml contiene todas las palabras claves en español pero puede ser editado en tiempo real para poner las palabras que mas le gusten)

cmd + 0 permite acceder a una consola:
  list-videos, este comando hace una lista de los nombres de los videos disponibles en la carpeta -videos-
  list-mask, este comando hace una lista de los nombres de las mascaras disponibles en la carpeta -mask-
  help, abre el help.txt en el buffer 9 que puede accederse con cmd + 9


Para compilar en Linux, debe eliminar del archivo addons.make la linea que incluye ofxSyphon.


Existen unos ejecutables precompilados que pueden ser probados, son estables en algunos Sistemas Operativos y altamente inestables en otros...
:) sirvase.

<https://github.com/essteban/CineVivo-Releases/releases>
