# Buscador de directorios de http

Importante, esto funciona unicamente con HTTP debido a que no se utilizo ssl en la implementacion.

# Para buildear:

Compilar con g++

```
g++ -std=c++17 main.cpp -o scanner
```

Luego se le debe pasar de argumento la locacion de la file donde se encuentra la lista
de palabras a utilizar, en las files se encuentra una extremadamente simple.

```
./scanner ./LOCACION_LISTA
```


