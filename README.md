# Server-Client

The project implements a server-client application on a local Linux machine using C/C++. The server receives strings from clients, sorts the characters within each string, and aggregates all data by Linux user ID. The client can send multiple requests before receiving any response, and the server ensures that each portion of the sorted data is sent to the client only once.

Example for the client interaction:

send("thequickbrownfoxjumpsoverthelazydog")
recv(10) -> abcdeeefgh
recv(5) -> hijkl
recv(5) -> mnooo

send("test", 5)
recv(2) -> es
send("case", 4)
recv(4) -> aces


