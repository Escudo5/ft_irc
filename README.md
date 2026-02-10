# ft_irc




## Tareas y pasos para el proyecto

- [ ] Punto de entrada, gestionar argumentos (puerto y password) y arrancar el motor.
- [ ] Infraestructura de red, Crear socket, ponerlo en modo no-bloqueante usando `fcntl()`y empezar a escuchar.
- [ ] Bucle de eventos. implementar el unico `poll()` permitido para vigilar todos los FD.
- [ ] Gestion de clientes: Crear una ficha (objeto) para cada persona que se conecte para guardar el estado y su buffer de mensaje.



# Funciones:

- `Socket()`: Crea tunel de comunicacion.
- `setssockopt()`: Para poder utilizar el puerto inmediatamente despues de cerrar el servidor(buscar `SO_REUSEADDR`).
- `bind()`: Ata el socket al puerto que te pasan por argumento.
- `listen()`: Marca el  socket como pasivo, listo para aceptar gente.


## Estructura de server.hpp

Usamos STL `map` para poder acceder de forma rapida al fd del cliente.
Tenemos distintos metodos para gestionar datos y conexiones.
Tambien tenemos un vector con pollfd


# ** El `server_fd` tiene que ser vigilado por `poll()`**
Si fuera independiente hay que vigilarlo constantemente y gasta CPU.