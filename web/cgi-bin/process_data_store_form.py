#!/usr/bin/env python3
import os
import sys
import urllib.parse

file_path = sys.argv[1]

content = ""
if "E_FILE" in os.environ:

	content_length = int(os.environ["CONTENT_LENGTH"])
	data = sys.stdin.buffer.read(content_length)
	content = data
else:
	data = sys.stdin.read()
	content = data

def urldecode(data):
    return urllib.parse.unquote_plus(data)

decoded_message = ""    

if "CONTENT_TYPE" in os.environ:
    content_type = os.environ["CONTENT_TYPE"]
    location = os.environ["LOCATION"]
    file_name = os.environ["FILE_NAME"]

    if "image" in content_type:
        with open(file_path, "wb") as f:
            f.write(content)

        print("<!DOCTYPE html>")
        print("<html lang='en'>")
        print("<head>")
        print("    <meta charset='UTF-8'>")
        print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        print("    <title>Upload of a file</title>")
        print("    <link rel='stylesheet' href='/assets/styles.css'>")
        print("</head>")
        print("<body>")
        print("<header>")
        print("<div class='navMenu'>")
        print("<a href='/'>home</a>")
        print("<a href='/tour.html'>tour</a>")
        print("<a href='#'>tests</a>")
        print("<div class='dot'></div>")
        print("</div>")
        print("</header>")
        print("<section class='section-container'>")
        print("<div class='upload-container'>")
        
        print("<h1>Hello! Let's practice a bit with POST?</h1>")
        print("<span class='upload-span'>This project requires the creation of some CGI scripts.</span>")
        print("<span class='upload-span'>In this case, let's play around with the functionality of uploading an image file to our server and displaying that image here.</span>")
        print("</div>")
        print("<div class='upload-container'>")
        print("<h2>REMEMBER</h2>")
        print("<span class='upload-span'>Everything has a limit</span>")
        print("<span class='upload-span'>The submitted image must be up to - megabytes. More than that, and the server will not accept it!</span>")
        print("</div>")
        print("<div id='choose-image'>")
        print("<div>")
        print("<span class='upload-span'>Let's go? The image will appear in this <span class='colored'>highlighted space</span></span>")
        print("</div>")
        print("<div id='image'>")
        print(f"<img src='{location}/{file_name}' alt='submit-image'>")
        print("</div>")
        print("</div>")
        print("</section>")
        print("</body>")
        print("</html>")
    else:
        print("Content-Type: text/html")
        print("")
        print("<!DOCTYPE html>")
        print("<html lang='en'>")
        print("<head>")
        print("    <meta charset='UTF-8'>")
        print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        print("    <title>Upload of a file</title>")
        print("    <link rel='stylesheet' href='/assets/styles.css'>")
        print("</head>")
        print("<body>")
        print("<section class='section-container'>")
        print("<div class='upload-container'>")
        print("<h1>Hello! Let's practice a bit with POST?</h1>")
        print("<span class='upload-span'>This project requires the creation of some CGI scripts.</span>")
        print("<span class='upload-span'>In this case, let's play around with the functionality of uploading an image file to our server and displaying that image here.</span>")
        print("</div>")
        print("<div class='upload-container'>")
        print("<h2>REMEMBER</h2>")
        print("<span class='upload-span'>Everything has a limit</span>")
        print("<span class='upload-span'>The submitted image must be up to - megabytes. More than that, and the server will not accept it!</span>")
        print("</div>")
        print("<div id='choose-image'>")
        print("<p>Wait, it needs to be an image to appear here... Otherwise it's weird.</p>")
        print("</div>")
        print("</section>")
        print("</body>")
        print("</html>")
else:
	decoded_entrance = urllib.parse.unquote(content)
	pairs = decoded_entrance.split('&')
	with open(file_path, 'w') as file:
		for pair in pairs:
			key, key = pair.split('=')
			key = key.replace('+', ' ')
			file.write(f"{key}: {key}\n")
	print("HTTP/1.1 204 OK")
	print("")
