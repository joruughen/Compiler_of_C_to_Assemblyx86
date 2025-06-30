
# Como usar el Makefile

Para compilar el proyecto, simplemente ejecuta el siguiente comando en la terminal:
'''bash
make
'''
esto generara archivos .o y un ejecutable llamado `compiler`.
Para limpiar los archivos generados, puedes usar:
```bash
make clean
```
## Tests del compilador
```bash
make test
```
esto ejecutara los tests del compilador, generando los archivos .s para cada archivo .c en los directorios de la carpeta tests del proyecto.




# Streamlit
Ejecutar el siguiente comando:
```bash
g++ exp.cpp parser.cpp scanner.cpp token.cpp visitor.cpp main.cpp imp_type.cpp gencode.cpp imp_value.cpp imp_value_visitor.hh type_visitor.cpp type_visitor.hh -o compiler
```
# Para ejecutar la aplicación de Streamlit, primero asegúrate de tener instalado Streamlit. Luego, ejecuta el siguiente comando en la terminal:
```bash
streamlit run app.py
```
