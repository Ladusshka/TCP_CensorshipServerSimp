#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <set>




void censor(const std::set<std::string> & censored_words, std:: string & message){
    for (const auto& word : censored_words) {
            size_t pos = 0;
            while ((pos = message.find(word, pos)) != std::string::npos) {
                message.replace(pos, word.length(), std::string(word.length(), '-'));
                pos += word.length();
            }
        }
}




int main() {
     int server_fd  = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
     if(server_fd <1) {
         std::cerr<<"Could not create socket";
         return -1;
     }

    //configuring the sever
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1337);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.4");

    int bind_res = bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr) );

    if(bind_res == -1) {
        std::cerr<<"binding unsuccessful"<<std::endl;
        close(server_fd);
        return -1;
    }

   if( listen(server_fd, 3) == -1) {
       std::cerr<<"listening unsuccessful"<<std::endl;
       close(server_fd);
       return -1;
   }


    while(true) {
        sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &len);

        if(client_fd == -1) {
            std::cerr<<"could not connect user";
            break;
        }



        
        char buffer[1000];
        std::set<std::string> censored_words;
        std::string message;
        message.reserve(1024);

        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        bool reading_censorship = true;
        
        
        ssize_t bytes_received;
        std::string word  = {};
        ssize_t index = -1;
        while (reading_censorship){

            bytes_received = recv(client_fd, buffer, 1000, 0);
            
            for(ssize_t i=0; i<bytes_received; i++){
                if(buffer[i] == 0x1F){
                    reading_censorship = false;
                    censored_words.insert(word);
                    word.clear();
                    index = i;
                    break;
                }else if(buffer[i] == 0x1E){
                    censored_words.insert(word);
                    word.clear();
                }else{
                    word.push_back(buffer[i]);
                }
            }

             
        }

        
     

        bool end = false;


        for(ssize_t j = index+1; j< bytes_received; j++){
           message.push_back(buffer[j]);
                
                if(buffer[j] == 0x1F){
                    end = true;
                    censor(censored_words, message);
                    std::cout<<std::endl<<"END\n"<<message<<std::endl;
                    
                   send(client_fd, message.c_str(), message.length(), 0);
                   message.clear();
                    break;
                }else if(buffer[j] == 0x1E){
                  
                   censor(censored_words, message);
                   std::cout<<std::endl<<"MID\n"<<message<<std::endl;
                   send(client_fd, message.c_str(), message.length(), 0);
                   message.clear();

                }
        }
        
        
        while(!end){
            bytes_received = recv(client_fd, buffer, 1000, 0);

            
            for(ssize_t i=0; i<bytes_received; i++){
                message.push_back(buffer[i]);
                
                if(buffer[i] == 0x1F){
                    end = true;
                    censor(censored_words, message);
                    std::cout<<std::endl<<"END\n"<<message<<std::endl;
                    
                   send(client_fd, message.c_str(), message.length(), 0);
                   message.clear();
                    break;
                }else if(buffer[i] == 0x1E){
                  
                   censor(censored_words, message);
                   std::cout<<std::endl<<"MID\n"<<message<<std::endl;
                   send(client_fd, message.c_str(), message.length(), 0);
                   message.clear();

                }
            }
        }
        
       
        close(client_fd);

    }





    close(server_fd);


    return 0;
}
