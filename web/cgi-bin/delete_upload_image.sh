#!/bin/bash

# Define o cabeçalho Content-Type para informar que a saída é HTML
echo "Content-Type: text/html"
echo ""

# Verifique se foi fornecido um argumento com o nome da página
if [ $# -ne 1 ]; then
  echo "Uso: $0 <nome_da_pagina>"
  exit 1
fi

# Caminho da imagem passado como argumento
caminho_da_imagem="$1"

# Verifica se o arquivo existe
if [ -f "$caminho_da_imagem" ]; then
    # Substitui "imagem1.jpg" pelo caminho do arquivo passado por env
    html=$(cat <<EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Delete - Enviar Solicitação</title>
    <link rel="stylesheet" href="../assets/styles.css">
</head>
<body>
    <section class="main-section">
        <div class="navMenu">
            <a href="/">home</a>
            <a href="../pages/about.html">about</a>
            <a href="../pages/delete.html">delete</a>
            <a href="../pages/tour.html">tour</a>
            <a href="#">tests</a>
            <div class="dot"></div>
        </div>
    </section>
    <section class="section-container">
        <div class="section-button">
            <h1>Imagem a ser excluída </h1>
			<img src="$caminho_da_imagem" alt="Imagem de alguma coisa">
            <form method="POST" action="$caminho_da_imagem">
                <input type="hidden" name="_method" value="DELETE"> <!--form com campo oculto para especificar o método DELETE -->
                <select name="imagemSelecionada">
                    <option value="$caminho_da_imagem">Arquivo Gravado</option>
                </select>
                <button type="submit">excluir arquivo</button>
            </form>
        </div>
    </section>
    <footer>
        <div class="contact">
            <a href="https://github.com/usuario-github-lfranca">lfranca-</a>
            <a href="https://github.com/seu-usuario-github">cleticia</a>
        </div>
        <h3 class="title-footer">&copy; 2023 WebServ</h3>
    </footer>
</body>
</html>
EOF
)

    # Imprime o conteúdo HTML modificado
    echo "$html"
else
    # Imprime uma mensagem de erro caso o arquivo não seja encontrado
    echo "Arquivo não encontrado: $caminho_da_imagem"
fi