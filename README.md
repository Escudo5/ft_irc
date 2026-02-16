# ft_irc




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