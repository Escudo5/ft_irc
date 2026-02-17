# ft_irc 

Hola! Este archivo documenta nuestro viaje construyendo un servidor IRC desde cero (C++ 98).

## 1. El Flujo de Datos 
Hemos aprendido que internet no envía "mensajes", envía un flujo continuo de bytes (`stream`).
- **Problema:** A veces recibimos medio mensaje, o dos mensajes juntos.
- **Solución:** Implementamos un buffer  en cada Cliente.
  - Guardamos todo lo que llega con `appendBuffer()`.
  - Usamos un bucle `while` para buscar saltos de línea (`\n`).
  - Cortamos y procesamos cada línea completa, guardando el resto para después.

## 2. El Parser
Una vez tenemos una línea limpia, tenemos que entenderla. El protocolo IRC es texto puro.
- **Estructura:** `[Prefijo] COMANDO [Argumentos...] [:Mensaje final]`
- **Logro:** Hemos creado un parser que ignora mayúsculas/minúsculas en el comando y separa prefijos y argumentos correctamente.

## 3. El Despachador 
¿Cómo ejecutamos código diferente para cada comando sin un `if/else` infinito?
- **Solución:** Usamos un `std::map<std::string, FunctionPointer>`.
- Es como un directorio telefónico: Buscamos "NICK" y nos da la dirección de la función `_handleNick`.
- Eficiente, limpio y escalable.

---
## Compilación y Ejecución
```bash
make
./ircserv <port> <password>
```
Conéctate con `nc localhost <port>` y prueba a escribir `PASS tu_pass`, `NICK sergio`, etc.





## Tareas y pasos para el proyecto

- [ ] Punto de entrada, gestionar argumentos (puerto y password) y arrancar el motor.
- [X] Infraestructura de red, Crear socket, ponerlo en modo no-bloqueante usando `fcntl()`y empezar a escuchar.
- [X] Bucle de eventos. implementar el uncio `poll()` permitido para vigilar todos los FD.
- [ ] Gestion de clientes: Crear una ficha (objeto) para cada persona que se conecte para guardar el estado y su buffer de mensaje.
- [ ] hacer funcion qeu procese el comando


# Funciones:

- `Socket()`: Crea tunel de comunicacion. Le pide al sistema un FD para empezar a hablar.
- `setssockopt()`: Para poder utilizar el puerto inmediatamente despues de cerrar el servidor(buscar `SO_REUSEADDR`).
- `bind()`: Ata el socket al puerto que te pasan por argumento.
- `listen()`: Marca el  socket como pasivo, listo para aceptar gente.
- `accept()`: Cuando suena el telefono, esta funcion acepta la conexion y crea un nuevo socket especifico para hablar con el cliente en particular.
- `poll()`: Vigila todos los sockets(el de escucha y los de cada cliente) y te avisa de cual tiene datos para leer o recibir.
- `recv()/send()`: se usan para recibir y enviar mensajes a traves de los sockets de los clientes.
- `htons()/htonl()`: Convierte numeros del orden de bytes al orden de red, para que las maquinas se entiendan.
- `fcntl()`: para marcar los sockets como no bloqueantes.


## Estructura de server.hpp

Usamos STL `map` para poder acceder de forma rapida al fd del cliente.
Tenemos distintos metodos para gestionar datos y conexiones.
Tambien tenemos un vector con pollfd


# ** El `server_fd` tiene que ser vigilado por `poll()`**
Si fuera independiente hay que vigilarlo constantemente y gasta CPU.




## Uso de stringstream para tokenizar comandos

```c++
#include <sstream>

std::string frase = "NICK paco123";
std::stringstream ss(frase); // 1. Cargamos
std::string palabra;

ss >> palabra; // Ahora palabra vale "NICK"
ss >> palabra; // Ahora palabra vale "paco123"
```
