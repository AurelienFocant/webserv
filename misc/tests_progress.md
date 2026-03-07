Before starting please verify that the server is launched
press enter to continue


1. Test GET http://localhost:7070/ -> 200 OK + serve index
content returned: BLBLBLBLBLBL

2. Test POST http://localhost:7070/ with a size of 0 -> 405 METHOD NOT ALLOWED

3. Test HEAD http://localhost:7070/ -> 405 METHOD NOT ALLOWED

4. Test GET http://localhost:7070/directory -> 301 MOVED PERMANEMTLY -> 200 OK + serve index
content returned: BLBLBLBLBLBL

5. Test GET http://localhost:7070/directory/youpi.bad_extension -> 200 OK + serve index
content returned: BLBLBLBLBLBL INDEX

6. Test GET http://localhost:7070/directory/youpi.bla  -> 200 OK (prioritize prefix > ext location for _matched_location)
content returned: 

7. Test GET Expected 404 on http://localhost:7070/directory/oulalala -> 404 NOT FOUND + error page
content returned: <!DOCTYPE html>

<html lang="en">
	<head>
		<meta charset="UTF-8">
		<title>404 – Page Not Found</title>
	</head>
	<body>

		<div class="container">
			<h1>WebServ 404</h1>
			<p>Oops! The page you're looking for doesn’t exist.</p>
			<a href="/">Back to Home</a>
		</div>

	</body>
</html>


8. Test GET http://localhost:7070/directory/nop -> 301 MOVED PERMANEMTLY -> 200 OK + serve index
content returned: 

9. Test GET http://localhost:7070/directory/nop/ -> 200 OK + serve index
content returned: INDEX

10. Test GET http://localhost:7070/directory/nop/other.pouic -> 200 OK + serve other.pouic content
content returned: POUIC

11. Test GET Expected 404 on http://localhost:7070/directory/nop/other.pouac -> 404 NOT FOUND + error page
content returned: <!DOCTYPE html>

<html lang="en">
	<head>
		<meta charset="UTF-8">
		<title>404 – Page Not Found</title>
	</head>
	<body>

		<div class="container">
			<h1>WebServ 404</h1>
			<p>Oops! The page you're looking for doesn’t exist.</p>
			<a href="/">Back to Home</a>
		</div>

	</body>
</html>


12. Test GET Expected 404 on http://localhost:7070/directory/Yeah -> -> 404 NOT FOUND + error page (corrected 403 to 404)
content returned:

13. Test GET http://localhost:7070/directory/Yeah/not_happy.bad_extension ->200 OK + serve second index
content returned: NOOOOO


14. Test POST http://localhost:7070/directory/youpi.bla with a size of 100000000 -> Hanging somewhere :(
14. Test POST http://localhost:7070/directory/youpi.bla with a size of 100000000 -> now we return 413 Request Entity Too Large and it's wrong 
FATAL ERROR ON LAST TEST: bad status code

