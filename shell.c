#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <synchapi.h>



void createForeProcess(char* cmd) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    BOOL check = CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (check == TRUE) {
    	printf("Tien trinh da duoc tao thanh cong \nTat tien trinh de tiep tuc su dung shell\n");
	} else {
		printf("Tien trinh khong ton tai\n");
	}
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

PROCESS_INFORMATION createBackProcess(char* cmd) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    BOOL check = CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (check == TRUE) {
    	printf("Tao tien trinh chay song song thanh cong\n");
	} else {
		printf("Loi khong tao duoc tien trinh\n");
	}
    return pi;
}

int main(){
 	PROCESS_INFORMATION list[10];
	while (1) {
	    char command[100];
	    printf("MyShell>");
	    fgets(command, sizeof(command), stdin);
	    char* token = strtok(command," ");
	    int cnt = 0;
	    char **cmd = (char **)malloc(10 * sizeof(char *));
	    while (token) {
	    	cmd[cnt] = strdup(token);
	    	token = strtok(NULL, " ");
	    	cnt++;
		}
		if (cnt == 2) {
			if (strcmp(cmd[0], "bopen") == 0) {
				cmd[1][strlen(cmd[1])-1] = '\0';
				strcat(cmd[1], ".exe");
				createBackProcess(cmd[1]);
			} else if (strcmp(cmd[0], "fopen") == 0) {
				cmd[1][strlen(cmd[1])-1] = '\0';
				strcat(cmd[1], ".exe");
				createForeProcess(cmd[1]);
			}
		}
	}	
}
