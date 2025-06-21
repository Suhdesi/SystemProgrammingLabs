# tcp_server.py
import socket

# Step 1: Create socket
server_socket = socket.socket(socket.AF_INET, socket.SOCKET_STREAM)

# Step 2: Bind to address and port
server_socket.bind('localhost', 12345))

# Step 3: Listen for incoming connections
server_socket.listen(1)
print("Server is listening on port 12345...")

# Step 4: Accept connection
conn, addr = server_socket.accept()
print(f"Connected by {addr}")

# Step 5: Receive data
data = conn.recv(1024).decode()
print(f"Received from client: {data}")

# Step 6: Send response
conn.send("Hello from Server!".encode())

# Step 7: Close connection
conn.close()
server_socket.close()
