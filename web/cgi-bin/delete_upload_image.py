import os
import sys

# Verifique se foi fornecido um argumento com o nome do diretório
if len(sys.argv) != 2:
	print("Usage: python script.py <directory path>")
	sys.exit(1)

# Caminho do diretório passado como argumento
dir_path = sys.argv[1]

# Verifica se o diretório existe
if os.path.isdir(dir_path):
	# Lista os arquivos no diretório
	files = os.listdir(dir_path)

	# Cria o conteúdo HTML
	html = f"""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Delete some file</title>
	<link rel="stylesheet" href="../assets/styles.css">
</head>
<body>
	<section class="main-section">
		<div class="navMenu">
			<a href="/">home</a>
			<a href="/about.html">about</a>
			<a href="/tour.html">tour</a>
			<div class="dot"></div>
		</div>
	</section>
	<section class="section-container">
		<div class="section-button">
			<h1>Files in Directory</h1>
			<form method="POST" action="/images/delete">
				<input type="hidden" name="_method" value="DELETE"> <!-- form com campo oculto para especificar o método DELETE -->
				<ul>
"""
	# Adicione opções para cada arquivo no diretório
	for file in files:
		html += f"                    <li>{file} <button type=\"submit\" name=\"arquivoSelecionado\" value=\"{file}\">Excluir</button></li>"

	html += """
				</ul>
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
</html>
"""

	# Imprime o conteúdo HTML modificado
	print(html)
