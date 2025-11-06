#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>

typedef struct List {
	PROCESS_INFORMATION pi;
	char name[10];
	int status;
	struct List* next;
} List;

List* head = NULL;

void addProcessToList(PROCESS_INFORMATION pi, char* name) {
	List* tmp = (List*)malloc(sizeof(List));
	tmp->pi = pi;
	strcpy(tmp->name, name);
	tmp->status = 1;
	tmp->next = NULL;
	if (head == NULL) {
		head = tmp;
	} else {
		List* cur = head;
		while (cur->next != NULL) {
			cur = cur->next;
		}
		cur->next = tmp;
	}
}

void removeProcess(int num) {
	List* cur = head;
	List* pre_cur = NULL;
	int cnt = 0;
	while (cur != NULL && cnt < num) {
		cnt++;
		pre_cur = cur;
		cur = cur->next;
	}
	if (cur == NULL) {
		printf("Khong tim thay process thu %d\n", num);
		return;
	}
	printf("Dang xoa tien trinh thu %d...\n", num);
	TerminateProcess(cur->pi.hProcess, 0);
	CloseHandle(cur->pi.hProcess);
	CloseHandle(cur->pi.hThread);
	if (pre_cur == NULL) {
		head = cur->next;
	} else {
		pre_cur->next = cur->next;
	}
	free(cur);
	printf("Da xoa thanh cong\n");
}

void stopProcess(int num) {
	List* cur = head;
	int cnt = 0;
	while (cur != NULL && cnt < num) {
		cur = cur->next;
		cnt++;
	}
	if (cur == NULL) {
		printf("Khong tim thay process thu %d\n", num);
		return;
	}
	SuspendThread(cur->pi.hThread);
	cur->status = 0;
	printf("Da tam dung tien trinh\n");
}

void resumeProcess(int num) {
	List* cur = head;
	int cnt = 0;
	while (cur != NULL && cnt < num) {
		cur = cur->next;
		cnt++;
	}
	if (cur == NULL) {
		printf("Khong tim thay tien trinh thu %d\n", num);
		return;
	} else if (cur->status != 0) {
		printf("Tien trinh chua tam dung\n");
		return;
	}
	ResumeThread(cur->pi.hThread);
	cur->status = 1;
	printf("Tien trinh tiep tuc hoat dong\n");	
}

void list() {
	List* cur = head;
	List* pre_cur = NULL;
	int cnt = 0;
	printf("Danh sach tien trinh la\n");
	while (cur != NULL) {
		DWORD result = WaitForSingleObject(cur->pi.hProcess, 0);
		if (result == WAIT_OBJECT_0) {
			List* tmp = cur;
			if (pre_cur == NULL) {
				head = cur->next;
			} else {
				pre_cur->next = cur->next;
			}
			cur = cur->next;
			free(tmp);
			continue;
		} else {	
			char status[10];
			if (cur->status == 1) {
				strcpy(status, "Running");
			} else if (cur->status == 0) {
				strcpy(status, "Stopping");
			}
			printf("%d. Process ID: %d Process Name: %s Status: %s\n",cnt ,cur->pi.dwProcessId, cur->name, status);
		}
		pre_cur = cur;
		cur = cur->next;
		cnt++;
	}
}

int chooseProcessFromList() {
	list();
	int choose;
	printf("Ban muon chon tien trinh nao: ");
	scanf("%d", &choose);
	getchar();
	return choose;
}

void createForeProcess(char* cmd) {
	strcat(cmd, ".exe");
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
	char name[10];
	strcpy(name, cmd);
	strcat(cmd, ".exe");
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    BOOL check = CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    if (check == TRUE) {
    	printf("Tao tien trinh chay song song thanh cong\n");
    	addProcessToList(pi, name);
	} else {
		printf("Loi khong tao duoc tien trinh\n");
	}
    return pi;
}

void help() {
   FILE *file = fopen("help.txt", "r"); 
   if (file == NULL) {
       printf("Loi khong thay file help\n");
   }
   char buffer[100]; 
   while (fgets(buffer, sizeof(buffer), file)) {
       printf("%s", buffer); 
   }
   fclose(file); 
}

void cur_time() {
	time_t current_time;
    time(&current_time);
    struct tm *local_time = localtime(&current_time);
    printf("Ngay/Thang/Nam: %02d/%02d/%04d\n", 
           local_time->tm_mday, 
           local_time->tm_mon + 1, 
           local_time->tm_year + 1900);
    printf("Thoi gian: %02d:%02d:%02d\n", 
           local_time->tm_hour, 
           local_time->tm_min, 
           local_time->tm_sec);
}

int main(){
	while (1) {
	    char command[100];
	    printf("MyShell>");
	    fgets(command, sizeof(command), stdin);
	    command[strlen(command)-1] = '\0';
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
				createBackProcess(cmd[1]);
			} else if (strcmp(cmd[0], "fopen") == 0) {
				createForeProcess(cmd[1]);
			} 
		} else if (cnt == 1) {
			if (strcmp(cmd[0], "list") == 0) {
				list();
			} else if (strcmp(cmd[0], "exit") == 0) {
				break;
			} else if (strcmp(cmd[0], "remove") == 0) {
				int cnt = chooseProcessFromList();
				removeProcess(cnt);
			} else if (strcmp(cmd[0], "stop") == 0) {
				int cnt = chooseProcessFromList();
				stopProcess(cnt);
			} else if (strcmp(cmd[0], "resume") == 0) {
				int cnt = chooseProcessFromList();
				resumeProcess(cnt);
			} else if (strcmp(cmd[0], "help") == 0) {
				help();
			} else if (strcmp(cmd[0], "time") == 0) {
				cur_time();
			}
		}
		printf("\n");
	}	
}
