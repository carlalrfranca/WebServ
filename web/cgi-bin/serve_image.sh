#!/bin/bash

# Verifique se foi fornecido um argumento com o caminho da imagem
if [ $# -ne 1 ]; then
  echo "Uso: $0 <caminho_da_imagem>"
  exit 1
fi

# Caminho da imagem passado como argumento
caminho_da_imagem="$1"

# Verifique se o arquivo de imagem existe
if [ ! -f "$caminho_da_imagem" ]; then
  echo "Arquivo de imagem não encontrado."
  exit 1
fi

cat "$caminho_da_imagem"













#!/bin/bash

# Define o cabeçalho Content-Type para informar que a saída é HTML
echo "Content-Type: text/html"
echo ""

# Verifique se foi fornecido um argumento com o nome do diretório
if [ $# -ne 1 ]; then
  echo "Uso: $0 <caminho_do_diretorio>"
  exit 1
fi

# Caminho do diretório passado como argumento
caminho_do_diretorio="$1"

# Verifica se o diretório existe
if [ -d "$caminho_do_diretorio" ]; then
    # Lista os arquivos no diretório
    arquivos=$(ls "$caminho_do_diretorio")

    # Substitui "imagem1.jpg" pelo caminho do diretório passado por env
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
            <a href="/about.html">about</a>
            <a href="/delete.html">delete</a>
            <a href="/tour.html">tour</a>
            <a href="#">tests</a>
            <div class="dot"></div>
        </div>
    </section>
    <section class="section-container">
        <div class="section-button">
            <h1>Arquivos no Diretório</h1>
            <form method="POST" action="seu_script_de_exclusao.sh">
                <input type="hidden" name="_method" value="DELETE"> <!-- form com campo oculto para especificar o método DELETE -->
                <select name="arquivoSelecionado">
EOF
)

    # Adicione opções para cada arquivo no diretório
    for arquivo in $arquivos; do
        html+="                    <option value=\"$arquivo\">$arquivo</option>"
    done

    html+=(
"                </select>
                <button type=\"submit\">Excluir arquivo</button>
            </form>
        </div>
    </section>
    <footer>
        <div class=\"contact\">
            <a href=\"https://github.com/usuario-github-lfranca\">lfranca-</a>
            <a href=\"https://github.com/seu-usuario-github\">cleticia</a>
        </div>
        <h3 class=\"title-footer\">&copy; 2023 WebServ</h3>
    </footer>
</body>
</html>"
)

    # Imprime o conteúdo HTML modificado
    echo "$html"
else
    # Imprime uma mensagem de erro caso o diretório não seja encontrado
    echo "Diretório não encontrado: $caminho_do_diretorio"
fi
