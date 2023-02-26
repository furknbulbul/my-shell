#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <queue>
#include <sys/wait.h>

using namespace std;
queue<string> history;//history is hold as a queue


//listdir function runs the ls command
void listdir(){
    pid_t pid = fork(); 
    if(pid == 0){
        //child process runs the command 
        execl("/bin/ls", "ls", NULL);
    } 
    else if(pid > 0){
        //parent waits
        wait(NULL);
    }
    
}

//mycomputer function runs the hostname command
void mycomputername(){
    pid_t pid = fork();
    if(pid == 0){
        //child process runs the command 
        execl("/bin/hostname", "hostname", "-s", NULL);
    } 
    else if(pid > 0){
        //parent waits
        wait(NULL);
    }
    
}

//function to open gedit
void hellotext() {
    pid_t pid = fork();
    if(pid == 0) {
        //child process runs the command 
        execl("/bin/gedit", "gedit", NULL);
    }
    else if(pid > 0){
        //parent waits gedit to be closed 
        wait(NULL);
    }
}

//function to return ip address
void myip() {
    pid_t pid = fork ();
     if(pid == 0) {
        //child process runs the command 
        execl("/bin/hostname", "hostname", "-I", NULL);
    }
    else if(pid > 0){
        //parent waits gedit to be closed 
        wait(NULL);
    }

}
//print file line by line when enter pressed
void print_file(string filename){
    fstream myfile;
    string line;
    myfile.open(filename);
    pid_t pid = fork();
    //create process and handle all printing in child
    if(pid == 0){
        getline(myfile, line);
        cout << line;
        while(getline(myfile, line)){
            string tmp;
            getline(cin, tmp);
            cout << line ;
        }
        exit(0);
    }
    else if(pid > 0){
        //wait until printing done
        wait(NULL);
    }
}

//copies a file to another file
void copy_file_another(string filename, string newfile){
    pid_t pid = fork();
    if(pid == 0){
        char* filename_ptr = &*filename.begin();
        char* newfile_ptr = &*newfile.begin();

        execl("/bin/cp", "cp", filename_ptr,  newfile_ptr, NULL);
    } 
    else if(pid > 0){
        wait(NULL);
    }
}

//checks the history queue and return boolean
bool dididothat(string command){
    queue<string> cpy_history = history;
    //traverse over queue and check command is exist
    while(!cpy_history.empty()){
        string element = cpy_history.front(); 
        if (element == command){
            return true;
        }
        cpy_history.pop();
    }
    return false;
}


//print username for every command input
void print_username(){
    string username = getlogin();
    cout << username << " >>>" ; 
}

//parse command line and return a vector of strings
vector<string> parse_command(string str){
    vector<string> vc;

    char* ptr;
    char* str_ptr = &*str.begin();

    ptr = strtok(str_ptr, " ");
    while(ptr != NULL){
        vc.push_back(ptr);
        ptr=strtok(NULL, " ");
    }

    return vc;
}

void handle_history(vector<string> tokens){
    if(history.size() == 15){ //if history size greater than remove element from queue
        history.pop(); 
    }
    //format string according to the string format of dididothat command
    string queue_string = "\"";
    for (int i = 0; i< tokens.size()-1; i++){
        queue_string +=tokens[i];
        queue_string += " ";
    }
    queue_string += tokens[tokens.size()-1];
    queue_string += "\"";
    history.push(queue_string); // push formatted string to the queue
}


void handle_command(string line){
    if (line == "") {
        return;
    }
    string command = "";
    vector<string> tokens = parse_command(line); //tokens are parsed and returned by parse_command function
    command = tokens[0];

    //according to the first token of command handle it by calling necessary functions
    if (command == "exit") {
        exit(0);
    }

    else if (command ==  "listdir"){
        listdir();
    }
    else if (command == "mycomputer"){
        mycomputername();
    }
    else if (command == "hellotext"){
        hellotext();
    }
    else if (command == "whatsmyip"){
        myip();
    }

    else if (command == "printfile"){
        string filename = tokens[1];
        if(tokens.size() == 2){
            print_file(filename);
        }else if (tokens.size() == 4 && tokens[2]== ">"){ //if redirecting the printfile
            string newfile = tokens[3];
            copy_file_another(filename, newfile);
        }
    }

    else if (command == "dididothat"){
        string search_command = "";
        for(int i = 1; i < tokens.size()-1; i++) {
            search_command += tokens[i];
            search_command += " ";
        }
        search_command += tokens[tokens.size()-1];
        bool did = dididothat(search_command);
        if(did) {cout << "Yes" << endl;} else if(!did) {cout << "No" <<endl;}
    }

    handle_history(tokens); //pushed the typed command into the queue after formatting
    

}

int main() {
    while (true){
        print_username(); //print username
        string line;
        getline(cin, line); //get input
        handle_command(line); //handle the command
    }
    return 0;
}