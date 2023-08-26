#!/usr/bin/python3

import os

# Imprime o cabeçalho HTTP Content-Type para indicar que estamos enviando uma resposta em HTML
print("Content-type: text/html\n")

# Inicia o corpo do HTML
print("<html>")
print("<head><title>Resposta do Script CGI</title></head>")
print("<body>")
print("<h1>Parâmetros Recebidos:</h1>")

# Parseia os parâmetros da query string e os imprime no HTML
query_string = os.environ.get("QUERY_STRING", "")
parameters = query_string.split("&")
for param in parameters:
    key, value = param.split("=")
    print(f"<p>{key} = {value}</p>")

# Finaliza o corpo do HTML
print("</body>")
print("</html>")
