CC=g++
CFLAGS=-std=c++11
LINKERFLAGS=-std=-L/usr/local/lib -l cryptopp -l boost_serialization -l boost_program_options -l boost_system
TMP_PATH=async_chat/out

all: out_dir client_out server_out

out_dir:
	mkdir -p $(TMP_PATH)

client_out: messages shared client client.o
	$(CC) $(LINKERFLAGS) $(TMP_PATH)/chat_message.o $(TMP_PATH)/chat_message_client_notice.o $(TMP_PATH)/chat_message_text.o $(TMP_PATH)/chat_message_text2.o $(TMP_PATH)/chat_data_packet.o $(TMP_PATH)/app.o $(TMP_PATH)/chat_connection.o $(TMP_PATH)/input_handler.o $(TMP_PATH)/client_options.o $(TMP_PATH)/chat_client_connection.o $(TMP_PATH)/client_app.o $(TMP_PATH)/chat_client.o $(TMP_PATH)/client.o -o client

server_out: messages shared server server.o
	$(CC) $(LINKERFLAGS) $(TMP_PATH)/chat_message.o $(TMP_PATH)/chat_message_client_notice.o $(TMP_PATH)/chat_message_text.o $(TMP_PATH)/chat_message_text2.o $(TMP_PATH)/chat_data_packet.o $(TMP_PATH)/app.o $(TMP_PATH)/chat_connection.o $(TMP_PATH)/server_options.o $(TMP_PATH)/chat_server_connection.o $(TMP_PATH)/server_app.o $(TMP_PATH)/chat_server.o $(TMP_PATH)/server.o -o server
	
client.o: async_chat/client.cpp
	$(CC) $(CFLAGS) -c async_chat/client.cpp -o $(TMP_PATH)/client.o

server.o: async_chat/server.cpp
	$(CC) $(CFLAGS) -c async_chat/server.cpp -o $(TMP_PATH)/server.o
	
messages: async_chat/messages/chat_message.cpp async_chat/messages/chat_message_client_notice.cpp async_chat/messages/chat_message_text.cpp async_chat/messages/chat_message_text2.cpp async_chat/messages/chat_data_packet.cpp
	$(CC) $(CFLAGS) -c async_chat/messages/chat_message.cpp -o $(TMP_PATH)/chat_message.o
	$(CC) $(CFLAGS) -c async_chat/messages/chat_message_client_notice.cpp -o $(TMP_PATH)/chat_message_client_notice.o
	$(CC) $(CFLAGS) -c async_chat/messages/chat_message_text.cpp -o $(TMP_PATH)/chat_message_text.o
	$(CC) $(CFLAGS) -c async_chat/messages/chat_message_text2.cpp -o $(TMP_PATH)/chat_message_text2.o
	$(CC) $(CFLAGS) -c async_chat/messages/chat_data_packet.cpp -o $(TMP_PATH)/chat_data_packet.o
	
shared: async_chat/shared/app.cpp async_chat/shared/chat_connection.cpp
	$(CC) $(CFLAGS) -c async_chat/shared/app.cpp -o $(TMP_PATH)/app.o
	$(CC) $(CFLAGS) -c async_chat/shared/chat_connection.cpp -o $(TMP_PATH)/chat_connection.o	
	
client: async_chat/client/client_options.cpp async_chat/client/chat_client_connection.cpp async_chat/client/client_app.cpp async_chat/client/chat_client.cpp async_chat/client/input_handler.cpp
	$(CC) $(CFLAGS) -c async_chat/client/client_options.cpp -o $(TMP_PATH)/client_options.o
	$(CC) $(CFLAGS) -c async_chat/client/chat_client_connection.cpp -o $(TMP_PATH)/chat_client_connection.o
	$(CC) $(CFLAGS) -c async_chat/client/client_app.cpp -o $(TMP_PATH)/client_app.o
	$(CC) $(CFLAGS) -c async_chat/client/chat_client.cpp -o $(TMP_PATH)/chat_client.o
	$(CC) $(CFLAGS) -c async_chat/client/input_handler.cpp -o $(TMP_PATH)/input_handler.o
	
server: async_chat/server/server_options.cpp async_chat/server/chat_server_connection.cpp async_chat/server/server_app.cpp async_chat/server/chat_server.cpp
	$(CC) $(CFLAGS) -c async_chat/server/server_options.cpp -o $(TMP_PATH)/server_options.o
	$(CC) $(CFLAGS) -c async_chat/server/chat_server_connection.cpp -o $(TMP_PATH)/chat_server_connection.o
	$(CC) $(CFLAGS) -c async_chat/server/server_app.cpp -o $(TMP_PATH)/server_app.o
	$(CC) $(CFLAGS) -c async_chat/server/chat_server.cpp -o $(TMP_PATH)/chat_server.o
	
clean:
	rm -f $(TMP_PATH)/*.o client server
