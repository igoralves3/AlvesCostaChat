# -*- coding: utf-8 -*-
"""
Created on Fri Oct 31 20:46:27 2025

@author: Igor
"""

import socket
import threading

# Configurações do servidor
HOST = '127.0.0.1'  # Endereço local (localhost)
PORT = 5000         # Porta para escutar conexões

class ClientStruct:
    def __init__(self, conn,user, message, privateM, lobby):
        self.user = user
        self.conn = conn
        self.message = message
        self.privateM = privateM
        self.lobby = lobby

clients = []

clientStructs = []

def handle_client(conn, addr):
    
    clients.append(conn)
    print(f"[NOVA CONEXÃO] Cliente conectado: {addr}")
    while True:
        try:
            data = conn.recv(1024)
            if not data:
                print(f"[DESCONECTADO] Cliente {addr}")
                break

            mensagem = data.decode('utf-8')

            params = mensagem.split(':')

            nuser=params[0]#.replace(' ', '')
            nmessage=params[1]#.replace(' ', '')
            nprivateM=params[2].replace(' ', '')
            nlobby=params[3]#.replace(' ', '')

            newClientStruct = ClientStruct(conn=conn,user=nuser,message=nmessage,privateM=nprivateM,lobby=nlobby)

            clientStructs.append(newClientStruct)

            print(f"[{addr}] {mensagem}")

            # Envia resposta ao cliente
            resposta = f"{mensagem}"
            #for c in clients:
             #   c.sendall(params[1].encode('utf-8')+b'\0')
            
            for cs in clientStructs:
                if cs.privateM == "N":
                   
                    cs.conn.sendall((nuser + ": " +nmessage).encode('utf-8')+b'\0')
                else: 
                    if cs.privateM == "S" and cs.lobby == nlobby and cs.lobby != "None":
                        
                        cs.conn.sendall((nuser + ": " +nmessage).encode('utf-8')+b'\0')

            #clientStructs.remove(newClientStruct)

        except ConnectionResetError:
            print(f"[ERRO] Cliente {addr} desconectou inesperadamente.")
            break

    conn.close()


# Cria o socket (IPv4 + TCP)
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Permite reusar o endereço se o servidor for reiniciado
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# Associa o socket ao endereço e porta
server_socket.bind((HOST, PORT))

# Começa a escutar
server_socket.listen(5)

print(f"Servidor escutando em {HOST}:{PORT}")

while True:
    # Aceita uma nova conexão
    conn, addr = server_socket.accept()
    print(f"Conectado por {addr}")
    
    # NÃO receba dados aqui. Apenas inicie a thread.
    # A thread "handle_client" é que vai cuidar de receber os dados.
    
    thread = threading.Thread(target=handle_client, args=(conn, addr))
    thread.start()
    # Mensagem que será enviada
    #mensagem = data.decode('utf-8') #"Olá, cliente! Esta é uma mensagem do servidor."
    #conn.sendall(mensagem.encode('utf-8'))  # envia em bytes