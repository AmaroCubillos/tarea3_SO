# Sincronización de threads
Este proyecto simula la sincronización y gestión de fábricas de ingredientes y chefs para elaborar alta cocina. Utiliza programación paralela en C con subprocesos, semáforos y buffers circulares para coordinar la producción, el almacenamiento y el consumo de componentes para garantizar la exclusión mutua y evitar conflictos sobre recursos compartidos.

## Entrada
El programa recibe parámetros a través de la línea de comandos para personalizar la simulación:
Ejemplo:

./programa -b <tam_buffer> -h <prod_harina> -c <prod_carne> -v <prod_vegetales> -e <prod_especias> -x <chefs_x> -y <chefs_y> -z <chefs_z> -i <num_platos>

-b: Tamaño del buffer para cada ingrediente.

-h: Número de productores de harina.

-c: Número de productores de carne.

-v: Número de productores de vegetales.

-e: Número de productores de especias.

-x: Número de chefs de tipo X.

-y: Número de chefs de tipo Y.

-z: Número de chefs de tipo Z.

-i: Cantidad total de platos a preparar.

## Funcionalidad

Productores: Threads que generan ingredientes (harina, carne, vegetales, especias) y los colocan en buffers circulares.

Chefs: Threads consumidores que toman ingredientes específicos y utilizan utensilios (batidora o sartén) para preparar los platillos.

Sincronización: Semáforos y mutex aseguran la exclusión mutua y la coordinación entre threads, evitando bloqueos y accesos no seguros.
Terminación: El programa concluye tras alcanzar el número total de platos especificado.
