#!/bin/bash

# Imprime o cabeçalho HTTP Content-Type para indicar que estamos enviando uma resposta em HTML


# Inicia o corpo do HTML
echo "<!DOCTYPE html>"
echo "<html lang='pt-br'>"
echo "<head>
		<meta charset='UTF-8'>
    	<meta name='viewport' content='width=device-width, initial-scale=1.0'>
		<title>Informações submetidas no formulário</title>
		<link rel='stylesheet' href='/assets/styleCGIPageForm.css'>
	</head>"
echo "<body>"
echo "<header>
		<section class='main-section'>
			<div class='navMenu'>
				<a href='/'>return</a>
				<div class='dot'></div>
			</div>
		</section>"
echo "<h1>Informações Recebidas</h1>"
echo "</header>"
echo "<section class='test-container submitted-info'>"

# Parseia os parâmetros da query string e os imprime no HTML
IFS="&"
for param in $QUERY_STRING; do
  IFS="="
  set -- $param
  key=$1
  value=$2
 	decodedkey=$(echo "$key" | sed 's/%40/@/g')
	decodedvalue=$(echo "$value" | sed 's/%40/@/g')
  echo "<div class='form-info'>"
  echo "<span class='field'>$decodedkey | </span>"
  echo "<span>$decodedvalue</span>"
  echo "</div>"
done

# Finaliza o corpo do HTML
echo "</section>"
echo "</body>"
echo "</html>"