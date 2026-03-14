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

14. Test POST http://localhost:7070/directory/youpi.bla with a size of 100000000

15. Test POST http://localhost:7070/directory/youpla.bla with a size of 100000000

16. Test POST http://localhost:7070/directory/youpi.bla with a size of 100000 with special headers

17. Test POST http://localhost:7070/post_body with a size of 0 -> 200 OK small adaptation to handle POST non CGI/non multipart + normalize location names ('/') and ignore trailing slash detection for POST

18. Test POST http://localhost:7070/post_body with a size of 100 -> 200 OK bodyHandlerTransferEncoding why _content_length + 2??? (hardcode fix: if (before_len + _content_length - 2 > max_body))

19. Test POST http://localhost:7070/post_body with a size of 200

20. Test POST http://localhost:7070/post_body with a size of 101

21. Test multiple workers(5) doing multiple times(15): GET on /

22. Test multiple workers(20) doing multiple times(5000): GET on /

23. Test multiple workers(128) doing multiple times(50): GET on /directory/nop

24. Test multiple workers(20) doing multiple times(5): Post on /directory/youpi.bla with size 100000000

