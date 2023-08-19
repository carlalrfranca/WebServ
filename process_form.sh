#!/bin/bash

# Imprime o cabeçalho HTTP Content-Type para indicar que estamos enviando uma resposta em HTML

# Inicia o corpo do HTML
echo "<html>"
echo "<head><title>Resposta do Script CGI</title></head>"
echo "<body>"
echo "<h1>Parâmetros Recebidos:</h1>"

# Parseia os parâmetros da query string e os imprime no HTML
IFS="&"
for param in $QUERY_STRING; do
  IFS="="
  set -- $param
  key=$1
  value=$2
  echo "<p>$key = $value</p>"
done

# Finaliza o corpo do HTML
echo "</body>"
echo "</html>"