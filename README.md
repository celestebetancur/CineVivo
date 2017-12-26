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
