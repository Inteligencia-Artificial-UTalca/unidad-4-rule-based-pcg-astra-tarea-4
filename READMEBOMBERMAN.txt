README: Generador de Mapas Bomberman PCG
Para ejecutar este programa, sigue estos pasos:

Compila el código usando G++ con el siguiente comando:

g++ BombermanPCG.cpp -o mapa

Una vez compilado, ejecuta el programa:

./mapa


Los parámetros predefinidos en el código (BombermanPCG.cpp) son aquellos que resultaron óptimos tras el proceso de búsqueda Grid Search para la creación de los mapas. Si deseas experimentar con diferentes tipos de mapas, puedes modificar las proporciones de los elementos (muros destructibles, power-ups y enemigos) directamente en el código fuente.